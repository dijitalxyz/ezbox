/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_upnp_device_type.c
 *
 * Description  : upnp device type settings
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

struct upnp_device_type_tuple {
	int index;
	char *name;
	char *path;
};

static struct upnp_device_type_tuple ezcfg_support_upnp_device_types[] = {
	{ EZCFG_UPNP_DEV_TYPE_UNKNOWN, NULL, NULL },
	{ EZCFG_UPNP_DEV_TYPE_IGD1, EZCFG_UPNP_DEV_TYPE_IGD1_STRING, "/igd1/InternetGatewayDevice1.xml" },
};


int ezcfg_util_upnp_device_type(char *name)
{
	int i;
	struct upnp_device_type_tuple *dtp;

	if (name == NULL) {
		return EZCFG_UPNP_DEV_TYPE_UNKNOWN;
	}

	for (i = 1; i < ARRAY_SIZE(ezcfg_support_upnp_device_types); i++) {
		dtp = &(ezcfg_support_upnp_device_types[i]);
		if (strcmp(dtp->name, name) == 0)
			return dtp->index;
        }

	return EZCFG_UPNP_DEV_TYPE_UNKNOWN;
}

char *ezcfg_util_upnp_get_device_type_description_path(int type)
{
	int i;
	struct upnp_device_type_tuple *dtp;
	for (i = 1; i < ARRAY_SIZE(ezcfg_support_upnp_device_types); i++) {
		dtp = &(ezcfg_support_upnp_device_types[i]);
		if (dtp->index == type)
			return dtp->path;
        }
	return NULL;
}
