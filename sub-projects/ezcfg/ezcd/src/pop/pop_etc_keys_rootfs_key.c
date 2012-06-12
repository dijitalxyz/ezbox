/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_keys_rootfs_key.c
 *
 * Description  : ezbox /etc/keys/rootfs_key file generating program
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
#include <net/if.h>

#include "ezcd.h"
#include "pop_func.h"

#define ROOTFS_KEY_FILE_PATH	"/etc/keys/rootfs_key"

int pop_etc_keys_rootfs_key(int flag)
{
        FILE *file = NULL;
	char name[32];
	char buf[256];
	int rc;

	switch (flag) {
	case RC_ACT_BOOT :
		/* generate /etc/keys/rootfs_key */
		file = fopen(ROOTFS_KEY_FILE_PATH, "w");
		if (file == NULL)
			return (EXIT_FAILURE);

		snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(DMCRYPT, ROOTFS_KEY));
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		if (rc > 0) {
			fprintf(file, "%s", buf);
		}

		fclose(file);
		break;

	case RC_ACT_STOP :
		/* remove /etc/keys/rootfs_key */
		unlink(ROOTFS_KEY_FILE_PATH);
		break;

	default :
		break;
	}

	return (EXIT_SUCCESS);
}
