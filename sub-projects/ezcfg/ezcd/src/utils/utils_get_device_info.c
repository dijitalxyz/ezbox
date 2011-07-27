/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_get_device_info.c
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

#define DEVICE_INFO_PATH_INDEX          1
#define DEVICE_INFO_FS_TYPE_INDEX       2
/*
 * Returns boot device path string
 * It is the caller's duty to free the returned string.
 */
char *utils_get_boot_device_path(void)
{
	return utils_file_get_keyword_by_index("/proc/cmdline", "boot_dev=", DEVICE_INFO_PATH_INDEX);
}

char *utils_get_boot_device_fs_type(void)
{
	return utils_file_get_keyword_by_index("/proc/cmdline", "boot_dev=", DEVICE_INFO_FS_TYPE_INDEX);
}

char *utils_get_data_device_path(void)
{
	int i;
	struct stat stat_buf;

	for (i = 3; i > 0; i--) {
		if (stat(BOOT_CONFIG_FILE_PATH, &stat_buf) == 0) {
			if (S_ISREG(stat_buf.st_mode)) {
				/* get data device path string */
				return utils_file_get_keyword_by_index(BOOT_CONFIG_FILE_PATH, "data_dev=", DEVICE_INFO_PATH_INDEX);
			}
		}
		/* wait a second then try again */
		sleep(1);
	}
	return NULL;
}

char *utils_get_data_device_fs_type(void)
{
	int i;
	struct stat stat_buf;

	for (i = 3; i > 0; i--) {
		if (stat(BOOT_CONFIG_FILE_PATH, &stat_buf) == 0) {
			if (S_ISREG(stat_buf.st_mode)) {
				/* get data device path string */
				return utils_file_get_keyword_by_index(BOOT_CONFIG_FILE_PATH, "data_dev=", DEVICE_INFO_FS_TYPE_INDEX);
			}
		}
		/* wait a second then try again */
		sleep(1);
	}
	return NULL;
}
