/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ctrl/ctrl_upnp.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-06   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

#if 1
#define DBG(format, args...) do { \
	char path[256]; \
	FILE *fp; \
	snprintf(path, 256, "/tmp/%d-debug.txt", getpid()); \
	fp = fopen(path, "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

int ezcfg_ctrl_handle_upnp_message(char **argv, char *output, int len, void *rte)
{
	struct ezcfg *ezcfg;
	struct ezcfg_worker *worker;
	struct ezcfg_master *master;
	struct ezcfg_upnp *upnp;
	struct ezcfg_upnp_ssdp *ssdp;

	if (argv == NULL || argv[0] == NULL) {
		return -1;
	}

	if (strcmp(argv[0], "upnp") != 0) {
		return -1;
	}

	if (argv[1] == NULL) {
		return -1;
	}

	worker = rte;
	ezcfg = ezcfg_worker_get_ezcfg(worker);
	master = ezcfg_worker_get_master(worker);

	if (strcmp(argv[1], "ssdp") == 0) {
		if (argv[2] == NULL) {
			return -1;
		}

		ssdp = ezcfg_upnp_ssdp_new(ezcfg);
		if (ssdp == NULL) {
			return -1;
		}

		if (strcmp(argv[2], "notify_alive") == 0) {
			if (ezcfg_master_upnp_mutex_lock(master) == 0) {
				upnp = ezcfg_master_get_upnp(master);
				if (upnp != NULL) {
					ezcfg_upnp_ssdp_set_upnp(ssdp, upnp);
					ezcfg_upnp_ssdp_notify_alive(ssdp);
				}
				ezcfg_master_upnp_mutex_unlock(master);
				ezcfg_upnp_ssdp_delete(ssdp);
				return 0;
			}
		}
		else if (strcmp(argv[2], "notify_byebye") == 0) {
			if (ezcfg_master_upnp_mutex_lock(master) == 0) {
				upnp = ezcfg_master_get_upnp(master);
				if (upnp != NULL) {
					ezcfg_upnp_ssdp_set_upnp(ssdp, upnp);
					ezcfg_upnp_ssdp_notify_byebye(ssdp);
				}
				ezcfg_master_upnp_mutex_unlock(master);
				ezcfg_upnp_ssdp_delete(ssdp);
				return 0;
			}
		}
		else if (strcmp(argv[2], "msearch_request") == 0) {
			char *ST = argv[3];
			if ((ST == NULL) || (*ST == '\0')) {
				ST = "ssdp:all";
			}
			if (ezcfg_master_upnp_mutex_lock(master) == 0) {
				upnp = ezcfg_master_get_upnp(master);
				if (upnp != NULL) {
					ezcfg_upnp_ssdp_set_upnp(ssdp, upnp);
					if (ezcfg_upnp_ssdp_set_priv_data(ssdp, ST) == true) {
						ezcfg_upnp_ssdp_msearch_request(ssdp);
					}
				}
				ezcfg_master_upnp_mutex_unlock(master);
				ezcfg_upnp_ssdp_delete(ssdp);
				return 0;
			}
		}

		ezcfg_upnp_ssdp_delete(ssdp);
	}
	return -1;
}
