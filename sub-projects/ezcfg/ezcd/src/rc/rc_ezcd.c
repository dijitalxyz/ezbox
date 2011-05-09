/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_ezcd.c
 *
 * Description  : ezbox run ezcfg daemon service
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-06-13   0.1       Write it from scratch
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
#include "utils.h"
#include "pop_func.h"

#if 0
#define DBG printf
#else
#define DBG(format, arg...)
#endif

int rc_ezcd(int flag)
{
	char buf[256];
	int rc;
	proc_stat_t *pidList;

	switch (flag) {
	case RC_BOOT :
		/* FIXME: nvram is not ready now!!! */
		/* ezcfg config file should be prepared */
		snprintf(buf, sizeof(buf), "%s -d", CMD_EZCD);
		system(buf);

		/* wait until nvram is ready */
		if (utils_ezcd_wait_up(0) == false) {
			return (EXIT_FAILURE);
		}

		/* FIXME: nvram is ready now!!! */
		pop_etc_ezcfg_conf(flag);
		break;

#if 0
	case RC_START :
		/* FIXME: nvram is not ready now!!! */
		/* ezcfg config file should be prepared by rc_ezcd(RC_BOOT) */
		snprintf(buf, sizeof(buf), "%s -d", CMD_EZCD);
		system(buf);

		/* wait at most 10 seconds until nvram is ready */
		if (utils_ezcd_wait_up(10) == false) {
			return (EXIT_FAILURE);
		}

		/* FIXME: nvram is ready now!!! */
		/* reload ezcfg info */
		rc_ezcd(RC_RELOAD);
		break;
#endif

	case RC_STOP :
		pidList = utils_find_pid_by_name("ezcd");
		while (pidList) {
			int i;
			for (i = 0; pidList[i].pid > 0; i++) {
				kill(pidList[i].pid, SIGTERM);
			}
			free(pidList);
			/* sleep 1 second to make sure ezcd is down */
			sleep(1);
			pidList = utils_find_pid_by_name("ezcd");
		}
		break;

	case RC_RELOAD :
		/* re-generate ezcfg config file */
		pop_etc_ezcfg_conf(flag);
		/* send signal to ezcd to reload config */
		pidList = utils_find_pid_by_name("ezcd");
		if (pidList) {
			int i;
			for (i = 0; pidList[i].pid > 0; i++) {
				kill(pidList[i].pid, SIGHUP);
			}
			free(pidList);
		}

		/* wait at most 10 seconds until nvram is ready */
		if (utils_ezcd_wait_up(10) == false) {
			return (EXIT_FAILURE);
		}

		break;
	}
	return (EXIT_SUCCESS);
}
