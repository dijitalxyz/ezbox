/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_wpa_supplicant_conf.c
 *
 * Description  : ezbox /etc/wpa_supplicant.conf file generating program
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-08-02   0.1       Write it from scratch
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

int pop_etc_wpa_supplicant_conf(int flag)
{
        FILE *file = NULL;
	char name[64];
	char buf[64];
	int rc;

	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WLAN, IFNAME), buf, sizeof(buf));
	if (rc <= 0)
		return (EXIT_FAILURE);

	/* generate /etc/wpa_supplicant-$ifname.conf */
	snprintf(name, sizeof(name), "/etc/wpa_supplicant-%s.conf", buf);
	file = fopen(buf, "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	switch (flag) {
	case RC_START :
		/* Set the wpa_cli control interface */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WLAN, IFNAME), buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "ctrl_interface=/var/run/wpa_supplicant-%s.conf\n", buf);
		}

		/* Set the wireless network */
		fprintf(file, "network={\n");

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, SCAN_SSID));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", SERVICE_OPTION(WPA_SUPPLICANT, SCAN_SSID), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, BSSID));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", SERVICE_OPTION(WPA_SUPPLICANT, BSSID), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, KEY_MGMT));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", SERVICE_OPTION(WPA_SUPPLICANT, KEY_MGMT), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, PROTO));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", SERVICE_OPTION(WPA_SUPPLICANT, PROTO), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, IEEE80211W));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", SERVICE_OPTION(WPA_SUPPLICANT, IEEE80211W), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, PSK));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", SERVICE_OPTION(WPA_SUPPLICANT, PSK), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, PAIRWISE));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", SERVICE_OPTION(WPA_SUPPLICANT, PAIRWISE), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, GROUP));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", SERVICE_OPTION(WPA_SUPPLICANT, GROUP), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, EAP));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", SERVICE_OPTION(WPA_SUPPLICANT, EAP), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, CA_CERT));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", SERVICE_OPTION(WPA_SUPPLICANT, CA_CERT), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, PRIV_KEY));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", SERVICE_OPTION(WPA_SUPPLICANT, PRIV_KEY), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, PRIV_KEY_PWD));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", SERVICE_OPTION(WPA_SUPPLICANT, PRIV_KEY_PWD), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, PHASE2));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", SERVICE_OPTION(WPA_SUPPLICANT, PHASE2), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, IDENTITY));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", SERVICE_OPTION(WPA_SUPPLICANT, IDENTITY), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, PASSWORD));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", SERVICE_OPTION(WPA_SUPPLICANT, PASSWORD), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, WEP_KEY0));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", SERVICE_OPTION(WPA_SUPPLICANT, WEP_KEY0), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, WEP_KEY1));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", SERVICE_OPTION(WPA_SUPPLICANT, WEP_KEY1), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, WEP_KEY2));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", SERVICE_OPTION(WPA_SUPPLICANT, WEP_KEY2), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, WEP_KEY3));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=\"%s\"\n", SERVICE_OPTION(WPA_SUPPLICANT, WEP_KEY3), buf);
		}

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WPA_SUPPLICANT, WEP_TX_KEYIDX));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "\t%s=%s\n", SERVICE_OPTION(WPA_SUPPLICANT, WEP_TX_KEYIDX), buf);
		}

		/* end of wireless network */
		fprintf(file, "}\n");

		break;
	}

	fclose(file);
	return (EXIT_SUCCESS);
}
