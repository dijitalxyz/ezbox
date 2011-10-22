/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_check_service.c
 *
 * Description  : ezbox check service status
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-10   0.1       Write it from scratch
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
#define DBG printf
#else
#define DBG(format, arg...)
#endif

bool utils_service_binding_lan(char *name)
{
	char buf[64];
	int rc;

	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if ((rc < 0) || (strcmp(buf, "lan") != 0)){
		return false;
	}
	return true;
}

bool utils_service_binding_wan(char *name)
{
	char buf[64];
	int rc;

	rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	if ((rc < 0) || (strcmp(buf, "wan") != 0)){
		return false;
	}
	return true;
}

