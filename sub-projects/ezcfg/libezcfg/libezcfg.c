/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg.c
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
 * ezcfg - library context
 *
 * load/save the ezbox config and system environment
 * allows custom logging
 */

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
	struct socket	*next;		/* Linkage                      */
	int		sock;		/* Listening socket             */
	struct usa	lsa;		/* Local socket address         */
	struct usa	rsa;		/* Remote socket address        */
};

/*
 * ezbox config context
 */
struct ezcfg {
	int		stop_flag;	/* Should we stop event loop    */
	char		*nvram;		/* Non-volatile memory          */
	struct socket	*listening_sockets;
	int		threads_max;	/* MAX number of threads        */
	int		num_threads;	/* Number of threads            */
	int		num_idle;	/* Number of idle threads       */

	pthread_mutex_t	mutex;		/* Protects (max|num)_threads   */
	pthread_rwlock_t rwlock;	/* Protects options, callbacks  */
	pthread_cond_t	thr_cond;	/* Condvar for thread sync      */

	struct socket	queue[20];	/* Accepted sockets             */
	int		sq_head;	/* Head of the socket queue     */
	int		sq_tail;	/* Tail of the socket queue     */
	pthread_cond_t	empty_cond;     /* Socket queue empty condvar   */
	pthread_cond_t	full_cond;      /* Socket queue full condvar    */

	void (*log_fn)(struct ezcfg *ezcfg,
                       int priority, const char *file, int line, const char *fn,
                       const char *format, va_list args);
	struct ezcfg_list_node properties_list;
	int		log_priority;
};

void ezcfg_log(struct ezcfg *ezcfg,
               int priority, const char *file, int line, const char *fn,
               const char *format, ...)
{
	va_list args;

	va_start(args, format);
	ezcfg->log_fn(ezcfg, priority, file, line, fn, format, args);
	va_end(args);
}

static void log_stderr(struct ezcfg *ezcfg,
                       int priority, const char *file, int line, const char *fn,
                       const char *format, va_list args)
{
	fprintf(stderr, "libezcfg: %s: ", fn);
	vfprintf(stderr, format, args);
}

/**
 * ezcfg_get_log_priority:
 * @ezcfg: ezcfg library context
 *
 * The initial logging priority is read from the ezcfg config file
 * at startup.
 *
 * Returns: the current logging priority
 **/
int ezcfg_get_log_priority(struct ezcfg *ezcfg)
{
	return ezcfg->log_priority;
}

/**
 * ezcfg_set_log_priority:
 * @ezcfg: ezcfg library context
 * @priority: the new logging priority
 *
 * Set the current logging priority. The value controls which messages
 * are logged.
 **/
void ezcfg_set_log_priority(struct ezcfg *ezcfg, int priority)
{
	char num[32];

	ezcfg->log_priority = priority;
	snprintf(num, sizeof(num), "%u", ezcfg->log_priority);
	ezcfg_add_property(ezcfg, "EZCFG_LOG", num);
}

struct ezcfg_list_entry *ezcfg_add_property(struct ezcfg *ezcfg, const char *key, const char *value)
{
	if (value == NULL) {
		struct ezcfg_list_entry *list_entry;

		list_entry = ezcfg_get_properties_list_entry(ezcfg);
		list_entry = ezcfg_list_entry_get_by_name(list_entry, key);
		if (list_entry != NULL)
			ezcfg_list_entry_delete(list_entry);
		return NULL;
	}
        return ezcfg_list_entry_add(ezcfg, &ezcfg->properties_list, key, value, 1, 0);
}

struct ezcfg_list_entry *ezcfg_get_properties_list_entry(struct ezcfg *ezcfg)
{
	return ezcfg_list_get_entry(&ezcfg->properties_list);
}

