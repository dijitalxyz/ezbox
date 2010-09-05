/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : nvram.c
 *
 * Description  : ezbox config interface
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-08-22   0.1       Write it from scratch
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
		char buf[1024*8];
		struct timeval tv;
		struct timezone tz;

		vsnprintf(buf, sizeof(buf), format, args);
		gettimeofday(&tv, &tz);
		fprintf(stderr, "%llu.%06u [%u] %s(%d): %s",
		        (unsigned long long) tv.tv_sec, (unsigned int) tv.tv_usec,
		        (int) getpid(), fn, line, buf);
	} else {
		vsyslog(priority, format, args);
        }
}

static void nvram_show_usage(void)
{
	printf("Usage: nvram [commands]\n");
	printf("\n");
	printf("  [commands]--\n");
	printf("    get <name>          get nvram value by <name>\n");
	printf("    set <name> <value>  set nvram <name> with <value>\n");
	printf("    unset <name>        remove nvram <name> and its <value>\n");
	printf("    show                list all nvram\n");
	printf("    commit              save nvram to storage device\n");
	printf("\n");
}

int nvram_main(int argc, char **argv)
{
	int rc = 0;
	struct ezcfg *ezcfg = NULL;
	struct ezcfg_nvram *nvram = NULL;

	debug = true;

	ezcfg = ezcfg_new();
	if (ezcfg == NULL) {
		printf("error : %s\n", "ezcfg_new");
		rc = 1;
		goto exit;
	}

	ezcfg_log_init("nvram");
	ezcfg_set_log_fn(ezcfg, log_fn);
	info(ezcfg, "version %s\n", VERSION);

	nvram = ezcfg_nvram_new(ezcfg);
	if (nvram == NULL) {
		err(ezcfg, "nvram new: %m\n");
		rc = 2;
		goto exit;
	}
	ezcfg_nvram_set_type(nvram, 1);
	ezcfg_nvram_set_store_path(nvram, "/tmp/ezcfg/nvram.bin");
	ezcfg_nvram_set_total_space(nvram, EZCFG_NVRAM_SPACE);
	ezcfg_nvram_initialize(nvram);

	if (argc < 2) {
		err(ezcfg, "need more arguments.\n");
		rc = 3;
		nvram_show_usage();
		goto exit;
	}

	if (strcmp(argv[1], "get") == 0) {
		if (argc != 3) {
			err(ezcfg, "number of arguments is incorrect.\n");
			rc = 4;
			nvram_show_usage();
			goto exit;
		}

		if (strlen(argv[2]) > 0) {
			char *value = ezcfg_nvram_get_node_value(nvram, argv[2]);
			if (value == NULL) {
				printf("no nvram named [%s]\n", argv[2]);
			}
			else {
				printf("%s=%s\n", argv[2], value);
				free(value);
			}
		}
		else {
			err(ezcfg, "the nvram name is empty.\n");
			rc = 5;
			nvram_show_usage();
			goto exit;
		}
	}
	else if (strcmp(argv[1], "set") == 0) {
		if (argc != 4) {
			err(ezcfg, "number of arguments is incorrect.\n");
			rc = 4;
			nvram_show_usage();
			goto exit;
		}

		if (strlen(argv[2]) > 0) {
			bool result = ezcfg_nvram_set_node_value(nvram, argv[2], argv[3]);
			if (result == false) {
				printf("can not set nvram [%s] to [%s]\n", argv[2], argv[3]);
			}
		}
		else {
			err(ezcfg, "the nvram name is empty.\n");
			rc = 5;
			nvram_show_usage();
			goto exit;
		}
	}
	else if (strcmp(argv[1], "unset") == 0) {
		if (argc != 3) {
			err(ezcfg, "number of arguments is incorrect.\n");
			rc = 4;
			nvram_show_usage();
			goto exit;
		}

		if (strlen(argv[2]) > 0) {
			bool result = ezcfg_nvram_unset_node_value(nvram, argv[2]);
			if (result == false) {
				printf("can not remove nvram [%s]\n", argv[2]);
			}
		}
		else {
			err(ezcfg, "the nvram name is empty.\n");
			rc = 5;
			nvram_show_usage();
			goto exit;
		}
	}
	else if (strcmp(argv[1], "show") == 0) {
		if (argc != 2) {
			err(ezcfg, "number of arguments is incorrect.\n");
			rc = 4;
			nvram_show_usage();
			goto exit;
		}

		int buf_len;
		char *buf = ezcfg_nvram_get_all_nodes(nvram, &buf_len);
		if (buf == NULL) {
			printf("can not get all nvram info\n");
		}
		else {
			char *line = buf;
			int line_len;
			while (buf_len > 0) {
				line_len = strlen(line) + 1; /* +1 for '\0' */
				printf("%s\n", line);
				buf_len -= line_len;
				line += line_len;
			}
			free(buf);
		}
	}
	else if (strcmp(argv[1], "commit") == 0) {
		if (argc != 2) {
			err(ezcfg, "number of arguments is incorrect.\n");
			rc = 4;
			nvram_show_usage();
			goto exit;
		}

		bool result = ezcfg_nvram_commit(nvram);
		if (result == false) {
			printf("can not commit nvram\n");
		}
		else {
			printf("commit nvram OK!\n");
		}
	}

exit:
	if (nvram != NULL)
		ezcfg_nvram_delete(nvram);

	if (ezcfg != NULL)
		ezcfg_delete(ezcfg);

	return rc;
}
