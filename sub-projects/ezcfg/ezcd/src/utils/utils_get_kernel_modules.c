/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_get_kernel_modules.c
 *
 * Description  : ezcfg get kernel modules function
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2011-02-27   0.1       Write it from scratch
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
/*
 * Returns kernel modules string
 * It is the caller's duty to free the returned string.
 */
char *utils_get_kernel_modules(void)
{
        FILE *file;
	char *p = NULL;
	char *q = NULL;
	char *v = NULL;
	char buf[64];

	/* get kernel version */
	file = fopen("/proc/cmdline", "r");
	if (file == NULL)
		return NULL;

	memset(buf, 0, sizeof(buf));
	if (fgets(buf, sizeof(buf), file) == NULL)
		goto func_out;

	q = strstr(buf, "modules=");
	if (q == NULL)
		goto func_out;

	/* skip "modules=" */
	p = q + 8;
	q = strchr(p, ' ');
	if (q != NULL)
		*q = '\0';
func_out:
	fclose(file);
	if (p != NULL)
		v = strdup(p);
	return (v);
}
