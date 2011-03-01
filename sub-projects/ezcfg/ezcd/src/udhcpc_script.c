/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : udhcpc_script.c
 *
 * Description  : ezbox udhcpc script program
 *
 * Copyright (C) 2010-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-03-01   0.1       Write it from scratch
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

int udhcpc_script_main(int argc, char **argv)
{
	if (argc < 2) {
		printf("error, argc=[%d]\n", argc);
		return (EXIT_FAILURE);
	}

	printf("%s argc=[%d] %s %s\n", __func__, argc, argv[0], argv[1]);
	return (EXIT_SUCCESS);
}
