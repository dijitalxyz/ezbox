/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : upfw.c
 *
 * Description  : ezbox config interface
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-12-01   0.1       Write it from scratch
 * ============================================================================
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcd.h"

static void upfw_show_usage(void)
{
	printf("Usage: upfw -f <filename> [-d <device>] [-p <pattern>]\n");
	printf("\n");
	printf("  -f <filename> set firmware file name\n");
	printf("  -d <device>   set firmware stored device node\n");
	printf("  -p <pattern>  check firmware code pattern\n");
	printf("  -h            show help info\n");
	printf("\n");
}

int upfw_main(int argc, char **argv)
{
	int c = 0;
	int rc = 0;
	char *file_name = NULL;
	char *device_node = NULL;
	char *code_pattern = NULL;
	char *buf;
	int buf_len;

	for (;;) {
		c = getopt(argc, argv, "d:f:p:h");
		if (c == EOF) break;
		switch (c) {
			case 'd' :
				device_node = optarg;
				break;
			case 'f' :
				file_name = optarg;
				break;
			case 'p' :
				code_pattern = optarg;
				break;
			case 'h' :
			default:
				upfw_show_usage();
				return 0;
		}
	}

	if (file_name == NULL) {
		upfw_show_usage();
		return 0;
	}

        return rc;
}
