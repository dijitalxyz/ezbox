/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_mount_partition.c
 *
 * Description  : ezbox mount partition
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

static int install_fs_modules(char *fs_type)
{
	int rc;
	if (strcmp(fs_type, "ntfs-3g") == 0) {
		rc = utils_install_kernel_module("fuse", NULL);
	}
	else if (strcmp(fs_type, "vfat") == 0) {
		rc = utils_install_kernel_module("vfat", NULL);
	}
	else if (strcmp(fs_type, "fat") == 0) {
		rc = utils_install_kernel_module("fat", NULL);
	}
	return rc;
}

int utils_mount_partition(char *dev, char *path, char *fs_type, char *args)
{
	char buf[128];
	char *p;

	if (dev == NULL || path == NULL)
		return (EXIT_FAILURE);

	p = (args == NULL) ? "" : args;

	if (fs_type != NULL) {
		install_fs_modules(fs_type);
		if (strcmp(fs_type, "ntfs-3g") == 0) {
			snprintf(buf, sizeof(buf), "%s %s %s %s", "/usr/bin/ntfs-3g", p, dev, path);
		}
		else {
			snprintf(buf, sizeof(buf), "%s %s -t %s %s %s", CMD_MOUNT, p, fs_type, dev, path);
		}
	}
	else {
		snprintf(buf, sizeof(buf), "%s %s %s %s", CMD_MOUNT, p, dev, path);
	}
	system(buf);
	return (EXIT_SUCCESS);
}

int utils_umount_partition(char *path)
{
	char buf[128];

	if (path == NULL)
		return (EXIT_FAILURE);

	/* umount dev from path */
	snprintf(buf, sizeof(buf), "%s %s", CMD_UMOUNT, path);
	system(buf);
	return (EXIT_SUCCESS);
}

int utils_remount_partition(char *dev, char *path, char *fs_type, char *args)
{
	if (dev == NULL || path == NULL)
		return (EXIT_FAILURE);

	/* first umount dev from path */
	utils_umount_partition(path);

	/* wait a second */
	sleep(1);

	/* mount dev to path */
	return utils_mount_partition(dev, path, fs_type, args);
}

int utils_mount_boot_partition_readonly(void)
{
	char buf[KERNEL_COMMAND_LINE_SIZE];
	int rc, ret = EXIT_FAILURE;
	int i;
	struct stat stat_buf;
	char dev_buf[64];
	char fs_type_buf[64];
	char *dev = NULL;
	char *fs_type = NULL;
	char *args = NULL;

	/* prepare boot device path */
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
		ret = EXIT_SUCCESS;
		break;
	}

	return (ret);
}

int utils_mount_boot_partition_writable(void)
{
	char buf[KERNEL_COMMAND_LINE_SIZE];
	int rc, ret = EXIT_FAILURE;
	int i;
	struct stat stat_buf;
	char dev_buf[64];
	char fs_type_buf[64];
	char *dev = NULL;
	char *fs_type = NULL;
	char *args = NULL;

	/* prepare boot device path */
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
			args = NULL;
		else
			args = "-w";
	}

	i = (dev == NULL) ? 0 : 10;
	for ( ; i > 0; sleep(1), i--) {
		if (stat(dev, &stat_buf) != 0)
			continue;

		if (S_ISBLK(stat_buf.st_mode) == 0)
			continue;

		/* mount /dev/sda1 /boot */
		rc = utils_mount_partition(dev, "/boot", fs_type, args);
		ret = EXIT_SUCCESS;
		break;
	}

	return (ret);
}

#if 0
static int remount_boot_partition(char *args)
{
	char buf[KERNEL_COMMAND_LINE_SIZE];
	int rc;
	char dev_buf[64];

	if (args == NULL)
		args = "";

	/* prepare boot device path */
	rc = utils_get_boot_device_path(buf, sizeof(buf));
	if (rc <= 0)
		return (EXIT_FAILURE);

	snprintf(dev_buf, sizeof(dev_buf), "/dev/%s", buf);
	snprintf(buf, sizeof(buf), "%s -o remount %s %s %s", CMD_MOUNT, args, dev_buf, "/boot");
	system(buf);
	return (EXIT_SUCCESS);
}
#endif

int utils_remount_boot_partition_readonly(void)
{
	/* first umount dev from /boot */
	utils_umount_partition("/boot");

	/* wait a second */
	sleep(1);

	/* prepare boot device path */
	return utils_mount_boot_partition_readonly();
}

int utils_remount_boot_partition_writable(void)
{
	/* first umount dev from /boot */
	utils_umount_partition("/boot");

	/* wait a second */
	sleep(1);

	/* prepare boot device path */
	return utils_mount_boot_partition_writable();
}

int utils_mount_data_partition_writable(void)
{
	char buf[64];
	char dev_buf[64];
	char fs_type_buf[64];
	char *dev = NULL;
	char *fs_type = NULL;
	char *args = NULL;
	int rc, ret = EXIT_FAILURE;
	int i;
	struct stat stat_buf;

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
		ret = EXIT_SUCCESS;
		break;
	}

	return (ret);
}
