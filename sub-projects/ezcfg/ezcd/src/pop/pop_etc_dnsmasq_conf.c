/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_dnsmasq_conf.c
 *
 * Description  : ezbox /etc/dnsmasq.conf file generating program
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-02   0.1       Write it from scratch
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

int pop_etc_dnsmasq_conf(int flag)
{
        FILE *file = NULL;
	char buf[32];
	int rc;

	/* generate /etc/hosts */
	file = fopen("/etc/dnsmasq.conf", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	switch (flag) {
	case RC_START :
		/* Never forward plain names (without a dot or domain part) */
		if (nvram_match(NVRAM_SERVICE_OPTION(DNSMASQ, DOMAIN_NEEDED), "1") == 0) {
			fprintf(file, "%s\n", SERVICE_OPTION(DNSMASQ, DOMAIN_NEEDED));
		}

		/* get upstream servers from somewhere other that /etc/resolv.conf */
		if (nvram_match(NVRAM_SERVICE_OPTION(LAN, DHCPD_WAN_DNS_ENABLE), "0") == 0) {
			FILE *fp = NULL;
			int i;
			char name[32];
			fprintf(file, "%s=%s\n", SERVICE_OPTION(DNSMASQ, RESOLV_FILE), "/etc/resolv.conf.dnsmasq");
			fp = fopen("/etc/resolv.conf.dnsmasq", "w");
			if (fp != NULL) {
				for (i = 1; i <= 3; i++) {
					snprintf(name, sizeof(name), "%s%d",
						NVRAM_SERVICE_OPTION(LAN, DHCPD_DNS), i);
					rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
					if (buf[0] != '\0') {
						fprintf(file, "nameserver %s\n", buf);
					}
				}
				fclose(fp);
			}
		}

		/* listen for DHCP and DNS requests only on specified interfaces */
		fprintf(file, "%s=%s\n", SERVICE_OPTION(DNSMASQ, INTERFACE), "lo");
		if (utils_service_binding_lan(NVRAM_SERVICE_OPTION(RC, DNSMASQ_BINDING)) == true) {
			rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(LAN, IFNAME), buf, sizeof(buf));
			if (buf[0] != '\0') {
				fprintf(file, "%s=%s\n", SERVICE_OPTION(DNSMASQ, INTERFACE), buf);
			}
		}
		else if (utils_service_binding_wan(NVRAM_SERVICE_OPTION(RC, DNSMASQ_BINDING)) == true) {
			rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, IFNAME), buf, sizeof(buf));
			if (buf[0] != '\0') {
				fprintf(file, "%s=%s\n", SERVICE_OPTION(DNSMASQ, INTERFACE), buf);
			}
		}

		break;
	}

	fclose(file);
	return (EXIT_SUCCESS);
}
