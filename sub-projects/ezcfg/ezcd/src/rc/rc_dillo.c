/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_dillo.c
 *
 * Description  : ezbox run dillo web browser
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-06-01   0.1       Write it from scratch
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

int rc_dillo(int flag)
{
	int rc;
	char buf[64];

	switch (flag) {
	case RC_START :
		rc = nvram_match(NVRAM_SERVICE_OPTION(RC, DILLO_ENABLE), "1");
		if (rc < 0) {
			return (EXIT_FAILURE);
		}

		/* start dillo web browser */
		snprintf(buf, sizeof(buf), "start-stop-daemon -S -b -n dillo -a /usr/bin/dillo");
		system(buf);

		break;

	case RC_STOP :
		system("start-stop-daemon -K -s KILL -n dillo");
		break;

	case RC_RESTART :
		rc = rc_dillo(RC_STOP);
		sleep(1);
		rc = rc_dillo(RC_START);
		break;
	}
	return (EXIT_SUCCESS);
}
