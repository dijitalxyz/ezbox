/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : main.c
 *
 * Description  : EZCD main program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-06-13   0.1       Write it from scratch
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

int main(int argc, char **argv)
{
	char *name = strrchr(argv[0], '/');
	name = name ? name+1 : argv[0];

	if (!strcmp(argv[0], "/init")) {
		return preinit_main(argc, argv);
	}
	else if (!strcmp(argv[0], "/sbin/init")) {
		return init_main(argc, argv);
	}
	else if (!strcmp(argv[0], "/sbin/agent_env")) {
		return agent_env_main(argc, argv);
	}
	else if (!strcmp(name, "rc")) {
		return rc_main(argc, argv);
	}
	else if (!strcmp(name, "ezcd")) {
		return ezcd_main(argc, argv);
	}
	else if (!strcmp(name, "ezcm")) {
		return ezcm_main(argc, argv);
	}
	else if (!strcmp(name, "nvram")) {
		return nvram_main(argc, argv);
	}
	else if (!strcmp(name, "shelld")) {
		return shelld_main(argc, argv);
	}
#if (HAVE_EZBOX_SERVICE_EZCFG_IGRSD == 1)
	else if (!strcmp(name, "igrs_monitor")) {
		return igrs_monitor_main(argc, argv);
	}
#endif
#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD == 1)
	else if (!strcmp(name, "upnp_monitor")) {
		return upnp_monitor_main(argc, argv);
	}
#endif
	else if (!strcmp(name, "ubootenv")) {
		return ubootenv_main(argc, argv);
	}
#if (HAVE_EZBOX_WAN_NIC == 1)
	else if (!strcmp(name, "udhcpc.script") ||
	         !strcmp(argv[0], UDHCPC_SCRIPT_FILE_PATH)) {
		return udhcpc_script_main(argc, argv);
	}
#endif
	else if (!strcmp(name, "upfw")) {
		return upfw_main(argc, argv);
	}
	else {
		printf("Unkown name [%s]!\n", name);
		return (EXIT_FAILURE);
	}
}
