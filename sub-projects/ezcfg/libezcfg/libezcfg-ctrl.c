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

#define _GNU_SOURCE
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
#include <assert.h>
#include <pthread.h>

#include "libezcfg.h"
#include "libezcfg-private.h"

struct ezcfg_ctrl {
	struct ezcfg *ezcfg;
	struct ezcfg_socket *socket;
};

void ezcfg_ctrl_delete(struct ezcfg_ctrl *ezctrl)
{
	assert(ezctrl);

	if (ezctrl->socket != NULL) {
		ezcfg_socket_delete(ezctrl->socket);
	}
	free(ezctrl);
}

struct ezcfg_ctrl *ezcfg_ctrl_new_from_socket(struct ezcfg *ezcfg, const int family, const unsigned char proto, const char *socket_path)
{
	struct ezcfg_ctrl *ezctrl;

	assert(ezcfg != NULL);
	assert(socket_path != NULL);

	ezctrl = calloc(1, sizeof(struct ezcfg_ctrl));
	if (ezctrl == NULL) {
		err(ezcfg, "new controller fail: %m\n");
		return NULL;
	}
	ezctrl->ezcfg = ezcfg;

	ezctrl->socket = ezcfg_socket_new(ezcfg, family, proto, socket_path);
	if (ezctrl->socket == NULL) {
		err(ezcfg, "controller add socket[%s] fail: %m\n", socket_path);
		goto fail_exit;
	}

	if (ezcfg_socket_enable_receiving(ezctrl->socket) < 0) {
		err(ezcfg, "enable socket [%s] receiving fail: %m\n", socket_path);
		ezcfg_socket_close_sock(ezctrl->socket);
		goto fail_exit;
	}

	if (ezcfg_socket_set_remote(ezctrl->socket, AF_LOCAL, EZCFG_CTRL_SOCK_PATH) < 0) {
		err(ezcfg, "set remote socket [%s] receiving fail: %m\n", EZCFG_CTRL_SOCK_PATH);
		ezcfg_socket_close_sock(ezctrl->socket);
		goto fail_exit;
	}

	return ezctrl;

fail_exit:
	ezcfg_ctrl_delete(ezctrl);
	return NULL;
}

int ezcfg_ctrl_connect(struct ezcfg_ctrl *ezctrl)
{
	assert(ezctrl != NULL);
	return ezcfg_socket_connect_remote(ezctrl->socket);
}

int ezcfg_ctrl_read(struct ezcfg_ctrl *ezctrl, void *buf, int len, int flags)
{
	assert(ezctrl != NULL);
	return ezcfg_socket_read(ezctrl->socket, buf, len, flags);
}

int ezcfg_ctrl_write(struct ezcfg_ctrl *ezctrl, const void *buf, int len, int flags)
{
	assert(ezctrl != NULL);
	return ezcfg_socket_write(ezctrl->socket, buf, len, flags);
}

