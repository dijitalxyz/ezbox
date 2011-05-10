/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_system.c
 *
 * Description  : ezbox run system services
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-02   0.1       Write it from scratch
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

int rc_system(int flag)
{
        FILE *file = NULL;
	char cmdline[1024];

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

		/* /var */
		mkdir("/var", 0777);
		mkdir("/var/lock", 0777);
		mkdir("/var/log", 0777);
		mkdir("/var/run", 0777);
		mkdir("/var/tmp", 0777);

		/* /tmp */
		//mkdir("/tmp", 0777);
		snprintf(cmdline, sizeof(cmdline), "%s -rf /tmp", CMD_RM);
		system(cmdline);
		symlink("/var/tmp", "/tmp");

		/* init shms */
		mkdir("/dev/shm", 0777);

		/* Mount /dev/pts */
		mkdir("/dev/pts", 0777);
		mount("devpts", "/dev/pts", "devpts", MS_MGC_VAL, NULL);

		mknod("/dev/console", S_IRWXU|S_IFCHR, makedev(5, 1));

		/* run in root HOME path */
		mkdir(ROOT_HOME_PATH, 0755);
		chdir(ROOT_HOME_PATH);

		/* init hotplug2 */
		rc_hotplug2(RC_BOOT);
		file = fopen("/proc/sys/kernel/hotplug", "w");
		if (file != NULL)
		{
			fprintf(file, "%s", "");
			fclose(file);
		}

		/* load preinit kernel modules */
		rc_load_modules(RC_BOOT);

		/* prepare nvram storage path */
		mount("/dev/sda2", "/var", "ext4", MS_MGC_VAL, NULL);
		snprintf(cmdline, sizeof(cmdline), "%s a+rwx /var", CMD_CHMOD);
		system(cmdline);
		snprintf(cmdline, sizeof(cmdline), "%s -rf /var/lock", CMD_RM);
		system(cmdline);
		snprintf(cmdline, sizeof(cmdline), "%s -rf /var/run", CMD_RM);
		system(cmdline);
		snprintf(cmdline, sizeof(cmdline), "%s -rf /var/tmp", CMD_RM);
		system(cmdline);
		mkdir("/var/lock", 0777);
		mkdir("/var/log", 0777);
		mkdir("/var/run", 0777);
		mkdir("/var/tmp", 0777);

		/* start ezcfg daemon */
		rc_ezcd(RC_BOOT);

		/* setup network base files */
		rc_netbase(RC_BOOT);

		break;

	case RC_RESTART :
	case RC_STOP :
		/* run in root HOME path */
		chdir(ROOT_HOME_PATH);

		/* stop WAN interface binding services */
		rc_wan_services(RC_STOP);

		/* bring down WAN interface */
		rc_wan(RC_STOP);

		/* stop LAN interface binding services */
		rc_lan_services(RC_STOP);

		/* bring down LAN interface */
		rc_lan(RC_STOP);

		/* stop ezcfg daemon */
		rc_ezcd(RC_STOP);

		if (flag == RC_STOP) {
			snprintf(cmdline, sizeof(cmdline), "%s", CMD_POWEROFF);
			system(cmdline);
		}
		else if (flag == RC_RESTART) {
			snprintf(cmdline, sizeof(cmdline), "%s", CMD_REBOOT);
			system(cmdline);
		}
		break;

	case RC_START :
		/* run in root HOME path */
		chdir(ROOT_HOME_PATH);

		/* restart ezcfg daemon */
		rc_ezcd(RC_RELOAD);

		/* restart hotplug2 */
		rc_hotplug2(RC_RESTART);

		/* re-generate network base files */
		rc_netbase(RC_RESTART);

		/* load kernel modules */
		rc_load_modules(RC_START);

		/* misc files for the base system */
		rc_base_files(RC_START);

		/* build /etc/passwd & /etc/group, 
		 * also setup root password
		 */
		rc_login(RC_START);

		/* bring up loopback interface */
		rc_loopback(RC_START);

		/* bring up LAN interface link up but not configurate it */
		rc_lan_if(RC_START);

		/* bring up WAN interface link up but not configurate it */
		/* LAN interface alias need it */
		rc_wan_if(RC_START);

		/* bring up LAN interface link up and configurate it */
		rc_lan(RC_START);

		/* start LAN interface binding services */
		rc_lan_services(RC_START);

		/* bring up WAN interface link up and configurate it */
		rc_wan(RC_START);

		/* start WAN interface binding services */
		rc_wan_services(RC_START);

		break;
	}

	return (EXIT_SUCCESS);
}
