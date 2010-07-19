/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg-socket.c
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
#include <assert.h>
#include <pthread.h>

#include "libezcfg.h"
#include "libezcfg-private.h"

/*
 * unified socket address. For IPv6 support, add IPv6 address structure
 * in the union u.
 */
struct usa {
	socklen_t len;
	union {
		struct sockaddr sa;
		struct sockaddr_un sun;
        } u;
};

/*
 * structure used to describe listening socket, or socket which was
 * accept()-ed by the master thread and queued for future handling
 * by the worker thread.
 */
struct ezcfg_socket {
	struct ezcfg_socket *next;	/* Linkage                      */
	int		sock;		/* Listening socket             */
	struct usa	lsa;		/* Local socket address         */
	struct usa	rsa;		/* Remote socket address        */
};

/**
 * ezcfg_socket_delete:
 *
 * Delete ezcfg unified socket.
 *
 * Returns:
 **/
void ezcfg_socket_delete(struct ezcfg_socket *sp)
{
	if (sp == NULL)
		return ;
	if (sp->sock) {
		close(sp->sock);
	}
	free(sp);
}

/**
 * ezcfg_socket_new:
 *
 * Create ezcfg unified socket.
 *
 * Returns: a new ezcfg socket
 **/
struct ezcfg_socket *ezcfg_socket_new(struct ezcfg *ezcfg, int family, const char *socket_path)
{
	struct stat statbuf;
	struct ezcfg_socket *sp = NULL;
	int sock = -1;
	struct usa *usa = NULL;

	if (socket_path == NULL)
		return NULL;

	/* initialize socket */
	if ((sp = calloc(1, sizeof(struct ezcfg_socket))) == NULL)
		return NULL;

	if (family != AF_LOCAL) {
		err(ezcfg, "unknown socket family %d\n", family);
		goto fail_exit;
	}

	if (family == AF_LOCAL) {
		usa = &(sp->lsa);
		usa->u.sun.sun_family = AF_LOCAL;
		if (socket_path[0] == '@') {
			/* translate leading '@' to abstract namespace */
			util_strscpy(usa->u.sun.sun_path, sizeof(usa->u.sun.sun_path), socket_path);
			usa->u.sun.sun_path[0] = '\0';
			usa->len = offsetof(struct sockaddr_un, sun_path) + strlen(socket_path);
		} else if (stat(socket_path, &statbuf) == 0 && S_ISSOCK(statbuf.st_mode)) {
			/* existing socket file */
			util_strscpy(usa->u.sun.sun_path, sizeof(usa->u.sun.sun_path), socket_path);
			usa->len = offsetof(struct sockaddr_un, sun_path) + strlen(socket_path);
		} else {
			/* no socket file, assume abstract namespace socket */
			util_strscpy(&usa->u.sun.sun_path[1], sizeof(usa->u.sun.sun_path)-1, socket_path);
			usa->len = offsetof(struct sockaddr_un, sun_path) + strlen(socket_path)+1;
		}

		if ((sock = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
			err(ezcfg, "socket error\n");
			goto fail_exit;
		}
		sp->sock = sock;
	}
	return sp;
fail_exit:
	if (sp != NULL)
		free(sp);
	if (sock >= 0)
		close(sock);
	return NULL;
}

int ezcfg_socket_get_sock(const struct ezcfg_socket *sp)
{
	if (sp == NULL)
		return -1;
	return sp->sock;
}

char *ezcfg_socket_get_remote_socket_path(struct ezcfg_socket *sp)
{
	if (sp == NULL)
		return NULL;
	return sp->rsa.u.sun.sun_path;
}

/**
 * ezcfg_socket_calloc:
 *
 * Allocate ezcfg socket buffer.
 *
 * Returns:
 **/
struct ezcfg_socket *ezcfg_socket_calloc(struct ezcfg *ezcfg, int size)
{
	struct ezcfg_socket *sp;
	if (size <= 0) {
		err(ezcfg, "%s: calloc size should be > 0", __func__);
		return NULL;
	}
	sp = calloc(size, sizeof(struct ezcfg_socket));
	if (sp == NULL) {
		err(ezcfg, "%s: calloc fail", __func__);
	}
	return sp;
}

int ezcfg_socket_copy(struct ezcfg_socket *dst, struct ezcfg_socket *src)
{
	*dst = *src;
	return 0;
}

/**
 * ezcfg_socket_delete_list:
 *
 * Delete ezcfg socket list linked with sp.
 *
 * Returns:
 **/
void ezcfg_socket_list_delete(struct ezcfg_socket *sp)
{
	struct ezcfg_socket *next;
	if (sp == NULL)
		return ;
	for (next = sp->next; sp ; sp = next) {
		if (sp->sock >= 0)
			close(sp->sock);
		free(sp);
	}
}

/**
 * ezcfg_socket_list_insert:
 *
 * Add socket to list header.
 *
 * Returns:
 **/
int ezcfg_socket_list_insert(struct ezcfg_socket **list, struct ezcfg_socket *sp)
{
	if (sp == NULL || list == NULL) {
		return -1;
	}
	sp->next = *list;
	*list = sp;
	return 0;
}

struct ezcfg_socket * ezcfg_socket_list_next(struct ezcfg_socket **list)
{
	if (list == NULL) {
		return NULL;
	}
	return (*list)->next;
}

/**
 * ezcfg_socket_enable_receiving:
 * @list: the socket list which should receive events
 * @sock: the listening socket which should receive events
 *
 * Binds the @list to the event source.
 *
 * Returns: 0 on success, otherwise a negative error value.
 */
int ezcfg_socket_enable_receiving(struct ezcfg_socket *sp)
{
	int err = 0;
	const int on = 1;
	struct usa *usa = NULL;

	if (sp == NULL)
		return -EINVAL;

	usa = &(sp->lsa);
	if (usa->u.sun.sun_family != 0) {
		err = bind(sp->sock,
		           (struct sockaddr *)&usa->u.sun, usa->len);
	} else {
		return -EINVAL;
	}
	if (err < 0) {
		return err;
	}

	/* enable receiving of sender credentials */
	setsockopt(sp->sock, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on));
	return 0;
}

/**
 * ezcfg_socket_set_receive_buffer_size:
 * @sp: the socket which should receive events
 * @size: the size in bytes
 *
 * Set the size of the kernel socket buffer. This call needs the
 * appropriate privileges to succeed.
 *
 * Returns: 0 on success, otherwise -1 on error.
 */
int ezcfg_socket_set_receive_buffer_size(struct ezcfg_socket *sp, int size)
{
	if (sp == NULL)
		return -1;
	if (setsockopt(sp->sock, SOL_SOCKET, SO_RCVBUFFORCE, &size, sizeof(size)) < 0)
		return -1;
	return 0;
}

int ezcfg_socket_queue_set_socket(struct ezcfg_socket *queue, int position, const struct ezcfg_socket *sp)
{
	queue[position] = *sp;
	return 0;
}

struct ezcfg_socket *ezcfg_socket_new_accepted_socket(const struct ezcfg_socket *listener)
{
	struct ezcfg_socket *accepted = NULL;
	if (listener == NULL)
		return NULL;

	accepted = calloc(1, sizeof(struct ezcfg_socket));
	if (accepted == NULL)
		return NULL;

	accepted->rsa.len = sizeof(accepted->rsa.u.sun);
	accepted->lsa = listener->lsa;
	if ((accepted->sock = accept(listener->sock, &(accepted->rsa.u.sa),
	                            &(accepted->rsa.len))) == -1) {
		free(accepted);
		return NULL;
        }

	return accepted;
}

void ezcfg_socket_close_sock(struct ezcfg_socket *sp)
{
	if (sp == NULL)
		return;

	if (sp->sock >= 0)
		close(sp->sock);
}
