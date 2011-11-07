/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_rc.c
 *
 * Description  : system rc running
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-09   0.1       Write it from scrach
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
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

#if 0
#define DBG(format, args...) do { \
	pid_t pid; \
	char path[256]; \
	FILE *fp; \
	pid = getpid(); \
	snprintf(path, 256, "/tmp/%d-debug.txt", pid); \
	fp = fopen(path, "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

bool ezcfg_util_rc(char *func, char *act, int s)
{
	char cmd[64];
	int i;

	if (s < 1) {
		i = snprintf(cmd, sizeof(cmd), "/sbin/rc %s %s", func, act);
	}
	else {
		i = snprintf(cmd, sizeof(cmd), "/sbin/rc %d %s %s", s, func, act);
	}

	if (i < sizeof(cmd)) {
		system(cmd);
		return true;
	}
	else {
		return false;
	}
}

bool ezcfg_util_rc_list(ezcfg_rc_triple_t *list, char *func, char *act, int s)
{
	ezcfg_rc_triple_t *rcp;

	if ((list == NULL) || (func == NULL))
		return false;

	rcp = list;
	while(rcp->service != NULL) {
		if (strcmp(rcp->service, func) == 0) {
			rcp->action = act;
			rcp->wait = s;
			return true;
		}
		rcp++;
	}

	return true;
}
