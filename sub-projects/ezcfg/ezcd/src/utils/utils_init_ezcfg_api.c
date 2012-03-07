/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_init_ezcfg_api.c
 *
 * Description  : initialize ezcfg-api settings
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2012-03-07   0.1       Write it from scratch
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

bool utils_init_ezcfg_api(const char *path)
{
	if (path == NULL) {
		return false;
	}

	/* ezcfg_api_master */
	if (ezcfg_api_master_initialized() == false) {
		if (ezcfg_api_master_set_config_file(path) < 0) {
			return false;
		}
	}

	/* ezcfg_api_ctrl */
	if (ezcfg_api_ctrl_initialized() == false) {
		if (ezcfg_api_ctrl_set_config_file(path) < 0) {
			return false;
		}
	}

	/* ezcfg_api_nvram */
	if (ezcfg_api_nvram_initialized() == false) {
		if (ezcfg_api_nvram_set_config_file(path) < 0) {
			return false;
		}
	}

	/* ezcfg_api_uuid */
	if (ezcfg_api_uuid_initialized() == false) {
		if (ezcfg_api_uuid_set_config_file(path) < 0) {
			return false;
		}
	}

	return true;
}
