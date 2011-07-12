/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_modules.c
 *
 * Description  : ezbox /etc/modules generating program
 *
 * Copyright (C) 2008-2011 by ezbox-project
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

int pop_etc_modules(int flag)
{
	FILE *file;
	char *kmod = NULL;
	char *p, *q;
	char buf[256];
	int rc;

	file = fopen("/etc/modules", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	switch (flag) {
	case RC_BOOT :
		/* get the kernel module name from kernel cmdline */
		kmod = utils_get_kernel_modules();
		if (kmod != NULL) {
			p = kmod;
			while(p != NULL) {
				q = strchr(p, ',');
				if (q != NULL)
					*q = '\0';
				fprintf(file, "%s\n", p);
				if (q != NULL)
					p = q+1;
				else
					p = NULL;
			}
			free(kmod);
		}
		break;
	case RC_RESTART :
	case RC_START :
		/* get the kernel module name from nvram */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(SYS, MODULES), buf, sizeof(buf));
		if (rc > 0) {
			p = buf;
			while(p != NULL) {
				q = strchr(p, ',');
				if (q != NULL)
					*q = '\0';
				fprintf(file, "%s\n", p);
				if (q != NULL)
					p = q+1;
				else
					p = NULL;
			}
		}
		break;
	}

	fclose(file);
	return (EXIT_SUCCESS);
}
