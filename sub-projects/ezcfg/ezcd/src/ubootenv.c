/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : ubootenv.c
 *
 * Description  : ezbox config interface
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-10-27   0.1       Write it from scratch
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

#define UBOOT_ENV_ENTRY_SIZE	 4*1024 /* 4KB */
#define UBOOT_ENV_BLOCK_SIZE	64*1024 /* 64KB */

static void ubootenv_show_usage(void)
{
	printf("Usage: ubootenv [commands]\n");
	printf("\n");
	printf("  [commands]--\n");
	printf("    get <name>          get u-boot environment parameter by <name>\n");
	printf("    set <name> <value>  set u-boot environment parameter <name> with <value>\n");
	printf("    list                list all u-boot environment parameters\n");
	printf("    check               check u-boot environment parameters' integrity\n");
	printf("\n");
}

int ubootenv_main(int argc, char **argv)
{
	int rc = 0;
	char *buf;

	if (argc < 2) {
		printf("need more arguments.\n");
		ubootenv_show_usage();
		return -EZCFG_E_ARGUMENT ;
	}

	if (strcmp(argv[1], "get") == 0) {
		if (argc != 3) {
			printf("number of arguments is incorrect.\n");
			ubootenv_show_usage();
			return -EZCFG_E_ARGUMENT ;
		}

		if (strlen(argv[2]) > 0) {
			buf = malloc(UBOOT_ENV_ENTRY_SIZE);
			if (buf == NULL) {
				printf("error! not enough memory.\n");
				return -EZCFG_E_RESOURCE;
			}
			memset(buf, 0, UBOOT_ENV_ENTRY_SIZE);
			rc = ezcfg_api_ubootenv_get(argv[2], buf, UBOOT_ENV_ENTRY_SIZE);
			if (rc < 0) {
				printf("ERROR\n");
			}
			else {
				printf("%s=%s\n", argv[2], buf);
			}
			free(buf);
		}
		else {
			printf("the u-boot environment parameter name is empty.\n");
			ubootenv_show_usage();
			rc = -EZCFG_E_ARGUMENT ;
		}
	}
	else if (strcmp(argv[1], "set") == 0) {
		if (argc < 3 || argc > 4) {
			printf("number of arguments is incorrect.\n");
			ubootenv_show_usage();
			return -EZCFG_E_ARGUMENT ;
		}

		buf = (argc == 3 ? NULL : argv[3]);
		if (strlen(argv[2]) > 0) {
			rc = ezcfg_api_ubootenv_set(argv[2], buf);
			if (rc < 0) {
				printf("ERROR\n");
			}
			else {
				printf("OK\n");
			}
		}
		else {
			printf("the u-boot environment parameter name is empty.\n");
			ubootenv_show_usage();
			rc = -EZCFG_E_ARGUMENT ;
		}
	}
	else if (strcmp(argv[1], "list") == 0) {
		if (argc == 2) {
			buf = malloc(UBOOT_ENV_BLOCK_SIZE);
			if (buf == NULL) {
				printf("error! not enough memory.\n");
				return -EZCFG_E_RESOURCE;
			}
			memset(buf, 0, UBOOT_ENV_BLOCK_SIZE);
			rc = ezcfg_api_ubootenv_list(buf, UBOOT_ENV_BLOCK_SIZE);
			if (rc < 0) {
				printf("ERROR\n");
			}
			else {
				printf("%s", buf);
			}
			free(buf);
		}
		else {
			printf("number of arguments is incorrect.\n");
			ubootenv_show_usage();
			rc = -EZCFG_E_ARGUMENT ;
		}
	}
	else if (strcmp(argv[1], "check") == 0) {
		if (argc == 2) {
			buf = malloc(UBOOT_ENV_BLOCK_SIZE);
			if (buf == NULL) {
				printf("error! not enough memory.\n");
				return -EZCFG_E_RESOURCE;
			}
			memset(buf, 0, UBOOT_ENV_BLOCK_SIZE);
			rc = ezcfg_api_ubootenv_check(buf, UBOOT_ENV_BLOCK_SIZE);
			if (rc < 0) {
				printf("ERROR\n");
			}
			else {
				uint32_t crc;
				char *p;
				crc = *((uint32_t *)buf);
				p = buf+sizeof(uint32_t);
				printf("crc=[%lx]\n", crc);
				crc = utils_crc32(p, UBOOT_ENV_BLOCK_SIZE-(sizeof(uint32_t)));
				printf("mycrc=[%lx]\n", crc);
			}
			free(buf);
		}
		else {
			printf("number of arguments is incorrect.\n");
			ubootenv_show_usage();
			rc = -EZCFG_E_ARGUMENT ;
		}
	}
	else {
		printf("unknown ubootenv command: %s\n", argv[1]);
		ubootenv_show_usage();
		rc = -EZCFG_E_ARGUMENT ;
	}

        return rc;
}
