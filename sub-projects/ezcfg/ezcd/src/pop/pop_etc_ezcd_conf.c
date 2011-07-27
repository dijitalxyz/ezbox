/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_ezcd_conf.c
 *
 * Description  : ezbox /etc/ezcd.conf generating program
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-24   0.1       Write it from scratch
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

static int generate_ezcd_conf_file(FILE *file, int flag)
{
	char name[64];
	char buf[256];
	int rc;
	int i;
	int socket_number = 0;
	int auth_number = 0;

	/* setup ezcfg common info */
	fprintf(file, "[%s]\n", EZCFG_EZCFG_SECTION_COMMON);

	/* log_level */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_LOG_LEVEL);
	if (flag == RC_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_LOG_LEVEL, buf);
	}

	/* rules_path */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_RULES_PATH);
	if (flag == RC_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_RULES_PATH, buf);
	}

	/* socket_number */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_SOCKET_NUMBER);
	if (flag == RC_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SOCKET_NUMBER, buf);
		socket_number = atoi(buf);
	}

	/* locale */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_LOCALE);
	if (flag == RC_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_LOCALE, buf);
	}

	/* auth_number */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_AUTH_NUMBER);
	if (flag == RC_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_AUTH_NUMBER, buf);
		auth_number = atoi(buf);
	}

	fprintf(file, "\n");

	/* setup nvram storage info */
	for(i = 0; i < EZCFG_NVRAM_STORAGE_NUM; i++) {

		fprintf(file, "[%s]\n", EZCFG_EZCFG_SECTION_NVRAM);

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_NVRAM,
		         i, EZCFG_EZCFG_KEYWORD_BUFFER_SIZE);
		if (flag == RC_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_BUFFER_SIZE, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_NVRAM,
		         i, EZCFG_EZCFG_KEYWORD_BACKEND_TYPE);
		if (flag == RC_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_BACKEND_TYPE, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_NVRAM,
		         i, EZCFG_EZCFG_KEYWORD_CODING_TYPE);
		if (flag == RC_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_CODING_TYPE, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_NVRAM,
		         i, EZCFG_EZCFG_KEYWORD_STORAGE_PATH);
		if (flag == RC_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_STORAGE_PATH, buf);
		}
		fprintf(file, "\n");
	}

	/* setup socket info */
	for(i = 0; i < socket_number; i++) {

		fprintf(file, "[%s]\n", EZCFG_EZCFG_SECTION_SOCKET);

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_SOCKET,
		         i, EZCFG_EZCFG_KEYWORD_DOMAIN);
		if (flag == RC_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_DOMAIN, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_SOCKET,
		         i, EZCFG_EZCFG_KEYWORD_TYPE);
		if (flag == RC_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_TYPE, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_SOCKET,
		         i, EZCFG_EZCFG_KEYWORD_PROTOCOL);
		if (flag == RC_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_PROTOCOL, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_SOCKET,
		         i, EZCFG_EZCFG_KEYWORD_ADDRESS);
		if (flag == RC_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_ADDRESS, buf);
		}
		fprintf(file, "\n");
	}

	/* setup auth info */
	for(i = 0; i < auth_number; i++) {

		fprintf(file, "[%s]\n", EZCFG_EZCFG_SECTION_AUTH);

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_AUTH,
		         i, EZCFG_EZCFG_KEYWORD_TYPE);
		if (flag == RC_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_TYPE, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_AUTH,
		         i, EZCFG_EZCFG_KEYWORD_USER);
		if (flag == RC_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_USER, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_AUTH,
		         i, EZCFG_EZCFG_KEYWORD_REALM);
		if (flag == RC_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_REALM, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_AUTH,
		         i, EZCFG_EZCFG_KEYWORD_DOMAIN);
		if (flag == RC_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_DOMAIN, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_AUTH,
		         i, EZCFG_EZCFG_KEYWORD_SECRET);
		if (flag == RC_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SECRET, buf);
		}
		fprintf(file, "\n");
	}

	return EXIT_SUCCESS;
}

int pop_etc_ezcd_conf(int flag)
{
	FILE *file;

	switch (flag) {
	case RC_BOOT :
	case RC_START :
	case RC_RELOAD :
	case RC_RESTART :
		/* get ezcd config from nvram */
		file = fopen(EZCD_CONFIG_FILE_PATH, "w");
		if (file == NULL)
			return (EXIT_FAILURE);

		generate_ezcd_conf_file(file, flag);

		fclose(file);
		break;
	}

	return (EXIT_SUCCESS);
}
