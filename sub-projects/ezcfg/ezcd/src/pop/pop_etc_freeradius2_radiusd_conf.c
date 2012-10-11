/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_freeradius2_radiusd_conf.c
 *
 * Description  : ezbox /etc/freeradius2/radiusd.conf file generating program
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2012-10-11   0.1       Write it from scratch
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

static int gen_radiusd_general_confs(FILE *file)
{
	int rc;
	char buf[256];

	/* general options */
	/* prefix = /usr */
	fprintf(file, "prefix = %s\n", "/usr");

	/* exec_prefix = /usr */
	fprintf(file, "exec_prefix = %s\n", "/usr");

	/* sysconfdir = /etc */
	fprintf(file, "sysconfdir = %s\n", "/etc");

	/* localstatedir = /var */
	fprintf(file, "localstatedir = %s\n", "/var");

	/* sbindir = /usr/sbin */
	fprintf(file, "sbindir = %s\n", "/usr/sbin");

	/* logdir = /var/log */
	fprintf(file, "logdir = %s\n", "/var/log");

	/* raddbdir = /etc/freeradius2 */
	fprintf(file, "raddbdir = %s\n", "/etc/freeradius2");

	/* radacctdir = /var/db/radacct */
	fprintf(file, "radacctdir = %s\n", "/var/db/radacct");

	/* name */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, NAME), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s = %s\n", EZCFG_FREERADIUS2_OPT_KEYWORD_NAME, buf);
	}

	return EXIT_SUCCESS;
}

int pop_etc_freeradius2_radiusd_conf(int flag)
{
        FILE *file = NULL;

	/* generate /etc/freeradius2/radiusd.conf */
	file = fopen("/etc/freeradius2/radiusd.conf", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	/* general configs */
	gen_radiusd_general_confs(file);

	fclose(file);
	return (EXIT_SUCCESS);
}
