/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_emc2_configs.c
 *
 * Description  : ezbox /etc/emc2/configs/ files generating program
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-07-06   0.1       Write it from scratch
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

int pop_etc_emc2_configs(int flag)
{
        FILE *file = NULL;
	char name[32];
	char buf[64];
	int rc;

	mkdir("/etc/emc2/configs", 0755);

	/* generate /etc/emc2/configs/ezcnc.ini */
	file = fopen("/etc/emc2/configs/ezcnc.ini", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	switch (flag) {
	case RC_START :

		break;
	}

	fclose(file);
	return (EXIT_SUCCESS);
}
