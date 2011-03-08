/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_ezcfg_conf.c
 *
 * Description  : ezbox /etc/ezcfg.conf generating program
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-02   0.1       Write it from scratch
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

int pop_etc_ezcfg_conf(int flag)
{
	FILE *file;
	char name[64];
	char buf[256];
	int rc;
	int i;

	switch (flag) {
	case RC_BOOT :
	case RC_START :
	case RC_RELOAD :
	case RC_RESTART :
		/* get ezcd config from nvram */
		file = fopen("/etc/ezcfg.conf", "w");
		if (file == NULL)
			return (EXIT_FAILURE);

		for(i = 0; i < EZCFG_NVRAM_STORAGE_NUM; i++) {

			fprintf(file, "[%s]\n", EZCFG_EZCFG_SECTION_NVRAM);

			snprintf(name, sizeof(name), "%s%s.%d.%s",
			         EZCFG_EZCFG_NVRAM_PREFIX,
			         EZCFG_EZCFG_SECTION_NVRAM,
			         i, EZCFG_EZCFG_KEYWORD_BUFFER_SIZE);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc >= 0) {
				fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_BUFFER_SIZE, buf);
			}

			snprintf(name, sizeof(name), "%s%s.%d.%s",
			         EZCFG_EZCFG_NVRAM_PREFIX,
			         EZCFG_EZCFG_SECTION_NVRAM,
			         i, EZCFG_EZCFG_KEYWORD_BACKEND_TYPE);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc >= 0) {
				fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_BACKEND_TYPE, buf);
			}

			snprintf(name, sizeof(name), "%s%s.%d.%s",
			         EZCFG_EZCFG_NVRAM_PREFIX,
			         EZCFG_EZCFG_SECTION_NVRAM,
			         i, EZCFG_EZCFG_KEYWORD_CODING_TYPE);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc >= 0) {
				fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_CODING_TYPE, buf);
			}

			snprintf(name, sizeof(name), "%s%s.%d.%s",
			         EZCFG_EZCFG_NVRAM_PREFIX,
			         EZCFG_EZCFG_SECTION_NVRAM,
			         i, EZCFG_EZCFG_KEYWORD_STORAGE_PATH);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc >= 0) {
				fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_STORAGE_PATH, buf);
			}
		}

		fclose(file);
		break;
	}

	return (EXIT_SUCCESS);
}
