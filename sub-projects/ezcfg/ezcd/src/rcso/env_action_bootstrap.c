/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : env_action_bootstrap.c
 *
 * Description  : ezbox env agent runs bootstrap service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2013-05-21   0.1       Write it from scratch
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

#define FDISK_COMMAND_FILE	"/tmp/fdisk-command"

static int generate_fdisk_command(char *name)
{
	FILE *fp = fopen(name, "w");
	if (fp == NULL)
		return (EXIT_FAILURE);

	/* sda1 */
	/* add a new partition */
	fprintf(fp, "n\n");
	/* primary partition */
	fprintf(fp, "p\n");
	/* partition number is 1 */
	fprintf(fp, "1\n");
	/* first cylinder use default */
	fprintf(fp, "\n");
	/* last cylinder use 256M */
	fprintf(fp, "+256M\n");

	/* toggle a bootable flag */
	fprintf(fp, "a\n");
	/* partition number is 1 */
	fprintf(fp, "1\n");

	/* sda2 */
	/* add a new partition */
	fprintf(fp, "n\n");
	/* extended */
	fprintf(fp, "e\n");
	/* partition number is 2 */
	fprintf(fp, "2\n");
	/* first cylinder use default */
	fprintf(fp, "\n");
	/* last cylinder use default */
	fprintf(fp, "\n");

	/* sda5 */
	/* add a new partition */
	fprintf(fp, "n\n");
	/* logical */
	fprintf(fp, "l\n");
	/* first cylinder use default */
	fprintf(fp, "\n");
	/* last cylinder use 512M */
	fprintf(fp, "+512M\n");

	/* change a partition's system id */
	fprintf(fp, "t\n");
	/* partition number is 5 */
	fprintf(fp, "5\n");
	/* Linux swap */
	fprintf(fp, "82\n");

	/* sda6 */
	/* add a new partition */
	fprintf(fp, "n\n");
	/* logical */
	fprintf(fp, "l\n");
	/* first cylinder use default */
	fprintf(fp, "\n");
	/* last cylinder use default */
	fprintf(fp, "\n");

	/* write table to disk and exit */
	fprintf(fp, "w\n");

	fclose(fp);

	return (EXIT_SUCCESS);
}

static int generate_grub_cfg(char *name)
{
	char boot_args[1024];
	char distro_name[64];
	char sw_ver[16];
	FILE *fp;
	int ret;

	ret = utils_get_bootcfg_keyword(NVRAM_SERVICE_OPTION(SYS, BOOT_ARGS) "=", boot_args, sizeof(boot_args));
	if (ret < 0) {
		return (EXIT_FAILURE);
	}

	ret = utils_get_bootcfg_keyword(NVRAM_SERVICE_OPTION(SYS, DISTRO_NAME) "=", distro_name, sizeof(distro_name));
	if (ret < 0) {
		return (EXIT_FAILURE);
	}

	ret = utils_get_bootcfg_keyword(NVRAM_SERVICE_OPTION(SYS, SOFTWARE_VERSION) "=", sw_ver, sizeof(sw_ver));
	if (ret < 0) {
		return (EXIT_FAILURE);
	}

	fp = fopen(name, "w");
	if (fp == NULL)
		return (EXIT_FAILURE);

	fprintf(fp, "set default=\"%d\"\n", 0);
	fprintf(fp, "set timeout=%d\n", 0);
	fprintf(fp, "set root='(hd0,msdos1)'\n");

	fprintf(fp, "menuentry 'ezbox %s %s' --class gnu-linux --class gnu --class os {\n", distro_name, sw_ver);
	fprintf(fp, "\tlinux\t/vmlinuz %s\n", boot_args);
	fprintf(fp, "}\n");

	fclose(fp);

	return (EXIT_SUCCESS);
}

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int env_action_bootstrap(int argc, char **argv)
#endif
{
	int ret, flag;
	char cmd[256];
	char name[64];
	char hdd_dev[64];
	char boot_dev[64];
	char data_dev[64];
	char boot_mount_point[64];
	char data_mount_point[64];

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "bootstrap")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	/* get HDD device name */
	ret = utils_get_hdd_device_path(hdd_dev, sizeof(hdd_dev));
	if (ret < 1) {
		return (EXIT_FAILURE);
	}

	/* get boot partition device name */
	ret = utils_get_boot_device_path(boot_dev, sizeof(boot_dev));
	if (ret < 1) {
		return (EXIT_FAILURE);
	}

	/* get data partition device name */
	ret = utils_get_data_device_path(data_dev, sizeof(data_dev));
	if (ret < 1) {
		return (EXIT_FAILURE);
	}

	if (strncmp(boot_dev, hdd_dev, strlen(hdd_dev)) != 0) {
		return (EXIT_FAILURE);
	}
	if (strncmp(data_dev, hdd_dev, strlen(hdd_dev)) != 0) {
		return (EXIT_FAILURE);
	}

	snprintf(boot_mount_point, sizeof(boot_mount_point), "/mnt/boot");
	snprintf(data_mount_point, sizeof(data_mount_point), "/mnt/data");

	flag = utils_get_rc_act_type(argv[1]);

	switch (flag) {
	case RC_ACT_START :
		/* cleanup HDD partition table */
		snprintf(cmd, sizeof(cmd), "%s if=/dev/zero of=/dev/%s bs=1024 count=1024", CMD_DD, hdd_dev);
		utils_system(cmd);

		/* generate fdisk command */
		snprintf(name, sizeof(name), "%s-%d", FDISK_COMMAND_FILE, getpid());
		generate_fdisk_command(name);

		/* make partitions */
		snprintf(cmd, sizeof(cmd), "%s < %s", CMD_FDISK, name);
		utils_system(cmd);

		unlink(name);

		/* make boot device filesystem */
		snprintf(cmd, sizeof(cmd), "%s /dev/%s", CMD_MKFS_EXT4, boot_dev);
		utils_system(cmd);

		/* mkdir -p boot_mount_point */
		snprintf(cmd, sizeof(cmd), "%s -p %s", CMD_MKDIR, boot_mount_point);
		utils_system(cmd);

		/* mount boot partition */
		snprintf(cmd, sizeof(cmd), "%s /dev/%s %s", CMD_MOUNT, boot_dev, boot_mount_point);
		utils_system(cmd);

		/* install bootloader */
		snprintf(cmd, sizeof(cmd), "%s --boot-directory=%s /dev/%s", CMD_GRUB_INSTALL, boot_mount_point, hdd_dev);
		utils_system(cmd);

		/* generate grub.cfg */
		snprintf(name, sizeof(name), "%s/grub/grub.cfg", boot_mount_point);
		generate_grub_cfg(name);

		/* copy needed boot files */
		snprintf(cmd, sizeof(cmd), "%s /boot/* %s/", CMD_CP, boot_mount_point);
		utils_system(cmd);

		/* umount boot partition */
		snprintf(cmd, sizeof(cmd), "%s %s", CMD_UMOUNT, boot_mount_point);
		utils_system(cmd);

		/* make data device filesystem */
		snprintf(cmd, sizeof(cmd), "%s /dev/%s", CMD_MKFS_EXT4, data_dev);
		utils_system(cmd);

		/* mount data partition */
		snprintf(cmd, sizeof(cmd), "%s /dev/%s %s", CMD_MOUNT, data_dev, data_mount_point);
		utils_system(cmd);

		ret = EXIT_SUCCESS;
		break;

	default:
		ret = EXIT_FAILURE;
		break;
	}
	return (ret);
}
