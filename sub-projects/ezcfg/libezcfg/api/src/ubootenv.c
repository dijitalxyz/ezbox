/* ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : ubootenv.c
 *
 * Description  : ezcfg API for u-boot environment parameters manipulate
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-10-27   0.1       Write it from scratch
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

#include "libezcfg.h"
#include "libezcfg-private.h"

#include "ezcfg-api.h"

/**************************************************************************
 *
 * ubootenv design spec
 *
 * 1. the partition name "u-boot-env" indicate that the partition is a
 * u-boot environment data block
 *
 * 2. the partition name "u-boot-env2" indicate that the partition is a
 * redunant u-boot environment data block
 *
 **************************************************************************
 */

/* from u-boot */
/**************************************************************************
 *
 * Support for persistent environment data
 *
 * The "environment" is stored as a list of '\0' terminated
 * "name=value" strings. The end of the list is marked by a double
 * '\0'. New entries are always added at the end. Deleting an entry
 * shifts the remaining entries to the front. Replacing an entry is a
 * combination of deleting the old value and adding the new one.
 *
 * The environment is preceeded by a 32 bit CRC over the data part.
 *
 **************************************************************************
 */

#if 0
#define DBG printf
#else
#define DBG(format, args...)
#endif

static bool uboot_info_initialized = false;

static unsigned long long ubootenv_size = 0;
static unsigned int ubootenv_erasesize = 0;
static char ubootenv_dev_name[16] = "";

static unsigned long long ubootenv2_size = 0;
static unsigned int ubootenv2_erasesize = 0;
static char ubootenv2_dev_name[16] = "";
static bool ubootenv_redundant = false;

static void init_uboot_mtd_device_info(void)
{
	char line[128];
	int index;
	unsigned long long size;
	int erasesize;
	char name[64];
	FILE *fp = fopen("/proc/mtd", "r");
	if (fp == NULL) {
		return;
	}
	if (fgets(line, sizeof(line), fp) != (char *) NULL) {
		/* read mtd device info */
		for (; fgets(line, sizeof(line), fp);) {
			if (sscanf(line, "mtd%d: %8llx %8x %s\n", &index, &size, &erasesize, name) != 4) {
				continue;
			}
			if (strcmp(name, "\"u-boot-env\"") == 0) {
				sprintf(ubootenv_dev_name, "/dev/mtd%d", index);
				ubootenv_size = size;
				ubootenv_erasesize = erasesize;
				uboot_info_initialized = true;
			}
			if (strcmp(name, "\"u-boot-env2\"") == 0) {
				sprintf(ubootenv2_dev_name, "/dev/mtd%d", index);
				ubootenv2_size = size;
				ubootenv2_erasesize = erasesize;
				ubootenv_redundant = true;
			}
		}
	}
	fclose(fp);
}

/**
 * ezcfg_api_ubootenv_get:
 * @name: u-boot env parameter name
 * @value: buffer to store u-boot env parameter value
 * @len: buffer size
 *
 **/
int ezcfg_api_ubootenv_get(const char *name, char *value, size_t len)
{
	int rc = 0;

	if (uboot_info_initialized == false) {
		init_uboot_mtd_device_info();
	}

	if (ubootenv_redundant == false) {

	} else {

	}
		
	return rc;
}

/**
 * ezcfg_api_ubootenv_set:
 * @name: u-boot env parameter name
 * @value: buffer stored u-boot env parameter value
 *
 **/
int ezcfg_api_ubootenv_set(const char *name, const char *value)
{
	int rc = 0;

	if (uboot_info_initialized == false) {
		init_uboot_mtd_device_info();
	}

	if (ubootenv_redundant == false) {

	} else {

	}
		
	return rc;
}

/**
 * ezcfg_api_ubootenv_list:
 * @list: buffer to store u-boot env parameter pairs
 * @len: buffer size
 *
 **/
int ezcfg_api_ubootenv_list(char *list, size_t len)
{
	int rc = 0;
	FILE *fp = NULL;
	char *buf = NULL;
	char *data = NULL, *end = NULL;
	char *tmp = NULL;
	unsigned long crc = 0;

	if (list == NULL || len < 1) {
		return -EZCFG_E_ARGUMENT ;
	}

	if (uboot_info_initialized == false) {
		init_uboot_mtd_device_info();
	}

	if (ubootenv_redundant == false) {
		fp = fopen(ubootenv_dev_name, "r");
		if (fp == NULL) {
			rc = -EZCFG_E_RESOURCE;
			goto func_out;
		}
		buf = (char *)malloc(ubootenv_size);
		if (buf == NULL) {
			rc = -EZCFG_E_SPACE;
			goto func_out;
		}
		memset(buf, 0, ubootenv_size);
		fread(buf, ubootenv_size, 1, fp);
		crc = *(unsigned long *)(buf);
		data = buf + sizeof(unsigned long);

		/* find \0\0 string */
		end = data+1;
		while (end < buf+ubootenv_size && (*(end-1) != '\0' || *end != '\0')) end++;
		if (end == buf+ubootenv_size) {
			rc = -EZCFG_E_PARSE;
			goto func_out;
		}

		tmp = data;
		while (len > 1 && tmp != end) {
			*list = *tmp == '\0' ? '\n' : *tmp ;
			list++;
			tmp++;
			len--;
		}
		*list = '\0';
	} else {

	}
func_out:
	if (fp != NULL) {
		fclose(fp);
	}
	if (buf != NULL) {
		free(buf);
	}
	return rc;
}

