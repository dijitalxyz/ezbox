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

struct type_desc_pair {
	int index;
	char *path;
};

static struct type_desc_pair ezcfg_support_upnp_type_descs[] = {
	{ EZCFG_UPNP_TYPE_UNKNOWN, NULL },
	{ EZCFG_UPNP_TYPE_IGD1, "/igd1/InternetGatewayDevice1.xml" },
};


int ezcfg_util_upnp_type(char *name)
{
	if (name == NULL)
		return EZCFG_UPNP_TYPE_UNKNOWN;

	if (strcmp(name, EZCFG_UPNP_TYPE_IGD1_STRING) == 0)
		return EZCFG_UPNP_TYPE_IGD1;
	else
		return EZCFG_UPNP_TYPE_UNKNOWN;
}

char *ezcfg_util_upnp_get_type_description_path(int type)
{
	int i;
	struct type_desc_pair *tdp;
	for (i = 1; i < ARRAY_SIZE(ezcfg_support_upnp_type_descs); i++) {
		tdp = &(ezcfg_support_upnp_type_descs[i]);
		if (tdp->index == type)
			return tdp->path;
        }
	return NULL;
}
