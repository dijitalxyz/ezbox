/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : preinit.c
 *
 * Description  : ezbox /etc/modules generating program
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

int pop_etc_modules(int flag)
{
	FILE *file;
	char cmd[64];
	char buf[32];

	file = fopen("/etc/modules", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	switch (flag) {
	case RC_BOOT :
		/* get the kernel module name from kernel cmdline */
		break;
	case RC_RESTART :
	case RC_START :
		/* get the kernel module name from nvram */
		break;
	}

	fclose(file);
	return (EXIT_SUCCESS);
}
