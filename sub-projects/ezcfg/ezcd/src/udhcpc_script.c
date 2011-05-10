/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : udhcpc_script.c
 *
 * Description  : ezbox udhcpc script program
 *
 * Copyright (C) 2010-2011 by ezbox-project
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
#include "pop_func.h"

static int udhcpc_deconfig(void)
{
	char buf[128];
	char *iface;

	iface = getenv("interface");
	if (iface == NULL)
		return (EXIT_FAILURE);

	snprintf(buf, sizeof(buf), "%s %s 0.0.0.0", CMD_IFCONFIG, iface);
	system(buf);
	return (EXIT_SUCCESS);
}

static int udhcpc_bound(void)
{
	char buf[128];
	char *iface;
	char *ipaddr;
	char *subnet;
	char *bcast;
	char *router;
	char *domain;
	char *dns;
	char *p, *savep, *token;
	//FILE *file;
	int status, i, rc;

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
	system(buf);

	rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, IPADDR), ipaddr);
	if (subnet != NULL) {
		rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, NETMASK), subnet);
	}

	router = getenv("router");
	if (router != NULL) {
		printf("deleting routers\n");
		snprintf(buf, sizeof(buf), "%s del default gw 0.0.0.0 dev %s", CMD_ROUTE, iface);
		status = system(buf);
		while((WIFEXITED(status) == true) &&
		      (WEXITSTATUS(status) == 0)) {
			status = system(buf);
		}

		for (i = 1, p = router; ; i++, p = NULL) {
			token = strtok_r(p, " ", &savep);
			if (token == NULL)
				break;
			snprintf(buf, sizeof(buf), "%s add default gw %s dev %s metric %d",
			         CMD_ROUTE, token, iface, i);
			system(buf);
		}

		rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, GATEWAY), router);
	}

#if 0
	snprintf(buf, sizeof(buf), "/etc/resolv.conf-%s", iface);
	file = fopen(buf, "w");
	if (file != NULL) {
		domain = getenv("domain");
		if (domain != NULL) {
        		printf("adding domain %s to %s\n", domain, buf);
			fprintf(file, "domain %s\n", domain);
		}

		dns = getenv("dns");
		if (dns != NULL) {
			for (p = dns; ; p = NULL) {
				token = strtok_r(p, " ", &savep);
				if (token == NULL)
					break;
        			printf("adding dns %s to %s\n", token, buf);
				fprintf(file, "nameserver %s\n", token);
			}
		}
		fclose(file);
	}
#endif
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

	pop_etc_resolv_conf(RC_RESTART);

	return (EXIT_SUCCESS);
}

static int udhcpc_renew(void)
{
	char buf[128];
	char *iface;
	char *ipaddr;
	char *subnet;
	char *bcast;
	char *router;
	char *domain;
	char *dns;
	char *p, *savep, *token;
	//FILE *file;
	int status, i, rc;

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
	system(buf);

	rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, IPADDR), ipaddr);
	if (subnet != NULL) {
		rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, NETMASK), subnet);
	}

	router = getenv("router");
	if (router != NULL) {
		printf("deleting routers\n");
		snprintf(buf, sizeof(buf), "%s del default gw 0.0.0.0 dev %s", CMD_ROUTE, iface);
		status = system(buf);
		while((WIFEXITED(status) == true) &&
		      (WEXITSTATUS(status) == 0)) {
			status = system(buf);
		}

		for (i = 1, p = router; ; i++, p = NULL) {
			token = strtok_r(p, " ", &savep);
			if (token == NULL)
				break;
			snprintf(buf, sizeof(buf), "%s add default gw %s dev %s metric %d",
			         CMD_ROUTE, token, iface, i);
			system(buf);
		}

		rc = ezcfg_api_nvram_set(NVRAM_SERVICE_OPTION(WAN, GATEWAY), router);
	}

#if 0
	snprintf(buf, sizeof(buf), "/etc/resolv.conf-%s", iface);
	file = fopen(buf, "w");
	if (file != NULL) {
		domain = getenv("domain");
		if (domain != NULL) {
        		printf("adding domain %s to %s\n", domain, buf);
			fprintf(file, "domain %s\n", domain);
		}

		dns = getenv("dns");
		if (dns != NULL) {
			for (p = dns; ; p = NULL) {
				token = strtok_r(p, " ", &savep);
				if (token == NULL)
					break;
        			printf("adding dns %s to %s\n", token, buf);
				fprintf(file, "nameserver %s\n", token);
			}
		}
		fclose(file);
	}
#endif
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

	pop_etc_resolv_conf(RC_RESTART);

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
		printf("error, argc=[%d]\n", argc);
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[1], "deconfig") == 0) {
		printf("udhcpc operation %s\n", argv[1]);
		ret = udhcpc_deconfig();
	}
	else if (strcmp(argv[1], "bound") == 0) {
		printf("udhcpc operation %s\n", argv[1]);
		ret = udhcpc_bound();
	}
	else if (strcmp(argv[1], "renew") == 0) {
		printf("udhcpc operation %s\n", argv[1]);
		ret = udhcpc_renew();
	}
	else if (strcmp(argv[1], "nak") == 0) {
		printf("udhcpc operation %s\n", argv[1]);
		ret = udhcpc_nak();
	}
	else {
		printf("unknown operation %s\n", argv[1]);
		ret = EXIT_FAILURE;
	}

	return ret;
}
