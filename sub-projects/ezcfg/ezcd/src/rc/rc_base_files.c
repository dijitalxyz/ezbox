/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_base_files.c
 *
 * Description  : ezbox run base files service
 *
 * Copyright (C) 2010 by ezbox-project
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
#include "pop_func.h"

int rc_base_files(int flag)
{
	char cmdline[256];

	switch (flag) {
	case RC_START :
		/* set hostname */
		pop_etc_hostname(RC_START);
		snprintf(cmdline, sizeof(cmdline), "%s /etc/hostname > /proc/sys/kernel/hostname", CMD_CAT);		
		system(cmdline);

		/* generate /etc/profile */
		pop_etc_profile(RC_START);

		/* generate /etc/banner */
		pop_etc_banner(RC_START);

		/* generate /etc/mtab */
		pop_etc_mtab(RC_START);
		break;
	}

	return (EXIT_SUCCESS);
}