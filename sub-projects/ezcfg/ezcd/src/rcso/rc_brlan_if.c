/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_brlan_if.c
 *
 * Description  : ezbox ethernet LAN bridge interface manipulate
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2012-09-05   0.1       Write it from scratch
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

#if 1
#define DBG(format, args...) do {\
	FILE *dbg_fp = fopen("/tmp/brlan_if.log", "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, arg...)
#endif

static int brlan_if_user_defined(int argc, char **argv)
{
	int i, ret;
	char cmdline[256];
	if (argc < 1) {
		return (EXIT_FAILURE);
	}

	snprintf(cmdline, sizeof(cmdline), "%s", CMD_BRCTL);
	for (i = 0; i < argc; i++) {
		strcat(cmdline, " ");
		strcat(cmdline, argv[i]);
	}
	ret = utils_system(cmdline);
	if (ret == 0)
		return (EXIT_SUCCESS);
	else
		return (EXIT_FAILURE);
}

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int rc_brlan_if(int argc, char **argv)
#endif
{
	char brlan_ifname[IFNAMSIZ];
	char brlan_ifnames[64];
	int if_argc;
	char *if_argv[8];
	char cmdline[256];
	int flag, ret;
	int i;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "brlan_if")) {
		return (EXIT_FAILURE);
	}

	ret = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(BRLAN, IFNAME), brlan_ifname, sizeof(brlan_ifname));
	if (ret < 0) {
		return (EXIT_FAILURE);
	}

	ret = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(BRLAN, IFNAMES), brlan_ifnames, sizeof(brlan_ifnames));
	if (ret < 0) {
		return (EXIT_FAILURE);
	}
	if_argc = utils_parse_args(brlan_ifnames, strlen(brlan_ifnames) + 1, if_argv);

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[1]);

	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_STOP :
		/* bring down LAN bridge interface */
		snprintf(cmdline, sizeof(cmdline), "%s link set %s down", CMD_IP, brlan_ifname);
		ret = utils_system(cmdline);
		/* remove interfaces from bridge */
		for (i = 0; i < if_argc; i++) {
			snprintf(cmdline, sizeof(cmdline), "%s delif %s %s", CMD_BRCTL, brlan_ifname, if_argv[i]);
			ret = utils_system(cmdline);
		}
		/* remove from bridge */
		snprintf(cmdline, sizeof(cmdline), "%s delbr %s", CMD_BRCTL, brlan_ifname);
		ret = utils_system(cmdline);

		if (flag == RC_ACT_STOP) {
			ret = EXIT_SUCCESS;
			break;
		}

		/* RC_ACT_RESTART fall through */
	case RC_ACT_BOOT :
	case RC_ACT_START :
		/* add bridge */
		snprintf(cmdline, sizeof(cmdline), "%s addbr %s", CMD_BRCTL, brlan_ifname);
		ret = utils_system(cmdline);

		/* add interfaces to bridge */
		for (i = 0; i < if_argc; i++) {
			snprintf(cmdline, sizeof(cmdline), "%s addif %s %s", CMD_BRCTL, brlan_ifname, if_argv[i]);
			ret = utils_system(cmdline);
		}
		/* bring up LAN interface, but not config it */
		snprintf(cmdline, sizeof(cmdline), "%s link set %s up", CMD_IP, brlan_ifname);
		ret = utils_system(cmdline);
		ret = EXIT_SUCCESS;
		break;

	case RC_ACT_USRDEF :
		ret = brlan_if_user_defined(argc-2, &(argv[2]));
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}

	return (ret);
}
