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

#if 0
#define DBG printf
#else
#define DBG(format, arg...)
#endif

int rc_ezcd(int flag)
{
	char cmdline[256];
	proc_stat_t *pidList;

	switch (flag) {
	case RC_BOOT :
	case RC_START :
		pop_etc_ezcfg_conf(flag);
		snprintf(cmdline, sizeof(cmdline), "%s -d", CMD_EZCD);
		system(cmdline);
		/* sleep 1 second to make sure ezcd is up */
		sleep(1);
		break;

	case RC_STOP :
		pidList = utils_find_pid_by_name("ezcd");
		if (pidList) {
			int i;
			for (i = 0; pidList[i].pid > 0; i++) {
				kill(pidList[i].pid, SIGHUP);
			}
			free(pidList);
		}
		/* sleep 1 second to make sure ezcd is down */
		sleep(1);
		break;

	case RC_RESTART :
		rc_ezcd(RC_STOP);
		rc_ezcd(RC_START);
	}
	return (EXIT_SUCCESS);
}
