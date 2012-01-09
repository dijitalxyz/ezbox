/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_iptables_firewall.c
 *
 * Description  : ezbox /etc/iptables/firewall generating program
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2012-01-08   0.1       Write it from scratch
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

/********************
 * firewall data structure
 ********************/
typedef struct fw_param_s {
	char lan_ifname[IFNAMSIZ];
	char wan_ifname[IFNAMSIZ];
} fw_param_t;

static void delete_fw_param(fw_param_t *fwp)
{
	free(fwp);
}

static bool init_fw_param(fw_param_t *fwp)
{
	return true;
}

/********************
 * mangle table
 ********************/
static bool build_mangle_table(FILE *fp, fw_param_t *fwp)
{
	char buf[1024];

	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		"*mangle");
	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		":PREROUTING ACCEPT [0:0]");
	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		":OUTPUT ACCEPT [0:0]");

	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		"COMMIT");

	return true;
}

/********************
 * nat table
 ********************/
static bool build_nat_prerouting(FILE *fp, char *buf, size_t size, fw_param_t *fwp)
{
	return true;
}

static bool build_nat_postrouting(FILE *fp, char *buf, size_t size, fw_param_t *fwp)
{
	return true;
}

static bool build_nat_table(FILE *fp, fw_param_t *fwp)
{
	char buf[1024];

	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		"*nat");
	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		":PREROUTING ACCEPT [0:0]");
	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		":POSTROUTING ACCEPT [0:0]");
	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		":OUTPUT ACCEPT [0:0]");

	build_nat_prerouting(fp, buf, sizeof(buf), fwp);
	build_nat_postrouting(fp, buf, sizeof(buf), fwp);

	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		"COMMIT");

	return true;
}

/********************
 * filter table
 ********************/
static bool build_filter_input(FILE *fp, char *buf, size_t size, fw_param_t *fwp)
{
	return true;
}

static bool build_filter_output(FILE *fp, char *buf, size_t size, fw_param_t *fwp)
{
	return true;
}

static bool build_filter_forward(FILE *fp, char *buf, size_t size, fw_param_t *fwp)
{
	return true;
}

static bool build_filter_table(FILE *fp, fw_param_t *fwp)
{
	char buf[1024];

	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		"*filter");
	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		":INPUT ACCEPT [0:0]");
	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		":FORWARD ACCEPT [0:0]");
	utils_file_print_line(fp, buf, sizeof(buf), "%s\n",
		":OUTPUT ACCEPT [0:0]");

	build_filter_input(fp, buf, sizeof(buf), fwp);
	build_filter_output(fp, buf, sizeof(buf), fwp);
	build_filter_forward(fp, buf, sizeof(buf), fwp);

	return true;
}

int pop_etc_iptables_firewall(int flag)
{
	FILE *file = NULL;
	int ret = EXIT_FAILURE;
	fw_param_t *fwp = NULL;

	file = fopen("/etc/iptables/firewall", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_STOP :
		if (flag == RC_ACT_STOP) {
			ret = EXIT_SUCCESS;
			break;
		}

		/* RC_ACT_RESTART fall through */
	case RC_ACT_START :
		fwp = malloc(sizeof(fw_param_t));
		if (fwp == NULL) {
			ret = EXIT_FAILURE;
			break;
		}

		/* initialize fw_param */
		if (init_fw_param(fwp) == false) {
			ret = EXIT_FAILURE;
			break;
		}

		/* generate MANGLE table */
		if (build_mangle_table(file, fwp) == false) {
			ret = EXIT_FAILURE;
			break;
		}

		/* generate NAT table */
		if (build_nat_table(file, fwp) == false) {
			ret = EXIT_FAILURE;
			break;
		}

		/* generate FILTER table */
		if (build_filter_table(file, fwp) == false) {
			ret = EXIT_FAILURE;
			break;
		}

		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}

	if (file != NULL)
		fclose(file);

	if (fwp != NULL)
		delete_fw_param(fwp);

	if (ret == EXIT_FAILURE) {
		unlink("/etc/iptables/firewall");
	}

	return (ret);
}
