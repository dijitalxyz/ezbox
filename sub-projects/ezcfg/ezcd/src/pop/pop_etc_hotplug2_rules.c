/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_hotplug2_rules.c
 *
 * Description  : ezbox /etc/hotplug2.rules file generating program
 *
 * Copyright (C) 2008-2011 by ezbox-project
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

int pop_etc_hotplug2_rules(int flag)
{
        FILE *file = NULL;

	/* generate /etc/hotplug2.rules */
	file = fopen("/etc/hotplug2.rules", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	fprintf(file, "%s\n", "DEVICENAME ~~ (null|full|ptmx|tty|zero|gpio|hvc) {");
	fprintf(file, "\t%s\n", "nothrottle");
	fprintf(file, "\t%s\n", "makedev /dev/%DEVICENAME% 0666");
	fprintf(file, "\t%s\n", "next");
	fprintf(file, "%s\n", "}");

	fprintf(file, "%s\n", "DEVICENAME ~~ (tun|tap[0-9]) {");
	fprintf(file, "\t%s\n", "nothrottle");
	fprintf(file, "\t%s\n", "makedev /dev/net/%DEVICENAME% 0644");
	fprintf(file, "%s\n", "}");

	fprintf(file, "%s\n", "DEVICENAME ~~ (ppp) {");
	fprintf(file, "\t%s\n", "nothrottle");
	fprintf(file, "\t%s\n", "makedev /dev/%DEVICENAME% 0600");
	fprintf(file, "\t%s\n", "next");
	fprintf(file, "%s\n", "}");

	fprintf(file, "%s\n", "DEVICENAME ~~ (controlC[0-9]|pcmC0D0*|timer) {");
	fprintf(file, "\t%s\n", "nothrottle");
	fprintf(file, "\t%s\n", "makedev /dev/snd/%DEVICENAME% 0644");
	fprintf(file, "\t%s\n", "next");
	fprintf(file, "%s\n", "}");

	fprintf(file, "%s\n", "DEVICENAME ~~ (lp[0-9]) {");
	fprintf(file, "\t%s\n", "nothrottle");
	fprintf(file, "\t%s\n", "makedev /dev/%DEVICENAME% 0644");
	fprintf(file, "\t%s\n", "next");
	fprintf(file, "%s\n", "}");

	fprintf(file, "%s\n", "DEVPATH is set, SUBSYSTEM ~~ (input) {");
	fprintf(file, "\t%s\n", "nothrottle");
	fprintf(file, "\t%s\n", "makedev /dev/input/%DEVICENAME% 0644");
	fprintf(file, "%s\n", "}");

	fprintf(file, "%s\n", "DEVICENAME == device-mapper {");
	fprintf(file, "\t%s\n", "nothrottle");
	fprintf(file, "\t%s\n", "makedev /dev/mapper/control 0600");
	fprintf(file, "%s\n", "}");

	fprintf(file, "%s\n", "DEVPATH is set {");
	fprintf(file, "\t%s\n", "nothrottle");
	fprintf(file, "\t%s\n", "makedev /dev/%DEVICENAME% 0644");
	fprintf(file, "%s\n", "}");

	fprintf(file, "%s\n", "FIRMWARE is set, ACTION == add {");
	fprintf(file, "\t%s\n", "nothrottle");
	fprintf(file, "\t%s\n", "load-firmware /lib/firmware");
	fprintf(file, "\t%s\n", "next");
	fprintf(file, "%s\n", "}");

	switch (flag) {
	case RC_BOOT :
		fprintf(file, "%s\n", "SUBSYSTEM ~~ button {");
		fprintf(file, "\t%s\n", "exec kill -USR1 1");
		fprintf(file, "%s\n", "}");
		break;

	case RC_RESTART :
	case RC_START :
		fprintf(file, "%s\n", "SUBSYSTEM ~~ (net|button|usb|platform|ieee1394|block|atm) {");
		fprintf(file, "\t%s\n", "exec /sbin/hotplug-call %SUBSYSTEM%");
		fprintf(file, "%s\n", "}");

		fprintf(file, "%s\n", "DEVICENAME ~~ (watchdog) {");
		fprintf(file, "\t%s\n", "exec /sbin/watchdog -t 5 /dev/watchdog");
		fprintf(file, "\t%s\n", "next");
		fprintf(file, "%s\n", "}");

#if (HAVE_EZBOX_SERVICE_EMC2 == 1)
		fprintf(file, "%s\n", "DEVICENAME ~~ (rtai_shm) {");
		fprintf(file, "\t%s\n", "makedev /dev/%DEVICENAME% 0666");
		fprintf(file, "%s\n", "}");

		fprintf(file, "%s\n", "DEVICENAME ~~ (rtf[0-9]*) {");
		fprintf(file, "\t%s\n", "makedev /dev/%DEVICENAME% 0666");
		fprintf(file, "\t%s\n", "next");
		fprintf(file, "%s\n", "}");
#endif
		break;
	}

	fclose(file);
	return (EXIT_SUCCESS);
}
