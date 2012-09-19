/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_ppp_rp_pppoe_options.c
 *
 * Description  : ezbox /etc/ppp/rp-pppoe-options file generating program
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2012-09-13   0.1       Write it from scratch
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

int pop_etc_ppp_rp_pppoe_options(int flag)
{
        FILE *file = NULL;

	/* generate /etc/ppp/rp-pppoe-options */
	file = fopen("/etc/ppp/rp-pppoe-options", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	fclose(file);
	return (EXIT_SUCCESS);
}
