/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg-worker.c
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
 * accept()-ed by the worker thread and queued for future handling
 * by the worker thread.
 */
struct socket {
	struct socket	*next;		/* Linkage                      */
	int		sock;		/* Listening socket             */
	struct usa	lsa;		/* Local socket address         */
	struct usa	rsa;		/* Remote socket address        */
};

/*
 * ezcfg_worker:
 *
 * Opaque object handling one event source.
 * Multi-threads model - worker part.
 */
struct ezcfg_worker {
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

void ezcfg_worker_delete(struct ezcfg_worker *worker)
{
	struct socket *sp;
	if (worker == NULL)
		return;
	if (worker->nvram)
		free(worker->nvram);
	for(sp = worker->listening_sockets; sp ; sp = worker->listening_sockets) {
		worker->listening_sockets = sp->next;
		close(sp->sock);
		free(sp);
	}
	free(worker);
}

/**
 * ezcfg_worker_new:
 *
 * Create ezcfg worker.
 *
 * Returns: a new ezcfg worker
 **/

static struct ezcfg_worker *ezcfg_worker_new(struct ezcfg *ezcfg)
{
	struct ezcfg_worker *ezcfg_worker = NULL;

	ezcfg_worker = calloc(1, sizeof(struct ezcfg_worker));
	if (ezcfg_worker) {
		/* initialize ezcfg library context */
		memset(ezcfg_worker, 0, sizeof(struct ezcfg_worker));

		ezcfg_worker->nvram = calloc(EZCFG_NVRAM_SPACE, sizeof(char));
		if(ezcfg_worker->nvram == NULL) {
			err(ezcfg, "calloc nvram.");
			goto fail_exit;
		}

		/* initialize nvram */
		memset(ezcfg_worker->nvram, 0, EZCFG_NVRAM_SPACE);

		/*
		 * ignore SIGPIPE signal, so if client cancels the request, it
		 * won't kill the whole process.
		 */
		signal(SIGPIPE, SIG_IGN);

		/* initialize thread mutex */
		pthread_rwlock_init(&ezcfg_worker->rwlock, NULL);
		pthread_mutex_init(&ezcfg_worker->mutex, NULL);
		pthread_cond_init(&ezcfg_worker->thr_cond, NULL);
		pthread_cond_init(&ezcfg_worker->empty_cond, NULL);
		pthread_cond_init(&ezcfg_worker->full_cond, NULL);

		/* set ezcfg library context */
		ezcfg_worker->ezcfg = ezcfg;

		return ezcfg_worker;
	}
fail_exit:
	ezcfg_worker_delete(ezcfg_worker);
	return NULL;
}

/**
 * ezcfg_worker_new_from_socket:
 * @ezcfg: ezcfg library context
 * @socket_path: unix socket path
 *
 * Create new ezcfg worker and connect to a specified socket. The
 * path to a socket either points to an existing socket file, or if
 * the socket path starts with a '@' character, an abstract namespace
 * socket will be used.
 *
 * A socket file will not be created. If it does not already exist,
 * it will fall-back and connect to an abstract namespace socket with
 * the given path. The permissions adjustment of a socket file, as
 * well as the later cleanup, needs to be done by the caller.
 *
 * The initial refcount is 1, and needs to be decremented to
 * release the resources of the ezcfg worker.
 *
 * Returns: a new ezcfg worker, or #NULL, in case of an error
 **/
struct ezcfg_worker *ezcfg_worker_new_from_socket(struct ezcfg *ezcfg, const char *socket_path)
{
	struct ezcfg_worker *ezcfg_worker = NULL;
	struct stat statbuf;
	struct socket *listener = NULL;
	int sock = -1;
	struct usa *usa = NULL;

	if (ezcfg == NULL)
		return NULL;
	if (socket_path == NULL)
		return NULL;

	/* initialize unix domain socket */
	if ((listener = calloc(1, sizeof(struct socket))) == NULL) 
		return NULL;

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
		err(ezcfg, "socket error\n");
		goto fail_exit;
	}


	ezcfg_worker = ezcfg_worker_new(ezcfg);
	if (ezcfg_worker == NULL) {
		err(ezcfg, "new worker fail: %m\n");
		goto fail_exit;
	}

	listener->sock = sock;
	listener->next = ezcfg_worker->listening_sockets;
	ezcfg_worker->listening_sockets = listener;
	return ezcfg_worker;
fail_exit:
	if (listener != NULL)
		free(listener);
	if (sock >= 0)
		close(sock);
	ezcfg_worker_delete(ezcfg_worker);
	return NULL;
}

/**
 * ezcfg_worker_enable_receiving:
 * @ezcfg_worker: the worker which should receive events
 *
 * Binds the @ezcfg_worker socket to the event source.
 *
 * Returns: 0 on success, otherwise a negative error value.
 */
int ezcfg_worker_enable_receiving(struct ezcfg_worker *ezcfg_worker)
{
	int err;
	const int on = 1;
	struct socket *sp;
	struct usa *usa = NULL;

	for(sp = ezcfg_worker->listening_sockets; sp ; sp = sp->next) {

		usa = &(sp->lsa);
		if (usa->u.sun.sun_family != 0) {
			err = bind(sp->sock,
		                   (struct sockaddr *)&usa->u.sun, usa->len);
		} else {
			return -EINVAL;
		}
		if (err < 0) {
			err(ezcfg_worker->ezcfg, "bind failed: %m\n");
			return err;
		}
		/* enable receiving of sender credentials */
		setsockopt(sp->sock, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on));
	}

	return 0;
}

/**
 * ezcfg_worker_set_receive_buffer_size:
 * @ezcfg_worker: the worker which should receive events
 * @size: the size in bytes
 *
 * Set the size of the kernel socket buffer. This call needs the
 * appropriate privileges to succeed.
 *
 * Returns: 0 on success, otherwise -1 on error.
 */
int ezcfg_worker_set_receive_buffer_size(struct ezcfg_worker *ezcfg_worker, int size)
{
	struct socket *sp;

	if (ezcfg_worker == NULL)
		return -1;
	for(sp = ezcfg_worker->listening_sockets; sp ; sp = sp->next) {
		if (setsockopt(sp->sock, SOL_SOCKET, SO_RCVBUFFORCE, &size, sizeof(size)) < 0)
			return -1;
	}
	return 0;
}

static void ezcfg_worker_thread(struct ezcfg_worker *worker) 
{

}

struct ezcfg_worker *ezcfg_worker_start(struct ezcfg *ezcfg, const char *socket_path)
{
	struct ezcfg_worker *worker = NULL;
	int stacksize = sizeof(struct ezcfg_worker) * 2;

	worker = ezcfg_worker_new_from_socket(ezcfg, socket_path);
	if (worker == NULL)
		return NULL;
	/* Start worker thread */
	ezcfg_thread_start(ezcfg, stacksize, (ezcfg_thread_func_t) ezcfg_worker_thread, worker);
	return worker;
}
