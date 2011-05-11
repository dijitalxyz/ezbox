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
#include "rc_func.h"
#include "pop_func.h"

static void config_wan_static(void)
{
	char name[32];
	char buf[256];
	int i;
	int rc;

	/* IP address */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, STATIC_IPADDR), buf, sizeof(buf));
	rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, IPADDR), buf);

	/* netmask */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, STATIC_NETMASK), buf, sizeof(buf));
	rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, NETMASK), buf);

	/* gateway IP address */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, STATIC_GATEWAY), buf, sizeof(buf));
	rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, GATEWAY), buf);

	/* DNS */
	for(i=1; i<=3; i++) {
		snprintf(name, sizeof(name), "%s%d", NVRAM_SERVICE_OPTION(WAN, STATIC_DNS), i);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		snprintf(name, sizeof(name), "%s%d", NVRAM_SERVICE_OPTION(WAN, DNS), i);
		rc = ezcfg_api_nvram_set(name, buf);
	}

	pop_etc_resolv_conf(RC_START);
}

static void deconfig_wan_static(void)
{
	char name[32];
	int i;
	int rc;

	pop_etc_resolv_conf(RC_STOP);

	/* IP address */
	rc = ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(WAN, IPADDR));

	/* netmask */
	rc = ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(WAN, NETMASK));

	/* gateway IP address */
	rc = ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(WAN, GATEWAY));

	/* DNS */
	for(i=1; i<=3; i++) {
		snprintf(name, sizeof(name), "%s%d", NVRAM_SERVICE_OPTION(WAN, DNS), i);
		rc = ezcfg_api_nvram_unset(name);
	}
}

static int start_wan(void)
{
	char wan_ifname[IFNAMSIZ];
	char buf[256];
	int wan_type;
	int rc;

	wan_type = utils_get_wan_type();
	if (wan_type == WAN_TYPE_UNKNOWN)
		return (EXIT_FAILURE);

	switch (wan_type) {
	case WAN_TYPE_DHCP :
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, DHCP_IFNAME), wan_ifname, sizeof(wan_ifname));
		if (rc < 0)
			return (EXIT_FAILURE);

		/* start DHCP client process */
		snprintf(buf, sizeof(buf), "%s %s &", CMD_IFUP, wan_ifname);
		system(buf);

		break;

	case WAN_TYPE_STATIC :
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, STATIC_IFNAME), wan_ifname, sizeof(wan_ifname));
		if (rc < 0)
			return (EXIT_FAILURE);

		/* config Static IP address */
		snprintf(buf, sizeof(buf), "%s %s", CMD_IFUP, wan_ifname);
		system(buf);

		config_wan_static();

		/* start WAN interface binding services */
		rc_wan_services(RC_START);

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

	switch (wan_type) {
	case WAN_TYPE_DHCP :
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, DHCP_IFNAME), wan_ifname, sizeof(wan_ifname));
		if (rc < 0)
			return (EXIT_FAILURE);

		/* stop DHCP client process */
		snprintf(buf, sizeof(buf), "%s %s", CMD_IFDOWN, wan_ifname);
		system(buf);

		break;

	case WAN_TYPE_STATIC :
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, STATIC_IFNAME), wan_ifname, sizeof(wan_ifname));
		if (rc < 0)
			return (EXIT_FAILURE);

		/* stop WAN interface binding services */
		rc_wan_services(RC_STOP);

		deconfig_wan_static();

		/* deconfig Static IP address */
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
