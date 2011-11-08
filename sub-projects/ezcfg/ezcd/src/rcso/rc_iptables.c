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
 * 2011-10-22   0.2       Modify it to use rcso framework
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
static int lan_iptables(int flag)
{
	int rc;
	rc = utils_nvram_cmp(NVRAM_SERVICE_OPTION(RC, IPTABLES_ENABLE), "1");
	if (rc < 0) {
		return (EXIT_FAILURE);
	}

	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_STOP :

		if (flag == RC_ACT_STOP) {
			break;
		}

	/* RC_ACT_RESTART fall through */
	case RC_ACT_START :
		break;

	default :
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
#endif

#if (HAVE_EZBOX_WAN_NIC == 1)
static int wan_iptables(int flag)
{
	int rc;
	rc = utils_nvram_cmp(NVRAM_SERVICE_OPTION(RC, IPTABLES_ENABLE), "1");
	if (rc < 0) {
		return (EXIT_FAILURE);
	}

	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_STOP :
		pop_etc_l7_protocols(RC_ACT_STOP);
		if (flag == RC_ACT_STOP) {
			break;
		}

	/* RC_ACT_RESTART fall through */
	case RC_ACT_START :
		pop_etc_l7_protocols(RC_ACT_START);
		break;

	default :
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
#endif

/* FIXME: be careful of the sequences */
static char * iptables_modules[] = {
	"x_tables",
	"ip_tables",
	"iptable_filter",
	"nf_conntrack",
	"nf_defrag_ipv4",
	"nf_conntrack_ipv4",
	"nf_nat",
	"iptable_nat",
	"xt_state",
	"xt_tcpudp",
};

static int load_iptables_modules(int flag)
{
	int ret, i;

	switch (flag) {
	case RC_ACT_BOOT :
		/* manage iptables configuration options */
		mkdir("/etc/l7-protocols", 0755);
		ret = EXIT_SUCCESS;
		break;

	case RC_ACT_START :
		for (i = 0; i < ARRAY_SIZE(iptables_modules); i++) {
			utils_install_kernel_module(iptables_modules[i], NULL);
		}
		ret = EXIT_SUCCESS;
		break;

	case RC_ACT_STOP :
		for (i = ARRAY_SIZE(iptables_modules)-1; i >= 0; i--) {
			utils_remove_kernel_module(iptables_modules[i]);
		}
		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}

	return ret;
}

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int rc_iptables(int argc, char **argv)
#endif
{
	int flag;

	if (argc < 3) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "iptables")) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[2]);

	if (strcmp(argv[1], "load") == 0) {
		return load_iptables_modules(flag);
	}
#if (HAVE_EZBOX_LAN_NIC == 1)
	else if (strcmp(argv[1], "lan") == 0) {
		return lan_iptables(flag);
	}
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
	else if (strcmp(argv[1], "wan") == 0) {
		return wan_iptables(flag);
	}
#endif
	else {
		return (EXIT_FAILURE);
	}
}
