/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_preboot.c
 *
 * Description  : ezbox prepare to boot system
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

/* Linux kernel char __initdata boot_command_line[COMMAND_LINE_SIZE]; */
#define COMMAND_LINE_SIZE	512

int rc_preboot(int flag)
{
        FILE *file = NULL;
	char buf[COMMAND_LINE_SIZE];
	int rc;
	char *p, *q;
	int i;
	struct stat stat_buf;
	char dev_buf[64];
	char fs_type_buf[64];
	char *dev = NULL;
	char *fs_type = NULL;
	char *args = NULL;

	switch (flag) {
	case RC_BOOT :
		/* /proc */
		mkdir("/proc", 0555);
		mount("proc", "/proc", "proc", MS_MGC_VAL, NULL);

		/* sysfs -> /sys */
		mkdir("/sys", 0755);
		mount("sysfs", "/sys", "sysfs", MS_MGC_VAL, NULL);

		/* /dev */
		mkdir("/dev", 0755);
		// mount("tmpfs", "/dev", "tmpfs", MS_MGC_VAL, NULL);

		/* /etc */
		mkdir("/etc", 0755);

		/* /boot */
		mkdir("/boot", 0777);

		/* /var */
		mkdir("/var", 0777);
		mkdir("/var/lock", 0777);
		mkdir("/var/log", 0777);
		mkdir("/var/run", 0777);
		mkdir("/var/tmp", 0777);

		/* /tmp */
		snprintf(buf, sizeof(buf), "%s -rf /tmp", CMD_RM);
		system(buf);
		symlink("/var/tmp", "/tmp");

		/* init shms */
		mkdir("/dev/shm", 0777);

		/* Mount /dev/pts */
		mkdir("/dev/pts", 0777);
		mount("devpts", "/dev/pts", "devpts", MS_MGC_VAL, NULL);

		mknod("/dev/console", S_IRWXU|S_IFCHR, makedev(5, 1));

		/* run in root HOME path */
		mkdir(ROOT_HOME_PATH, 0755);
		setenv("HOME", ROOT_HOME_PATH, 1);
		chdir(ROOT_HOME_PATH);

		/* init hotplug2 */
		rc_hotplug2(RC_BOOT);
		file = fopen("/proc/sys/kernel/hotplug", "w");
		if (file != NULL)
		{
			fprintf(file, "%s", "");
			fclose(file);
		}

		/* get the kernel module name from /proc/cmdline */
		rc = utils_get_kernel_modules(buf, sizeof(buf));
		if (rc > 0) {
			/* load preboot kernel modules */
			p = buf;
			while(p != NULL) {
				q = strchr(p, ',');
				if (q != NULL)
					*q = '\0';

				utils_install_kernel_module(p, NULL);

				if (q != NULL)
					p = q+1;
				else
					p = NULL;
			}
		}

		/* prepare boot device path */
#if 0
		rc = utils_get_boot_device_path(buf, sizeof(buf));
		if (rc > 0) {
			int i;
			struct stat stat_buf;
			char path[64];
			char fs_type[64];

			snprintf(path, sizeof(path), "/dev/%s", buf);

			for (i = 10; i > 0; i--) {
				if (stat(path, &stat_buf) == 0) {
					if (S_ISBLK(stat_buf.st_mode)) {
						/* mount /dev/sda1 /boot */
						rc = utils_get_boot_device_fs_type(fs_type, sizeof(fs_type));
						if (rc > 0) {
							if (strcmp(fs_type, "ntfs-3g") == 0) {
								snprintf(buf, sizeof(buf), "%s -o ro %s /boot", "/usr/bin/ntfs-3g", path);
							}
							else {
								snprintf(buf, sizeof(buf), "%s -r -t %s %s /boot", CMD_MOUNT, fs_type, path);
							}
						}
						else {
							snprintf(buf, sizeof(buf), "%s -r %s /boot", CMD_MOUNT, path);
						}
						system(buf);
					}
					break;
				}
				/* wait a second then try again */
				sleep(1);
			}
		}
#else
		rc = utils_get_boot_device_path(buf, sizeof(buf));
		if (rc > 0) {
			snprintf(dev_buf, sizeof(dev_buf), "/dev/%s", buf);
			dev = dev_buf;
		}

		rc = utils_get_boot_device_fs_type(buf, sizeof(buf));
		if (rc > 0) {
			snprintf(fs_type_buf, sizeof(fs_type_buf), "%s", buf);
			fs_type = fs_type_buf;
			if (strcmp(fs_type, "ntfs-3g") == 0)
				args = "-o ro";
			else
				args = "-r";
		}

		i = (dev == NULL) ? 0 : 10;
		for ( ; i > 0; sleep(1), i--) {
			if (stat(dev, &stat_buf) != 0)
				continue;

			if (S_ISBLK(stat_buf.st_mode) == 0)
				continue;

			/* mount /dev/sda1 /boot */
			rc = utils_mount_partition(dev, "/boot", fs_type, args);
			break;
		}
#endif

		break;
	}

	return (EXIT_SUCCESS);
}
