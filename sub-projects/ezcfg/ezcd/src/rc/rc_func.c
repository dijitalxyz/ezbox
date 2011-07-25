/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_func.c
 *
 * Description  : ezbox rc service functions program
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-02   0.1       Write it from scratch
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

rc_func_t rc_functions[] = {
	{ "base_files", RC_BOOT, rc_base_files },
	{ "base_ldconfig", RC_BOOT, rc_ldconfig },
	{ "ezcd", RC_BOOT, rc_ezcd },
	{ "ezcfg_httpd", RC_BOOT, rc_ezcfg_httpd },
	{ "nvram", RC_BOOT, rc_nvram },
	{ "ezcm", RC_BOOT, rc_ezcm },
	{ "hotplug2", RC_BOOT, rc_hotplug2 },
	{ "init", RC_BOOT, rc_init },
	{ "load_modules", RC_BOOT, rc_load_modules },
	{ "login", RC_BOOT, rc_login },
	{ "loopback", RC_BOOT, rc_loopback },
	{ "netbase", RC_BOOT, rc_netbase },
	{ "system", RC_BOOT, rc_system },
#if (HAVE_EZBOX_LAN_NIC == 1)
	{ "lan", RC_BOOT, rc_lan },
	{ "lan_if", RC_BOOT, rc_lan_if },
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
	{ "wan", RC_BOOT, rc_wan },
	{ "wan_if", RC_BOOT, rc_wan_if },
#endif
#if (HAVE_EZBOX_SERVICE_TELNETD == 1)
	{ "telnetd", RC_BOOT, rc_telnetd },
#endif
#if (HAVE_EZBOX_SERVICE_DNSMASQ == 1)
	{ "dnsmasq", RC_BOOT, rc_dnsmasq },
#endif
#if (HAVE_EZBOX_SERVICE_NANO_X == 1)
	{ "nano_x", RC_BOOT, rc_nano_x },
#elif (HAVE_EZBOX_SERVICE_KDRIVE == 1)
	{ "kdrive", RC_BOOT, rc_kdrive },
#endif
#if (HAVE_EZBOX_SERVICE_DILLO == 1)
	{ "dillo", RC_BOOT, rc_dillo },
#endif
#if (HAVE_EZBOX_SERVICE_EMC2 == 1)
	{ "realtime", RC_BOOT, rc_realtime },
	{ "emc2", RC_BOOT, rc_emc2 },
	{ "emc2_latency_test", RC_BOOT, rc_emc2_latency_test },
#endif
};

rc_func_t *utils_find_rc_func(char *name)
{
	int i;
	rc_func_t *f;

	for (i = 0; i < ARRAY_SIZE(rc_functions); i++) {
		f = &rc_functions[i];
		if (strcmp(f->name, name) == 0) return f;
	}

	return NULL;
}
