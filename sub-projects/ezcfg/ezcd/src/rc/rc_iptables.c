/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_iptables.c
 *
 * Description  : firewall and port-mapping
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-17   0.1       Write it from scratch
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

#if (HAVE_EZBOX_LAN_NIC == 1)
int rc_lan_iptables(int flag)
{
	int rc;
	rc = nvram_match(NVRAM_SERVICE_OPTION(RC, IPTABLES_ENABLE), "1");
	if (rc < 0) {
		return (EXIT_FAILURE);
	}

	switch (flag) {
	case RC_START :
		break;

	case RC_STOP :
		break;

	case RC_RESTART :
		rc = rc_lan_iptables(RC_STOP);
		rc = rc_lan_iptables(RC_START);
		break;
	}
	return (EXIT_SUCCESS);
}
#endif

#if (HAVE_EZBOX_WAN_NIC == 1)
int rc_wan_iptables(int flag)
{
	int rc;
	rc = nvram_match(NVRAM_SERVICE_OPTION(RC, IPTABLES_ENABLE), "1");
	if (rc < 0) {
		return (EXIT_FAILURE);
	}

	switch (flag) {
	case RC_START :
		pop_etc_l7_protocols(RC_START);
		break;

	case RC_STOP :
		pop_etc_l7_protocols(RC_STOP);
		break;

	case RC_RESTART :
		rc = rc_wan_iptables(RC_STOP);
		rc = rc_wan_iptables(RC_START);
		break;
	}
	return (EXIT_SUCCESS);
}
#endif

int rc_iptables(int flag)
{
	switch (flag) {
	case RC_BOOT :
		/* manage iptables configuration options */
		mkdir("/etc/l7-protocols", 0755);
		break;
	}
	return (EXIT_SUCCESS);
}
