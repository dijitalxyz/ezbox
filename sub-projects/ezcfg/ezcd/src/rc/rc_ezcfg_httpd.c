/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_ezcfg_httpd.c
 *
 * Description  : ezbox run ezcfg httpd service
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-20   0.1       Write it from scratch
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
#include "rc_func.h"

int rc_ezcfg_httpd(int flag)
{
	int rc;
	int ip[4];
	char buf[256];

	rc = nvram_match(NVRAM_SERVICE_OPTION(EZCFG, HTTPD_ENABLE), "1");
	if (rc < 0) {
		return (EXIT_FAILURE);
	}

	buf[0] = '\0';
#if (HAVE_EZBOX_LAN_NIC == 1)
	if (utils_service_binding_lan(NVRAM_SERVICE_OPTION(EZCFG, HTTPD_BINDING)) == true) {
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(LAN, IPADDR), buf, sizeof(buf));
	} else
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
	if (utils_service_binding_wan(NVRAM_SERVICE_OPTION(EZCFG, HTTPD_BINDING)) == true) {
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, IPADDR), buf, sizeof(buf));
	} else
#endif
#if ((HAVE_EZBOX_LAN_NIC == 1) || (HAVE_EZBOX_WAN_NIC == 1))
	{
		return (EXIT_FAILURE);
	}
#endif

	if (rc < 0) {
		return (EXIT_FAILURE);
	}
	rc = sscanf(buf, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
	if (rc != 4) {
		return (EXIT_FAILURE);
	}

	snprintf(buf, sizeof(buf), "%d.%d.%d.%d:%s",
		ip[0], ip[1], ip[2], ip[3],
		EZCFG_PROTO_HTTP_PORT_NUMBER_STRING);

	switch (flag) {
	case RC_START :
		/* add ezcfg httpd listening socket */
		rc = ezcfg_api_nvram_insert_socket(
			EZCFG_SOCKET_DOMAIN_INET_STRING,
			EZCFG_SOCKET_TYPE_STREAM_STRING,
			EZCFG_SOCKET_PROTO_HTTP_STRING,
			buf);

		/* restart ezcfg daemon */
		if (rc >= 0) {
			rc_ezcd(RC_RELOAD);
		}
		break;

	case RC_STOP :
		/* delete ezcfg httpd listening socket */
		rc = ezcfg_api_nvram_remove_socket(
			EZCFG_SOCKET_DOMAIN_INET_STRING,
			EZCFG_SOCKET_TYPE_STREAM_STRING,
			EZCFG_SOCKET_PROTO_HTTP_STRING,
			buf);

		/* restart ezcfg daemon */
		if (rc >= 0) {
			rc_ezcd(RC_RELOAD);
		}
		break;
	}

	return (EXIT_SUCCESS);
}

#if (HAVE_EZBOX_LAN_NIC == 1)
int rc_lan_ezcfg_httpd(int flag)
{
	if (utils_service_binding_lan(NVRAM_SERVICE_OPTION(EZCFG, HTTPD_BINDING)) == true) {
		return rc_ezcfg_httpd(flag);
	}
	else {
		return (EXIT_FAILURE);
	}
}
#endif

#if (HAVE_EZBOX_WAN_NIC == 1)
int rc_wan_ezcfg_httpd(int flag)
{
	if (utils_service_binding_wan(NVRAM_SERVICE_OPTION(EZCFG, HTTPD_BINDING)) == true) {
		return rc_ezcfg_httpd(flag);
	}
	else {
		return (EXIT_FAILURE);
	}
}
#endif
