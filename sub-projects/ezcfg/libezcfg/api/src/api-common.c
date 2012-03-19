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
	return ezcfg_common_get_root_path(ezcfg);
}

/**
 * ezcfg_api_ezcfg_get_sem_ezcfg_path:
 *
 **/
char *ezcfg_api_common_get_sem_ezcfg_path(struct ezcfg *ezcfg)
{
	return ezcfg_common_get_sem_ezcfg_path(ezcfg);
}

/**
 * ezcfg_api_ezcfg_get_shm_ezctp_path:
 *
 **/
#if (HAVE_EZBOX_SERVICE_EZCTP == 1)
char *ezcfg_api_common_get_shm_ezctp_path(struct ezcfg *ezcfg)
{
	return ezcfg_common_get_shm_ezctp_path(ezcfg);
}
#endif
