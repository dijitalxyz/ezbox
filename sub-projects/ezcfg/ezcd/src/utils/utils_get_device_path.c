/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_get_device_path.c
 *
 * Description  : ezcfg get device path function
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-06-23   0.1       Write it from scratch
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

#include "ezcd.h"

/*
 * Returns boot device path string
 * It is the caller's duty to free the returned string.
 */
char *utils_get_boot_device_path(void)
{
	return utils_file_get_keyword("/proc/cmdline", "boot_dev=");
}

#define DATA_DEVICE_PATH_FILE	"/boot/data_device"

char *utils_get_data_device_path(void)
{
	int i;
	struct stat stat_buf;

	for (i = 10; i > 0; i--) {
		if (stat(DATA_DEVICE_PATH_FILE, &stat_buf) == 0) {
			if (S_ISREG(stat_buf.st_mode)) {
				/* get data device path string */
				return utils_file_get_keyword(DATA_DEVICE_PATH_FILE, "data_dev=");
			}
		}
		/* wait a second then try again */
		sleep(1);
	}
	return NULL;
}
