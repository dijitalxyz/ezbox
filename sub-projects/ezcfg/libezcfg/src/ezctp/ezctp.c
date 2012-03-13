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
static bool fill_ezctp_info(struct ezcfg_ezctp *ezctp, const char *conf_path)
{
	char *p;

	if (conf_path == NULL) {
		return false;
	}
	p = ezcfg_util_get_conf_string(conf_path, EZCFG_EZCFG_SECTION_EZCTP, 0, EZCFG_EZCFG_KEYWORD_SHM_SIZE);
	if (p == NULL) {
		return false;
	}
	else {
		ezctp->shm_size = atoi(p);
		free(p);
	}

	return true;
}

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
	key_t key;

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

	if (fill_ezctp_info(ezctp, ezcfg_common_get_config_file(ezcfg)) == false) {
                DBG("<6>pid=[%d] %s(%d) fill ezctp info error.\n", getpid(), __func__, __LINE__);
                goto fail_exit;
	}

	/* prepare shared memory */
	key = ftok(ezcfg_common_get_shm_ezctp_path(ezcfg), EZCFG_SHM_PROJID_EZCTP);
	if (key == -1) {
		DBG("<6>pid=[%d] ftok error.\n", getpid());
		goto fail_exit;
	}

	/* create shared memory */
	/* this is the first place to create the shared memory, must IPC_EXCL */
	ezctp->shm_id = shmget(key, ezctp->shm_size, IPC_CREAT|IPC_EXCL|00666);
	if (ezctp->shm_id < 0) {
		DBG("<6>pid=[%d] %s(%d) try to create sem error.\n", getpid(), __func__, __LINE__);
		goto fail_exit;
	}

	return ezctp;

fail_exit:
	ezcfg_ezctp_delete(ezctp);
	return NULL;
}

