/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_get_bootcfg_keyword.c
 *
 * Description  : ezcfg get keyword from ezbox_boot.cfg file function
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2011-07-27   0.1       Write it from scratch
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

int utils_get_bootcfg_keyword(char *name, char *buf, int buf_len)
{
	int len, ret=-1;
	struct stat stat_buf;
	char *keyword=NULL, *value=NULL;

	if ((name == NULL) || (buf == NULL) || (buf_len < 1))
		return -1;

	len = strlen(name) + 2;
	keyword = malloc(len);
	if (keyword == NULL)
		return -1;

	snprintf(keyword, len, "%s=", name);

	if ((stat(BOOT_CONFIG_FILE_PATH, &stat_buf) == 0) &&
	    (S_ISREG(stat_buf.st_mode))) {
		/* get keyword's value from boot.cfg file */
		value = utils_file_get_keyword(BOOT_CONFIG_FILE_PATH, keyword);
		if (value != NULL) {
			ret = snprintf(buf, buf_len, "%s", value);
			free(value);
		}
	}
	free(keyword);
	return ret;
}
