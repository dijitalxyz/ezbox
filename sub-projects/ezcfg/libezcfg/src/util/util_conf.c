/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_conf.c
 *
 * Description  : get string from config file
 *
 * Copyright (C) 2010-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-02-28   0.1       Write it from scrach
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
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

/*
 * Returns string from config file
 * It is the caller's duty to free the returned string.
 */
char *ezcfg_util_get_conf_string(char *path, char *keyword)
{
	FILE *file;
	char *p = NULL;
	char *v = NULL;
	char line[128];
	int keyword_len;

	if ((path == NULL) || (keyword == NULL))
		return NULL;

	keyword_len = strlen(keyword);
	if (keyword_len < 1)
		return NULL;

	/* get string from config file */
	file = fopen(path, "r");
	if (file == NULL)
		return NULL;

	while (fgets(line, sizeof(line), file) != NULL) {
		if (strncmp(line, keyword, keyword_len) == 0) {
			p = line+keyword_len;
			if (*p == '=') {
				p++;
				v = strdup(p);
				goto func_out;
			}
		}
	}
func_out:
	fclose(file);
	return v;
}
