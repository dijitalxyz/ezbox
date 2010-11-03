/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_boot.c
 *
 * Description  : ezbox rc boot service program
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

int rc_system(int flag)
{
        FILE *file = NULL;
	int ret = 0;

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

		/* /tmp */
		mkdir("/tmp", 0777);

		/* /var */
		mkdir("/var", 0777);
		mkdir("/var/lock", 0777);
		mkdir("/var/log", 0777);
		mkdir("/var/run", 0777);

		/* hotplug2 */
		pop_etc_hotplug2_rules(RC_BOOT);
		mknod("/dev/console", S_IRWXU|S_IFCHR, makedev(5, 1));
		ret = system("/sbin/hotplug2 --set-worker /lib/hotplug2/worker_fork.so --set-rules-file /etc/hotplug2.rules --no-persistent --set-coldplug-cmd /sbin/udevtrigger");
		ret = system("/sbin/hotplug2 --set-worker /lib/hotplug2/worker_fork.so --set-rules-file /etc/hotplug2.rules --persistent &");

		/* init shms */
		mkdir("/dev/shm", 0777);

		/* Mount /dev/pts */
		mkdir("/dev/pts", 0777);
		mount("devpts", "/dev/pts", "devpts", MS_MGC_VAL, NULL);

		/* init hotplug */
		file = fopen("/proc/sys/kernel/hotplug", "w");
		if (file != NULL)
		{
			fprintf(file, "%s", "");
			fclose(file);
		}

		/* load preinit kernel modules */
		pop_etc_modules(RC_BOOT);
		rc_load_kernel_modules(RC_BOOT);

		/* setup welcome banner */
		pop_etc_banner(RC_BOOT);

		/* setup inittab for /sbin/init */
		pop_etc_inittab(RC_BOOT);
		break;

	case RC_STOP :
		break;

	case RC_START :
		break;
	}

	return (EXIT_SUCCESS);
}
