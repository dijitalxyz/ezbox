/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_file_get_keyword.c
 *
 * Description  : ezcfg get keyword from file function
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-06-23   0.1       Write it from scratch
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

char *utils_file_get_keyword_by_index(char *filename, char *keyword, int idx)
{
        FILE *file;
	char *p = NULL;
	char *q = NULL;
	char *v = NULL;
	int i;
	char buf[1024];

	/* open file */
	file = fopen(filename, "r");
	if (file == NULL)
		return NULL;

	memset(buf, 0, sizeof(buf));

	while (utils_file_get_line(file, buf, sizeof(buf), "#", LINE_TAIL_STRING) == true) {

		q = strstr(buf, keyword);
		if (q == NULL)
			continue;

		/* skip key word length */
		p = q + strlen(keyword);
		q = strchr(p, ' ');
		if (q != NULL)
			*q = '\0';

		if (idx > 0) {
			/* get index part string */
			for (i = 1; (i < idx) && (p != NULL); i++) {
				p = strchr(p, ',');
				if (p != NULL)
					p++;
			}

			/* split string */
			if (p != NULL) {
				q = strchr(p, ',');
				if (q != NULL)
					*q = '\0';
			}
		}

		/* found the keyword string, stop searching */
		break;
	}

	fclose(file);
	if (p != NULL) {
		v = strdup(p);
	}
	return (v);
}

char *utils_file_get_keyword(char *filename, char *keyword)
{
	return utils_file_get_keyword_by_index(filename, keyword, 0);
}
