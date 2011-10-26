/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_dnsmasq.c
 *
 * Description  : ezbox run dns & dhcp service
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-17   0.1       Write it from scratch
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

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int rc_dnsmasq(int argc, char **argv)
#endif
{
	int rc;
	int flag, ret;

	if (argc < 3) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "dnsmasq")) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[1]);

	if (strcmp(argv[1], "lan") == 0 &&
            utils_service_binding_lan(NVRAM_SERVICE_OPTION(RC, DNSMASQ_BINDING)) == true) {
		/* It's good */
	}
	else if (strcmp(argv[1], "wan") == 0 &&
            utils_service_binding_wan(NVRAM_SERVICE_OPTION(RC, DNSMASQ_BINDING)) == true) {
		/* It's good */
	}
	else {
		return (EXIT_FAILURE);
	}

	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_STOP :
		system("start-stop-daemon -K -n dnsmasq");
		if (flag == RC_ACT_STOP) {
			ret = EXIT_SUCCESS;
			break;
		}

		/* RC_ACT_RESTART fall through */
	case RC_ACT_START :
		rc = utils_nvram_match(NVRAM_SERVICE_OPTION(RC, DNSMASQ_ENABLE), "1");
		if (rc < 0) {
			return (EXIT_FAILURE);
		}

		pop_etc_dnsmasq_conf(RC_ACT_START);
		system("start-stop-daemon -S -n dnsmasq -a /usr/sbin/dnsmasq");
		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}
	return (ret);
}
