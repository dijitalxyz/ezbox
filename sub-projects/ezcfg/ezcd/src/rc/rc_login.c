/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_login.c
 *
 * Description  : ezbox run login service
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-03   0.1       Write it from scratch
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
#include "pop_func.h"

int rc_login(int flag)
{
	FILE *fp;
	char type[32];
	char user[32];
	char passwd[64];
	char buf[64];
	pid_t pid;
	int i, auth_number;
	int rc = EXIT_FAILURE;

	switch (flag) {
	case RC_BOOT :
		/* generate /etc/passwd */
		pop_etc_passwd(RC_BOOT);

		/* generate /etc/group */
		pop_etc_group(RC_BOOT);

		rc = EXIT_SUCCESS;
		break;

	case RC_START :
		/* generate /etc/passwd */
		pop_etc_passwd(RC_START);

		/* generate /etc/group */
		pop_etc_group(RC_START);

		/* fall down to change passwd */

	case RC_RESTART :
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EZCFG, COMMON_AUTH_NUMBER), buf, sizeof(buf));
		if (rc < 0) {
			break;
		}
		auth_number = atoi(buf);
		if (auth_number < 1) {
			break;
		}

		/* get user name and password */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_USER), user, sizeof(user));
		if (rc < 0) {
			break;
		}

		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_SECRET), passwd, sizeof(passwd));
		if (rc < 0) {
			break;
		}

		/* set root password */
		pid = getpid();
		snprintf(buf, sizeof(buf), "/tmp/rc_login.%d", pid);
		fp = fopen(buf, "w");
		if (fp == NULL) {
			break;
		}
		for (i = 0; i < auth_number; i++) {
			snprintf(buf, sizeof(buf), "%s%s.%d.%s",
				EZCFG_EZCFG_NVRAM_PREFIX,
				EZCFG_EZCFG_SECTION_AUTH,
				i, EZCFG_EZCFG_KEYWORD_TYPE);
			rc = ezcfg_api_nvram_get(buf, type, sizeof(type));
			if (rc < 0) {
				continue;
			}
			if (strcmp(type, EZCFG_AUTH_TYPE_HTTP_BASIC_STRING) != 0) {
				continue;
			}

			snprintf(buf, sizeof(buf), "%s%s.%d.%s",
				EZCFG_EZCFG_NVRAM_PREFIX,
				EZCFG_EZCFG_SECTION_AUTH,
				i, EZCFG_EZCFG_KEYWORD_USER);
			rc = ezcfg_api_nvram_get(buf, user, sizeof(user));
			if (rc < 0) {
				continue;
			}

			snprintf(buf, sizeof(buf), "%s%s.%d.%s",
				EZCFG_EZCFG_NVRAM_PREFIX,
				EZCFG_EZCFG_SECTION_AUTH,
				i, EZCFG_EZCFG_KEYWORD_SECRET);
			rc = ezcfg_api_nvram_get(buf, passwd, sizeof(passwd));
			if (rc < 0) {
				continue;
			}
			fprintf(fp, "%s:%s\n", user, passwd);
		}
		fclose(fp);
		snprintf(buf, sizeof(buf), "%s /tmp/rc_login.%d | %s", CMD_CAT, pid, CMD_CHPASSWD);
		system(buf);
		snprintf(buf, sizeof(buf), "%s -rf /tmp/rc_login.%d", CMD_RM, pid);
		system(buf);

		rc = EXIT_SUCCESS;
		break;
	}
	return (rc);
}
