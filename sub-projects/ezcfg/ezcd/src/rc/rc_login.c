/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_login.c
 *
 * Description  : ezbox run login service
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-03   0.1       Write it from scratch
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

int rc_login(int flag)
{
	switch (flag) {
	case RC_BOOT :
		/* generate /etc/passwd */
		pop_etc_passwd(RC_BOOT);

		/* generate /etc/group */
		pop_etc_group(RC_BOOT);

		break;

	case RC_START :
		rc_login(RC_BOOT);

		/* set root password */

		break;

	}
	return (EXIT_SUCCESS);
}
