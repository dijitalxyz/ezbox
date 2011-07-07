/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_emc2.c
 *
 * Description  : ezbox run EMC2 Enhanced Machine Controller service
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-07-05   0.1       Write it from scratch
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

int rc_emc2(int flag)
{
	int rc;
	char buf[64];

	switch (flag) {
	case RC_START :
		rc = nvram_match(NVRAM_SERVICE_OPTION(RC, EMC2_ENABLE), "1");
		if (rc < 0) {
			return (EXIT_FAILURE);
		}

		pop_etc_emc2_rtapi_conf(RC_START);
		pop_etc_emc2_configs(RC_START);

		/* start EMC2 Enhanced Machine Controller service */
		snprintf(buf, sizeof(buf), "start-stop-daemon -S -b -n emcsvr -a /usr/bin/emcsvr -- -ini /etc/emc2/configs/ezcnc.ini");
		system(buf);

		break;

	case RC_STOP :
		system("start-stop-daemon -K -s KILL -n emc2");
		break;

	case RC_RESTART :
		rc = rc_emc2(RC_STOP);
		sleep(1);
		rc = rc_emc2(RC_START);
		break;
	}
	return (EXIT_SUCCESS);
}
