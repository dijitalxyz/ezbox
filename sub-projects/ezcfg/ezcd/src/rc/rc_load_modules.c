/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_load_modules.c
 *
 * Description  : ezbox run load kernel modules service
 *
 * Copyright (C) 2010 by ezbox-project
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

int rc_load_modules(int flag)
{
	FILE *file;
	char cmdline[64];
	char buf[32];
	int ret;
	char *kver, *cmd;

	kver = utils_get_kernel_version();
	if (kver == NULL)
		return (EXIT_FAILURE);

	switch (flag) {
	case RC_BOOT :
	case RC_START :
		cmd = "insmod";
		break;
	case RC_STOP :
		cmd = "rmmod";
		break;
	default :
		return (EXIT_FAILURE);
	}

	pop_etc_modules(flag);

	file = fopen("/etc/modules", "r");
	if (file == NULL)
		return (EXIT_FAILURE);

	while(fgets(buf, sizeof(buf), file) != NULL)
	{
		if(buf[0] != '#')
		{
			int len = strlen(buf);
			while((len > 0) && 
			      (buf[len] == '\0' || 
			       buf[len] == '\r' || 
			       buf[len] == '\n'))
			{
				buf[len] = '\0';
				len --;
			}
			if(len > 0)
			{
				snprintf(cmdline, sizeof(cmdline), "%s /lib/modules/%s/%s.ko", cmd, kver, buf);
				ret = system(cmdline);
			}
		}
	}
	fclose(file);
	return (EXIT_SUCCESS);
}
