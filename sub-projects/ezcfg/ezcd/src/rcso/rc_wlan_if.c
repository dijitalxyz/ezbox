/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_wlan_if.c
 *
 * Description  : ezbox run network WLAN interface service
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-08-04   0.1       Write it from scratch
 * 2011-10-04   0.2       Modify it to use rcso framework
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

#if 0
static int start_wlan_if(void)
{
	int rc;
	char buf[64];

	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WLAN, NODE_TYPE), buf, sizeof(buf));
	if (rc > 0) {
#if (HAVE_EZBOX_SERVICE_WPA_SUPPLICANT == 1)
		if (strcmp(buf, "sta") == 0) {
			/* wireless client node */
			return rc_wpa_supplicant(RC_ACT_START);
		}
#endif
	}
	return (EXIT_FAILURE);
}

static int stop_wlan_if(void)
{
	int rc;
	char buf[64];

	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WLAN, NODE_TYPE), buf, sizeof(buf));
	if (rc > 0) {
#if (HAVE_EZBOX_SERVICE_WPA_SUPPLICANT == 1)
		if (strcmp(buf, "sta") == 0) {
			/* wireless client node */
			return rc_wpa_supplicant(RC_ACT_STOP);
		}
#endif
	}
	return (EXIT_FAILURE);
}
#endif

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int rc_wlan_if(int argc, char **argv)
#endif
{
	char wlan_ifname[IFNAMSIZ];
	char cmdline[256];
	int flag, ret;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "wlan_if")) {
		return (EXIT_FAILURE);
	}

	ret = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(SYS, WLAN_NIC), wlan_ifname, sizeof(wlan_ifname));
	if (ret < 0)
		return (EXIT_FAILURE);

	flag = utils_get_rc_act_type(argv[1]);

	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_STOP :
		/* clean WLAN interface phy link */
#if 0
		ret = stop_wlan_if();
#endif
		/* bring down WLAN interface */
		snprintf(cmdline, sizeof(cmdline), "%s %s down", CMD_IFCONFIG, wlan_ifname);
		ret = system(cmdline);
		if (flag == RC_ACT_STOP) {
			ret = EXIT_SUCCESS;
			break;
		}

		/* RC_ACT_RESTART fall through */
	case RC_ACT_BOOT :
	case RC_ACT_START :
		/* bring up WLAN interface, but not config it */
		snprintf(cmdline, sizeof(cmdline), "%s %s up", CMD_IFCONFIG, wlan_ifname);
		ret = system(cmdline);
		/* setup WLAN interface phy link */
#if 0
		ret = start_wlan_if();
#endif
		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}

	return (ret);
}
