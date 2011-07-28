/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_handle_kernel_module.c
 *
 * Description  : ezcfg install/remove kernel modules function
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-07-28   0.1       Write it from scratch
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

typedef struct mod_dep_s {
	char *name;
	char *list;
} mod_dep_t;

static mod_dep_t mod_depends[] = {
	{ "fat", "nls_base,nls_cp437,nls_iso8859-1" },
	{ "vfat", "nls_base,nls_cp437,nls_iso8859-1,fat" },
};

int utils_install_kernel_module(char *name, char *args)
{
	mod_dep_t *mdp;
	char buf[128];
	char *p, *q, *l;
	char *kver;
	int i;
	int ret = EXIT_FAILURE;

	if (name == NULL) {
		return ret;
	}

	kver = utils_get_kernel_version();
	if (kver == NULL) {
		return ret;
	}

	for (i = 0; i < ARRAY_SIZE(mod_depends); i++) {
		mdp = &mod_depends[i];
		if (strcmp(mdp->name, name) == 0) {
			l = strdup(mdp->list);
			if (l != NULL) {
				p = l;
				while(p != NULL) {
					q = strchr(p, ',');
					if (q != NULL)
						*q = '\0';

					snprintf(buf, sizeof(buf), "%s /lib/modules/%s/%s.ko", CMD_INSMOD, kver, p);
					system(buf);

					if (q != NULL)
						p = q+1;
					else
						p = NULL;
				}
				free(l);
			}
		}
        }

	/* insmod the kernel module directly */
	p = (args == NULL) ? "" : args;
	snprintf(buf, sizeof(buf), "%s /lib/modules/%s/%s.ko %s", CMD_INSMOD, kver, name, p);
	system(buf);
	free(kver);
	ret = EXIT_SUCCESS;

	return ret;
}

int utils_remove_kernel_module(char *name)
{
	mod_dep_t *mdp;
	char buf[128];
	char *p, *q, *l;
	int i;
	int ret = EXIT_FAILURE;

	if (name == NULL) {
		return ret;
	}

	for (i = 0; i < ARRAY_SIZE(mod_depends); i++) {
		mdp = &mod_depends[i];
		if (strcmp(mdp->name, name) == 0) {
			l = strdup(mdp->list);
			if (l != NULL) {
				p = l;
				while(p != NULL) {
					q = strrchr(p, ',');
					if (q != NULL) {
						p = q+1;
					}

					snprintf(buf, sizeof(buf), "%s %s", CMD_RMMOD, p);
					system(buf);

					if (q != NULL) {
						*q = '\0';
						p = l;
					}
					else {
						p = NULL;
					}
				}
				free(l);
			}
		}
        }

	/* rmmod the kernel module directly */
	snprintf(buf, sizeof(buf), "%s %s", CMD_RMMOD, name);
	system(buf);
	ret = EXIT_SUCCESS;

	return ret;
}
