/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_socket_protocol.c
 *
 * Description  : system language settings
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-24   0.1       Write it from scrach
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

struct proto_pair {
	int index;
	char *name;
};

struct proto_pair ezcfg_support_protocols[] = {
	{ EZCFG_PROTO_UNKNOWN, NULL },
	{ EZCFG_PROTO_HTTP, EZCFG_SOCKET_PROTO_HTTP_STRING },
	{ EZCFG_PROTO_SOAP_HTTP, EZCFG_SOCKET_PROTO_SOAP_HTTP_STRING },
	{ EZCFG_PROTO_IGRS, EZCFG_SOCKET_PROTO_IGRS_STRING },
	{ EZCFG_PROTO_ISDP, EZCFG_SOCKET_PROTO_ISDP_STRING },
	{ EZCFG_PROTO_UEVENT, EZCFG_SOCKET_PROTO_UEVENT_STRING },
	{ EZCFG_PROTO_SSDP, EZCFG_SOCKET_PROTO_SSDP_STRING },
};

int ezcfg_util_socket_protocol_get_index(char *name)
{
	int i;
	struct proto_pair *pip;
	for (i = 1; i < ARRAY_SIZE(ezcfg_support_protocols); i++) {
		pip = &(ezcfg_support_protocols[i]);
		if (strcmp(pip->name, name) == 0)
			return pip->index;
	}
	return EZCFG_PROTO_UNKNOWN;
}

