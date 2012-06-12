/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_dmcrypt_rootfs.c
 *
 * Description  : ezbox run LUKS/dm-crypt for rootfs service
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2012-06-12   0.1       Write it from scratch
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

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int rc_dmcrypt_rootfs(int argc, char **argv)
#endif
{
	int flag, ret;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "dmcrypt_rootfs")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[1]);

	switch (flag) {
	case RC_ACT_BOOT :
		/* prepare /etc/keys directory first */
		mkdir("/etc/keys", 0755);

		/* prepare dm-crypt rootfs key file */
		pop_etc_keys_rootfs_key(RC_ACT_BOOT);

		/* prepare dm-crypt rootfs partition */
		utils_mount_dmcrypt_rootfs_partition_writable();

		/* remove dm-crypt rootfs key file */
		pop_etc_keys_rootfs_key(RC_ACT_STOP);

		ret = EXIT_SUCCESS;
		break;

	case RC_ACT_STOP :
		/* remove dm-crypt rootfs partition */
		utils_umount_dmcrypt_rootfs_partition();

		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}
	return (ret);
}

