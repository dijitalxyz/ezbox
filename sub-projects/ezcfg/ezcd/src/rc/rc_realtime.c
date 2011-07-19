/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_realtime.c
 *
 * Description  : ezbox run realtime RTAI service
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
	FILE *fp = fopen("/tmp/realtime.debug", "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif


int rc_realtime(int flag)
{
	int rc;
	char buf[128];
	char cmd[128];
	char modpath[128];
	char *p, *q, *r;
	FILE *file;

	switch (flag) {
	case RC_START :
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, MODPATH), modpath, sizeof(modpath));
		if (rc <= 0) {
			return (EXIT_FAILURE);
		}

		/* start realtime RTAI service */
		/* get the kernel module name for realtime */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, MODULES), buf, sizeof(buf));
		if (rc > 0) {
			p = buf;
			while(p != NULL) {
				q = strchr(p, ',');
				if (q != NULL)
					*q = '\0';

				/* insmod kernel module */
				snprintf(cmd, sizeof(cmd), "%s %s %s/%s.ko",
					"/usr/bin/emc_module_helper", "insert", modpath, p);
				system(cmd);
				DBG("cmd=[%s]\n", cmd);

				if (q != NULL)
					p = q+1;
				else
					p = NULL;
			}
		}
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, RTAPI_DEBUG), buf, sizeof(buf));
		if (rc > 0) {
			file = fopen("/proc/rtapi/debug", "w");
			if (file != NULL) {
				/* set debug mode */
				fprintf(file, "%s", buf);
				fclose(file);
			}
		}
		break;

	case RC_STOP :
		/* stop realtime RTAI service */
		/* get the kernel module name for realtime */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, MODULES), buf, sizeof(buf));
		if (rc > 0) {
			p = buf;
			while(p != NULL) {
				/* split modules */
				q = strrchr(p, ',');
				if (q != NULL) {
					*q = '\0';
					p = q+1;
				}

				/* strip '/' in module path */
				r = strrchr(p, '/');
				if (r != NULL) {
					p = r+1;
				}

				/* insmod kernel module */
				snprintf(cmd, sizeof(cmd), "%s %s %s",
					"/usr/bin/emc_module_helper", "remove", p);
				system(cmd);
				DBG("cmd=[%s]\n", cmd);

				if (q != NULL)
					p = buf;
				else 
					p = NULL;
			}
		}
		break;

	case RC_RESTART :
		rc = rc_realtime(RC_STOP);
		sleep(1);
		rc = rc_realtime(RC_START);
		break;
	}
	return (EXIT_SUCCESS);
}
