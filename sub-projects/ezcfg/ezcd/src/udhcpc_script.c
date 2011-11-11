/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : udhcpc_script.c
 *
 * Description  : ezbox udhcpc script program
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-03-01   0.1       Write it from scratch
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

#if 0
#define DBG(format, args...) do {\
	FILE *fp = fopen("/dev/kmsg", "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

static int udhcpc_deconfig(void)
{
	char buf[128];
	char *iface;
	int rc;
	int i;

	iface = getenv("interface");
	if (iface == NULL)
		return (EXIT_FAILURE);

	utils_system("/sbin/rc resolv_conf stop");

	rc = ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(WAN, DHCP_LEASE));
	rc = ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(WAN, IPADDR));
	rc = ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(WAN, NETMASK));
	rc = ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(WAN, GATEWAY));
	rc = ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(WAN, DOMAIN));
	for (i=1; i<=3; i++) {
		snprintf(buf, sizeof(buf), "%s%d", NVRAM_SERVICE_OPTION(WAN, DNS), i);
		rc = ezcfg_api_nvram_unset(buf);
	}

#if 0
	snprintf(buf, sizeof(buf), "%s %s %s", CMD_IFCONFIG, iface, "0.0.0.0");
	utils_system(buf);
#endif

	return (EXIT_SUCCESS);
}

static int udhcpc_bound(void)
{
	char buf[128];
	char *lease;
	char *iface;
	char *ipaddr;
	char *subnet;
	char *bcast;
	char *router;
	char *domain;
	char *dns;
	char *p, *savep, *token;
	int i, rc;
	//int status, i, rc;

	lease = getenv("lease");
	if (lease == NULL) {
		/* default set to 1 hour */
		rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, DHCP_LEASE), "3600");
	}
	else {
		rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, DHCP_LEASE), lease);
	}

	iface = getenv("interface");
	ipaddr = getenv("ip");
	if ((iface == NULL) || (ipaddr == NULL))
		return (EXIT_FAILURE);

	subnet = getenv("subnet");
	bcast = getenv("broadcast");

	snprintf(buf, sizeof(buf), "%s %s %s %s %s %s %s", CMD_IFCONFIG,
	         iface, ipaddr,
	         subnet == NULL ? "" : "netmask",
	         subnet == NULL ? "" : subnet,
	         bcast == NULL ? "" : "broadcast",
	         bcast == NULL ? "" : bcast);
	utils_system(buf);

	rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, IPADDR), ipaddr);
	if (subnet != NULL) {
		rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, NETMASK), subnet);
	}

	router = getenv("router");
	if (router != NULL) {
		printf("deleting routers\n");
#if 0
		snprintf(buf, sizeof(buf), "%s del default gw 0.0.0.0 dev %s", CMD_ROUTE, iface);
		status = utils_system(buf);
		while((WIFEXITED(status) == true) &&
		      (WEXITSTATUS(status) == 0)) {
			status = utils_system(buf);
		}
#else
		utils_route_delete_default(iface);
#endif

		for (i = 1, p = router; ; i++, p = NULL) {
			token = strtok_r(p, " ", &savep);
			if (token == NULL)
				break;
			snprintf(buf, sizeof(buf), "%s add default gw %s dev %s metric %d",
			         CMD_ROUTE, token, iface, i);
			utils_system(buf);
		}

		rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, GATEWAY), router);
	}

	domain = getenv("domain");
	if (domain != NULL) {
		rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, DOMAIN), domain);
	}

	dns = getenv("dns");
	if (dns != NULL) {
		char name[32];
		int i;
		char *p, *savep;
		snprintf(buf, sizeof(buf), "%s", dns);
		for (i = 1, p = buf; i <= 3; p = NULL, i++) {
			dns = strtok_r(p, " ", &savep);
			if (dns == NULL)
				break;
			snprintf(name, sizeof(name), "%s%d",
				NVRAM_SERVICE_OPTION(WAN, DNS), i);
			rc = ezcfg_api_nvram_set(name, dns);
		}
	}

	utils_system("/sbin/rc resolv_conf restart");

	/* start WAN interface binding services */
	utils_system("/sbin/rc action wan_services_start");

	return (EXIT_SUCCESS);
}

static int udhcpc_renew(void)
{
	char buf[128];
	char *lease;
	char *iface;
	char *ipaddr;
	char *subnet;
	char *bcast;
	char *router;
	char *domain;
	char *dns;
	char *p, *savep, *token;
	int i, rc;
	//int status, i, rc;

	lease = getenv("lease");
	if (lease == NULL) {
		/* default set to 1 hour */
		rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, DHCP_LEASE), "3600");
	}
	else {
		rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, DHCP_LEASE), lease);
	}

	iface = getenv("interface");
	ipaddr = getenv("ip");
	if ((iface == NULL) || (ipaddr == NULL))
		return (EXIT_FAILURE);

	subnet = getenv("subnet");
	bcast = getenv("broadcast");

	snprintf(buf, sizeof(buf), "%s %s %s %s %s %s %s", CMD_IFCONFIG,
	         iface, ipaddr,
	         subnet == NULL ? "" : "netmask",
	         subnet == NULL ? "" : subnet,
	         bcast == NULL ? "" : "broadcast",
	         bcast == NULL ? "" : bcast);
	utils_system(buf);

	rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, IPADDR), ipaddr);
	if (subnet != NULL) {
		rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, NETMASK), subnet);
	}

	router = getenv("router");
	if (router != NULL) {
		printf("deleting routers\n");
#if 0
		snprintf(buf, sizeof(buf), "%s del default gw 0.0.0.0 dev %s", CMD_ROUTE, iface);
		status = utils_system(buf);
		while((WIFEXITED(status) == true) &&
		      (WEXITSTATUS(status) == 0)) {
			status = utils_system(buf);
		}
#else
		utils_route_delete_default(iface);
#endif

		for (i = 1, p = router; ; i++, p = NULL) {
			token = strtok_r(p, " ", &savep);
			if (token == NULL)
				break;
			snprintf(buf, sizeof(buf), "%s add default gw %s dev %s metric %d",
			         CMD_ROUTE, token, iface, i);
			utils_system(buf);
		}

		rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, GATEWAY), router);
	}

	domain = getenv("domain");
	if (domain != NULL) {
		rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, DOMAIN), domain);
	}

	dns = getenv("dns");
	if (dns != NULL) {
		char name[32];
		int i;
		char *p, *savep;
		snprintf(buf, sizeof(buf), "%s", dns);
		for (i = 1, p = buf; i <= 3; p = NULL, i++) {
			dns = strtok_r(p, " ", &savep);
			if (dns == NULL)
				break;
			snprintf(name, sizeof(name), "%s%d",
				NVRAM_SERVICE_OPTION(WAN, DNS), i);
			rc = ezcfg_api_nvram_set(name, dns);
		}
	}

	utils_system("/sbin/rc resolv_conf restart");

	/* start WAN interface binding services */
	utils_system("/sbin/rc action wan_services_start");

	return (EXIT_SUCCESS);
}

static int udhcpc_nak(void)
{
	char *message;

	message = getenv("message");
	if (message != NULL) {
		printf("received a nak: %s\n", message);
	}
	return (EXIT_SUCCESS);
}

int udhcpc_script_main(int argc, char **argv)
{
	int ret;
	if (argc < 2) {
		DBG("error, argc=[%d]\n", argc);
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[1], "deconfig") == 0) {
		DBG("udhcpc operation %s\n", argv[1]);
		ret = udhcpc_deconfig();
	}
	else if (strcmp(argv[1], "bound") == 0) {
		DBG("udhcpc operation %s\n", argv[1]);
		ret = udhcpc_bound();
	}
	else if (strcmp(argv[1], "renew") == 0) {
		DBG("udhcpc operation %s\n", argv[1]);
		ret = udhcpc_renew();
	}
	else if (strcmp(argv[1], "nak") == 0) {
		DBG("udhcpc operation %s\n", argv[1]);
		ret = udhcpc_nak();
	}
	else {
		DBG("unknown operation %s\n", argv[1]);
		ret = EXIT_FAILURE;
	}

	return ret;
}
