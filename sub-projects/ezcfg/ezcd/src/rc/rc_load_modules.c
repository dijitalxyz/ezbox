/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_load_modules.c
 *
 * Description  : ezbox run load kernel modules service
 *
 * Copyright (C) 2008-2011 by ezbox-project
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
#include "pop_func.h"

int rc_load_modules(int flag)
{
	FILE *file = NULL;
	char buf[32];
	int ret = EXIT_FAILURE;

	/* first generate /etc/modules */
	pop_etc_modules(flag);

	file = fopen("/etc/modules", "r");
	if (file == NULL) {
		return (EXIT_FAILURE);
	}

	switch (flag) {
	case RC_BOOT :
	case RC_START :
		while (utils_file_get_line(file,
			 buf, sizeof(buf), "#", LINE_TAIL_STRING) == true) {
			ret = utils_install_kernel_module(buf, NULL);
		}
		ret = EXIT_SUCCESS;
		break;

	case RC_STOP :
		while (utils_file_get_line(file,
			 buf, sizeof(buf), "#", LINE_TAIL_STRING) == true) {
			ret = utils_remove_kernel_module(buf);
		}
		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}

	if (file != NULL)
		fclose(file);

	return (ret);
}
