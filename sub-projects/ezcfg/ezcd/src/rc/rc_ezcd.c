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

int rc_ezcd(int flag)
{
	int ret;
	char cmdline[256];

	switch (flag) {
	case RC_START :
		snprintf(cmdline, sizeof(cmdline), "%s -d", CMD_EZCD);
		system(cmdline);
		break;

	case RC_STOP :
		snprintf(cmdline, sizeof(cmdline), "%s -q ezcd", CMD_KILLALL);
		system(cmdline);
		break;

	case RC_RESTART :
		rc_ezcd(RC_STOP);
		rc_ezcd(RC_START);
	}
	return (EXIT_SUCCESS);
}
