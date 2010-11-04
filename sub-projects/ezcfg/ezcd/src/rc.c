/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc.c
 *
 * Description  : ezbox rc program
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

int rc_main(int argc, char **argv)
{
	rc_func_t *f = NULL;
	int flag = RC_BOOT;
	int ret = EXIT_FAILURE;

	/* only accept two arguments */
	if (argc != 3)
		return (EXIT_FAILURE);

	if (argv[1] != NULL) {
		f = utils_find_rc_func(argv[1]);
	}

	if (argv[2] != NULL) {
		if (strcmp(argv[2], "restart") == 0)
			flag = RC_RESTART;
		else if (strcmp(argv[2], "start") == 0)
			flag = RC_START;
		else if (strcmp(argv[2], "stop") == 0)
			flag = RC_STOP;
	}

	if (flag != RC_BOOT && f != NULL) {
		ret = f->func(flag);
	}

	return (ret);
}
