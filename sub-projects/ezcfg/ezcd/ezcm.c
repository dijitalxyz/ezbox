/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : ezcm.c
 *
 * Description  : ezbox config interface
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-06-13   0.1       Write it from scratch
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

static bool debug = false;

static void log_fn(struct ezcfg *ezcfg, int priority,
                   const char *file, int line, const char *fn,
                   const char *format, va_list args)
{
	if (debug) {
		char buf[1024];
		struct timeval tv;
		struct timezone tz;

		vsnprintf(buf, sizeof(buf), format, args);
		gettimeofday(&tv, &tz);
		fprintf(stderr, "%llu.%06u [%u] %s: %s",
		        (unsigned long long) tv.tv_sec, (unsigned int) tv.tv_usec,
		        (int) getpid(), fn, buf);
	} else {
		vsyslog(priority, format, args);
        }
}

static void ezcm_show_usage(void)
{
	printf("Usage: ezcm [-m message]\n");
	printf("\n");
	printf("  -m\tmessage sent to ezcd\n");
	printf("  -D\tdebug mode\n");
	printf("  -h\thelp\n");
	printf("\n");
}

int ezcm_main(int argc, char **argv)
{
	int c = 0;
	int rc = 0;
	char buf[32];
	struct ezcfg *ezcfg = NULL;
	struct ezcfg_ctrl *ezctrl = NULL;

	memset(buf, 0, sizeof(buf));
	for (;;) {
		c = getopt( argc, argv, "Dhm:");
		if (c == EOF) break;
		switch (c) {
			case 'm':
				snprintf(buf, sizeof(buf), "GET /EZCFG HTTP/1.1\r\nMessage: %s\r\n\r\n", optarg);
				break;
			case 'D':
				debug = true;
				break;
			case 'h':
			default:
				ezcm_show_usage();
				return 0;
		}
        }

	//conn_fd = ezcd_client_connection(EZCD_SOCKET_PATH, 'a');
	ezcfg = ezcfg_new();
	if (ezcfg == NULL) {
		printf("error : %s\n", "ezcfg_new");
		rc = 1;
		goto exit;
	}

	ezcfg_log_init("ezcm");
	ezcfg_set_log_fn(ezcfg, log_fn);
	info(ezcfg, "version %s\n", VERSION);
	snprintf(buf, sizeof(buf), "%s-%d", EZCFG_CTRL_SOCK_PATH, getpid());

	ezctrl = ezcfg_ctrl_new_from_socket(ezcfg, AF_LOCAL, buf);

	if (ezctrl == NULL) {
		err(ezcfg, "%s\n", "Cannot initialize ezcm controller");
		rc = 2;
		goto exit;
	}

	if (ezcfg_ctrl_connect(ezctrl) < 0) {
		err(ezcfg, "controller connect fail: %m\n");
		rc = 3;
		goto exit;
	}

exit:
	ezcfg_delete(ezcfg);
	return rc;
}
