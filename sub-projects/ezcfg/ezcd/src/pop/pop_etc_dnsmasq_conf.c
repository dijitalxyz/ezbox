/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_dnsmasq_conf.c
 *
 * Description  : ezbox /etc/dnsmasq.conf file generating program
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

int pop_etc_dnsmasq_conf(int flag)
{
        FILE *file = NULL;

	/* generate /etc/hosts */
	file = fopen("/etc/dnsmasq.conf", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	switch (flag) {
	case RC_START :
		if (nvram_match(NVRAM_SERVICE_OPTION(DNSMASQ, DOMAIN_NEEDED), "1") == 0) {
			fprintf(file, "%s\n", SERVICE_OPTION(DNSMASQ, DOMAIN_NEEDED));
		}
		break;
	}

	fclose(file);
	return (EXIT_SUCCESS);
}
