/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_loopback.c
 *
 * Description  : ezbox run network loopback service
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-03   0.1       Write it from scratch
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

int rc_loopback(int flag)
{
	int ret = 0;
	char cmdline[256];

	switch (flag) {
	case RC_BOOT :
	case RC_START :
		/* bring up loopback interface */
		snprintf(cmdline, sizeof(cmdline), "%s lo", CMD_IFUP);
		ret = system(cmdline);
		break;

	case RC_STOP :
		/* bring down loopback interface */
		snprintf(cmdline, sizeof(cmdline), "%s lo", CMD_IFDOWN);
		ret = system(cmdline);
		break;

	case RC_RESTART :
		ret = rc_loopback(RC_STOP);
		ret = rc_loopback(RC_START);
		break;
	}

	return (EXIT_SUCCESS);
}
