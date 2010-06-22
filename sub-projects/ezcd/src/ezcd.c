/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : main.c
 *
 * Description  : EZCD main program
 *
 * Copyright (C) 2010 by TANG HUI
 *
 * History      Rev       Description
 * 2010-06-13   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>

#include "ezcd.h"

#define INVALID_SOCKET		(-1)
#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))

#ifdef DEBUG
#define DBG_PRINT(format, args...) fprintf(stderr, format, ##args)
#else
#define DBG_PRINT(format, args...)
#endif

typedef int SOCKET;

typedef void * (*ezcd_thread_func_t)(void *);

extern int threads_max;

/*
 * unified socket address. For IPv6 support, add IPv6 address structure
 * in the union u.
 */
struct usa {
	socklen_t len;
	union {
		struct sockaddr sa;
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
	int		stop_flag;	/* Should we stop event loop */
	char		*nvram;		/* NVRAM */
	struct socket 	*listening_sockets;
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

static void worker_thread(struct ezcd_context *ctx)
{
	while (ctx->stop_flag == 0) {
		printf("%s(%d) \n", __func__, __LINE__);
		sleep(1);
	}
}

static int start_thread(struct ezcd_context *ctx, ezcd_thread_func_t func, void *data)
{
	pthread_t       thread_id;
	pthread_attr_t  attr;
	int             retval;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr,
	                          sizeof(struct ezcd_context) * 2);

	if ((retval = pthread_create(&thread_id, &attr, func, data)) != 0)
		DBG_PRINT("%s: %s", __func__, strerror(retval));

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
        DBG_PRINT((DEBUG_MGS_PREFIX "%s: queued socket %d",
	           __func__, sp->sock));

	/* if there are no idle threads, start one */
	if ((ctx->num_idle == 0) &&
	    (ctx->num_threads < threads_max)) {
		if (start_thread(ctx,
		                 (ezcd_thread_func_t) worker_thread,
		                  ctx->nvram) != 0)
                        DBG_PRINT("Cannot start thread: %d", ERRNO);
		else
			ctx->num_threads++;
	}

	pthread_cond_signal(&ctx->empty_cond);
	pthread_mutex_unlock(&ctx->mutex);
}

static void accept_new_connection(const struct socket *listener, struct ezcd_context *ctx)
{
	struct socket   accepted;

	accepted.rsa.len = sizeof(accepted.rsa.u.sin);
	accepted.lsa = listener->lsa;
	if ((accepted.sock = accept(listener->sock,
	     &accepted.rsa.u.sa, &accepted.rsa.len)) == INVALID_SOCKET)
		return;

	/* put accepted socket structure into the queue */
	DBG_PRINT((DEBUG_MGS_PREFIX "%s: accepted socket %d",
                    __func__, accepted.sock));
	put_socket(ctx, &accepted);
}

static void monitor_thread(struct ezcd_context *ctx)
{
	fd_set read_set;
	struct socket *sp;
	struct timeval tv;
	int max_fd;

	while (ctx->stop_flag == 0) {
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
	struct ezcd_context *ctx;
	ctx = (struct ezcd_context *)malloc(sizeof(struct ezcd_context));
	if (ctx) {
		ctx->nvram = (char *)malloc(EZCD_SPACE);
		if (ctx->nvram == NULL) {
			free(ctx);
			return  NULL;
		}

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
		start_thread(ctx, (ezcd_thread_func_t) monitor_thread, ctx->nvram);
	}
	return ctx;
}

