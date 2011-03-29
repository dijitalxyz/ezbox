/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : socket/socket.c
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
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-socket.h"

static int set_non_blocking_mode(int sock)
{
	int flags;

	flags = fcntl(sock, F_GETFL, 0);
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);

	return 0;
}

static void close_socket_gracefully(int sock) {
	char buf[1024];
	int n;

	/* Send FIN to the client */
	shutdown(sock, 1);
	set_non_blocking_mode(sock);

	/* Read and discard pending data. If we do not do that and close the
	 * socket, the data in the send buffer may be discarded.
	 */
	do {
		n = read(sock, buf, sizeof(buf));
	} while (n > 0);

	/* Now we know that our FIN is ACK-ed, safe to close */
	close(sock);
}

/**
 * ezcfg_socket_delete:
 * Delete ezcfg unified socket.
 * Returns:
 **/
void ezcfg_socket_delete(struct ezcfg_socket *sp)
{
	struct ezcfg *ezcfg;
	ASSERT(sp != NULL);

	ezcfg = sp->ezcfg;

	if (sp->sock >= 0) {
		close(sp->sock);
		/* also remove the filesystem node */
		if (sp->lsa.domain == AF_LOCAL && sp->need_unlink == true) {
			
			if (unlink(sp->lsa.u.sun.sun_path) == -1) {
				err(ezcfg, "unlink fail: %m\n");
			}
		}
	}
	free(sp);
}

/**
 * ezcfg_socket_new:
 * Create ezcfg unified socket.
 * Returns: a new ezcfg socket
 **/
struct ezcfg_socket *ezcfg_socket_new(struct ezcfg *ezcfg, const int domain, const int type, const int proto, const char *socket_path)
{
	struct ezcfg_socket *sp = NULL;
	struct usa *usa = NULL;
	char *addr = NULL, *port;
	int sock_protocol = -1;

	ASSERT(ezcfg != NULL);
	ASSERT(socket_path != NULL);

	if ((domain != AF_LOCAL) &&
	    (domain != AF_INET)){
		err(ezcfg, "unknown socket family %d\n", domain);
		return NULL;
	}

	if (proto != EZCFG_PROTO_HTTP &&
	    proto != EZCFG_PROTO_SOAP_HTTP &&
	    proto != EZCFG_PROTO_IGRS &&
	    proto != EZCFG_PROTO_ISDP) {
		err(ezcfg, "unknown communication protocol %d\n", proto);
		return NULL;
	}

	/* initialize socket */
	if ((sp = calloc(1, sizeof(struct ezcfg_socket))) == NULL) {
		err(ezcfg, "calloc socket fail: %m\n");
		return NULL;
	}
	memset(sp, 0, sizeof(struct ezcfg_socket));
	sp->sock = -1;
	sp->ezcfg = ezcfg;
	sp->proto = proto;

	/* FIXME: should change sock_protocol w/r proto */
	sock_protocol = 0;

	switch (domain) {
	case AF_LOCAL:
		sp->sock = socket(AF_LOCAL, type, sock_protocol);
		if (sp->sock < 0) {
			err(ezcfg, "socket error\n");
			goto fail_exit;
		}
		usa = &(sp->lsa);
		usa->domain = AF_LOCAL;
		usa->type = type;
		usa->u.sun.sun_family = AF_LOCAL;
		strcpy(usa->u.sun.sun_path, socket_path);
		usa->len = offsetof(struct sockaddr_un, sun_path) + strlen(usa->u.sun.sun_path);
		/* translate leading '@' to abstract namespace */
		if (usa->u.sun.sun_path[0] == '@') {
			usa->u.sun.sun_path[0] = '\0';
		}
		break;

	case AF_INET:
		addr = strdup(socket_path);
		if (addr == NULL) {
			err(ezcfg, "can not alloc addr.\n");
			goto fail_exit;
		}
		port = strchr(addr, ':');
		if (port == NULL) {
			err(ezcfg, "socket_path format error.\n");
			goto fail_exit;
		}
		*port = '\0';
		port++;

		sp->sock = socket(AF_INET, type, sock_protocol);
		if (sp->sock < 0) {
			err(ezcfg, "socket error\n");
			goto fail_exit;
		}
		usa = &(sp->lsa);
		usa->domain = AF_INET;
		usa->type = type;
		usa->u.sin.sin_family = AF_INET;
		usa->u.sin.sin_port = htons((uint16_t)atoi(port));
		usa->u.sin.sin_addr.s_addr = inet_addr(addr);
		if (usa->u.sin.sin_addr.s_addr == 0) {
			err(ezcfg, "convert IP address error\n");
			goto fail_exit;
		}
		usa->len = sizeof(usa->u.sin);
		free(addr);
		break;

	default:
		err(ezcfg, "bad family [%d]\n", domain);
		goto fail_exit;
		break;
	}

	return sp;

fail_exit:
	if (addr != NULL) {
		free(addr);
	}

	if (sp != NULL) {
		if (sp->sock >= 0) {
			close(sp->sock);
		}
		free(sp);
	}
	return NULL;
}

int ezcfg_socket_get_sock(const struct ezcfg_socket *sp)
{
	ASSERT(sp != NULL);
	return sp->sock;
}

unsigned char ezcfg_socket_get_proto(const struct ezcfg_socket *sp)
{
	ASSERT(sp != NULL);
	return sp->proto;
}

struct ezcfg_socket *ezcfg_socket_get_next(const struct ezcfg_socket *sp)
{
	ASSERT(sp != NULL);
	return sp->next;
}

int ezcfg_socket_get_local_socket_len(struct ezcfg_socket *sp)
{
	ASSERT(sp != NULL);
	return sp->lsa.len;
}

int ezcfg_socket_get_local_socket_domain(struct ezcfg_socket *sp)
{
	ASSERT(sp != NULL);
	return sp->lsa.domain;
}

char *ezcfg_socket_get_local_socket_path(struct ezcfg_socket *sp)
{
	ASSERT(sp != NULL);
	return sp->lsa.u.sun.sun_path;
}

int ezcfg_socket_get_remote_socket_len(struct ezcfg_socket *sp)
{
	ASSERT(sp != NULL);
	return sp->rsa.len;
}

int ezcfg_socket_get_remote_socket_domain(struct ezcfg_socket *sp)
{
	ASSERT(sp != NULL);
	return sp->rsa.domain;
}

char *ezcfg_socket_get_remote_socket_path(struct ezcfg_socket *sp)
{
	ASSERT(sp != NULL);
	return sp->rsa.u.sun.sun_path;
}

/**
 * ezcfg_socket_calloc:
 * Allocate ezcfg socket buffer.
 * Returns:
 **/
struct ezcfg_socket *ezcfg_socket_calloc(struct ezcfg *ezcfg, int size)
{
	struct ezcfg_socket *sp;

	ASSERT(ezcfg != NULL);
	ASSERT(size > 0);

	sp = calloc(size, sizeof(struct ezcfg_socket));
	if (sp == NULL) {
		err(ezcfg, "calloc fail: %m\n");
	}
	memset(sp, 0, sizeof(struct ezcfg_socket));
	sp->sock = -1;
	sp->ezcfg = ezcfg;

	return sp;
}

/**
 * ezcfg_socket_list_delete_socket:
 * Delete a special socket from socket list.
 * Returns: new list header store in list.
 * Returns: true if delete sp in list, otherwise false.
 **/
bool ezcfg_socket_list_delete_socket(struct ezcfg_socket **list, struct ezcfg_socket *sp)
{
	struct ezcfg_socket *prev, *cur;

	ASSERT(list != NULL);

	if (sp == NULL) {
		return true;
	}

	cur = *list;
	if (sp == cur) {
		*list = cur->next;
		ezcfg_socket_delete(cur);
		return true;
	}

	prev = cur;
	cur = cur->next;
	while (cur != NULL) {
		if (sp == cur) {
			prev->next = cur->next;
			ezcfg_socket_delete(cur);
			return true;
		}
		prev = cur;
		cur = cur->next;
	}
	return false;
}

/**
 * ezcfg_socket_list_delete:
 * Delete ezcfg socket list linked with sp.
 * Returns:
 **/
void ezcfg_socket_list_delete(struct ezcfg_socket **list)
{
	struct ezcfg_socket *cur;

	ASSERT(list != NULL);

	cur = *list;
	while (cur != NULL) {
		*list = cur->next;
		ezcfg_socket_delete(cur);
		cur = *list;
	}
}

/**
 * ezcfg_socket_list_insert:
 * Add socket to list header.
 * Returns:
 **/
int ezcfg_socket_list_insert(struct ezcfg_socket **list, struct ezcfg_socket *sp)
{
	ASSERT(list != NULL);
	ASSERT(sp != NULL);

	sp->next = *list;
	*list = sp;
	return 0;
}

bool ezcfg_socket_list_in(struct ezcfg_socket **list, struct ezcfg_socket *sp)
{
	struct ezcfg *ezcfg;
	struct ezcfg_socket *cur;
	struct usa *usa = NULL, *usa2 = NULL;

	ASSERT(list != NULL);
	ASSERT(sp != NULL);

	ezcfg = sp->ezcfg;

	cur = *list;
	usa = &(sp->lsa);
	while (cur != NULL) {
		usa2 = &(cur->lsa);
		if ((sp->proto == cur->proto) &&
		    (usa->domain == usa2->domain) &&
		    (usa->type == usa2->type) &&
		    (usa->len == usa2->len)) {
			if (memcmp(&(usa->u.sa), &(usa2->u.sa), usa->len) == 0) {
				info(ezcfg, "find match socket\n");
				return true;
			}
		}
		cur = cur->next;
	}
	return false;
}

struct ezcfg_socket * ezcfg_socket_list_next(struct ezcfg_socket **list)
{
	ASSERT(list != NULL);

	return (*list)->next;
}

/**
 * ezcfg_socket_enable_receiving:
 * @sp: the listening socket which should receive events
 * Binds the @sp to the event source.
 * Returns: 0 on success, otherwise a negative error value.
 */
int ezcfg_socket_enable_receiving(struct ezcfg_socket *sp)
{
	int err = 0;
	struct usa *usa = NULL;
	struct ezcfg *ezcfg;

	ASSERT(sp != NULL);

	ezcfg = sp->ezcfg;
	usa = &(sp->lsa);

	switch(usa->domain) {
	case AF_LOCAL:
		err = bind(sp->sock,
		           (struct sockaddr *)&usa->u.sun, usa->len);
		break;

	case AF_INET:
		err = bind(sp->sock,
		           (struct sockaddr *)&usa->u.sin, usa->len);
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
 * Makes the @sp listening to the event source.
 * Returns: 0 on success, otherwise a negative error value.
 */
int ezcfg_socket_enable_listening(struct ezcfg_socket *sp, int backlog)
{
	int err = 0;
	struct usa *usa = NULL;
	struct ezcfg *ezcfg;

	ASSERT(sp != NULL);
	ASSERT(backlog > 0);

	ezcfg = sp->ezcfg;
	sp->backlog = backlog;
	usa = &(sp->lsa);

	switch(usa->domain) {
	case AF_LOCAL:
		err = listen(sp->sock, backlog);
		break;

	case AF_INET:
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
 * ezcfg_socket_enable_again:
 * @sp: the listening socket which will re-enable
 * Makes the @sp listening to the event source.
 * Returns: 0 on success, otherwise a negative error value.
 */
int ezcfg_socket_enable_again(struct ezcfg_socket *sp)
{
	int err = 0;
	struct usa *usa = NULL;
	struct ezcfg *ezcfg;
	int sock_protocol = -1;

	ASSERT(sp != NULL);

	ezcfg = sp->ezcfg;
	usa = &(sp->lsa);

	ezcfg_socket_close_sock(sp);

	/* FIXME: should change sock_protocol w/r sp->proto */
	sock_protocol = 0;
	sp->sock = socket(usa->domain, usa->type, sock_protocol);
	if (sp->sock < 0) {
		return -1;
	}

	err = ezcfg_socket_enable_receiving(sp);
	if (err < 0) {
		return err;
	}

	if (sp->backlog > 0) {
		err = ezcfg_socket_enable_listening(sp, sp->backlog);
		if (err < 0) {
			return err;
		}
	}

        ezcfg_socket_set_close_on_exec(sp);

	return 0;
}

/**
 * ezcfg_socket_set_receive_buffer_size:
 * @sp: the socket which should receive events
 * @size: the size in bytes
 * Set the size of the kernel socket buffer. This call needs the
 * appropriate privileges to succeed.
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

int ezcfg_socket_queue_get_socket(const struct ezcfg_socket *queue, int pos, struct ezcfg_socket *sp)
{
	*sp = queue[pos];
	return 0;
}

int ezcfg_socket_queue_set_socket(struct ezcfg_socket *queue, int pos, const struct ezcfg_socket *sp)
{
	queue[pos] = *sp;
	return 0;
}

struct ezcfg_socket *ezcfg_socket_new_accepted_socket(const struct ezcfg_socket *listener)
{
	struct ezcfg_socket *accepted;
	struct ezcfg *ezcfg;
	int domain;

	ASSERT(listener != NULL);
	ezcfg = listener->ezcfg;

	accepted = calloc(1, sizeof(struct ezcfg_socket));
	if (accepted == NULL) {
		err(ezcfg, "calloc fail: %m\n");
		return NULL;
	}
	memset(accepted, 0, sizeof(struct ezcfg_socket));
	accepted->ezcfg = ezcfg;
	accepted->sock = -1;
	accepted->proto = listener->proto;
	accepted->lsa = listener->lsa;
	accepted->need_unlink = listener->need_unlink;
	domain = listener->lsa.domain;
	accepted->rsa.domain = domain;

	switch(domain) {
	case AF_LOCAL:
		accepted->rsa.len = sizeof(accepted->rsa.u.sun);
		accepted->sock = accept(listener->sock,
		                        &(accepted->rsa.u.sa),
		                        &(accepted->rsa.len));
		if (accepted->sock == -1) {
			free(accepted);
			return NULL;
		}
		break;

	case AF_INET:
		accepted->rsa.len = sizeof(accepted->rsa.u.sin);
		accepted->sock = accept(listener->sock,
		                        &(accepted->rsa.u.sa),
		                        &(accepted->rsa.len));
		if (accepted->sock == -1) {
			free(accepted);
			return NULL;
		}
		break;

	default:
		err(ezcfg, "unknown socket family [%d]\n", domain);
		free(accepted);
		return NULL;
	}

	return accepted;
}

void ezcfg_socket_close_sock(struct ezcfg_socket *sp)
{
	struct ezcfg *ezcfg;

	ASSERT(sp != NULL);

	ezcfg = sp->ezcfg;

	if (sp->sock >= 0) {
		close_socket_gracefully(sp->sock);
		sp->sock = -1;
	}
}

void ezcfg_socket_set_close_on_exec(struct ezcfg_socket *sp)
{
	ASSERT(sp != NULL);

	if (sp->sock >= 0) {
		fcntl(sp->sock, F_SETFD, FD_CLOEXEC);
	}
}

void ezcfg_socket_set_need_unlink(struct ezcfg_socket *sp, bool need_unlink)
{
	ASSERT(sp != NULL);

	sp->need_unlink = need_unlink;
}

int ezcfg_socket_set_remote(struct ezcfg_socket *sp, int domain, const char *socket_path)
{
	struct ezcfg *ezcfg;
	struct usa *usa;

	ASSERT(sp != NULL);
	ASSERT(socket_path != NULL);

	ezcfg = sp->ezcfg;
	usa = &(sp->rsa);

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
		err(ezcfg, "bad family [%d]\n", domain);
		return -EINVAL;
	}

	return 0;
}

int ezcfg_socket_connect_remote(struct ezcfg_socket *sp)
{
	int err = 0;
	struct usa *usa;
	struct ezcfg *ezcfg;

	ASSERT(sp != NULL);

	ezcfg = sp->ezcfg;
	usa = &(sp->rsa);

	switch (usa->domain) {
	case AF_LOCAL:
		err = connect(sp->sock, (struct sockaddr *)&usa->u.sun, usa->len);
		break;
	default:
		err(ezcfg, "bad family [%d]\n", usa->domain);
		return -EINVAL;
	}

	if (err < 0) {
		return err;
	}

	/* enable receiving of sender credentials */
	//setsockopt(sp->sock, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on));
	return 0;
}

int ezcfg_socket_read(struct ezcfg_socket *sp, void *buf, int len, int flags)
{
	struct ezcfg *ezcfg;
	char * p;
	int status, n;
	int sock;

	ASSERT(sp != NULL);
	ASSERT(buf != NULL);
	ASSERT(len >= 0);

	ezcfg = sp->ezcfg;
	p = buf;
	status = 0;
	sock = sp->sock;
	memset(buf, '\0', len);

	while (status == 0) {
		n = read(sock, p + status, len - status);

		if (n < 0) {
			if (errno == EPIPE) {
				info(ezcfg, "pipe error: %m\n");
				return -EPIPE;
			}
			else if (errno == EINTR || errno == EAGAIN) {
				info(ezcfg, "interrupted: %m\n");
				continue;
			}
			else {
				err(ezcfg, "write fail: %m\n");
				return -errno;
			}
		}

		if (n == 0) {
			info(ezcfg, "remote end closed connection: %m\n");
			p = buf;
			p[len-1] = '\0';
			break;
		}
		status += n;
	}
	return status;
}

int ezcfg_socket_write(struct ezcfg_socket *sp, const void *buf, int len, int flags)
{
	struct ezcfg *ezcfg;
	const char *p;
	int status, n;
	int sock;

	ASSERT(sp != NULL);
	ASSERT(buf != NULL);
	ASSERT(len >= 0);

	ezcfg = sp->ezcfg;
	p = buf;
	status = 0;
	sock = sp->sock;

	while (status != len) {
		n = write(sock, p + status, len - status);
		if (n < 0) {
			if (errno == EPIPE) {
				info(ezcfg, "remote end closed connection: %m\n");
				return -EPIPE;
			}
			else if (errno == EINTR || errno == EAGAIN) {
				info(ezcfg, "interrupted: %m\n");
				continue;
			}
			else {
				err(ezcfg, "write fail: %m\n");
				return -errno;
			}
		}
		status += n;
	}

	return status;
}