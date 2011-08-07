/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_wpa_supplicant.c
 *
 * Description  : ezbox run wpa_supplicant service
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-08-04   0.1       Write it from scratch
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
#include "pop_func.h"

#if 0
#define DBG(format, args...) do {\
	FILE *fp = fopen("/tmp/wpa_supplicant.debug", "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif


int rc_wpa_supplicant(int flag)
{
	char wlan_nic[IFNAMSIZ];
	char cmd[256];
	int rc;

	switch (flag) {
	case RC_START :
		rc = nvram_match(NVRAM_SERVICE_OPTION(RC, WPA_SUPPLICANT_ENABLE), "1");
		if (rc < 0) {
			return (EXIT_FAILURE);
		}

		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(SYS, WLAN_NIC), wlan_nic, sizeof(wlan_nic));
		if (rc <= 0) {
			return (EXIT_FAILURE);
		}

		pop_etc_wpa_supplicant_conf(RC_START);
		snprintf(cmd, sizeof(cmd), "start-stop-daemon -S -n wpa_supplicant -a /usr/sbin/wpa_supplicant -- -B -D%s -i%s -c/etc/wpa_supplicant-%s.conf", "nl80211", wlan_nic, wlan_nic);
#if 0
		system("start-stop-daemon -S -n wpa_supplicant -a /usr/sbin/wpa_supplicant -- -i wlan0 -c /etc/wpa_supplicant-wlan0.conf");
#else
		system(cmd);
#endif
		break;

	case RC_STOP :
		system("start-stop-daemon -K -n wpa_supplicant");
		break;

	case RC_RESTART :
		rc = rc_wpa_supplicant(RC_STOP);
		sleep(1);
		rc = rc_wpa_supplicant(RC_START);
		break;
	}
	return (EXIT_SUCCESS);
}
