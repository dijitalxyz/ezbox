/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_sync_cfg_with_nvram.c
 *
 * Description  : ezcfg sync xxx.cfg with nvram keyword value function
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

int utils_sync_cfg_with_nvram(char *path)
{
	FILE *file = NULL;
	char path2[64];
	FILE *file2 = NULL;
	char buf[FILE_LINE_BUFFER_SIZE];
	char buf2[FILE_LINE_BUFFER_SIZE];
	char *p=NULL, *value=NULL;
	int ret = EXIT_FAILURE;
	int len, rc;

	if (path == NULL)
		return ret;

	file = fopen(path, "r");
	if (file == NULL) 
		return ret;

	snprintf(path2, sizeof(path2), "%s.new", path);
	file2 = fopen(path2, "w");
	if (file2 == NULL) {
		goto func_out;
	}

	while (utils_file_get_line(file, buf, sizeof(buf), "", LINE_TAIL_STRING) == true) {
		p = strchr(buf, '=');
		if (p != NULL) {
			*p = '\0';
			/* skip "sys." system configuration */
			if (strcmp(buf, EZCFG_SYS_NVRAM_PREFIX) != 0) {
				value = p+1;
				len = sizeof(buf) - strlen(buf) - 1;
				rc = ezcfg_api_nvram_get(buf, buf2, sizeof(buf2));
				if (rc >= 0) {
					snprintf(value, len, "%s", buf2);
				}
			}
			*p = '=';
		}
		fprintf(file2, "%s\n", buf);
	}

func_out:
	if (file != NULL)
		fclose(file);
	if (file2 != NULL) {
		fclose(file2);
		snprintf(buf, sizeof(buf), "%s -f %s %s", CMD_MV, path2, path);
		utils_system(buf);
	}
	return EXIT_SUCCESS;
}
