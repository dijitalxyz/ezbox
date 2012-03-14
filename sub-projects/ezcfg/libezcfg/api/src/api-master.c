/* ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : api-master.c
 *
 * Description  : ezcfg API for ezcfg master thread manipulate
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-02   0.1       Write it from scratch
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
#include "ezcfg-soap_http.h"

#include "ezcfg-api.h"

#if 1
#define DBG(format, args...) do {\
	FILE *dbg_fp = fopen("/dev/kmsg", "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

static bool debug = false;
static bool initialized = false;
static char config_file[EZCFG_PATH_MAX] = EZCFG_CONFIG_FILE_PATH;

static void log_fn(struct ezcfg *ezcfg, int priority,
                   const char *file, int line, const char *fn,
                   const char *format, va_list args)
{
	if (debug) {
		char buf[1024];
		struct timeval tv;
		struct timezone tz;

		vsnprintf(buf, sizeof(buf), format, args);
		gettimeofday(&tv, &tz);
		fprintf(stderr, "%llu.%06u [%u] %s(%d): %s",
		        (unsigned long long) tv.tv_sec, (unsigned int) tv.tv_usec,
		        (int) getpid(), fn, line, buf);
	}
#if 0
	else {
		vsyslog(priority, format, args);
	}
#endif
}

/**
 * ezcfg_api_master_initialized:
 *
 **/
bool ezcfg_api_master_initialized(void)
{
	return initialized;
}

/**
 * ezcfg_api_master_set_config_file:
 *
 **/
int ezcfg_api_master_set_config_file(const char *path)
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
 * ezcfg_api_master_start:
 * @argv: nvram name
 * @value: buffer to store nvram value
 * @len: buffer size
 *
 **/
struct ezcfg_master *ezcfg_api_master_start(const char *name, int threads_max)
{
	struct ezcfg *ezcfg = NULL;
	struct ezcfg_master *master = NULL;

	if (name == NULL || threads_max < EZCFG_THREAD_MIN_NUM) {
		//return -EZCFG_E_ARGUMENT ;
		return NULL;
	}

	ezcfg = ezcfg_new(config_file);
	if (ezcfg == NULL) {
		//return -EZCFG_E_RESOURCE ;
		return NULL;
	}

	ezcfg_log_init(name);
	ezcfg_common_set_log_fn(ezcfg, log_fn);

	master = ezcfg_master_start(ezcfg);
	if (master == NULL) {
		ezcfg_delete(ezcfg);
		return NULL;
	}

	ezcfg_master_set_threads_max(master, threads_max);

	return master;
}

int ezcfg_api_master_stop(struct ezcfg_master *master)
{
	if (master == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	ezcfg_master_stop(master);
	return 0;
}

int ezcfg_api_master_reload(struct ezcfg_master *master)
{
	if (master == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	ezcfg_master_reload(master);
	return 0;
}

#if (HAVE_EZBOX_SERVICE_EZCTP == 1)
int ezcfg_api_master_get_ezctp_shm_id(struct ezcfg_master *master, int *shm_id)
{
	if ((master == NULL) || (shm_id == NULL)) {
		return -EZCFG_E_ARGUMENT ;
	}

	*shm_id = ezcfg_master_get_ezctp_shm_id(master);
	if (-1 == *shm_id) {
		return -EZCFG_E_RESOURCE ;
	}
	return 0;
}

int ezcfg_api_master_get_ezctp_cq_unit_size(struct ezcfg_master *master, size_t *cq_unit_size)
{
	if ((master == NULL) || (cq_unit_size == NULL)) {
		return -EZCFG_E_ARGUMENT ;
	}

	*cq_unit_size = ezcfg_master_get_ezctp_cq_unit_size(master);
	if (*cq_unit_size < 1) {
		return -EZCFG_E_RESOURCE ;
	}
	return 0;
}

int ezcfg_api_master_insert_ezctp_data(struct ezcfg_master *master, void *data, size_t n, size_t size)
{
	if ((master == NULL) || (data == NULL) || (n < 1) || (size < 1)) {
		return -EZCFG_E_ARGUMENT ;
	}

	if (ezcfg_master_insert_ezctp_data(master, data, n, size) == false) {
		return -EZCFG_E_RESOURCE ;
	}
	return 0;
}
#endif
