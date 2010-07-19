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
#include <assert.h>
#include <pthread.h>

#include "libezcfg.h"
#include "libezcfg-private.h"

/*
 * ezcfg_worker:
 *
 * Opaque object handling one event source.
 * Multi-threads model - worker part.
 */
struct ezcfg_worker {
	struct ezcfg *ezcfg;
	struct ezcfg_master *master;
	struct ezcfg_socket *client;
	time_t birth_time;
	bool free_post_data;
	int64_t num_bytes_sent;
};

struct ezcfg_worker *ezcfg_worker_new(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;
	struct ezcfg_worker *worker;
	struct ezcfg_socket *client;
	if (master == NULL)
		return NULL;

	worker = calloc(1, sizeof(struct ezcfg_worker));
	if (worker == NULL)
		return NULL;

	ezcfg = ezcfg_master_get_ezcfg(master);
	client = ezcfg_socket_calloc(ezcfg, 1);
	if (client == NULL) {
		free(worker);
		return NULL;
	}

	memset(worker, 0x00, sizeof(struct ezcfg_worker));
	worker->ezcfg = ezcfg;
	worker->client = client;
	return worker;
}

void ezcfg_worker_thread(struct ezcfg_worker *worker) 
{
}
