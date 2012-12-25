/* ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : api-common.c
 *
 * Description  : ezcfg API for ezcfg common manipulate
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2012-03-04   0.1       Write it from scratch
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
#include <sys/shm.h>
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

#if 1
#define DBG(format, args...) do {\
	FILE *dbg_fp = fopen("/tmp/libezcfg-api-common.log", "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

static bool initialized = false;
static char config_file[EZCFG_PATH_MAX] = EZCFG_CONFIG_FILE_PATH;

/**
 * ezcfg_api_common_initialized:
 *
 **/
bool ezcfg_api_common_initialized(void)
{
	return initialized;
}

/**
 * ezcfg_api_common_get_config_file:
 *
 **/
char *ezcfg_api_common_get_config_file(void)
{
	return config_file;
}

/**
 * ezcfg_api_common_set_config_file:
 *
 **/
int ezcfg_api_common_set_config_file(const char *path)
{
	int rc = 0;
	size_t len;
	if (path == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	len = strlen(path);
	if (len >= sizeof(config_file)) {
		return -EZCFG_E_ARGUMENT ;
	}

	rc = snprintf(config_file, sizeof(config_file), "%s", path);
	if (rc < 0) {
		rc = -EZCFG_E_SPACE ;
	}
	initialized = true;
	return rc;
}

/**
 * ezcfg_api_common_new:
 *
 **/
struct ezcfg *ezcfg_api_common_new(char *path)
{
	if (path == NULL) {
		return NULL;
	}
	return ezcfg_new(path);
}

/**
 * ezcfg_api_common_delete:
 *
 **/
void ezcfg_api_common_delete(struct ezcfg *ezcfg)
{
	ezcfg_delete(ezcfg);
}

/**
 * ezcfg_api_common_get_root_path:
 *
 **/
char *ezcfg_api_common_get_root_path(struct ezcfg *ezcfg)
{
	if (ezcfg == NULL) {
		return NULL;
	}
	return ezcfg_common_get_root_path(ezcfg);
}

/**
 * ezcfg_api_common_get_sem_ezcfg_path:
 *
 **/
char *ezcfg_api_common_get_sem_ezcfg_path(struct ezcfg *ezcfg)
{
	if (ezcfg == NULL) {
		return NULL;
	}
	return ezcfg_common_get_sem_ezcfg_path(ezcfg);
}

/**
 * ezcfg_api_common_get_shm_ezcfg_path:
 *
 **/
char *ezcfg_api_common_get_shm_ezcfg_path(struct ezcfg *ezcfg)
{
	if (ezcfg == NULL) {
		return NULL;
	}
	return ezcfg_common_get_shm_ezcfg_path(ezcfg);
}

/**
 * ezcfg_api_ezcfg_get_shm_ezcfg_size:
 *
 **/
int ezcfg_api_common_get_shm_ezcfg_size(struct ezcfg *ezcfg, size_t *psize)
{
	int rc = 0;
	if ((ezcfg == NULL) || (psize == NULL)) {
		return -EZCFG_E_ARGUMENT ;
	}
	*psize = ezcfg_common_get_shm_ezcfg_size(ezcfg);
	return rc;
}

/**
 * ezcfg_api_common_increase_shm_ezcfg_nvram_queue_num:
 *
 **/
int ezcfg_api_common_increase_shm_ezcfg_nvram_queue_num(struct ezcfg *ezcfg)
{
	int rc = 0;
	int key, sem_id, shm_id;
	struct sembuf res;
	size_t shm_size;
	void *shm_addr = (void *)-1;
	char *path;
	size_t num;

	if (ezcfg == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	/* prepare for semaphore parameters */
	path = ezcfg_common_get_sem_ezcfg_path(ezcfg);
	if (path == NULL) {
		return -EZCFG_E_RESULT ;
	}

	key = ftok(path, EZCFG_SEM_PROJID_EZCFG);
	if (key == -1) {
		DBG("%s(%d): pid=[%d] ftok error.\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* create a semaphore set that only includes one semaphore */
	/* shm queue semaphore has been initialized in ezcd */
	sem_id = semget(key, EZCFG_SEM_NUMBER, 00666);
	if (sem_id < 0) {
		DBG("%s(%d): pid=[%d] semget error\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* prepare for shared memory parameters */
	path = ezcfg_common_get_shm_ezcfg_path(ezcfg);
	if (path == NULL) {
		return -EZCFG_E_RESOURCE ;
	}

	key = ftok(path, EZCFG_SHM_PROJID_EZCFG);
	if (key == -1) {
		DBG("%s(%d): pid=[%d] ftok error.\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	shm_size = ezcfg_common_get_shm_ezcfg_size(ezcfg);
	if (shm_size < ezcfg_shm_get_size()) {
		DBG("%s(%d): pid=[%d] shm_size is too small.\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* get shared memory */
	shm_id = shmget(key, shm_size, 00666);
	if (shm_id < 0) {
		DBG("%s(%d): pid=[%d] try to get shm error.\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* now require available resource */
	res.sem_num = EZCFG_SEM_SHM_QUEUE_INDEX;
	res.sem_op = -1;
	res.sem_flg = 0;

	if (semop(sem_id, &res, 1) == -1) {
		DBG("%s(%d): pid=[%d] semop require res error\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* attach share memeory */
	shm_addr = shmat(shm_id, NULL, 0);
	if ((void *) -1 == shm_addr) {
		DBG("%s(%d): pid=[%d] shmat error with errno=[%d]\n", __func__, __LINE__, getpid(), errno);
		rc = -EZCFG_E_RESOURCE ;
		goto func_out;
	}

	num = ezcfg_shm_get_ezcfg_nvram_queue_num(shm_addr);
	if (num < ezcfg_shm_get_ezcfg_nvram_queue_length(shm_addr)) {
		ezcfg_shm_set_ezcfg_nvram_queue_num(shm_addr, num+1);
	}
	else {
		/* queue is full */
		DBG("%s(%d): pid=[%d] queue is full\n", __func__, __LINE__, getpid());
		rc = -EZCFG_E_RESOURCE ;
		goto func_out;
	}

func_out:
	/* dettach share memeory */
	if ((void *) -1 != shm_addr) {
		if (shmdt(shm_addr) == -1) {
			DBG("%s(%d): pid=[%d] shmdt error\n", __func__, __LINE__, getpid());
			rc = -EZCFG_E_RESOURCE ;
		}
	}

	/* now release available resource */
	res.sem_num = EZCFG_SEM_SHM_QUEUE_INDEX;
	res.sem_op = 1;
	res.sem_flg = 0;

	if (semop(sem_id, &res, 1) == -1) {
		DBG("%s(%d): pid=[%d] semop release res error\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	return rc;
}

/**
 * ezcfg_api_common_decrease_shm_ezcfg_nvram_queue_num:
 *
 **/
int ezcfg_api_common_decrease_shm_ezcfg_nvram_queue_num(struct ezcfg *ezcfg)
{
	int rc = 0;
	int key, sem_id, shm_id;
	struct sembuf res;
	size_t shm_size;
	void *shm_addr = (void *)-1;
	char *path;
	size_t num;

	if (ezcfg == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	/* prepare for semaphore parameters */
	path = ezcfg_common_get_sem_ezcfg_path(ezcfg);
	if (path == NULL) {
		return -EZCFG_E_RESULT ;
	}

	key = ftok(path, EZCFG_SEM_PROJID_EZCFG);
	if (key == -1) {
		DBG("%s(%d): pid=[%d] ftok error.\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* create a semaphore set that only includes one semaphore */
	/* shm queue semaphore has been initialized in ezcd */
	sem_id = semget(key, EZCFG_SEM_NUMBER, 00666);
	if (sem_id < 0) {
		DBG("%s(%d): pid=[%d] semget error\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* prepare for shared memory parameters */
	path = ezcfg_common_get_shm_ezcfg_path(ezcfg);
	if (path == NULL) {
		return -EZCFG_E_RESOURCE ;
	}

	key = ftok(path, EZCFG_SHM_PROJID_EZCFG);
	if (key == -1) {
		DBG("%s(%d): pid=[%d] ftok error.\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	shm_size = ezcfg_common_get_shm_ezcfg_size(ezcfg);
	if (shm_size < ezcfg_shm_get_size()) {
		DBG("%s(%d): pid=[%d] shm_size is too small.\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* get shared memory */
	shm_id = shmget(key, shm_size, 00666);
	if (shm_id < 0) {
		DBG("%s(%d): pid=[%d] try to get shm error.\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* now require available resource */
	res.sem_num = EZCFG_SEM_SHM_QUEUE_INDEX;
	res.sem_op = -1;
	res.sem_flg = 0;

	if (semop(sem_id, &res, 1) == -1) {
		DBG("%s(%d): pid=[%d] semop require res error\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* attach share memeory */
	shm_addr = shmat(shm_id, NULL, 0);
	if ((void *) -1 == shm_addr) {
		DBG("%s(%d): pid=[%d] shmat error with errno=[%d]\n", __func__, __LINE__, getpid(), errno);
		rc = -EZCFG_E_RESOURCE ;
		goto func_out;
	}

	num = ezcfg_shm_get_ezcfg_nvram_queue_num(shm_addr);
	DBG("%s(%d): pid=[%d] num=[%lu]\n", __func__, __LINE__, getpid(), num);
	if (num > 0) {
		ezcfg_shm_set_ezcfg_nvram_queue_num(shm_addr, num-1);
	}
	else {
		/* queue is empty */
		DBG("%s(%d): pid=[%d] queue is empty\n", __func__, __LINE__, getpid());
		rc = -EZCFG_E_RESOURCE ;
		goto func_out;
	}

func_out:
	/* dettach share memeory */
	if ((void *) -1 != shm_addr) {
		if (shmdt(shm_addr) == -1) {
			DBG("%s(%d): pid=[%d] shmdt error\n", __func__, __LINE__, getpid());
			rc = -EZCFG_E_RESOURCE ;
		}
	}

	/* now release available resource */
	res.sem_num = EZCFG_SEM_SHM_QUEUE_INDEX;
	res.sem_op = 1;
	res.sem_flg = 0;

	if (semop(sem_id, &res, 1) == -1) {
		DBG("%s(%d): pid=[%d] semop release res error\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	return rc;
}

/**
 * ezcfg_api_common_increase_shm_ezcfg_rc_queue_num:
 *
 **/
int ezcfg_api_common_increase_shm_ezcfg_rc_queue_num(struct ezcfg *ezcfg)
{
	int rc = 0;
	int key, sem_id, shm_id;
	struct sembuf res;
	size_t shm_size;
	void *shm_addr = (void *)-1;
	char *path;
	size_t num;

	if (ezcfg == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	/* prepare for semaphore parameters */
	path = ezcfg_common_get_sem_ezcfg_path(ezcfg);
	if (path == NULL) {
		DBG("%s(%d): pid=[%d] ezcfg_common_get_sem_ezcfg_path() error.\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESULT ;
	}

	key = ftok(path, EZCFG_SEM_PROJID_EZCFG);
	DBG("%s(%d): pid=[%d] key=[%d] path=[%s].\n", __func__, __LINE__, getpid(), key, path);
	if (key == -1) {
		DBG("%s(%d): pid=[%d] ftok error.\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* create a semaphore set that only includes one semaphore */
	/* shm queue semaphore has been initialized in ezcd */
	sem_id = semget(key, EZCFG_SEM_NUMBER, 00666);
	if (sem_id < 0) {
		DBG("%s(%d): pid=[%d] semget error\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* prepare for shared memory parameters */
	path = ezcfg_common_get_shm_ezcfg_path(ezcfg);
	if (path == NULL) {
		DBG("%s(%d): pid=[%d] ezcfg_common_get_shm_ezcfg_path() error.\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	key = ftok(path, EZCFG_SHM_PROJID_EZCFG);
	DBG("%s(%d): pid=[%d] key=[%d] path=[%s].\n", __func__, __LINE__, getpid(), key, path);
	if (key == -1) {
		DBG("%s(%d): pid=[%d] ftok error.\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	shm_size = ezcfg_common_get_shm_ezcfg_size(ezcfg);
	if (shm_size < ezcfg_shm_get_size()) {
		DBG("%s(%d): pid=[%d] shm_size is too small.\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* get shared memory */
	shm_id = shmget(key, shm_size, 00666);
	if (shm_id < 0) {
		DBG("%s(%d): pid=[%d] try to get shm error.\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* now require available resource */
	res.sem_num = EZCFG_SEM_SHM_QUEUE_INDEX;
	res.sem_op = -1;
	res.sem_flg = 0;

	if (semop(sem_id, &res, 1) == -1) {
		DBG("%s(%d): pid=[%d] semop require res error\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* attach share memeory */
	shm_addr = shmat(shm_id, NULL, 0);
	if ((void *) -1 == shm_addr) {
		DBG("%s(%d): pid=[%d] shmat error with errno=[%d]\n", __func__, __LINE__, getpid(), errno);
		rc = -EZCFG_E_RESOURCE ;
		goto func_out;
	}

	num = ezcfg_shm_get_ezcfg_rc_queue_num(shm_addr);
	DBG("%s(%d): pid=[%d] num=[%lu]\n", __func__, __LINE__, getpid(), num);
	DBG("%s(%d): pid=[%d] rc_queue_length=[%lu]\n", __func__, __LINE__, getpid(), ezcfg_shm_get_ezcfg_rc_queue_length(shm_addr));
	if (num < ezcfg_shm_get_ezcfg_rc_queue_length(shm_addr)) {
		ezcfg_shm_set_ezcfg_rc_queue_num(shm_addr, num+1);
	}
	else {
		/* queue is full */
		DBG("%s(%d): pid=[%d] queue is full\n", __func__, __LINE__, getpid());
		rc = -EZCFG_E_RESOURCE ;
		goto func_out;
	}

func_out:
	/* dettach share memeory */
	if ((void *) -1 != shm_addr) {
		if (shmdt(shm_addr) == -1) {
			DBG("%s(%d): pid=[%d] shmdt error\n", __func__, __LINE__, getpid());
			rc = -EZCFG_E_RESOURCE ;
		}
	}

	/* now release available resource */
	res.sem_num = EZCFG_SEM_SHM_QUEUE_INDEX;
	res.sem_op = 1;
	res.sem_flg = 0;

	if (semop(sem_id, &res, 1) == -1) {
		DBG("%s(%d): pid=[%d] semop release res error\n", __func__, __LINE__, getpid());
		rc = -EZCFG_E_RESOURCE ;
	}

	DBG("%s(%d): pid=[%d] rc=[%d]\n", __func__, __LINE__, getpid(), rc);
	return rc;
}

/**
 * ezcfg_api_common_decrease_shm_ezcfg_rc_queue_num:
 *
 **/
int ezcfg_api_common_decrease_shm_ezcfg_rc_queue_num(struct ezcfg *ezcfg)
{
	int rc = 0;
	int key, sem_id, shm_id;
	struct sembuf res;
	size_t shm_size;
	void *shm_addr = (void *)-1;
	char *path;
	size_t num;

	if (ezcfg == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	/* prepare for semaphore parameters */
	path = ezcfg_common_get_sem_ezcfg_path(ezcfg);
	if (path == NULL) {
		DBG("%s(%d): pid=[%d] ezcfg_common_get_sem_ezcfg_path() error.\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESULT ;
	}

	key = ftok(path, EZCFG_SEM_PROJID_EZCFG);
	DBG("%s(%d): pid=[%d] key=[%d] path=[%s].\n", __func__, __LINE__, getpid(), key, path);
	if (key == -1) {
		DBG("%s(%d): pid=[%d] ftok error.\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* create a semaphore set that only includes one semaphore */
	/* shm queue semaphore has been initialized in ezcd */
	sem_id = semget(key, EZCFG_SEM_NUMBER, 00666);
	if (sem_id < 0) {
		DBG("%s(%d): pid=[%d] semget error\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* prepare for shared memory parameters */
	path = ezcfg_common_get_shm_ezcfg_path(ezcfg);
	if (path == NULL) {
		DBG("%s(%d): pid=[%d] ezcfg_common_get_shm_ezcfg_path() error.\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	key = ftok(path, EZCFG_SHM_PROJID_EZCFG);
	DBG("%s(%d): pid=[%d] key=[%d] path=[%s].\n", __func__, __LINE__, getpid(), key, path);
	if (key == -1) {
		DBG("%s(%d): pid=[%d] ftok error.\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	shm_size = ezcfg_common_get_shm_ezcfg_size(ezcfg);
	if (shm_size < ezcfg_shm_get_size()) {
		DBG("%s(%d): pid=[%d] shm_size is too small.\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* get shared memory */
	shm_id = shmget(key, shm_size, 00666);
	if (shm_id < 0) {
		DBG("%s(%d): pid=[%d] try to get shm error.\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* now require available resource */
	res.sem_num = EZCFG_SEM_SHM_QUEUE_INDEX;
	res.sem_op = -1;
	res.sem_flg = 0;

	if (semop(sem_id, &res, 1) == -1) {
		DBG("%s(%d): pid=[%d] semop require res error\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	/* attach share memeory */
	shm_addr = shmat(shm_id, NULL, 0);
	if ((void *) -1 == shm_addr) {
		DBG("%s(%d): pid=[%d] shmat error with errno=[%d]\n", __func__, __LINE__, getpid(), errno);
		rc = -EZCFG_E_RESOURCE ;
		goto func_out;
	}

	num = ezcfg_shm_get_ezcfg_rc_queue_num(shm_addr);
	if (num > 0) {
		ezcfg_shm_set_ezcfg_rc_queue_num(shm_addr, num-1);
	}
	else {
		/* queue is empty */
		DBG("%s(%d): pid=[%d] queue is empty\n", __func__, __LINE__, getpid());
		rc = -EZCFG_E_RESOURCE ;
		goto func_out;
	}

func_out:
	/* dettach share memeory */
	if ((void *) -1 != shm_addr) {
		if (shmdt(shm_addr) == -1) {
			DBG("%s(%d): pid=[%d] shmdt error\n", __func__, __LINE__, getpid());
			rc = -EZCFG_E_RESOURCE ;
		}
	}

	/* now release available resource */
	res.sem_num = EZCFG_SEM_SHM_QUEUE_INDEX;
	res.sem_op = 1;
	res.sem_flg = 0;

	if (semop(sem_id, &res, 1) == -1) {
		DBG("%s(%d): pid=[%d] semop release res error\n", __func__, __LINE__, getpid());
		return -EZCFG_E_RESOURCE ;
	}

	return rc;
}

#if (HAVE_EZBOX_SERVICE_EZCTP == 1)
/**
 * ezcfg_api_ezcfg_get_shm_ezctp_path:
 *
 **/
char *ezcfg_api_common_get_shm_ezctp_path(struct ezcfg *ezcfg)
{
	if (ezcfg == NULL) {
		return NULL;
	}
	return ezcfg_common_get_shm_ezctp_path(ezcfg);
}

/**
 * ezcfg_api_ezcfg_get_shm_ezctp_size:
 *
 **/
int ezcfg_api_common_get_shm_ezctp_size(struct ezcfg *ezcfg, size_t *psize)
{
	int rc = 0;
	if ((ezcfg == NULL) || (psize == NULL)) {
		return -EZCFG_E_ARGUMENT ;
	}
	*psize = ezcfg_common_get_shm_ezctp_size(ezcfg);
	return rc;
}
#endif
