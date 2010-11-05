/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ctrl/ctrl.c
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
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

struct ezcfg_ctrl {
	struct ezcfg *ezcfg;
	struct ezcfg_socket *socket;
};

void ezcfg_ctrl_delete(struct ezcfg_ctrl *ezctrl)
{
	ASSERT(ezctrl);

	if (ezctrl->socket != NULL) {
		ezcfg_socket_delete(ezctrl->socket);
	}
	free(ezctrl);
}

struct ezcfg_ctrl *ezcfg_ctrl_new_from_socket(struct ezcfg *ezcfg, const int family, const unsigned char proto, const char *local_socket_path, const char *remote_socket_path)
{
	struct ezcfg_ctrl *ezctrl;

	ASSERT(ezcfg != NULL);
	ASSERT(local_socket_path != NULL);
	ASSERT(remote_socket_path != NULL);

	ezctrl = calloc(1, sizeof(struct ezcfg_ctrl));
	if (ezctrl == NULL) {
		err(ezcfg, "new controller fail: %m\n");
		return NULL;
	}
	ezctrl->ezcfg = ezcfg;

	ezctrl->socket = ezcfg_socket_new(ezcfg, family, proto, local_socket_path);
	if (ezctrl->socket == NULL) {
		err(ezcfg, "controller add socket[%s] fail: %m\n", local_socket_path);
		goto fail_exit;
	}
	if (local_socket_path[0] != '@') {
		ezcfg_socket_set_need_unlink(ezctrl->socket, true);
	}

	if (ezcfg_socket_enable_receiving(ezctrl->socket) < 0) {
		err(ezcfg, "enable socket [%s] receiving fail: %m\n", local_socket_path);
		ezcfg_socket_close_sock(ezctrl->socket);
		goto fail_exit;
	}

	if (ezcfg_socket_set_remote(ezctrl->socket, AF_LOCAL, remote_socket_path) < 0) {
		err(ezcfg, "set remote socket [%s] receiving fail: %m\n", remote_socket_path);
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
	struct ezcfg *ezcfg;

	ASSERT(ezctrl != NULL);

	ezcfg = ezctrl->ezcfg;
	return ezcfg_socket_connect_remote(ezctrl->socket);
}

int ezcfg_ctrl_read(struct ezcfg_ctrl *ezctrl, void *buf, int len, int flags)
{
	struct ezcfg *ezcfg;

	ASSERT(ezctrl != NULL);

	ezcfg = ezctrl->ezcfg;
	return ezcfg_socket_read(ezctrl->socket, buf, len, flags);
}

int ezcfg_ctrl_write(struct ezcfg_ctrl *ezctrl, const void *buf, int len, int flags)
{
	struct ezcfg *ezcfg;

	ASSERT(ezctrl != NULL);

	ezcfg = ezctrl->ezcfg;
	return ezcfg_socket_write(ezctrl->socket, buf, len, flags);
}

struct ezcfg_socket *ezcfg_ctrl_get_socket(struct ezcfg_ctrl *ezctrl)
{
	return ezctrl->socket;
}
