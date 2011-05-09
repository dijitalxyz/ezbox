/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_rc.c
 *
 * Description  : system WAN connection settings
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

bool ezcfg_util_rc(char *func, char *act, int s, bool bg)
{
	char cmd[1024];
	int i;

	i = snprintf(cmd, sizeof(cmd), "rc %s %s %d %s",
		func, act, s,
		(bg == true) ? "&" : "");

	if (i < sizeof(cmd)) {
		system(cmd);
		return true;
	}
	else {
		return false;
	}
}
