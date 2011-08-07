/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_klogd.c
 *
 * Description  : ezbox run klog service
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-08-07   0.1       Write it from scratch
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

int rc_klogd(int flag)
{
	char cmd[128];
	int rc;
	switch (flag) {
	case RC_START :
		rc = nvram_match(NVRAM_SERVICE_OPTION(RC, KLOGD_ENABLE), "1");
		if (rc < 0) {
			return (EXIT_FAILURE);
		}
		snprintf(cmd, sizeof(cmd), "start-stop-daemon -S -b -n klogd -a %s", CMD_KLOGD);
		system(cmd);
		break;

	case RC_STOP :
		system("start-stop-daemon -K -n klogd");
		break;

	case RC_RESTART :
		rc = rc_klogd(RC_STOP);
		rc = rc_klogd(RC_START);
		break;
	}
	return (EXIT_SUCCESS);
}
