/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_nvram.c
 *
 * Description  : ezbox populate nvram command config file
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-24   0.1       Write it from scratch
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
#include "pop_func.h"

#if 0
#define DBG printf
#else
#define DBG(format, arg...)
#endif

int rc_nvram(int flag)
{
	switch (flag) {
	case RC_BOOT :
		/* generate nvram config file */
		pop_etc_nvram_conf(RC_BOOT);
		/* update nvram with ezbox_boot.cfg */
		utils_sync_nvram_with_cfg(BOOT_CONFIG_FILE_PATH);
		break;

	case RC_RELOAD :
		/* re-generate nvram config file */
		pop_etc_nvram_conf(RC_RELOAD);
		/* update nvram with ezbox_upgrade.cfg */
		utils_sync_nvram_with_cfg(UPGRADE_CONFIG_FILE_PATH);
		break;

	case RC_STOP :
		/* first make /boot writable */
		utils_remount_boot_partition_writable();
		/* remove ezbox_upgrade.cfg */
		unlink(UPGRADE_CONFIG_FILE_PATH);
		/* update ezbox_boot.cfg with nvram */
		utils_sync_cfg_with_nvram(BOOT_CONFIG_FILE_PATH);
		/* make /boot read-only */
		utils_remount_boot_partition_readonly();
		break;
	}
	return (EXIT_SUCCESS);
}
