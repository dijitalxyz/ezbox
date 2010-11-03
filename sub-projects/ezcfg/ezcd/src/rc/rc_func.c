/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_func.c
 *
 * Description  : ezbox rc service functions program
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-02   0.1       Write it from scratch
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
#include "rc_func.h"

struct rc_func rc_functions[] = {
	{ "base_files", RC_BOOT, rc_base_files },
	{ "ezcd", RC_BOOT, rc_ezcd },
	{ "hotplug2", RC_BOOT, rc_hotplug2 },
	{ "init", RC_BOOT, rc_init },
	{ "lan_if", RC_BOOT, rc_lan_if },
	{ "load_modules", RC_BOOT, rc_load_modules },
	{ "login", RC_BOOT, rc_login },
	{ "loopback", RC_BOOT, rc_loopback },
	{ "netbase", RC_BOOT, rc_netbase },
	{ "system", RC_BOOT, rc_system },
};

struct rc_func * utils_find_rc_func(char *name)
{
	struct rc_func *f;

	for (f = rc_functions; f < &rc_functions[STRUCT_LEN(rc_functions)]; f++)
		if (strcmp(f->name, name) == 0) return f;

	return NULL;
}
