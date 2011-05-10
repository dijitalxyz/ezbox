/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_wan.c
 *
 * Description  : ezbox run network WAN config service
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
#include "utils.h"

static int start_wan(void)
{
	char wan_ifname[IFNAMSIZ];
	char buf[256];
	int wan_type;
	int rc;

	wan_type = utils_get_wan_type();
	if (wan_type == WAN_TYPE_UNKNOWN)
		return (EXIT_FAILURE);

	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, IFNAME), wan_ifname, sizeof(wan_ifname));
	if (rc < 0)
		return (EXIT_FAILURE);

	switch (wan_type) {
	case WAN_TYPE_DHCP :
		/* start DHCP client process */
		snprintf(buf, sizeof(buf), "%s %s &", CMD_IFUP, wan_ifname);
		system(buf);

		break;
	}

	return (EXIT_SUCCESS);
}

static int stop_wan(void)
{
	char wan_ifname[IFNAMSIZ];
	char buf[256];
	int wan_type;
	int rc;

	wan_type = utils_get_wan_type();
	if (wan_type == WAN_TYPE_UNKNOWN)
		return (EXIT_FAILURE);

	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, IFNAME), wan_ifname, sizeof(wan_ifname));
	if (rc < 0)
		return (EXIT_FAILURE);

	switch (wan_type) {
	case WAN_TYPE_DHCP :
		/* stop DHCP client process */
		snprintf(buf, sizeof(buf), "%s %s", CMD_IFDOWN, wan_ifname);
		system(buf);

		break;
	}

	return (EXIT_SUCCESS);
}

int rc_wan(int flag)
{
	int ret = 0;

	switch (flag) {
	case RC_BOOT :
	case RC_START :
		/* bring up WAN interface and config it */
		ret = start_wan();
		break;

	case RC_STOP :
		/* bring down WAN interface and deconfig it */
		ret = stop_wan();
		break;

	case RC_RESTART :
		ret = rc_wan(RC_STOP);
		if (ret != EXIT_SUCCESS)
			return ret;

		ret = rc_wan(RC_START);
		break;
	}

	return ret;
}
