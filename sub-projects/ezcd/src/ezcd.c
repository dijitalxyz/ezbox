/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : main.c
 *
 * Description  : EZCD lib program
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-06-13   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>

#include "ezcd.h"

#define INVALID_SOCKET		(-1)
#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))

#define MAX_REQUEST_SIZE	8192

typedef int SOCKET;

#if !defined(FALSE)
enum {FALSE, TRUE};
#endif /* !FALSE */

typedef int bool_t;

typedef void * (*ezcd_thread_func_t)(void *);

/*
 * unified socket address. For IPv6 support, add IPv6 address structure
 * in the union u.
 */
struct usa {
	socklen_t len;
	union {
		struct sockaddr sa;
		struct sockaddr_un sun;
		struct sockaddr_in sin;
	} u;
};

/*
 * structure used to describe listening socket, or socket which was
 * accept()-ed by the monitor thread and queued for future handling
 * by the worker thread.
 */
struct socket {
	struct socket   *next;          /* Linkage                      */
	SOCKET          sock;           /* Listening socket             */
	struct usa      lsa;            /* Local socket address         */
	struct usa      rsa;            /* Remote socket address        */
};

/*
 * ezcd context
 */
struct ezcd_context {
	int		stop_flag;	/* Should we stop event loop    */
	char		*nvram;		/* NVRAM                        */
	struct socket 	*listening_sockets;
	int 		threads_max;    /* MAX number of threads        */
	int 		num_threads;    /* Number of threads            */
	int 		num_idle;       /* Number of idle threads       */

	pthread_mutex_t mutex;          /* Protects (max|num)_threads   */
	pthread_rwlock_t rwlock;        /* Protects options, callbacks  */
	pthread_cond_t 	thr_cond;       /* Condvar for thread sync      */

	struct socket   queue[20];      /* Accepted sockets             */
	int             sq_head;        /* Head of the socket queue     */
	int             sq_tail;        /* Tail of the socket queue     */
	pthread_cond_t  empty_cond;     /* Socket queue empty condvar   */
	pthread_cond_t  full_cond;      /* Socket queue full condvar    */
};

/*
 * client connection.
 */

struct ezcd_connection {
	struct ezcd_context *ctx;
	struct socket client;
	time_t birth_time;
};

/*
 * Write data to the IO channel - opened file descriptor, socket or SSL
 * descriptor. Return number of bytes written.
 */
static int64_t
push(FILE *fp, SOCKET sock, const char *buf, int64_t len)
{
	int64_t sent;
	int     n, k;

	sent = 0;
	while (sent < len) {

		/* How many bytes we send in this iteration */
		k = len - sent > INT_MAX ? INT_MAX : (int) (len - sent);
		if (fp != NULL) {
			n = fwrite(buf + sent, 1, k, fp);
			if (ferror(fp))
				n = -1;
		} else {
			n = send(sock, buf + sent, k, 0);
		}

		if (n < 0)
			break;

		sent += n;
	}

        return (sent);
}

/*
 * Read from IO channel - opened file descriptor or socket.
 * Return number of bytes read.
 */
static int
pull(FILE *fp, SOCKET sock, char *buf, int len)
{
	int     nread;

	if (fp != NULL) {
		nread = fread(buf, 1, (size_t) len, fp);
		if (ferror(fp))
			nread = -1;
	} else {
		nread = recv(sock, buf, (size_t) len, 0);
	}

	return (nread);
}

/*
 * Check whether full request is buffered. Return:
 *   -1         if request is malformed
 *    0         if request is not yet fully buffered
 *   >0         actual request length, including last \r\n\r\n
 */
static int
get_request_len(const char *buf, size_t buflen)
{
	const char *s, *e;
	int len = 0;

	for (s = buf, e = s + buflen - 1; len <= 0 && s < e; s++)
		/* Control characters are not allowed but >=128 is. */
		if (!isprint(* (unsigned char *) s) && *s != '\r' &&
		    *s != '\n' && * (unsigned char *) s < 128)
			len = -1;
		else if (s[0] == '\n' && s[1] == '\n')
			len = (int) (s - buf) + 2;
		else if (s[0] == '\n' && &s[1] < e &&
		         s[1] == '\r' && s[2] == '\n')
			len = (int) (s - buf) + 3;

	return (len);
}

/*
 * Keep reading the input (either opened file descriptor fd, or socket sock,
 * or SSL descriptor ssl) into buffer buf, until \r\n\r\n appears in the
 * buffer (which marks the end of HTTP request). Buffer buf may already
 * have some data. The length of the data is stored in nread.
 * Upon every read operation, increase nread by the number of bytes read.
 */
static int
read_request(FILE *fp, SOCKET sock, char *buf, int bufsiz, int *nread)
{
	int n, request_len;

	request_len = 0;
	while (*nread < bufsiz && request_len == 0) {
		n = pull(fp, sock, buf + *nread, bufsiz - *nread);
		if (n <= 0) {
			break;
		} else {
			*nread += n;
			request_len = get_request_len(buf, (size_t) *nread);
		}
	}

	return (request_len);
}

static int
set_non_blocking_mode(struct ezcd_connection *conn, SOCKET sock)
{
	int flags, ok = -1;

	if ((flags = fcntl(sock, F_GETFL, 0)) == -1) {
		printf("%s: fcntl(F_GETFL): %d\n", __func__, errno);
	} else if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) != 0) {
		printf("%s: fcntl(F_SETFL): %d\n", __func__, errno);
	} else {
		ok = 0; /* Success */
        }

        return (ok);
}

static void
close_socket_gracefully(struct ezcd_connection *conn, SOCKET sock)
{
	char buf[BUFSIZ];
        int n;

	/* Send FIN to the client */
	shutdown(sock, SHUT_WR);
	set_non_blocking_mode(conn, sock);

	/*
	 * Read and discard pending data. If we do not do that and close the
	 * socket, the data in the send buffer may be discarded. This
	 * behaviour is seen on Windows, when client keeps sending data
	 * when server decide to close the connection; then when client
	 * does recv() it gets no data back.
	 */
	do {
		n = pull(NULL, sock, buf, sizeof(buf));
	} while (n > 0);

	/* Now we know that our FIN is ACK-ed, safe to close */
	close(sock);
}

static void
close_connection(struct ezcd_connection *conn)
{
	printf("huedebug--%s(%d)\n", __func__, __LINE__);
	if (conn->client.sock != INVALID_SOCKET)
		close_socket_gracefully(conn, conn->client.sock);
}

static void
process_new_connection(struct ezcd_connection *conn)
{
	char buf[MAX_REQUEST_SIZE];
	int request_len, nread;

	printf("huedebug--%s(%d)\n", __func__, __LINE__);

	nread = 0;
	request_len = 0;
	memset(buf, 0, sizeof(buf));

	/* If next request is not pipelined, read it in */
	//if ((request_len = get_request_len(buf, (size_t) nread)) == 0) {
	{
	printf("huedebug--%s(%d)\n", __func__, __LINE__);
		request_len = read_request(NULL, conn->client.sock,
		                           buf, sizeof(buf), &nread);
	printf("huedebug--%s(%d) request_len=[%d], buf=[%s]\n", __func__, __LINE__, request_len, buf);
	}
	assert(nread >= request_len);

	printf("huedebug--%s(%d)\n", __func__, __LINE__);
	if (request_len <= 0)
		return; /* Remote end closed the connection */

	printf("huedebug--%s(%d)\n", __func__, __LINE__);
	/* 0-terminate the request: parse_request uses sscanf */
	buf[request_len - 1] = '\0';

	printf("huedebug--%s(%d)buf=[%s], len=[%d]\n", __func__, __LINE__, buf, request_len);
	push(NULL, conn->client.sock, (const char *) buf, (int64_t) request_len);
}

/*
 * Worker threads take accepted socket from the queue
 */
static bool_t
get_socket(struct ezcd_context *ctx, struct socket *sp)
{
	struct timespec ts;
	pthread_mutex_lock(&ctx->mutex);

	/* If the queue is empty, wait. We're idle at this point. */
	ctx->num_idle++;
	while (ctx->sq_head == ctx->sq_tail) {
		ts.tv_nsec = 0;
		ts.tv_sec = time(NULL) + 10;
		if (pthread_cond_timedwait(&ctx->empty_cond,
		    &ctx->mutex, &ts) != 0) {
			/* Timeout! release the mutex and return */
			pthread_mutex_unlock(&ctx->mutex);
			return (FALSE);
		}
	}
	assert(ctx->sq_head > ctx->sq_tail);

	/* We're going busy now: got a socket to process! */
	ctx->num_idle--;

	/* Copy socket from the queue and increment tail */
	*sp = ctx->queue[ctx->sq_tail % ARRAY_SIZE(ctx->queue)];
	ctx->sq_tail++;
	printf("%s: thread %p grabbed socket %d, going busy\n",
            __func__, (void *) pthread_self(), sp->sock);


	/* Wrap pointers if needed */
	while (ctx->sq_tail > (int) ARRAY_SIZE(ctx->queue)) {
		ctx->sq_tail -= ARRAY_SIZE(ctx->queue);
		ctx->sq_head -= ARRAY_SIZE(ctx->queue);
	}

	pthread_cond_signal(&ctx->full_cond);
	pthread_mutex_unlock(&ctx->mutex);

	return (TRUE);
}

static void worker_thread(struct ezcd_context *ctx)
{
	struct ezcd_connection conn;

	printf("huedebug--%s(%d)\n", __func__, __LINE__);

	memset(&conn, 0, sizeof(conn));

	while (get_socket(ctx, &conn.client) == TRUE) {
		printf("huedebug--%s(%d)\n", __func__, __LINE__);
		conn.birth_time = time(NULL);
		conn.ctx = ctx;
		process_new_connection(&conn);
		close_connection(&conn);
	}

	printf("huedebug--%s(%d)\n", __func__, __LINE__);
        /* Signal monitor that we're done with connection and exiting */
	pthread_mutex_lock(&ctx->mutex);
	ctx->num_threads--;
	ctx->num_idle--;
	pthread_cond_signal(&ctx->thr_cond);
	assert(ctx->num_threads >= 0);
	pthread_mutex_unlock(&ctx->mutex);
	printf("huedebug--%s(%d)\n", __func__, __LINE__);
}

static int start_thread(struct ezcd_context *ctx, ezcd_thread_func_t func, void *args)
{
	pthread_t       thread_id;
	pthread_attr_t  attr;
	int             retval;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr,
	                          sizeof(struct ezcd_context) * 2);

	if ((retval = pthread_create(&thread_id, &attr, func, args)) != 0)
		printf("%s: %s\n", __func__, strerror(retval));

	return (retval);
}

static void close_all_listening_sockets(struct ezcd_context *ctx)
{
	struct socket   *sp, *tmp;
	for (sp = ctx->listening_sockets; sp != NULL; sp = tmp) {
		tmp = sp->next;
                close(sp->sock);
		free(sp);
	}
	ctx->listening_sockets = NULL;
}

static void add_to_set(SOCKET fd, fd_set *set, int *max_fd)
{
	FD_SET(fd, set);
	if (fd > (SOCKET) *max_fd)
		*max_fd = (int) fd;
}

/*
 * deallocate ezcd context, free up the resources
 */
static void ezcd_finish(struct ezcd_context *ctx)
{
	close_all_listening_sockets(ctx);

	/* wait until all threads finish */
	pthread_mutex_lock(&ctx->mutex);
	while (ctx->num_threads > 0)
		pthread_cond_wait(&ctx->thr_cond, &ctx->mutex);
	pthread_mutex_unlock(&ctx->mutex);

	pthread_rwlock_destroy(&ctx->rwlock);
	pthread_mutex_destroy(&ctx->mutex);
	pthread_cond_destroy(&ctx->thr_cond);
	pthread_cond_destroy(&ctx->empty_cond);
	pthread_cond_destroy(&ctx->full_cond);

	/* signal ezcd_stop() that we're done */
	ctx->stop_flag = 2;
}

/*
 * monitor thread adds accepted socket to a queue
 */
static void put_socket(struct ezcd_context *ctx, const struct socket *sp)
{
	pthread_mutex_lock(&ctx->mutex);

	/* if the queue is full, wait */
	while (ctx->sq_head - ctx->sq_tail >= (int) ARRAY_SIZE(ctx->queue))
		pthread_cond_wait(&ctx->full_cond, &ctx->mutex);
	assert(ctx->sq_head - ctx->sq_tail < (int) ARRAY_SIZE(ctx->queue));

	/* copy socket to the queue and increment head */
	ctx->queue[ctx->sq_head % ARRAY_SIZE(ctx->queue)] = *sp;
	ctx->sq_head++;
        printf("%s: queued socket %d\n",
	           __func__, sp->sock);

	/* if there are no idle threads, start one */
	if ((ctx->num_idle == 0) &&
	    (ctx->num_threads < ctx->threads_max)) {
		if (start_thread(ctx,
		                 (ezcd_thread_func_t) worker_thread,
		                  ctx) != 0)
                        printf("Cannot start thread: %d\n", errno);
		else
			ctx->num_threads++;
	}

	pthread_cond_signal(&ctx->empty_cond);
	pthread_mutex_unlock(&ctx->mutex);
}

static void accept_new_connection(const struct socket *listener, struct ezcd_context *ctx)
{
	struct socket   accepted;

	printf("huedebug--%s(%d)\n", __func__, __LINE__);
	accepted.rsa.len = sizeof(accepted.rsa.u.sun);
	accepted.lsa = listener->lsa;
	if ((accepted.sock = accept(listener->sock,
	     &accepted.rsa.u.sa, &accepted.rsa.len)) == INVALID_SOCKET)
		return;

	/* put accepted socket structure into the queue */
	printf("%s: accepted socket %d sun_path=[%s]\n",
                    __func__, accepted.sock, accepted.rsa.u.sun.sun_path);
	put_socket(ctx, &accepted);
}

static void monitor_thread(struct ezcd_context *ctx)
{
	fd_set read_set;
	struct socket *sp;
	struct timeval tv;
	int max_fd;

	while (ctx->stop_flag == 0) {
		printf("huedebug--%s(%d)\n", __func__, __LINE__);
		FD_ZERO(&read_set);
		max_fd = -1;

		/* add listening sockets to the read set */
		pthread_rwlock_rdlock(&ctx->rwlock);
		for (sp = ctx->listening_sockets; sp != NULL; sp = sp->next)
			add_to_set(sp->sock, &read_set, &max_fd);
		pthread_rwlock_unlock(&ctx->rwlock);

		tv.tv_sec = 1;
		tv.tv_usec = 0;

		if (select(max_fd + 1, &read_set, NULL, NULL, &tv) < 0) {
			/* do nothing */
		} else {
			pthread_rwlock_rdlock(&ctx->rwlock);
			for (sp = ctx->listening_sockets; sp != NULL; sp = sp->next)
				if (FD_ISSET(sp->sock, &read_set))
					accept_new_connection(sp, ctx);
			pthread_rwlock_unlock(&ctx->rwlock);
		}
	}

	/* stop signal received: somebody called ezcd_stop. Quit. */
	ezcd_finish(ctx);
}

/*
 * public functions
 */
void ezcd_stop(struct ezcd_context *ctx)
{
	ctx->stop_flag = 1;

	/* wait until ezcd_finish() stops */
	while (ctx->stop_flag != 2)
		sleep(1);

	assert(ctx->num_threads == 0);
	free(ctx);
}

struct ezcd_context *ezcd_start(void)
{
	struct ezcd_context *ctx = NULL;
	struct socket *listener = NULL;
	SOCKET sock = -1;
	struct usa *usa = NULL;
	ctx = (struct ezcd_context *)malloc(sizeof(struct ezcd_context));
	if (ctx) {
		/* initialize ctx */
		memset(ctx, 0, sizeof(struct ezcd_context));

		/* initialize unix domain socket */
		if ((listener = calloc(1, sizeof(*listener))) == NULL) {
			goto fail_exit;
		}

		/* unset umask */
		umask(0);

		/* setup socket files directory */
		mkdir("/tmp/ezcd", 0777);

		usa = &(listener->lsa);
		usa->u.sun.sun_family = AF_UNIX;
		strcpy(usa->u.sun.sun_path, EZCD_SOCKET_PATH);
		usa->len = offsetof(struct sockaddr_un, sun_path) + strlen(usa->u.sun.sun_path);

		if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
			perror("socket error");
			goto fail_exit;
		}

		if (bind(sock, (struct sockaddr *)&(usa->u.sun), usa->len) < 0) {
			perror("bind error");
			goto fail_exit;
		}

		if (chmod (usa->u.sun.sun_path, 0666) < 0) {
			perror("chmod socket error");
			goto fail_exit;
		}

		if (listen(sock, 20) < 0) {
			perror("listen socket error");
			goto fail_exit;
		}

		listener->sock = sock;
		listener->next = ctx->listening_sockets;
		ctx->listening_sockets = listener;

		ctx->nvram = (char *)malloc(EZCD_SPACE);
		if (ctx->nvram == NULL) {
			goto fail_exit;
		}
		/* initialize nvram */
		memset(ctx->nvram, 0, EZCD_SPACE);

		/*
		 * ignore SIGPIPE signal, so if client cancels the request, it
		 * won't kill the whole process.
		 */
		signal(SIGPIPE, SIG_IGN);

		pthread_rwlock_init(&ctx->rwlock, NULL);
		pthread_mutex_init(&ctx->mutex, NULL);
		pthread_cond_init(&ctx->thr_cond, NULL);
		pthread_cond_init(&ctx->empty_cond, NULL);
		pthread_cond_init(&ctx->full_cond, NULL);

		/* Start monitor thread */
		start_thread(ctx, (ezcd_thread_func_t) monitor_thread, ctx);
		return ctx;
	}
fail_exit:
	if (ctx != NULL) {
		if (ctx->nvram != NULL)
			free(ctx->nvram);
		free(ctx);
	}
	if (listener != NULL) {
		free(listener);
	}
	if (sock >= 0) {
		close(sock);
	}
	return NULL;
}

void ezcd_set_threads_max(struct ezcd_context *ctx, int threads_max)
{
	assert(ctx);
	ctx->threads_max = threads_max;
}

int ezcd_set_listening_socket(struct ezcd_context *ctx, char *sock_name)
{
	return(EXIT_SUCCESS);
}

#define CLI_PATH    "/tmp/ezcd/ezci"        /* +5 for pid = 14 chars */
#define CLI_PERM    S_IRWXU            /* rwx for user only */

/* returns fd if all OK, -1 on error */
int ezcd_client_connection (const char *name, char project)
{
	int fd, len;
	struct sockaddr_un unix_addr;

	/* create a Unix domain stream socket */
	if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		return(-1);

	/* fill socket address structure w/our address */
	memset(&unix_addr, 0, sizeof(unix_addr));
	unix_addr.sun_family = AF_UNIX;
	sprintf(unix_addr.sun_path, "%s%05d-%c", CLI_PATH, getpid(), project);
	len = sizeof(unix_addr.sun_family) + strlen(unix_addr.sun_path);

	unlink (unix_addr.sun_path); /* in case it already exists */
	if (bind(fd, (struct sockaddr *) &unix_addr, len) < 0)
		goto error;
	if (chmod(unix_addr.sun_path, CLI_PERM) < 0)
		goto error;

	/* fill socket address structure w/server's addr */
	memset(&unix_addr, 0, sizeof(unix_addr));
		unix_addr.sun_family = AF_UNIX;
	strcpy(unix_addr.sun_path, name);
	len = sizeof(unix_addr.sun_family) + strlen(unix_addr.sun_path);

	if (connect (fd, (struct sockaddr *) &unix_addr, len) < 0)
		goto error;

	return (fd);

error:
	close (fd);
	return(-1);
}
