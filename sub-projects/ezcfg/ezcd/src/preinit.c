/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : preinit.c
 *
 * Description  : ezbox initramfs preinit program
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

int preinit_main(int argc, char **argv)
{
	/* unset umask */
	umask(0);

	/* run boot processes */
	rc_system(RC_BOOT);

	/* run init */
	/* if cmdline has root= switch_root to new root device */
	rc_init(RC_BOOT);

	return (EXIT_SUCCESS);
}
