/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_netbase.c
 *
 * Description  : ezbox run network base files service
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-03   0.1       Write it from scratch
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

int rc_netbase(int flag)
{
	switch (flag) {
	case RC_BOOT :
		/* manage network interfaces and configure some networking options */
		mkdir("/etc/network", 0755);
		mkdir("/etc/network/if-pre-up.d", 0755);
		mkdir("/etc/network/if-up.d", 0755);
		mkdir("/etc/network/if-down.d", 0755);
		mkdir("/etc/network/if-post-down.d", 0755);
		break;

	case RC_RESTART :
	case RC_START :
		pop_etc_network_interfaces(RC_START);
		pop_etc_hosts(RC_START);
		pop_etc_resolv_conf(RC_START);
		pop_etc_protocols(RC_START);
		break;
	}

	return (EXIT_SUCCESS);
}
