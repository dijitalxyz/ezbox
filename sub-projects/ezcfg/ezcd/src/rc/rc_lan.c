/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_lan.c
 *
 * Description  : ezbox run network LAN config service
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-04   0.1       Write it from scratch
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
#include <net/if.h>

#include "ezcd.h"

int rc_lan(int flag)
{
	int ret = 0;
	char lan_ifname[IFNAMSIZ];
	char cmdline[256];

	ret = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(LAN, IFNAME), lan_ifname, sizeof(lan_ifname));
	if (ret < 0)
		return (EXIT_FAILURE);

	switch (flag) {
	case RC_BOOT :
	case RC_START :
		/* bring up LAN interface and config it */
		snprintf(cmdline, sizeof(cmdline), "%s %s up", CMD_IFUP, lan_ifname);
		ret = system(cmdline);
		break;

	case RC_STOP :
		/* bring down LAN interface */
		snprintf(cmdline, sizeof(cmdline), "%s %s down", CMD_IFDOWN, lan_ifname);
		ret = system(cmdline);
		break;

	case RC_RESTART :
		ret = rc_lan(RC_STOP);
		ret = rc_lan(RC_START);
		break;
	}

	return (EXIT_SUCCESS);
}
