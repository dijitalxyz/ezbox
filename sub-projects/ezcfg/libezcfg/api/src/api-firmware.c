/* ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : api-firmware.c
 *
 * Description  : ezcfg API for firmware manipulate
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-12-03   0.1       Write it from scratch
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
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

#include "ezcfg-api.h"

#if 1
#define DBG printf
#else
#define DBG(format, args...)
#endif

typedef struct firmware_header_s {
	uint32_t crc; /* CRC for firmware */
	char magic[8]; /* code pattern */
} firmware_header_t;

/**
 * get firmware device path
 * must release the return string manually
 */
static char *get_firmware_device_path(void)
{
	char line[128];
	int index;
	unsigned long long size;
	int erasesize;
	char name[64];
	char *target = NULL;
	FILE *fp = fopen("/proc/mtd", "r");
	if (fp == NULL) {
		return NULL;
	}
	if (fgets(line, sizeof(line), fp) != (char *) NULL) {
		/* read mtd device info */
		for (; fgets(line, sizeof(line), fp);) {
			if (sscanf(line, "mtd%d: %8llx %8x %s\n", &index, &size, &erasesize, name) != 4) {
				continue;
			}
			if (strcmp(name, "\"firmware\"") == 0) {
				sprintf(name, "/dev/mtd%d", index);
				target = strdup(name);
				break;
                        }
                }
        }
        fclose(fp);
	return target;
}

static int check_running_code_pattern(char *pattern)
{
	return 0;
}

static int check_firmware_code_pattern(char *name, char *pattern)
{
	return 0;
}

/**
 * ezcfg_api_firmware_upgradet:
 * @name: firmware file name
 * @device: firmware stored device path
 * @pattern: firmware code pattern
 *
 **/
int ezcfg_api_firmware_upgrade(char *name, char *device, char *pattern)
{
	int rc = 0;
	char *target = NULL;
	firmware_header_t fwhdr;

	/* check if name is valid */
	if (name == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	target = device;

	/* get device node path */
	if (target == NULL) {
		target = get_firmware_device_path();
		if (target == NULL) {
			rc = -EZCFG_E_RESOURCE ;
			goto func_out;
		}
	}

	/* check code pattern */
	if (pattern != NULL) {
		/* check current running firmware code pattern */
		rc = check_running_code_pattern(pattern);
		if (rc < 0) {
			goto func_out;
		}

		/* check upgrading firmware code pattern */
		rc = check_firmware_code_pattern(name, pattern);
		if (rc < 0) {
			goto func_out;
		}
	}

func_out:
	if (target != NULL) {
		free(target);
	}
	return rc;
}

