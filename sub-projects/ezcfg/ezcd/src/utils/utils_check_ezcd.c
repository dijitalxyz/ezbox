/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_ezcd.c
 *
 * Description  : ezbox run ezcfg daemon service
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-09   0.1       Write it from scratch
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
#include "utils.h"

#if 0
#define DBG printf
#else
#define DBG(format, arg...)
#endif

bool utils_ezcd_is_alive(void)
{
	proc_stat_t *pidList;

	pidList = utils_find_pid_by_name("ezcd");
	if (pidList) {
		free(pidList);
		return true;
	}
	return false;
}

bool utils_ezcd_is_up(void)
{
	char buf[64];
	int rc;

	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(SYS, SERIAL_NUMBER), buf, sizeof(buf));
	if (rc < 0) {
		return false;
	}
	else {
		return true;
	}
}

bool utils_ezcd_wait_up(int s)
{
	if (s < 1) {
		do {
			sleep(1);
		} while(utils_ezcd_is_up() == false);
		return true;
	}
	else {
		while(s > 0) {
			sleep(1);
			s--;
			if (utils_ezcd_is_up() == true) {
				return true;
			}
		}
		return false;
	}
}
