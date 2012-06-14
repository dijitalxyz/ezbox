/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_sync_nvram_with_cfg.c
 *
 * Description  : ezcfg sync nvram keyword value from xxx.cfg file function
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2011-07-31   0.1       Write it from scratch
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

int utils_sync_nvram_with_cfg(char *path)
{
	FILE *file = NULL;
	char buf[FILE_LINE_BUFFER_SIZE];
	char *keyword=NULL, *value=NULL;
	int rc;

	if (path == NULL)
		return EXIT_FAILURE;

	file = fopen(path, "r");
	if (file == NULL)
		return EXIT_FAILURE;

	while (utils_file_get_line(file, buf, sizeof(buf), "#", LINE_TAIL_STRING) == true) {
		value = strchr(buf, '=');
		if (value != NULL) {
			*value = '\0';
			keyword = buf;
			value++;
			rc = ezcfg_api_nvram_set(keyword, value);
			if (rc < 0) {
				fclose(file);
				return EXIT_FAILURE;
			}
		}
	}

	fclose(file);
	return EXIT_SUCCESS;
}
