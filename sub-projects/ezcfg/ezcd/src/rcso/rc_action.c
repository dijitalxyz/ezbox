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
#include <dlfcn.h>

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

static int call_rc_function(int argc, char **argv)
{
	char path[64];
	char name[32];
	void *handle;
	union {
		rc_function_t func;
		void * obj;
	} alias;
	char *error;
	int ret = EXIT_FAILURE;

	/* process rc function */
	snprintf(name, sizeof(name), "rc_%s", argv[0]);
	snprintf(path, sizeof(path), "%s/%s.so", RCSO_PATH_PREFIX, name);
	handle = dlopen(path, RTLD_NOW);
	if (!handle) {
		snprintf(path, sizeof(path), "%s/%s.so", RCSO_PATH_PREFIX2, name);
		handle = dlopen(path, RTLD_NOW);
		if (!handle) {
			goto func_exit;
		}
	}

	/* clear any existing error */
	dlerror();

	alias.obj = dlsym(handle, name);

	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		goto func_exit;
	}

	/* handle rc operations */
	ret = alias.func(argc, argv);

func_exit:
	/* close loader handle */
	dlclose(handle);
	return (ret);
}

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int rc_action(int argc, char **argv)
#endif
{
	static char buf[128];
	char path[64];
	int fargc;
	char *fargv[RC_MAX_ARGS];
	FILE *fp;
	int ret = EXIT_FAILURE;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "action")) {
		return (EXIT_FAILURE);
	}

	/* check first place */
	snprintf(path, sizeof(path), "%s/%s", ACTION_PATH_PREFIX, argv[1]);
	fp = fopen(path, "r");
	if (fp == NULL) {
		/* check second place */
		snprintf(path, sizeof(path), "%s/%s", ACTION_PATH_PREFIX2, argv[1]);
		if (fp == NULL) {
			return (EXIT_FAILURE);
		}
	}

	/* read action file */
	while (utils_file_get_line(fp, buf, sizeof(buf), "#", LINE_TAIL_STRING) == true) {
		fargc = utils_parse_args(buf, strlen(buf) + 1, fargv);
		if (fargc > 0) {
			ret = call_rc_function(fargc, fargv);
			if (ret == EXIT_FAILURE) {
				DBG("<6> rc_action: %s error!\n", fargv[0]);
			}
		}
	}

	fclose(fp);
	return (ret);
}
