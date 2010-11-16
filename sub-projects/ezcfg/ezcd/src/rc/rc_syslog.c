/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_syslog.c
 *
 * Description  : ezbox run syslog/klog service
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-16   0.1       Write it from scratch
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

int rc_syslog(int flag)
{
	int rc;
	rc = nvram_match(EZCFG_NVRAM_RC_SYSLOG_ENABLE_NAME, "1");
	if (rc < 0) {
		return (EXIT_FAILURE);
	}
	switch (flag) {
	case RC_START :
		system("start-stop-daemon -S -b -n syslogd -a /sbin/syslogd -- -n -m 20");
		system("start-stop-daemon -S -b -n klogd -a /sbin/klogd -- -n");
		break;

	case RC_STOP :
		system("start-stop-daemon -K -n syslogd");
		system("start-stop-daemon -K -n klogd");
		break;

	case RC_RESTART :
		rc = rc_syslog(RC_STOP);
		rc = rc_syslog(RC_START);
		break;
	}
	return (EXIT_SUCCESS);
}
