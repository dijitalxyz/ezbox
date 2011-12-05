/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/master_load_common_conf.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-10-10   0.1       Split it from master.c
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

#if 0
#define DBG(format, args...) do { \
	char path[256]; \
	FILE *fp; \
	snprintf(path, 256, "/tmp/%d-debug.txt", getpid()); \
	fp = fopen(path, "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

void ezcfg_master_load_common_conf(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;
	char *p;

	if (master == NULL)
		return ;

	ezcfg = ezcfg_master_get_ezcfg(master);

	/* get log_level keyword */
	p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_LOG_LEVEL);
	if (p != NULL) {
		ezcfg_common_set_log_priority(ezcfg, ezcfg_util_log_priority(p));
		free(p);
		DBG("%s(%d) log_priority='%d'\n", __func__, __LINE__, ezcfg_common_get_log_priority(ezcfg));
	}

	/* find rules_path keyword */
	p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_RULES_PATH);
	if (p != NULL) {
		ezcfg_util_remove_trailing_char(p, '/');
		ezcfg_common_set_rules_path(ezcfg, p);
		DBG("%s(%d) rules_path='%s'\n", __func__, __LINE__, ezcfg_common_get_rules_path(ezcfg));
	}

	/* get locale */
	p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_LOCALE);
	if (p != NULL) {
		ezcfg_common_set_locale(ezcfg, p);
		DBG("%s(%d) locale='%s'\n", __func__, __LINE__, ezcfg_common_get_locale(ezcfg));
	}
}
