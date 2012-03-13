/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezctp/ezctp.c
 *
 * Description  : implement ezctp shared memory/circular queue
 * Warning      : must exclusively use it, say lock SHM/CQ before using it.
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2012-03-13   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

#if 0
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

struct ezcfg_ezctp {
	struct ezcfg *ezcfg;

	/* Shared Memory */
	int shm_id;
	size_t shm_size;
	void *shm_addr;

	/* Circular Queue */
	int cq_length;
	int cq_head;
	int cq_tail;
	bool cq_full;
	pthread_mutex_t cq_mutex; /* Protects circular queue operations */
};

/*
 * Private functions
 */

/*
 * Public functions
 */

bool ezcfg_ezctp_delete(struct ezcfg_ezctp *ezctp)
{
	struct ezcfg *ezcfg;

	ASSERT(ezctp != NULL);

	ezcfg = ezctp->ezcfg;

	/* lock ezctp access */
	pthread_mutex_lock(&ezctp->cq_mutex);

	if (ezctp->shm_addr != (void *) -1) {
		/* detach system V shared memory from system */
		shmdt(ezctp->shm_addr);
	}

	/* unlock ezctp access */
	pthread_mutex_unlock(&ezctp->cq_mutex);

	pthread_mutex_destroy(&ezctp->cq_mutex);
	free(ezctp);
	return true;
}

struct ezcfg_ezctp *ezcfg_ezctp_new(struct ezcfg *ezcfg)
{
	struct ezcfg_ezctp *ezctp;

	ASSERT(ezcfg != NULL);

	ezctp = malloc(sizeof(struct ezcfg_ezctp));
	if (ezctp == NULL) {
		err(ezcfg, "can not malloc ezctp\n");
		return NULL;
	}

	memset(ezctp, 0, sizeof(struct ezcfg_ezctp));

	ezctp->ezcfg = ezcfg;
	ezctp->shm_addr = (void *) -1;

	/* initialize ezctp/cq_mutex */
	pthread_mutex_init(&ezctp->cq_mutex, NULL);


	return ezctp;
}

