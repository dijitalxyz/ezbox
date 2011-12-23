/* ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : api-upnp.c
 *
 * Description  : ezcfg API for ezcfg upnp manipulate
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-23   0.1       Write it from scratch
 * ============================================================================
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/mount.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

#include "ezcfg-api.h"

#if 0
#define DBG(format, args...) do {\
	FILE *fp = fopen("/dev/kmsg", "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

static char task_file[EZCFG_PATH_MAX] = EZCFG_UPNP_TASK_FILE_PATH;

/**
 * ezcfg_api_upnp_set_task_file:
 *
 **/
int ezcfg_api_upnp_set_task_file(const char *path)
{
	int rc = 0;
	if (path == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	rc = strlen(path);
	if (rc >= sizeof(task_file)) {
		return -EZCFG_E_ARGUMENT ;
	}

	rc = snprintf(task_file, sizeof(task_file), "%s", path);
	if (rc < 0) {
		rc = -EZCFG_E_SPACE ;
	}
	return rc;
}

/**
 * ezcfg_api_upnp_open_task_file:
 *
 **/
FILE *ezcfg_api_upnp_open_task_file(const char *mode)
{
	int key, semid;
	struct sembuf res;

	key = ftok(EZCFG_SEM_EZCFG_PATH, EZCFG_SEM_PROJID_EZCFG);
	if (key == -1) {
		DBG("<6>upnp:pid=[%d] open task ftok error.\n", getpid());
		return NULL;
	}

	/* create a semaphore set that only includes one semaphore */
	/* rc semaphore has been initialized in ezcd */
	semid = semget(key, EZCFG_SEM_NUMBER, 00666);
	if (semid < 0) {
		DBG("<6>upnp:pid=[%d] open task semget error.\n", getpid());
		return NULL;
	}

	/* now require available resource */
	res.sem_num = EZCFG_SEM_UPNP_INDEX;
	res.sem_op = -1;
	res.sem_flg = 0;

	if (semop(semid, &res, 1) == -1) {
		DBG("<6>upnp: open task semop require res error\n");
		return NULL;
	}

	return fopen(task_file, mode);
}

/**
 * ezcfg_api_upnp_close_task_file:
 *
 **/
bool ezcfg_api_upnp_close_task_file(FILE *fp)
{
	int key, semid;
	struct sembuf res;

	if (fp == NULL) {
		return false;
	}

	key = ftok(EZCFG_SEM_EZCFG_PATH, EZCFG_SEM_PROJID_EZCFG);
	if (key == -1) {
		DBG("<6>upnp:pid=[%d] close ftok error.\n", getpid());
		return false;
	}

	/* create a semaphore set that only includes one semaphore */
	/* rc semaphore has been initialized in ezcd */
	semid = semget(key, EZCFG_SEM_NUMBER, 00666);
	if (semid < 0) {
		DBG("<6>upnp:pid close task semget error\n");
		return false;
	}

	res.sem_num = EZCFG_SEM_UPNP_INDEX;
	res.sem_op = 1;
	res.sem_flg = 0;

	fclose(fp);
	unlink(task_file);

	if (semop(semid, &res, 1) == -1) {
		DBG("<6>upnp: close task semop release res error\n");
		return false;
	}

	return true;
}
