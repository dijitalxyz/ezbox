/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_upnp_type.c
 *
 * Description  : upnp type settings
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-09   0.1       Write it from scrach
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

int ezcfg_util_upnp_type(char *name)
{
	if (name == NULL)
		return EZCFG_UPNP_TYPE_UNKNOWN;

	if (strcmp(name, EZCFG_UPNP_TYPE_IGD1_STRING) == 0)
		return EZCFG_UPNP_TYPE_IGD1;
	else
		return EZCFG_UPNP_TYPE_UNKNOWN;
}
