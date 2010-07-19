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

/* wire protocol magic must match */
#define EZCFG_CTRL_MAGIC	0xdead1dea

enum ezcfg_ctrl_msg_type {
	EZCFG_CTRL_UNKNOWN,
	EZCFG_CTRL_SET_LOG_LEVEL,
	EZCFG_CTRL_RELOAD_RULES,
	EZCFG_CTRL_SET_ENV,
	EZCFG_CTRL_SET_CHILDREN_MAX,
};

struct ezcfg_ctrl_msg_wire {
	char version[16];
	unsigned int magic;
	enum ezcfg_ctrl_msg_type type;
	union {
		int intval;
		char buf[256];
	};
};

struct ezcfg_ctrl_msg {
	struct ezcfg_ctrl *ezctrl;
	struct ezcfg_ctrl_msg_wire ctrl_msg_wire;
	pid_t pid;
};

struct ezcfg_ctrl {
	struct ezcfg *ezcfg;
	int sock;
	struct sockaddr_un saddr;
	socklen_t addrlen;
};

void ezcfg_ctrl_delete(struct ezcfg_ctrl *ezctrl)
{
	if (ezctrl == NULL)
		return;
	if (ezctrl->sock >= 0)
		close(ezctrl->sock);
	free(ezctrl);
}

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

struct ezcfg *ezcfg_ctrl_get_ezcfg(struct ezcfg_ctrl *ezctrl)
{
	return ezctrl->ezcfg;
}

int ezcfg_ctrl_get_fd(struct ezcfg_ctrl *ezctrl)
{
	if (ezctrl == NULL)
		return -1;
	return ezctrl->sock;
}

static int ctrl_send(struct ezcfg_ctrl *ezctrl, enum ezcfg_ctrl_msg_type type, int intval, const char *buf)
{
	struct ezcfg_ctrl_msg_wire ctrl_msg_wire;
	int err;

	memset(&ctrl_msg_wire, 0x00, sizeof(struct ezcfg_ctrl_msg_wire));
	strcpy(ctrl_msg_wire.version, "ezcfg-" VERSION);
	ctrl_msg_wire.magic = EZCFG_CTRL_MAGIC;
	ctrl_msg_wire.type = type;

	if (buf != NULL)
		util_strscpy(ctrl_msg_wire.buf, sizeof(ctrl_msg_wire.buf), buf);
	else
		ctrl_msg_wire.intval = intval;

	err = sendto(ezctrl->sock, &ctrl_msg_wire, sizeof(ctrl_msg_wire), 0,
	             (struct sockaddr *)&ezctrl->saddr, ezctrl->addrlen);
	if (err == -1) {
		err(ezctrl->ezcfg, "error sending message: %m\n");
	}
	return err;
}

int ezcfg_ctrl_send_set_log_level(struct ezcfg_ctrl *ezctrl, int priority)
{
	return ctrl_send(ezctrl, EZCFG_CTRL_SET_LOG_LEVEL, priority, NULL);
}

int ezcfg_ctrl_send_reload_rules(struct ezcfg_ctrl *ezctrl)
{
	return ctrl_send(ezctrl, EZCFG_CTRL_RELOAD_RULES, 0, NULL);
}

int ezcfg_ctrl_send_set_env(struct ezcfg_ctrl *ezctrl, const char *key)
{
	return ctrl_send(ezctrl, EZCFG_CTRL_SET_ENV, 0, key);
}

int ezcfg_ctrl_send_set_children_max(struct ezcfg_ctrl *ezctrl, int count)
{
	return ctrl_send(ezctrl, EZCFG_CTRL_SET_CHILDREN_MAX, count, NULL);
}

struct ezcfg_ctrl_msg *ezcfg_ctrl_receive_msg(struct ezcfg_ctrl *ezctrl)
{
	struct ezcfg_ctrl_msg *ezctrl_msg;
	ssize_t size;
	struct msghdr smsg;
	struct cmsghdr *cmsg;
	struct iovec iov;
	struct ucred *cred;
	char cred_msg[CMSG_SPACE(sizeof(struct ucred))];

	ezctrl_msg = calloc(1, sizeof(struct ezcfg_ctrl_msg));
	if (ezctrl_msg == NULL)
		return NULL;
	ezctrl_msg->ezctrl = ezctrl;

	iov.iov_base = &ezctrl_msg->ctrl_msg_wire;
	iov.iov_len = sizeof(struct ezcfg_ctrl_msg_wire);

	memset(&smsg, 0x00, sizeof(struct msghdr));
	smsg.msg_iov = &iov;
	smsg.msg_iovlen = 1;
	smsg.msg_control = cred_msg;
	smsg.msg_controllen = sizeof(cred_msg);

	size = recvmsg(ezctrl->sock, &smsg, 0);
	if (size <  0) {
		err(ezctrl->ezcfg, "unable to receive user ezcd message: %m\n");
		goto err;
	}
	cmsg = CMSG_FIRSTHDR(&smsg);
	cred = (struct ucred *) CMSG_DATA(cmsg);

	if (cmsg == NULL || cmsg->cmsg_type != SCM_CREDENTIALS) {
		err(ezctrl->ezcfg, "no sender credentials received, message ignored\n");
		goto err;
	}

	if (cred->uid != 0) {
		err(ezctrl->ezcfg, "sender uid=%i, message ignored\n", cred->uid);
		goto err;
	}

	ezctrl_msg->pid = cred->pid;

	if (ezctrl_msg->ctrl_msg_wire.magic != EZCFG_CTRL_MAGIC) {
		err(ezctrl->ezcfg, "message magic 0x%08x doesn't match, ignore it\n", ezctrl_msg->ctrl_msg_wire.magic);
		goto err;
	}

	dbg(ezctrl->ezcfg, "created ctrl_msg %p (%i)\n", ezctrl_msg, ezctrl_msg->ctrl_msg_wire.type);
	return ezctrl_msg;
err:
        return NULL;
}

