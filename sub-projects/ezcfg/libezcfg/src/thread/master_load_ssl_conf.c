/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/master_load_ssl_conf.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2012-02-04   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-ssl.h"

#if 0
#define DBG(format, args...) do { \
	char path[256]; \
	FILE *dbg_fp; \
	snprintf(path, 256, "/tmp/%d-debug.txt", getpid()); \
	dbg_fp = fopen(path, "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

void ezcfg_master_load_ssl_conf(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;
	char *p = NULL;
	int i;
	int ssl_number = -1;
	struct ezcfg_ssl *sslp = NULL;
	struct ezcfg_ssl **psp = NULL;
	int role;
	int method;

	if (master == NULL)
		return ;

	ezcfg = ezcfg_master_get_ezcfg(master);

	/* first get the auth number */
	p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_SSL_NUMBER);
	if (p != NULL) {
		ssl_number = atoi(p);
		free(p);
	}

	for (i = 0; i < ssl_number; i++) {
		/* check SSL role */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_SSL, i, EZCFG_EZCFG_KEYWORD_ROLE);
		if (p == NULL) {
			continue;
		}

		if (strcmp(p, EZCFG_SSL_ROLE_SERVER_STRING) == 0) {
			role = EZCFG_SSL_ROLE_SERVER;
		}
		else if (strcmp(p, EZCFG_SSL_ROLE_CLIENT_STRING) == 0) {
			role = EZCFG_SSL_ROLE_CLIENT;
		}
		else {
			/* unknown SSL role */
			role = EZCFG_SSL_ROLE_UNKNOWN;
		}
		free(p);

		/* check SSL method */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_SSL, i, EZCFG_EZCFG_KEYWORD_METHOD);
		if (p == NULL) {
			continue;
		}

		if (strcmp(p, EZCFG_SSL_METHOD_SSLV2_STRING) == 0) {
			method = EZCFG_SSL_METHOD_SSLV2;
		}
		else if (strcmp(p, EZCFG_SSL_METHOD_SSLV3_STRING) == 0) {
			method = EZCFG_SSL_METHOD_SSLV3;
		}
		else if (strcmp(p, EZCFG_SSL_METHOD_TLSV1_STRING) == 0) {
			method = EZCFG_SSL_METHOD_TLSV1;
		}
		else if (strcmp(p, EZCFG_SSL_METHOD_SSLV23_STRING) == 0) {
			method = EZCFG_SSL_METHOD_SSLV23;
		}
		else {
			/* unknown SSL method */
			free(p);
			continue;
		}
		free(p);

		/* initialize */
		sslp = ezcfg_ssl_new(ezcfg, role, method);

		if (sslp == NULL) {
			continue;
		}

		/* check if SSL is valid */
		if (ezcfg_ssl_is_valid(sslp) == false) {
			ezcfg_ssl_delete(sslp);
			continue;
		}

		/* check if auth is already set */
		psp = ezcfg_master_get_p_ssl(master);
		if (ezcfg_ssl_list_in(psp, sslp) == true) {
			info(ezcfg, "ssl entry already set\n");
			ezcfg_ssl_delete(sslp);
			continue;
		}

		/* add new authentication */
		psp = ezcfg_master_get_p_ssl(master);
		if (ezcfg_ssl_list_insert(psp, sslp) == true) {
			info(ezcfg, "insert ssl entry successfully\n");
			/* set sslp to NULL to avoid delete it */
			sslp = NULL;
		}
		else {
			err(ezcfg, "insert ssl entry failed: %m\n");
			ezcfg_ssl_delete(sslp);
		}
	}
}
