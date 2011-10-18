/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_action.c
 *
 * Description  : implement running action command rcso
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-10-15   0.1       Write it from scratch
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

#define ACTION_PATH_PREFIX "/lib/rcso/action"
#define ACTION_PATH_PREFIX2 "/usr/lib/rcso/action"

int rc_action(int argc, char **argv)
{
	char path[64];
	struct stat stat_buf;
	int ret;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "action")) {
		return (EXIT_FAILURE);
	}

	/* check first place */
	snprintf(path, sizeof(path), "%s/%s", ACTION_PATH_PREFIX, argv[1]);
	if ((stat(path, &stat_buf) != 0) ||
	    (S_ISREG(stat_buf.st_mode) == 0 && S_ISLNK(stat_buf.st_mode))) {
		/* check second place */
		snprintf(path, sizeof(path), "%s/%s", ACTION_PATH_PREFIX2, argv[1]);
		if ((stat(path, &stat_buf) != 0) ||
	    	(S_ISREG(stat_buf.st_mode) == 0 && S_ISLNK(stat_buf.st_mode))) {
			return (EXIT_FAILURE);
		}
	}

	ret = execv(path, argv + 1);
	if (ret  == -1)
		return (EXIT_FAILURE);

	return (EXIT_SUCCESS);
}
