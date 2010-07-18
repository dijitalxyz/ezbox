/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg-master.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-12   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdarg.h>
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

#include "libezcfg.h"
#include "libezcfg-private.h"

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
 * accept()-ed by the master thread and queued for future handling
 * by the worker thread.
 */
struct socket {
	struct socket	*next;		/* Linkage                      */
	int		sock;		/* Listening socket             */
	struct usa	lsa;		/* Local socket address         */
	struct usa	rsa;		/* Remote socket address        */
};

/*
 * ezcfg_master:
 *
 * Opaque object handling one event source.
 * Multi-threads model - master part.
 */
struct ezcfg_master {
	struct ezcfg *ezcfg;
	int stop_flag; /* Should we stop event loop */
	char *nvram; /* Non-volatile memory */
	int threads_max; /* MAX number of threads */
	int num_threads; /* Number of threads */
	int num_idle; /* Number of idle threads */

	pthread_mutex_t mutex; /* Protects (max|num)_threads */
	pthread_rwlock_t rwlock; /* Protects options, callbacks */
	pthread_cond_t thr_cond; /* Condvar for thread sync */

	struct socket *listening_sockets;
	struct socket queue[20]; /* Accepted sockets */
	int sq_head; /* Head of the socket queue */
	int sq_tail; /* Tail of the socket queue */
	pthread_cond_t empty_cond; /* Socket queue empty condvar */
	pthread_cond_t full_cond;  /* Socket queue full condvar */
};

static void ezcfg_master_delete(struct ezcfg_master *master)
{
	struct socket *sp;
	if (master == NULL)
		return;
	if (master->nvram)
		free(master->nvram);
	for(sp = master->listening_sockets; sp != NULL ; sp = master->listening_sockets) {
		master->listening_sockets = sp->next;
		close(sp->sock);
		free(sp);
	}
	free(master);
}

/**
 * ezcfg_master_new:
 *
 * Create ezcfg master.
 *
 * Returns: a new ezcfg master
 **/

static struct ezcfg_master *ezcfg_master_new(struct ezcfg *ezcfg)
{
	struct ezcfg_master *ezcfg_master = NULL;

	ezcfg_master = calloc(1, sizeof(struct ezcfg_master));
	if (ezcfg_master) {
		/* initialize ezcfg library context */
		memset(ezcfg_master, 0, sizeof(struct ezcfg_master));

		ezcfg_master->nvram = calloc(EZCFG_NVRAM_SPACE, sizeof(char));
		if(ezcfg_master->nvram == NULL) {
			err(ezcfg, "calloc nvram.");
			goto fail_exit;
		}

		/* initialize nvram */
		memset(ezcfg_master->nvram, 0, EZCFG_NVRAM_SPACE);

		/*
		 * ignore SIGPIPE signal, so if client cancels the request, it
		 * won't kill the whole process.
		 */
		signal(SIGPIPE, SIG_IGN);

		/* initialize thread mutex */
		pthread_rwlock_init(&ezcfg_master->rwlock, NULL);
		pthread_mutex_init(&ezcfg_master->mutex, NULL);
		pthread_cond_init(&ezcfg_master->thr_cond, NULL);
		pthread_cond_init(&ezcfg_master->empty_cond, NULL);
		pthread_cond_init(&ezcfg_master->full_cond, NULL);

		/* set ezcfg library context */
		ezcfg_master->ezcfg = ezcfg;

		return ezcfg_master;
	}
fail_exit:
	ezcfg_master_delete(ezcfg_master);
	return NULL;
}

/**
 * ezcfg_master_add_socket:
 * @ezcfg: ezcfg master context
 * @socket_path: unix socket path
 *
 * Add a specified socket to the master thread. The
 * path to a socket either points to an existing socket file, or if
 * the socket path starts with a '@' character, an abstract namespace
 * socket will be used.
 *
 * A socket file will not be created. If it does not already exist,
 * it will fall-back and connect to an abstract namespace socket with
 * the given path. The permissions adjustment of a socket file, as
 * well as the later cleanup, needs to be done by the caller.
 *
 * Returns: socket, or -1, in case of an error
 **/
static int ezcfg_master_add_socket(struct ezcfg_master *master, const char *socket_path)
{
	struct stat statbuf;
	struct socket *listener = NULL;
	int sock = -1;
	struct usa *usa = NULL;

	if (master == NULL)
		return -1;
	if (socket_path == NULL)
		return -1;

	/* initialize unix domain socket */
	if ((listener = calloc(1, sizeof(struct socket))) == NULL) 
		return -1;

	usa = &(listener->lsa);
	usa->u.sun.sun_family = AF_LOCAL;
	if (socket_path[0] == '@') {
		/* translate leading '@' to abstract namespace */
		util_strscpy(usa->u.sun.sun_path, sizeof(usa->u.sun.sun_path), socket_path);
		usa->u.sun.sun_path[0] = '\0';
		usa->len = offsetof(struct sockaddr_un, sun_path) + strlen(socket_path);
	} else if (stat(socket_path, &statbuf) == 0 && S_ISSOCK(statbuf.st_mode)) {
		/* existing socket file */
		util_strscpy(usa->u.sun.sun_path, sizeof(usa->u.sun.sun_path), socket_path);
		usa->len = offsetof(struct sockaddr_un, sun_path) + strlen(socket_path);
	} else {
		/* no socket file, assume abstract namespace socket */
		util_strscpy(&usa->u.sun.sun_path[1], sizeof(usa->u.sun.sun_path)-1, socket_path);
		usa->len = offsetof(struct sockaddr_un, sun_path) + strlen(socket_path)+1;
	}

	if ((sock = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
		err(master->ezcfg, "socket error\n");
		goto fail_exit;
	}

	listener->sock = sock;
	listener->next = master->listening_sockets;
	master->listening_sockets = listener;
	return sock;
fail_exit:
	if (listener != NULL)
		free(listener);
	if (sock >= 0)
		close(sock);
	return -1;
}

/**
 * ezcfg_master_enable_socket_receiving:
 * @master: the master which should receive events
 * @sock: the listening socket which should receive events
 *
 * Binds the @master socket to the event source.
 *
 * Returns: 0 on success, otherwise a negative error value.
 */
static int ezcfg_master_enable_socket_receiving(struct ezcfg_master *master, int sock)
{
	int err = 0;
	const int on = 1;
	struct socket *sp;
	struct usa *usa = NULL;

	for(sp = master->listening_sockets; sp != NULL; sp = sp->next) {
		if (sp->sock == sock)
			break;
	}

	if (sp == NULL)
		return -EINVAL;

	usa = &(sp->lsa);
	if (usa->u.sun.sun_family != 0) {
		err = bind(sp->sock,
		           (struct sockaddr *)&usa->u.sun, usa->len);
	} else {
		return -EINVAL;
	}
	if (err < 0) {
		err(master->ezcfg, "bind failed: %m\n");
		return err;
	}

	/* enable receiving of sender credentials */
	setsockopt(sp->sock, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on));
	return 0;
}

/**
 * ezcfg_master_set_receive_buffer_size:
 * @ezcfg_master: the master which should receive events
 * @size: the size in bytes
 *
 * Set the size of the kernel socket buffer. This call needs the
 * appropriate privileges to succeed.
 *
 * Returns: 0 on success, otherwise -1 on error.
 */
int ezcfg_master_set_receive_buffer_size(struct ezcfg_master *ezcfg_master, int size)
{
	struct socket *sp;

	if (ezcfg_master == NULL)
		return -1;
	for(sp = ezcfg_master->listening_sockets; sp ; sp = sp->next) {
		if (setsockopt(sp->sock, SOL_SOCKET, SO_RCVBUFFORCE, &size, sizeof(size)) < 0)
			return -1;
	}
	return 0;
}

/**
 * ezcfg_master_new_from_socket:
 * @ezcfg: ezcfg library context
 * @socket_path: unix socket path
 *
 * Create new ezcfg master and connect to a specified socket. The
 * path to a socket either points to an existing socket file, or if
 * the socket path starts with a '@' character, an abstract namespace
 * socket will be used.
 *
 * A socket file will not be created. If it does not already exist,
 * it will fall-back and connect to an abstract namespace socket with
 * the given path. The permissions adjustment of a socket file, as
 * well as the later cleanup, needs to be done by the caller.
 *
 * Returns: a new ezcfg master, or #NULL, in case of an error
 **/
static struct ezcfg_master *ezcfg_master_new_from_socket(struct ezcfg *ezcfg, const char *socket_path)
{
	struct ezcfg_master *master = NULL;
	int sock = -1;

	if (ezcfg == NULL)
		return NULL;
	if (socket_path == NULL)
		return NULL;

	master = ezcfg_master_new(ezcfg);
	if (master == NULL) {
		err(ezcfg, "new master fail: %m\n");
		goto fail_exit;
	}

	sock = ezcfg_master_add_socket(master, socket_path);
	if (sock == -1) {
		err(ezcfg, "add socket [%s] fail: %m\n", socket_path);
		goto fail_exit;
	}

	if (ezcfg_master_enable_socket_receiving(master, sock) < 0) {
		err(ezcfg, "enable socket [%s] receiving fail: %m\n", socket_path);
		goto fail_exit;
	}
	return master;
fail_exit:
	if (sock >= 0)
		close(sock);
	ezcfg_master_delete(master);
	return NULL;
}

/*
 * Deallocate ezcfg master context, free up the resources
 */
static void ezcfg_master_finish(struct ezcfg_master *master)
{
	//close_all_listening_sockets(master);

	/* Wait until all threads finish */
	pthread_mutex_lock(&master->mutex);
	while (master->num_threads > 0)
		pthread_cond_wait(&master->thr_cond, &master->mutex);
	pthread_mutex_unlock(&master->mutex);

	pthread_rwlock_destroy(&master->rwlock);
	pthread_mutex_destroy(&master->mutex);
	pthread_cond_destroy(&master->thr_cond);
	pthread_cond_destroy(&master->empty_cond);
	pthread_cond_destroy(&master->full_cond);

        /* signal ezcd_stop() that we're done */
        master->stop_flag = 2;
}

static void add_to_set(int fd, fd_set *set, int *max_fd)
{
	FD_SET(fd, set);
	if (fd > *max_fd)
	{
		*max_fd = (int) fd;
	}
}

// Master thread adds accepted socket to a queue
static void put_socket(struct ezcfg_master *master, const struct socket *sp)
{
	int stacksize = sizeof(struct ezcfg_master) * 2;
	
	pthread_mutex_lock(&master->mutex);

	// If the queue is full, wait
	while (master->sq_head - master->sq_tail >= (int) ARRAY_SIZE(master->queue)) {
		pthread_cond_wait(&master->full_cond, &master->mutex);
	}
	assert(master->sq_head - master->sq_tail < (int) ARRAY_SIZE(master->queue));

	// Copy socket to the queue and increment head
	master->queue[master->sq_head % ARRAY_SIZE(master->queue)] = *sp;
	master->sq_head++;
	info(master->ezcfg, "%s: queued socket %d", __func__, sp->sock);

	// If there are no idle threads, start one
	if (master->num_idle == 0 && master->num_threads < master->threads_max) {
		if (ezcfg_thread_start(master->ezcfg, stacksize, (ezcfg_thread_func_t) ezcfg_worker_thread, master) != 0) {
			err(master->ezcfg, "Cannot start thread: %d", errno);
		} else {
			master->num_threads++;
		}
	}

	pthread_cond_signal(&master->empty_cond);
	pthread_mutex_unlock(&master->mutex);
}

static void accept_new_connection(const struct socket *listener,
                                  struct ezcfg_master *master) {
	struct socket accepted;
	bool allowed;

	accepted.rsa.len = sizeof(accepted.rsa.u.sin);
	accepted.lsa = listener->lsa;
	if ((accepted.sock = accept(listener->sock, &accepted.rsa.u.sa,
	                            &accepted.rsa.len)) == -1) {
		return;
	}

	//allowed = check_acl(ctx, &accepted.rsa) == MG_SUCCESS;
	allowed = true;

	if (allowed == true) {
		// Put accepted socket structure into the queue
		info(master->ezcfg, "%s: accepted socket %d",
		     __func__, accepted.sock);
		put_socket(master, &accepted);
	} else {
		info(master->ezcfg, "%s: %s is not allowed to connect",
		     __func__, inet_ntoa(accepted.rsa.u.sin.sin_addr));
		close(accepted.sock);
	}
}

void ezcfg_master_thread(struct ezcfg_master *master) 
{
	fd_set read_set;
	struct socket *sp;
	struct timeval tv;
	int max_fd;

	while (master->stop_flag == 0) {
		FD_ZERO(&read_set);
		max_fd = -1;

		/* Add listening sockets to the read set */
		for (sp = master->listening_sockets; sp != NULL; sp = sp->next) {
			add_to_set(sp->sock, &read_set, &max_fd);
		}

		tv.tv_sec = 1;
		tv.tv_usec = 0;

		if (select(max_fd + 1, &read_set, NULL, NULL, &tv) < 0) {
			/* do nothing */
			do {} while(0);
		} else {
			for (sp = master->listening_sockets; sp != NULL; sp = sp->next)
				if (FD_ISSET(sp->sock, &read_set))
					accept_new_connection(sp, master);
		}
	}

	/* Stop signal received: somebody called ezcfg_stop. Quit. */
	ezcfg_master_finish(master);
}

struct ezcfg_master *ezcfg_master_start(struct ezcfg *ezcfg)
{
	struct ezcfg_master *master = NULL;
	int stacksize = sizeof(struct ezcfg_master) * 2;
	int sock;

	if (ezcfg == NULL)
		return NULL;

	/* There must be a ctrl socket */
	master = ezcfg_master_new_from_socket(ezcfg, EZCFG_CTRL_SOCK_PATH);
	if (master == NULL)
		return NULL;

	sock = ezcfg_master_add_socket(master, EZCFG_MONITOR_SOCK_PATH);
	if (sock == -1) {
		err(ezcfg, "can not add monitor socket");
	}

	/* Start master (listening) thread */
	ezcfg_thread_start(ezcfg, stacksize, (ezcfg_thread_func_t) ezcfg_master_thread, master);
	return master;
}

void ezcfg_master_stop(struct ezcfg_master *master)
{
	if (master == NULL)
		return;

	master->stop_flag = 1;

	/* Wait until ezcfg_master_finish() stops */
	while (master->stop_flag != 2)
		sleep(1);

	assert(master->num_threads == 0);
	ezcfg_master_delete(master);
}

void ezcfg_master_set_threads_max(struct ezcfg_master *master, int threads_max)
{
	if (master == NULL)
		return;
	master->threads_max = threads_max;
}
