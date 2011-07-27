/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_ldconfig.c
 *
 * Description  : ezbox run ldconfig service
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-07-08   0.1       Write it from scratch
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
#include "pop_func.h"

#if 1
#define DBG(format, args...) do {\
	FILE *fp = fopen("/tmp/ldconfig.debug", "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif


int rc_ldconfig(int flag)
{
	int rc;
	char buf[128];
	char cmd[128];
	char *p, *q;
	FILE *file;

	switch (flag) {
	case RC_START :
		/* start ldconfig service */
		/* get the LD_LIBRARY_PATH name for ldconfig */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(SYS, LD_LIBRARY_PATH), buf, sizeof(buf));
		if (rc > 0) {
			file = fopen("/etc/ld.so.conf", "w");
			if (file != NULL) {
				p = buf;
				while(p != NULL) {
					q = strchr(p, ',');
					if (q != NULL)
						*q = '\0';

					/* add LD_LIBRARY_PATH */
					fprintf(file, "%s\n", p);
					if (q != NULL)
						p = q+1;
					else
						p = NULL;
				}
				fclose(file);
				snprintf(cmd, sizeof(cmd), "%s -f %s", CMD_LDCONFIG, "/etc/ld.so.conf");
				system(cmd);
			}
		}
		break;

	case RC_STOP :
		/* stop ldconfig service */
		snprintf(cmd, sizeof(cmd), "%s -rf %s", CMD_RM, "/etc/ld.so.conf");
		system(cmd);
		break;

	case RC_RESTART :
		rc = rc_ldconfig(RC_STOP);
		sleep(1);
		rc = rc_ldconfig(RC_START);
		break;
	}
	return (EXIT_SUCCESS);
}
