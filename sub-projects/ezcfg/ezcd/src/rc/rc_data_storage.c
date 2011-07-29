/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_data_storage.c
 *
 * Description  : ezbox prepare dynamic data storage
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-07-28   0.1       Write it from scratch
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
#include "rc_func.h"
#include "utils.h"

#if 0
#define DBG(format, args...) do {\
	FILE *fp = fopen("/dev/kmsg", "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

#define DBG2() do {\
	pid_t pid = getpid(); \
	FILE *fp = fopen("/dev/kmsg", "a"); \
	if (fp) { \
		char buf[32]; \
		FILE *fp2; \
		int i; \
		for(i=pid; i<pid+30; i++) { \
			snprintf(buf, sizeof(buf), "/proc/%d/stat", i); \
			fp2 = fopen(buf, "r"); \
			if (fp2) { \
				if (fgets(buf, sizeof(buf)-1, fp2) != NULL) { \
					fprintf(fp, "pid=[%d] buf=%s\n", i, buf); \
				} \
				fclose(fp2); \
			} \
		} \
		fclose(fp); \
	} \
} while(0)

int rc_data_storage(int flag)
{
        FILE *file = NULL;
	char buf[64];
	char dev_buf[64];
	char fs_type_buf[64];
	char *dev = NULL;
	char *fs_type = NULL;
	char *args = NULL;
	int rc;
	int i;
	struct stat stat_buf;

	switch (flag) {
	case RC_BOOT :
		/* prepare dynamic data storage path */
		rc = utils_get_data_device_path(buf, sizeof(buf));
		if (rc > 0) {
			snprintf(dev_buf, sizeof(dev_buf), "/dev/%s", buf);
			dev = dev_buf;
		}

		rc = utils_get_data_device_fs_type(buf, sizeof(buf));
		if (rc > 0) {
			snprintf(fs_type_buf, sizeof(fs_type_buf), "%s", buf);
			fs_type = fs_type_buf;
			if (strcmp(fs_type, "ntfs-3g") != 0)
				args = "-w";
		}

		i = (dev == NULL) ? 0 : 10;
		for ( ; i > 0; sleep(1), i--) {
			if (stat(dev, &stat_buf) != 0)
				continue;

			if (S_ISBLK(stat_buf.st_mode) == 0)
				continue;

			/* mount /dev/sda2 /var */
			rc = utils_mount_partition(dev, "/var", fs_type, args);
			break;
		}

		snprintf(buf, sizeof(buf), "%s a+rwx /var", CMD_CHMOD);
		system(buf);
		snprintf(buf, sizeof(buf), "%s -rf /var/lock", CMD_RM);
		system(buf);
		snprintf(buf, sizeof(buf), "%s -rf /var/run", CMD_RM);
		system(buf);
		snprintf(buf, sizeof(buf), "%s -rf /var/tmp", CMD_RM);
		system(buf);
		mkdir("/var/lock", 0777);
		mkdir("/var/log", 0777);
		mkdir("/var/run", 0777);
		mkdir("/var/tmp", 0777);

		break;
	}

	return (EXIT_SUCCESS);
}
