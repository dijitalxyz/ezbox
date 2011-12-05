/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ctrl/ctrl.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
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

struct ezcfg_ctrl {
	struct ezcfg *ezcfg;
	struct ezcfg_socket *socket;
	char *buffer;
	int buffer_len;
};

void ezcfg_ctrl_delete(struct ezcfg_ctrl *ezctrl)
{
	ASSERT(ezctrl);

	if (ezctrl->socket != NULL) {
		ezcfg_socket_delete(ezctrl->socket);
	}
	if (ezctrl->buffer != NULL) {
		free(ezctrl->buffer);
	}
	free(ezctrl);
}

struct ezcfg_ctrl *ezcfg_ctrl_new(struct ezcfg *ezcfg)
{
	struct ezcfg_ctrl *ezctrl;

	ASSERT(ezcfg != NULL);

	ezctrl = calloc(1, sizeof(struct ezcfg_ctrl));
	if (ezctrl == NULL) {
		err(ezcfg, "new controller fail: %m\n");
		return NULL;
	}
	memset(ezctrl, 0, sizeof(struct ezcfg_ctrl));
	ezctrl->ezcfg = ezcfg;
	ezctrl->socket = NULL;
	ezctrl->buffer = NULL;

	return ezctrl;

//fail_exit:
	ezcfg_ctrl_delete(ezctrl);
	return NULL;
}

struct ezcfg_ctrl *ezcfg_ctrl_new_from_socket(struct ezcfg *ezcfg, const int family, const int proto, const char *local_socket_path, const char *remote_socket_path)
{
	struct ezcfg_ctrl *ezctrl;

	ASSERT(ezcfg != NULL);
	ASSERT(local_socket_path != NULL);
	ASSERT(remote_socket_path != NULL);

#if 0
	ezctrl = calloc(1, sizeof(struct ezcfg_ctrl));
	if (ezctrl == NULL) {
		err(ezcfg, "new controller fail: %m\n");
		return NULL;
	}
	ezctrl->ezcfg = ezcfg;
#else
	ezctrl = ezcfg_ctrl_new(ezcfg);
	if (ezctrl == NULL) {
		err(ezcfg, "new controller fail: %m\n");
		return NULL;
	}
#endif

	ezctrl->socket = ezcfg_socket_new(ezcfg, family, SOCK_STREAM, proto, local_socket_path);
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

struct ezcfg_socket *ezcfg_ctrl_get_socket(const struct ezcfg_ctrl *ezctrl)
{
	return ezctrl->socket;
}

bool ezcfg_ctrl_set_buffer(struct ezcfg_ctrl *ezctrl, char *buf, int buf_len)
{
        char *p;

	ASSERT(ezctrl != NULL);
	ASSERT(buf != NULL);
	ASSERT(buf_len >= 0);

	p = malloc(buf_len);
	if (p == NULL) {
		return false;
	}

	memcpy(p, buf, buf_len);

	if (ezctrl->buffer != NULL) {
		free(ezctrl->buffer);
	}
	ezctrl->buffer = p;
	ezctrl->buffer_len = buf_len;

	return true;
}

char *ezcfg_ctrl_get_buffer(const struct ezcfg_ctrl *ezctrl)
{
	ASSERT(ezctrl != NULL);
	return ezctrl->buffer;
}

int ezcfg_ctrl_get_buffer_len(const struct ezcfg_ctrl *ezctrl)
{
	ASSERT(ezctrl != NULL);
	return ezctrl->buffer_len;
}

void ezcfg_ctrl_reset_attributes(struct ezcfg_ctrl *ezctrl)
{
	struct ezcfg *ezcfg;

	ASSERT(ezctrl != NULL);

	ezcfg = ezctrl->ezcfg;
}

int ezcfg_ctrl_handle_message(struct ezcfg_ctrl *ezctrl, char *output, int len)
{
	struct ezcfg *ezcfg;

	ASSERT(ezctrl != NULL);
	ASSERT(output != NULL);
	ASSERT(len > 0);

	ezcfg = ezctrl->ezcfg;

	DBG("mydebug: %s-%s(%d) bufffer=[%s]\n", __FILE__, __func__, __LINE__, ezctrl->buffer);

	return 0;
}
