/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : common/shm.c
 *
 * Description  : wrap system V IPC shared memory operations
 * Warning      : must exclusively use it, say lock EZCFG_SEM_SHM_INDEX before using it.
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2012-03-16   0.1       Write it from scratch
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

struct ezcfg_shm {
	int ezcfg_sem_id;
	int ezcfg_shm_id;
	size_t ezcg_shm_size;
#if (HAVE_EZBOX_SERVICE_EZCTP == 1)
	/* ezctp shared memory */
	int ezctp_shm_id;
	size_t ezctp_shm_size;
	/* ezctp circular queue */
	size_t ezctp_cq_unit_size; /* the size per unit in the queue */
	size_t ezctp_cq_length; /* the length of queue */
	size_t ezctp_cq_free; /* the free units of queue */
	int ezctp_cq_head;
	int ezctp_cq_tail;
#endif
};

/*
 * Private functions
 */

/*
 * Public functions
 */
int ezcfg_shm_get_ezcfg_sem_id(const struct ezcfg_shm *shm)
{
	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);

	return shm->ezcfg_sem_id;
}

#if (HAVE_EZBOX_SERVICE_EZCTP == 1)
bool ezcfg_shm_insert_ezctp_market_data(struct ezcfg_shm *shm, const void *data, size_t n, size_t size)
{
	bool insert_flag = false;
	size_t i;
	char *src, *dst;
	void *ezctp_shm_addr;

	ASSERT(shm != NULL);
	ASSERT(shm != (void *)-1);
	ASSERT(data != NULL);
	ASSERT(n > 0);
	ASSERT(size > 0);

	if ((shm->ezctp_cq_free >= n) && (shm->ezctp_shm_id != -1) &&
	    ((ezctp_shm_addr = shmat(shm->ezctp_shm_id, NULL, 0)) != (void *)-1)) {
		/* there's enough free units in CQ */
		for (i=0; i<n; i++) {
			src = (char *)data + (i * size);
			dst = (char *)ezctp_shm_addr + (shm->ezctp_cq_tail * shm->ezctp_cq_unit_size);
			/* copy the data to CQ */
			memcpy(dst, src, size);
			shm->ezctp_cq_tail = (shm->ezctp_cq_tail + 1) % shm->ezctp_cq_length;
		}
		shmdt(ezctp_shm_addr);
		shm->ezctp_cq_free -= n;
		insert_flag = true;
	}

	return insert_flag;
}
#endif
