/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/master.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
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
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

#if 1
#define DBG(format, args...) do { \
	char path[256]; \
	FILE *fp; \
	snprintf(path, 256, "/tmp/%d-debug.txt", getpid()); \
	fp = fopen(path, "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

/*
 * ezcfg_master:
 *
 * Opaque object handling one event source.
 * Multi-threads model - master part.
 */
struct ezcfg_master {
	pthread_t thread_id;
	sigset_t *sigset;
	int semid; /* for semaphore control */
	struct ezcfg *ezcfg;
	int stop_flag; /* Should we stop event loop */
	int threads_max; /* MAX number of threads */
	int num_threads; /* Number of threads */
	int num_idle; /* Number of idle threads */

	pthread_mutex_t thread_mutex; /* Protects (max|num)_threads */
	pthread_rwlock_t thread_rwlock; /* Protects options, callbacks */
	pthread_cond_t thread_sync_cond; /* Condvar for thread sync */

	struct ezcfg_socket *listening_sockets;
	pthread_mutex_t ls_mutex; /* Protects listening_sockets */

	struct ezcfg_auth *auths;
	pthread_mutex_t auth_mutex; /* Protects auths */

	struct ezcfg_socket *queue; /* Accepted sockets */
	int sq_len; /* Length of the socket queue */
	int sq_head; /* Head of the socket queue */
	int sq_tail; /* Tail of the socket queue */
	pthread_cond_t sq_empty_cond; /* Socket queue empty condvar */
	pthread_cond_t sq_full_cond;  /* Socket queue full condvar */

	struct ezcfg_nvram *nvram; /* Non-volatile memory */

#if (HAVE_EZBOX_SERVICE_EZCFG_IGRSD == 1)
	struct ezcfg_igrs *igrs; /* IGRS global state */
	pthread_mutex_t igrs_mutex; /* Protects igrs */
#endif

#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD == 1)
	struct ezcfg_upnp *upnp; /* UPnP global state */
	pthread_mutex_t upnp_mutex; /* Protects upnp */
#endif

	struct ezcfg_worker *workers; /* Worker list */
};

static void master_delete(struct ezcfg_master *master)
{
	if (master == NULL)
		return;
	if (master->semid >= 0) {
		/* remove semaphore */
		if (semctl(master->semid, 0, IPC_RMID) == -1) {
			DBG("<6>pid=[%d] semctl IPC_RMID error\n", getpid());
		}
		else {
			DBG("<6>pid=[%d] remove sem OK.\n", getpid());
		}
	}
#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD == 1)
	if (master->upnp) {
		ezcfg_upnp_delete(master->upnp);
	}
#endif
	if (master->nvram) {
		ezcfg_nvram_delete(master->nvram);
	}
	if (master->queue) {
		free(master->queue);
	}
	ezcfg_socket_list_delete(&(master->listening_sockets));
	ezcfg_auth_list_delete(&(master->auths));
	free(master);
}

/**
 * master_new:
 *
 * Create ezcfg master.
 *
 * Returns: a new ezcfg master
 **/
static struct ezcfg_master *master_new(struct ezcfg *ezcfg)
{
	struct ezcfg_master *master;
	int key;
	struct sembuf res[EZCFG_SEM_NUMBER];

	ASSERT(ezcfg != NULL);

	master = calloc(1, sizeof(struct ezcfg_master));
	if (master == NULL) {
		err(ezcfg, "calloc ezcfg_master fail: %m\n");
		return NULL;
	}
	/* initialize ezcfg library context */
	memset(master, 0, sizeof(struct ezcfg_master));

	/* prepare semaphore */
	key = ftok(EZCFG_SEM_EZCFG_PATH, EZCFG_SEM_PROJID_EZCFG);
	if (key == -1) {
		DBG("<6>pid=[%d] ftok error.\n", getpid());
		goto fail_exit;
	}

	/* create a semaphore set */
	/* this is the first place to create the semaphore, must IPC_EXCL */
	master->semid = semget(key, EZCFG_SEM_NUMBER, IPC_CREAT|IPC_EXCL|00666);
	if (master->semid < 0) {
		DBG("<6>pid=[%d] %s(%d) try to create sem error.\n", getpid(), __func__, __LINE__);
		goto fail_exit;
	}

	/* initialize semaphore */
	res[EZCFG_SEM_CTRL_INDEX].sem_num = EZCFG_SEM_CTRL_INDEX;
	res[EZCFG_SEM_CTRL_INDEX].sem_op = 1;
	res[EZCFG_SEM_CTRL_INDEX].sem_flg = 0;

	res[EZCFG_SEM_NVRAM_INDEX].sem_num = EZCFG_SEM_NVRAM_INDEX;
	res[EZCFG_SEM_NVRAM_INDEX].sem_op = 1;
	res[EZCFG_SEM_NVRAM_INDEX].sem_flg = 0;

	res[EZCFG_SEM_RC_INDEX].sem_num = EZCFG_SEM_RC_INDEX;
	res[EZCFG_SEM_RC_INDEX].sem_op = 1;
	res[EZCFG_SEM_RC_INDEX].sem_flg = 0;

	if (semop(master->semid, res, EZCFG_SEM_NUMBER) == -1) {
		DBG("<6>pid=[%d] semop release_res error\n", getpid());
		goto fail_exit;
	}

	/* get nvram memory */
	master->nvram = ezcfg_nvram_new(ezcfg);
	if(master->nvram == NULL) {
		err(ezcfg, "calloc nvram fail: %m\n");
		goto fail_exit;
	}

	/* initialize nvram */
	ezcfg_nvram_fill_storage_info(master->nvram, ezcfg_common_get_config_file(ezcfg));
	ezcfg_nvram_initialize(master->nvram);

	/* initialize socket queue */
	master->sq_len = EZCFG_MASTER_SOCKET_QUEUE_LENGTH;
	master->queue = ezcfg_socket_calloc(ezcfg, master->sq_len);
	if(master->queue == NULL) {
		err(ezcfg, "calloc socket queue.");
		goto fail_exit;
	}

	/*
	 * ignore SIGPIPE signal, so if client cancels the request, it
	 * won't kill the whole process.
	 */
	signal(SIGPIPE, SIG_IGN);

	/* initialize thread mutex */
	pthread_mutex_init(&(master->thread_mutex), NULL);
	pthread_rwlock_init(&(master->thread_rwlock), NULL);
	pthread_cond_init(&(master->thread_sync_cond), NULL);
	pthread_mutex_init(&(master->ls_mutex), NULL);
	pthread_mutex_init(&(master->auth_mutex), NULL);
#if (HAVE_EZBOX_SERVICE_EZCFG_IGRSD == 1)
	pthread_mutex_init(&(master->igrs_mutex), NULL);
#endif
#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD == 1)
	pthread_mutex_init(&(master->upnp_mutex), NULL);
#endif
	pthread_cond_init(&(master->sq_empty_cond), NULL);
	pthread_cond_init(&(master->sq_full_cond), NULL);

	/* set ezcfg library context */
	master->ezcfg = ezcfg;
	return master;

fail_exit:
	master_delete(master);
	return NULL;
}

/**
 * master_add_socket:
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
 * caller must lock the ls_mutex
 *
 * Returns: socket, or NULL, in case of an error
 **/
static struct ezcfg_socket *master_add_socket(struct ezcfg_master *master, int family, int type, int proto, const char *socket_path)
{
	struct ezcfg_socket *listener;
	struct ezcfg *ezcfg;

	ASSERT(master != NULL);
	ezcfg = master->ezcfg;

	ASSERT(socket_path != NULL);

	/* initialize unix domain socket */
	listener = ezcfg_socket_new(ezcfg, family, type, proto, socket_path);
	if (listener == NULL) {
		err(ezcfg, "init socket fail: %m\n");
		return NULL;
	}

	if ((family == AF_LOCAL) &&
	    (socket_path[0] != '@')) {
		ezcfg_socket_set_need_unlink(listener, true);
	}

	if (ezcfg_socket_list_insert(&(master->listening_sockets), listener) < 0) {
		err(ezcfg, "insert listener socket fail: %m\n");
		ezcfg_socket_delete(listener);
		listener = NULL;
	}

	return listener;
}

/**
 * master_new_from_socket:
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
static struct ezcfg_master *master_new_from_socket(struct ezcfg *ezcfg, const char *socket_path)
{
	struct ezcfg_master *master = NULL;
	struct ezcfg_socket *sp = NULL;

	ASSERT(ezcfg != NULL);
	ASSERT(socket_path != NULL);

	master = master_new(ezcfg);
	if (master == NULL) {
		err(ezcfg, "new master fail: %m\n");
		return NULL;
	}

	sp = master_add_socket(master, AF_LOCAL, SOCK_STREAM, EZCFG_PROTO_CTRL, socket_path);
	if (sp == NULL) {
		err(ezcfg, "add socket [%s] fail: %m\n", socket_path);
		goto fail_exit;
	}

	if (ezcfg_socket_enable_receiving(sp) < 0) {
		err(ezcfg, "enable socket [%s] receiving fail: %m\n", socket_path);
		ezcfg_socket_list_delete_socket(&(master->listening_sockets), sp);
		goto fail_exit;
	}

	if (ezcfg_socket_enable_listening(sp, master->sq_len) < 0) {
		err(ezcfg, "enable socket [%s] listening fail: %m\n", socket_path);
		ezcfg_socket_list_delete_socket(&(master->listening_sockets), sp);
		goto fail_exit;
	}

	ezcfg_socket_set_close_on_exec(sp);

	return master;

fail_exit:
	/* don't delete sp, ezcfg_master_delete will do it! */
	master_delete(master);
	return NULL;
}

/*
 * Deallocate ezcfg master context, free up the resources
 */
static void ezcfg_master_finish(struct ezcfg_master *master)
{
	struct ezcfg_worker *worker;

	pthread_mutex_lock(&(master->thread_mutex));

	/* Close all listening sockets */
	pthread_mutex_lock(&(master->ls_mutex));
	ezcfg_socket_list_delete(&(master->listening_sockets));
	master->listening_sockets = NULL;
	pthread_mutex_unlock(&(master->ls_mutex));

	/* Close all auths */
	pthread_mutex_lock(&(master->auth_mutex));
	ezcfg_auth_list_delete(&(master->auths));
	master->auths = NULL;
	pthread_mutex_unlock(&(master->auth_mutex));

	/* Close all workers' socket */
	worker = master->workers;
	while (worker != NULL) {
		ezcfg_worker_close_connection(worker);
		worker = ezcfg_worker_get_next(worker);
	}

	/* Wait until all threads finish */
	while (master->num_threads > 0)
		pthread_cond_wait(&(master->thread_sync_cond), &(master->thread_mutex));
	master->threads_max = 0;

	pthread_mutex_unlock(&(master->thread_mutex));

	pthread_cond_destroy(&(master->sq_empty_cond));
	pthread_cond_destroy(&(master->sq_full_cond));

	pthread_mutex_destroy(&(master->ls_mutex));
	pthread_mutex_destroy(&(master->auth_mutex));

	pthread_cond_destroy(&(master->thread_sync_cond));
	pthread_rwlock_destroy(&(master->thread_rwlock));
	pthread_mutex_destroy(&(master->thread_mutex));

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

/* Master thread adds accepted socket to a queue */
static void put_socket(struct ezcfg_master *master, const struct ezcfg_socket *sp)
{
	struct ezcfg *ezcfg;
	int stacksize;
	
	ASSERT(master != NULL);
	ASSERT(sp != NULL);

	ezcfg = master->ezcfg;
	stacksize = 0;

	pthread_mutex_lock(&(master->thread_mutex));

	/* If the queue is full, wait */
	while (master->sq_head - master->sq_tail >= master->sq_len) {
		pthread_cond_wait(&(master->sq_full_cond), &(master->thread_mutex));
	}
	ASSERT(master->sq_head - master->sq_tail < master->sq_len);

	/* Copy socket to the queue and increment head */
	ezcfg_socket_queue_set_socket(master->queue, master->sq_head % master->sq_len, sp);
	master->sq_head++;

	/* If there are no idle threads, start one */
	if (master->num_idle == 0 && master->num_threads < master->threads_max) {
		struct ezcfg_worker *worker;

		worker = ezcfg_worker_new(master);
		if (worker != NULL) {
			if (ezcfg_thread_start(ezcfg, stacksize, ezcfg_worker_get_p_thread_id(worker), (ezcfg_thread_func_t) ezcfg_worker_thread, worker) != 0) {
				err(ezcfg, "Cannot start thread: %m\n");
			} else {
				master->num_threads++;
				/* add to worker list */
				ezcfg_worker_set_next(worker, master->workers);
				master->workers = worker;
			}
		} else {
			err(ezcfg, "Cannot prepare worker thread: %m\n");
		}
	}

	pthread_cond_signal(&(master->sq_empty_cond));
	pthread_mutex_unlock(&(master->thread_mutex));
}

static bool accept_new_connection(struct ezcfg_master *master,
                                  struct ezcfg_socket *listener)
{
	struct ezcfg *ezcfg;
	struct ezcfg_socket *accepted;
	bool allowed;

	ASSERT(master != NULL);
	ASSERT(listener != NULL);

	ezcfg = master->ezcfg;

	accepted = ezcfg_socket_new_accepted_socket(listener);
	if (accepted == NULL) {
		err(ezcfg, "new accepted socket fail.\n");
		return false;
	}

	if (ezcfg_socket_get_proto(listener) == EZCFG_PROTO_UEVENT) {
		if (ezcfg_master_handle_uevent_socket(master, listener, accepted) == false) {
			ezcfg_socket_delete(accepted);
			return false;
		}
	}
#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD == 1)
	else if (ezcfg_socket_get_proto(listener) == EZCFG_PROTO_UPNP_SSDP) {
		if (ezcfg_master_handle_upnp_ssdp_socket(master, listener, accepted) == false) {
			ezcfg_socket_delete(accepted);
			return false;
		}
	}
#endif

	allowed = true;

	if (allowed == true) {
		put_socket(master, accepted);
		/*FIXME: don't ezcfg_socket_delete(), it has been copy to queue */
		free(accepted);
	}
	else {
		ezcfg_socket_delete(accepted);
	}

	return true;
}

pthread_t *ezcfg_master_get_p_thread_id(struct ezcfg_master *master)
{
	return &(master->thread_id);
}

void ezcfg_master_set_sigset(struct ezcfg_master *master, sigset_t *sigset)
{
	master->sigset = sigset;
}

void ezcfg_master_thread(struct ezcfg_master *master) 
{
	fd_set read_set;
	struct ezcfg *ezcfg;
	struct ezcfg_socket *sp;
	struct timeval tv;
	int max_fd;
	int retval;

	ASSERT(master != NULL);

	ezcfg = master->ezcfg;

	while (master->stop_flag == 0) {
		FD_ZERO(&read_set);
		max_fd = -1;

		/* Add listening sockets to the read set */
		/* lock mutex before handling listening_sockets */
		pthread_mutex_lock(&(master->ls_mutex));

		for (sp = master->listening_sockets; sp != NULL; sp = ezcfg_socket_list_next(&sp)) {
			add_to_set(ezcfg_socket_get_sock(sp), &read_set, &max_fd);
		}

		/* unlock mutex after handling listening_sockets */
		pthread_mutex_unlock(&(master->ls_mutex));

		/* wait up to EZCFG_MASTER_WAIT_TIME seconds. */
		tv.tv_sec = EZCFG_MASTER_WAIT_TIME;
		tv.tv_usec = 0;

		retval = select(max_fd + 1, &read_set, NULL, NULL, &tv);
		if (retval == -1) {
			perror("select()");
			err(ezcfg, "select() %m\n");
		}
		else if (retval == 0) {
			/* no data arrived, do nothing */
			do {} while(0);
		}
		else {
			/* lock mutex before handling listening_sockets */
			pthread_mutex_lock(&(master->ls_mutex));

			for (sp = master->listening_sockets;
			     sp != NULL;
			     sp = ezcfg_socket_list_next(&sp)) {
				if (FD_ISSET(ezcfg_socket_get_sock(sp), &read_set)) {
					if (accept_new_connection(master, sp) == false) {
						/* re-enable the socket */
						err(ezcfg, "accept_new_connection() failed\n");

						if (ezcfg_socket_enable_again(sp) < 0) {
							err(ezcfg, "ezcfg_socket_enable_again() failed\n");
							ezcfg_socket_list_delete_socket(&(master->listening_sockets), sp);
						}
					}
				}
			}

			/* unlock mutex after handling listening_sockets */
			pthread_mutex_unlock(&(master->ls_mutex));
		}
	}

	/* Stop signal received: somebody called ezcfg_stop. Quit. */
	ezcfg_master_finish(master);
}

struct ezcfg_master *ezcfg_master_start(struct ezcfg *ezcfg)
{
	struct ezcfg_master *master = NULL;
	int stacksize;
	struct ezcfg_socket * sp;

	ASSERT(ezcfg != NULL);

	/* stacksize = sizeof(struct ezcfg_master) * 2; */
	stacksize = 0;

	/* There must be a ctrl socket */
	master = master_new_from_socket(ezcfg, EZCFG_SOCK_CTRL_PATH);
	if (master == NULL) {
		err(ezcfg, "can not initialize control socket");
		goto start_out;
	}

	/* setup nvram socket */
	/* lock mutex before handling listening_sockets */
	pthread_mutex_lock(&(master->ls_mutex));

	sp = master_add_socket(master, AF_LOCAL, SOCK_STREAM, EZCFG_PROTO_SOAP_HTTP, EZCFG_SOCK_NVRAM_PATH);

	/* unlock mutex after handling listening_sockets */
	pthread_mutex_unlock(&(master->ls_mutex));

	if (sp == NULL) {
		err(ezcfg, "can not add nvram socket");
		goto start_out;
	}

	if (ezcfg_socket_enable_receiving(sp) < 0) {
		err(ezcfg, "enable socket [%s] receiving fail: %m\n", EZCFG_SOCK_NVRAM_PATH);
		ezcfg_socket_list_delete_socket(&(master->listening_sockets), sp);
		goto start_out;
	}

	if (ezcfg_socket_enable_listening(sp, master->sq_len) < 0) {
		err(ezcfg, "enable socket [%s] listening fail: %m\n", EZCFG_SOCK_NVRAM_PATH);
		ezcfg_socket_list_delete_socket(&(master->listening_sockets), sp);
		goto start_out;
	}

	ezcfg_socket_set_close_on_exec(sp);

	/* setup uevent socket */
	/* lock mutex before handling listening_sockets */
	pthread_mutex_lock(&(master->ls_mutex));

	//sp = master_add_socket(master, AF_NETLINK, SOCK_DGRAM, EZCFG_PROTO_UEVENT, EZCFG_SOCK_UEVENT_PATH);
	sp = master_add_socket(master, AF_NETLINK, SOCK_DGRAM, EZCFG_PROTO_UEVENT, "kernel");
	//sp = master_add_socket(master, AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC|SOCK_NONBLOCK, EZCFG_PROTO_UEVENT, "kernel");

	/* unlock mutex after handling listening_sockets */
	pthread_mutex_unlock(&(master->ls_mutex));

	if (sp == NULL) {
		err(ezcfg, "can not add udev socket");
		goto load_other_sockets;
	}

	if (ezcfg_socket_enable_receiving(sp) < 0) {
		err(ezcfg, "enable socket [%s] receiving fail: %m\n", EZCFG_SOCK_UEVENT_PATH);
		ezcfg_socket_list_delete_socket(&(master->listening_sockets), sp);
		goto load_other_sockets;
	}

	ezcfg_socket_set_close_on_exec(sp);

load_other_sockets:

	/* lock mutex before handling auths */
	pthread_mutex_lock(&(master->auth_mutex));
	ezcfg_master_load_auth_conf(master);
	/* unlock mutex after handling auths */
	pthread_mutex_unlock(&(master->auth_mutex));

#if (HAVE_EZBOX_SERVICE_EZCFG_IGRSD ==  1)
	/* lock mutex before handling igrs */
	pthread_mutex_lock(&(master->igrs_mutex));
	ezcfg_master_load_igrs_conf(master);
	/* unlock mutex after handling auths */
	pthread_mutex_unlock(&(master->igrs_mutex));
#endif

#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD ==  1)
	/* lock mutex before handling upnp */
	pthread_mutex_lock(&(master->upnp_mutex));
	ezcfg_master_load_upnp_conf(master);
	/* unlock mutex after handling auths */
	pthread_mutex_unlock(&(master->upnp_mutex));
#endif

	/* lock mutex before handling listening_sockets */
	pthread_mutex_lock(&(master->ls_mutex));
	ezcfg_master_load_socket_conf(master);
	/* unlock mutex after handling listening_sockets */
	pthread_mutex_unlock(&(master->ls_mutex));

start_out:
	/* Start master (listening) thread */
	if (master != NULL) {
		if (ezcfg_thread_start(ezcfg, stacksize, &(master->thread_id), (ezcfg_thread_func_t) ezcfg_master_thread, master) != 0) {
			ASSERT(master->num_threads == 0);
			master_delete(master);
			master = NULL;
		}
	}
	return master;
}

void ezcfg_master_stop(struct ezcfg_master *master)
{
	struct sembuf res[EZCFG_SEM_NUMBER];
	bool sem_required = true;

	if (master == NULL)
		return;

	/* now require available resource */
	res[EZCFG_SEM_CTRL_INDEX].sem_num = EZCFG_SEM_CTRL_INDEX;
	res[EZCFG_SEM_CTRL_INDEX].sem_op = -1;
	res[EZCFG_SEM_CTRL_INDEX].sem_flg = 0;

	res[EZCFG_SEM_NVRAM_INDEX].sem_num = EZCFG_SEM_NVRAM_INDEX;
	res[EZCFG_SEM_NVRAM_INDEX].sem_op = -1;
	res[EZCFG_SEM_NVRAM_INDEX].sem_flg = 0;

	res[EZCFG_SEM_RC_INDEX].sem_num = EZCFG_SEM_RC_INDEX;
	res[EZCFG_SEM_RC_INDEX].sem_op = 0; /* don't lock rc */
	res[EZCFG_SEM_RC_INDEX].sem_flg = 0;

	if (semop(master->semid, res, EZCFG_SEM_NUMBER) == -1) {
		DBG("<6>pid=[%d] semop require_res error\n", getpid());
		sem_required = false;
	}

	master->stop_flag = 1;

	/* Wait until ezcfg_master_finish() stops */
	while (master->stop_flag != 2)
		sleep(1);

	/* now release resource */
	if (sem_required == true) {
		res[EZCFG_SEM_CTRL_INDEX].sem_num = EZCFG_SEM_CTRL_INDEX;
		res[EZCFG_SEM_CTRL_INDEX].sem_op = 1;
		res[EZCFG_SEM_CTRL_INDEX].sem_flg = 0;

		res[EZCFG_SEM_NVRAM_INDEX].sem_num = EZCFG_SEM_NVRAM_INDEX;
		res[EZCFG_SEM_NVRAM_INDEX].sem_op = 1;
		res[EZCFG_SEM_NVRAM_INDEX].sem_flg = 0;

		res[EZCFG_SEM_RC_INDEX].sem_num = EZCFG_SEM_RC_INDEX;
		res[EZCFG_SEM_RC_INDEX].sem_op = 0; /* don't unlock rc */
		res[EZCFG_SEM_RC_INDEX].sem_flg = 0;

		if (semop(master->semid, res, EZCFG_SEM_NUMBER) == -1) {
			DBG("<6>pid=[%d] semop release_res error\n", getpid());
		}
	}

	ASSERT(master->num_threads == 0);
	master_delete(master);
}

void ezcfg_master_reload(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;
	struct sembuf res[EZCFG_SEM_NUMBER];

	if (master == NULL)
		return;

	/* now require available resource */
	res[EZCFG_SEM_CTRL_INDEX].sem_num = EZCFG_SEM_CTRL_INDEX;
	res[EZCFG_SEM_CTRL_INDEX].sem_op = -1;
	res[EZCFG_SEM_CTRL_INDEX].sem_flg = 0;

	res[EZCFG_SEM_NVRAM_INDEX].sem_num = EZCFG_SEM_NVRAM_INDEX;
	res[EZCFG_SEM_NVRAM_INDEX].sem_op = -1;
	res[EZCFG_SEM_NVRAM_INDEX].sem_flg = 0;

	res[EZCFG_SEM_RC_INDEX].sem_num = EZCFG_SEM_RC_INDEX;
	res[EZCFG_SEM_RC_INDEX].sem_op = 0; /* don't lock rc */
	res[EZCFG_SEM_RC_INDEX].sem_flg = 0;

	if (semop(master->semid, res, EZCFG_SEM_NUMBER) == -1) {
		DBG("<6>pid=[%d] semop require_res error\n", getpid());
		return;
	}

	ezcfg = master->ezcfg;

	/* lock thread mutex */
	pthread_mutex_lock(&(master->thread_mutex));

	/* lock listening sockets mutex */
	pthread_mutex_lock(&(master->ls_mutex));

	/* wait all wokers stop first */
	while (master->num_threads > 0)
		pthread_cond_wait(&(master->thread_sync_cond), &(master->thread_mutex));

	/* lock auths mutex */
	pthread_mutex_lock(&(master->auth_mutex));

	/* initialize ezcfg common info */
	ezcfg_master_load_common_conf(master);

	/* initialize nvram */
	ezcfg_nvram_fill_storage_info(master->nvram, ezcfg_common_get_config_file(ezcfg));
	ezcfg_nvram_reload(master->nvram);

	if (master->auths != NULL) {
		ezcfg_auth_list_delete(&(master->auths));
		master->auths = NULL;
	}

	ezcfg_master_load_auth_conf(master);

	/* unlock auths mutex */
	pthread_mutex_unlock(&(master->auth_mutex));

#if (HAVE_EZBOX_SERVICE_EZCFG_IGRSD == 1)
	/* lock igrs mutex */
	pthread_mutex_lock(&(master->igrs_mutex));

	ezcfg_master_load_igrs_conf(master);

	/* unlock upnp mutex */
	pthread_mutex_unlock(&(master->igrs_mutex));
#endif

#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD == 1)
	/* lock upnp mutex */
	pthread_mutex_lock(&(master->upnp_mutex));

	ezcfg_master_load_upnp_conf(master);

	/* unlock upnp mutex */
	pthread_mutex_unlock(&(master->upnp_mutex));
#endif

	/* reload sockets */
	ezcfg_master_load_socket_conf(master);

	/* unlock listening sockets mutex */
	pthread_mutex_unlock(&(master->ls_mutex));

	/* unlock thread mutex */
	pthread_mutex_unlock(&(master->thread_mutex));

	/* now release resource */
	res[EZCFG_SEM_CTRL_INDEX].sem_num = EZCFG_SEM_CTRL_INDEX;
	res[EZCFG_SEM_CTRL_INDEX].sem_op = 1;
	res[EZCFG_SEM_CTRL_INDEX].sem_flg = 0;

	res[EZCFG_SEM_NVRAM_INDEX].sem_num = EZCFG_SEM_NVRAM_INDEX;
	res[EZCFG_SEM_NVRAM_INDEX].sem_op = 1;
	res[EZCFG_SEM_NVRAM_INDEX].sem_flg = 0;

	res[EZCFG_SEM_RC_INDEX].sem_num = EZCFG_SEM_RC_INDEX;
	res[EZCFG_SEM_RC_INDEX].sem_op = 0; /* don't unlock rc */
	res[EZCFG_SEM_RC_INDEX].sem_flg = 0;

	if (semop(master->semid, res, EZCFG_SEM_NUMBER) == -1) {
		DBG("<6>pid=[%d] semop release_res error\n", getpid());
		return;
	}
}

void ezcfg_master_set_threads_max(struct ezcfg_master *master, int threads_max)
{
	if (master == NULL)
		return;
	master->threads_max = threads_max;
}

struct ezcfg *ezcfg_master_get_ezcfg(struct ezcfg_master *master)
{
	return master->ezcfg;
}

bool ezcfg_master_is_stop(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;

	ASSERT(master != NULL);

	ezcfg = master->ezcfg;

	return (master->stop_flag != 0);
}

bool ezcfg_master_get_socket(struct ezcfg_master *master, struct ezcfg_socket *sp, int wait_time)
{
	struct ezcfg *ezcfg;
	struct timespec ts;

	ASSERT(master != NULL);
	ASSERT(wait_time >= 0);

	ezcfg = master->ezcfg;

	pthread_mutex_lock(&(master->thread_mutex));
	/* If the queue is empty, wait. We're idle at this point. */
	master->num_idle++;
	while (master->sq_head == master->sq_tail) {
		ts.tv_nsec = 0;
		ts.tv_sec = time(NULL) + wait_time;
		if (pthread_cond_timedwait(&(master->sq_empty_cond), &(master->thread_mutex), &ts) != 0) {
			/* Timeout! release the mutex and return */
			pthread_mutex_unlock(&(master->thread_mutex));
			return false;
		}
	}
	ASSERT(master->sq_head > master->sq_tail);

	/* We're going busy now: got a socket to process! */
	master->num_idle--;

	/* Copy socket from the queue and increment tail */
	ezcfg_socket_queue_get_socket(master->queue, master->sq_tail % master->sq_len, sp);
	master->sq_tail++;

	/* Wrap pointers if needed */
	while (master->sq_tail >= master->sq_len) {
		master->sq_tail -= master->sq_len;
		master->sq_head -= master->sq_len;
	}
	pthread_cond_signal(&(master->sq_full_cond));
	pthread_mutex_unlock(&(master->thread_mutex));

	return true;
}

void ezcfg_master_stop_worker(struct ezcfg_master *master, struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;
	struct ezcfg_worker *cur, *prev;

	ASSERT(master != NULL);
	ASSERT(worker != NULL);

	ezcfg = master->ezcfg;

	pthread_mutex_lock(&(master->thread_mutex));

	/* remove worker from worker list */
	cur = master->workers;
	if (cur == worker) {
		master->workers = ezcfg_worker_get_next(cur);
	}

	prev = cur;
	cur = ezcfg_worker_get_next(cur);
	while (cur != NULL) {
		if (cur == worker) {
			cur = ezcfg_worker_get_next(worker);
			ezcfg_worker_set_next(prev, cur);
			break;
		}
		prev = cur;
		cur = ezcfg_worker_get_next(cur);
	}

	/* clean worker resource */
	ezcfg_worker_delete(worker);

	master->num_threads--;
	master->num_idle--;
	pthread_cond_signal(&(master->thread_sync_cond));
	ASSERT(master->num_threads >= 0);

	pthread_mutex_unlock(&(master->thread_mutex));
}

struct ezcfg_nvram *ezcfg_master_get_nvram(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;

	ASSERT(master != NULL);

	ezcfg = master->ezcfg;

	return master->nvram;
}

int ezcfg_master_get_sq_len(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;

	ASSERT(master != NULL);

	ezcfg = master->ezcfg;

	return master->sq_len;
}

struct ezcfg_socket *ezcfg_master_get_listening_sockets(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;

	ASSERT(master != NULL);

	ezcfg = master->ezcfg;

	return master->listening_sockets;
}

struct ezcfg_socket **ezcfg_master_get_p_listening_sockets(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;

	ASSERT(master != NULL);

	ezcfg = master->ezcfg;

	return &(master->listening_sockets);
}

struct ezcfg_auth *ezcfg_master_get_auths(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;

	ASSERT(master != NULL);

	ezcfg = master->ezcfg;

	return master->auths;
}

struct ezcfg_auth **ezcfg_master_get_p_auths(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;

	ASSERT(master != NULL);

	ezcfg = master->ezcfg;

	return &(master->auths);
}

int ezcfg_master_auth_mutex_lock(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;

	ASSERT(master != NULL);

	ezcfg = master->ezcfg;

	return pthread_mutex_lock(&(master->auth_mutex));
}

int ezcfg_master_auth_mutex_unlock(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;

	ASSERT(master != NULL);

	ezcfg = master->ezcfg;

	return pthread_mutex_unlock(&(master->auth_mutex));
}
