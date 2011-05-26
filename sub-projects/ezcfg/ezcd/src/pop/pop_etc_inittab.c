/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_inittab.c
 *
 * Description  : ezbox /etc/inittab file generating program
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

int pop_etc_inittab(int flag)
{
        FILE *file = NULL;

	/* generate /etc/inittab */
	file = fopen("/etc/inittab", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	fprintf(file, "%s\n", "::sysinit:/sbin/rc system start");
	//fprintf(file, "%s\n", "::shutdown:/sbin/rc system stop");
	fprintf(file, "%s\n", "tts/0::askfirst:/bin/ash --login");
	fprintf(file, "%s\n", "ttyS0::askfirst:/bin/ash --login");
	fprintf(file, "%s\n", "tty1::askfirst:/bin/ash --login");

	fclose(file);
	return (EXIT_SUCCESS);
}
