/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg-ctrl.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-12   0.1       Write it from scratch
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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>

#include "libezcfg.h"
#include "libezcfg-private.h"

struct ezcfg_ctrl {
	struct ezcfg *ezcfg;
	int sock;
	struct sockaddr_un saddr;
	socklen_t addrlen;
};

struct ezcfg_ctrl *ezcfg_ctrl_new_from_socket(struct ezcfg *ezcfg, const char *socket_path)
{
	struct ezcfg_ctrl *ezctrl;

	ezctrl = calloc(1, sizeof(struct ezcfg_ctrl));
	if (ezctrl == NULL)
		return NULL;
	ezctrl->ezcfg = ezcfg;

	ezctrl->sock = socket(AF_LOCAL, SOCK_DGRAM, 0);
	if (ezctrl->sock < 0) {
		err(ezcfg, "error getting socket: %m\n");
		return NULL;
	}

	ezctrl->saddr.sun_family = AF_LOCAL;
	strcpy(ezctrl->saddr.sun_path, socket_path);
	ezctrl->addrlen = offsetof(struct sockaddr_un, sun_path) + strlen(ezctrl->saddr.sun_path);
	/* translate leading '@' to abstract namespace */
	if (ezctrl->saddr.sun_path[0] == '@')
		ezctrl->saddr.sun_path[0] = '\0';

	return ezctrl;
}

int ezcfg_ctrl_enable_receiving(struct ezcfg_ctrl *ezctrl)
{
	int err;
	const int feature_on = 1;

	err= bind(ezctrl->sock, (struct sockaddr *)&ezctrl->saddr, ezctrl->addrlen);
	if (err < 0) {
		err(ezctrl->ezcfg, "bind failed: %m\n");
		return err;
	}

	/* enable receiving of the sender credentials */
	setsockopt(ezctrl->sock, SOL_SOCKET, SO_PASSCRED, &feature_on, sizeof(feature_on));
	return 0;
}
