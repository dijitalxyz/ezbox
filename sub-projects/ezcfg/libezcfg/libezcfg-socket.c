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
	int domain;
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
	struct ezcfg *ezcfg;
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
	struct ezcfg *ezcfg;
	assert(sp != NULL);

	ezcfg = sp->ezcfg;

	dbg(ezcfg, "fixme: %s(%d)\n", __func__, __LINE__);
	if (sp->sock >= 0) {
		close(sp->sock);
		/* also remove the filesystem node */
		dbg(ezcfg, "fixme: %s(%d) path=[%s]\n", __func__, __LINE__, sp->lsa.u.sun.sun_path);
		if (sp->lsa.domain == AF_LOCAL) {
			
			if (unlink(sp->lsa.u.sun.sun_path) == -1) {
				err(ezcfg, "%d unlink errno=[%d]\n", __LINE__, errno);
			}
		}
	}
	free(sp);
	dbg(ezcfg, "fixme: %s(%d)\n", __func__, __LINE__);
}

/**
 * ezcfg_socket_new:
 *
 * Create ezcfg unified socket.
 *
 * Returns: a new ezcfg socket
 **/
struct ezcfg_socket *ezcfg_socket_new(struct ezcfg *ezcfg, int domain, const char *socket_path)
{
	struct ezcfg_socket *sp = NULL;
	struct usa *usa = NULL;

	assert(ezcfg != NULL);
	assert(socket_path != NULL);

	dbg(ezcfg, "fixme: %s(%d)\n", __func__, __LINE__);

	if (domain != AF_LOCAL) {
		err(ezcfg, "%s(%d): unknown socket family %d\n", __func__, __LINE__, domain);
		return NULL;
	}

	/* initialize socket */
	if ((sp = calloc(1, sizeof(struct ezcfg_socket))) == NULL) {
		err(ezcfg, "%s(%d): calloc socket fail.\n", __func__, __LINE__);
		return NULL;
	}
	memset(sp, 0, sizeof(struct ezcfg_socket));
	sp->sock = -1;
	sp->ezcfg = ezcfg;

	switch (domain) {
	case AF_LOCAL:
		sp->sock = socket(AF_LOCAL, SOCK_STREAM, 0);
		if (sp->sock < 0) {
			err(ezcfg, "socket error\n");
			goto fail_exit;
		}
		usa = &(sp->lsa);
		usa->domain = AF_LOCAL;
		usa->u.sun.sun_family = AF_LOCAL;
		strcpy(usa->u.sun.sun_path, socket_path);
		usa->len = offsetof(struct sockaddr_un, sun_path) + strlen(usa->u.sun.sun_path);
		/* translate leading '@' to abstract namespace */
		if (usa->u.sun.sun_path[0] == '@')
			usa->u.sun.sun_path[0] = '\0';

		break;

	default:
		err(ezcfg, "%s(%d): bad family [%d].\n", __func__, __LINE__, domain);
		goto fail_exit;
		break;
	}

	return sp;

fail_exit:
	if (sp != NULL) {
		if (sp->sock >= 0)
			close(sp->sock);
		free(sp);
	}
	return NULL;
}

int ezcfg_socket_get_sock(const struct ezcfg_socket *sp)
{
	assert(sp != NULL);
	return sp->sock;
}

char *ezcfg_socket_get_local_socket_path(struct ezcfg_socket *sp)
{
	assert(sp != NULL);
	return sp->lsa.u.sun.sun_path;
}

char *ezcfg_socket_get_remote_socket_path(struct ezcfg_socket *sp)
{
	assert(sp != NULL);
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

	assert(ezcfg != NULL);
	assert(size > 0);

	sp = calloc(size, sizeof(struct ezcfg_socket));
	if (sp == NULL) {
		err(ezcfg, "%s: calloc fail", __func__);
	}
	memset(sp, 0, sizeof(struct ezcfg_socket));
	sp->sock = -1;
	sp->ezcfg = ezcfg;

	return sp;
}

/**
 * ezcfg_socket_delete_list:
 *
 * Delete ezcfg socket list linked with sp.
 *
 * Returns:
 **/
void ezcfg_socket_list_delete(struct ezcfg_socket **list)
{
	struct ezcfg_socket *cur;

	assert(list != NULL);

	cur = *list;
	while (cur != NULL) {
		*list = cur->next;
		ezcfg_socket_delete(cur);
		cur = *list;
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
	assert(list != NULL);
	assert(sp != NULL);

	sp->next = *list;
	*list = sp;
	return 0;
}

struct ezcfg_socket * ezcfg_socket_list_next(struct ezcfg_socket **list)
{
	assert(list != NULL);

	return (*list)->next;
}

/**
 * ezcfg_socket_enable_receiving:
 * @sp: the listening socket which should receive events
 *
 * Binds the @sp to the event source.
 *
 * Returns: 0 on success, otherwise a negative error value.
 */
int ezcfg_socket_enable_receiving(struct ezcfg_socket *sp)
{
	int err = 0;
	//const int on = 1;
	struct usa *usa = NULL;
	struct ezcfg *ezcfg;

	assert(sp != NULL);

	ezcfg = sp->ezcfg;
	usa = &(sp->lsa);

	switch(usa->domain) {
	case AF_LOCAL:
		err = bind(sp->sock,
		           (struct sockaddr *)&usa->u.sun, usa->len);
		break;
	default:
		err(ezcfg, "unknown family [%d]\n", usa->domain);
		return -EINVAL;
	}

	if (err < 0) {
		return err;
	}

	/* enable receiving of sender credentials */
	//setsockopt(sp->sock, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on));
	return 0;
}

/**
 * ezcfg_socket_enable_listening:
 * @sp: the listening socket which start receiving events
 *
 * Makes the @sp listening to the event source.
 *
 * Returns: 0 on success, otherwise a negative error value.
 */
int ezcfg_socket_enable_listening(struct ezcfg_socket *sp, int backlog)
{
	int err = 0;
	//const int on = 1;
	struct usa *usa = NULL;
	struct ezcfg *ezcfg;

	assert(sp != NULL);
	assert(backlog > 0);

	ezcfg = sp->ezcfg;
	usa = &(sp->lsa);

	switch(usa->domain) {
	case AF_LOCAL:
		err = listen(sp->sock, backlog);
		break;
	default:
		err(ezcfg, "unknown family [%d]\n", usa->domain);
		return -EINVAL;
	}

	if (err < 0) {
		return err;
	}

	/* enable receiving of sender credentials */
	//setsockopt(sp->sock, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on));
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

int ezcfg_socket_queue_get_socket(const struct ezcfg_socket *queue, int position, struct ezcfg_socket *sp)
{
	*sp = queue[position];
	return 0;
}

int ezcfg_socket_queue_set_socket(struct ezcfg_socket *queue, int position, const struct ezcfg_socket *sp)
{
	queue[position] = *sp;
	return 0;
}

struct ezcfg_socket *ezcfg_socket_new_accepted_socket(const struct ezcfg_socket *listener)
{
	struct ezcfg_socket *accepted;
	struct ezcfg *ezcfg;

	assert(listener != NULL);
	ezcfg = listener->ezcfg;

	accepted = calloc(1, sizeof(struct ezcfg_socket));
	if (accepted == NULL) {
		err(ezcfg, "%s(%d): calloc fail.\n", __func__, __LINE__);
		return NULL;
	}
	memset(accepted, 0, sizeof(struct ezcfg_socket));
	accepted->ezcfg = ezcfg;
	accepted->lsa = listener->lsa;
	accepted->rsa.len = sizeof(accepted->rsa.u.sun);
	if ((accepted->sock = accept(listener->sock, &(accepted->rsa.u.sa),
	                            &(accepted->rsa.len))) == -1) {
		free(accepted);
		return NULL;
        }

	return accepted;
}

void ezcfg_socket_close_sock(struct ezcfg_socket *sp)
{
	assert(sp != NULL);

	if (sp->sock >= 0) {
		close(sp->sock);
		sp->sock = -1;
	}
}

void ezcfg_socket_set_close_on_exec(struct ezcfg_socket *sp)
{
	assert(sp != NULL);

	if (sp->sock >= 0) {
		fcntl(sp->sock, F_SETFD, FD_CLOEXEC);
	}
}

int ezcfg_socket_set_remote(struct ezcfg_socket *sp, int domain, const char *socket_path)
{
	struct ezcfg *ezcfg;
	struct usa *usa;

	assert(sp != NULL);
	assert(socket_path != NULL);

	ezcfg = sp->ezcfg;
	usa = &(sp->rsa);

	dbg(ezcfg, "fixme: %s(%d)\n", __func__, __LINE__);

	switch (domain) {
	case AF_LOCAL:
		usa->domain = AF_LOCAL;
		usa->u.sun.sun_family = AF_LOCAL;
		strcpy(usa->u.sun.sun_path, socket_path);
		usa->len = offsetof(struct sockaddr_un, sun_path) + strlen(usa->u.sun.sun_path);
		/* translate leading '@' to abstract namespace */
		if (usa->u.sun.sun_path[0] == '@')
			usa->u.sun.sun_path[0] = '\0';

		break;

	default:
		err(ezcfg, "%s(%d): bad family [%d].\n", __func__, __LINE__, domain);
		return -EINVAL;
	}

	return 0;
}

int ezcfg_socket_connect_remote(struct ezcfg_socket *sp)
{
	int err = 0;
	//const int on = 1;
	struct usa *usa;
	struct ezcfg *ezcfg;

	assert(sp != NULL);

	ezcfg = sp->ezcfg;
	usa = &(sp->rsa);

	dbg(ezcfg, "fixme: %s(%d) remote domain=[%d]\n", __func__, __LINE__, usa->domain);
	dbg(ezcfg, "fixme: %s(%d) remote family=[%d]\n", __func__, __LINE__, usa->u.sun.sun_family);
	dbg(ezcfg, "fixme: %s(%d) remote path=[%s]\n", __func__, __LINE__, usa->u.sun.sun_path);
	dbg(ezcfg, "fixme: %s(%d) remote len=[%d]\n", __func__, __LINE__, usa->len);

	switch (usa->domain) {
	case AF_LOCAL:
		err = connect(sp->sock,
		           (struct sockaddr *)&usa->u.sun, usa->len);

		break;

	default:
		err(ezcfg, "%s(%d): bad family [%d].\n", __func__, __LINE__, usa->domain);
		return -EINVAL;
	}

	if (err < 0) {
		return err;
	}

	/* enable receiving of sender credentials */
	//setsockopt(sp->sock, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on));
	return 0;
}
