/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_mdev.c
 *
 * Description  : ezbox run mdev to generate /dev/ node service
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-08-11   0.1       Write it from scratch
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

int rc_mdev(int flag)
{
	FILE *file;
	int ret;
	char cmdline[256];


	switch (flag) {
	case RC_BOOT :
	case RC_START :
	case RC_RESTART :

		pop_etc_mdev_conf(flag);

		file = fopen("/proc/sys/kernel/hotplug", "w");
                if (file != NULL)
		{
			fprintf(file, "%s", CMD_MDEV);
			fclose(file);
		}

		if (flag == RC_BOOT) {
			snprintf(cmdline, sizeof(cmdline), "%s -s", CMD_MDEV);
			ret = system(cmdline);
		}

		break;

	case RC_STOP :
		file = fopen("/proc/sys/kernel/hotplug", "w");
                if (file != NULL)
		{
			fprintf(file, "%s", "");
			fclose(file);
		}

		break;
	}

	return (EXIT_SUCCESS);
}
