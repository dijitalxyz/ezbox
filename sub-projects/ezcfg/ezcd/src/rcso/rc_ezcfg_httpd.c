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
 * 2011-10-21   0.1       Modify it to use rcso frame
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

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int rc_ezcfg_httpd(int argc, char **argv)
#endif
{
	int rc = -1;
	int ip[4];
	char buf[256];
	int flag, ret;

	if (argc < 3) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "ezcfg_httpd")) {
		return (EXIT_FAILURE);
	}

	buf[0] = '\0';
#if (HAVE_EZBOX_LAN_NIC == 1)
	if (strcmp(argv[1], "lan") == 0 &&
	    utils_service_binding_lan(NVRAM_SERVICE_OPTION(EZCFG, HTTPD_BINDING)) == true) {
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(LAN, IPADDR), buf, sizeof(buf));
		if (rc < 0) {
			return (EXIT_FAILURE);
		}
	} else
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
	if (strcmp(argv[1], "wan") == 0 &&
	    utils_service_binding_wan(NVRAM_SERVICE_OPTION(EZCFG, HTTPD_BINDING)) == true) {
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, IPADDR), buf, sizeof(buf));
		if (rc < 0) {
			return (EXIT_FAILURE);
		}
	} else
#endif
#if ((HAVE_EZBOX_LAN_NIC == 1) || (HAVE_EZBOX_WAN_NIC == 1))
	{
		return (EXIT_FAILURE);
	}
#endif

	rc = sscanf(buf, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
	if (rc != 4) {
		return (EXIT_FAILURE);
	}

	snprintf(buf, sizeof(buf), "%d.%d.%d.%d:%s",
		ip[0], ip[1], ip[2], ip[3],
		EZCFG_PROTO_HTTP_PORT_NUMBER_STRING);

	flag = utils_get_rc_act_type(argv[2]);

	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_STOP :
		/* delete ezcfg httpd listening socket */
		rc = ezcfg_api_nvram_remove_socket(
			EZCFG_SOCKET_DOMAIN_INET_STRING,
			EZCFG_SOCKET_TYPE_STREAM_STRING,
			EZCFG_SOCKET_PROTO_HTTP_STRING,
			buf);

		/* restart ezcfg daemon */
		/* FIXME: do it in action config file */
#if 0
		if (rc >= 0) {
			rc_ezcd(RC_ACT_RELOAD);
		}
#endif
		if (flag == RC_ACT_STOP) {
			ret = EXIT_SUCCESS;
			break;
		}

		/* RC_ACT_RESTART fall through */

	case RC_ACT_START :
		rc = utils_nvram_match(NVRAM_SERVICE_OPTION(EZCFG, HTTPD_ENABLE), "1");
		if (rc < 0) {
			return (EXIT_FAILURE);
		}

		/* add ezcfg httpd listening socket */
		rc = ezcfg_api_nvram_insert_socket(
			EZCFG_SOCKET_DOMAIN_INET_STRING,
			EZCFG_SOCKET_TYPE_STREAM_STRING,
			EZCFG_SOCKET_PROTO_HTTP_STRING,
			buf);

		/* restart ezcfg daemon */
		/* FIXME: do it in config file */
#if 0
		if (rc >= 0) {
			rc_ezcd(RC_ACT_RELOAD);
		}
#endif
		ret = EXIT_SUCCESS;
		break;

	default:
		ret = EXIT_FAILURE;
		break;
	}

	return (ret);
}
