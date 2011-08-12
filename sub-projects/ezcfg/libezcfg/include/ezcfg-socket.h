/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-socket.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-12   0.1       Write it from scratch
 * 2011-08-09             Add linux netlink socket
 * ============================================================================
 */

#ifndef _EZCFG_SOCKET_H_
#define _EZCFG_SOCKET_H_

#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <linux/netlink.h>

#include "ezcfg.h"

/*
 * unified socket address. For IPv6 support, add IPv6 address structure
 * in the union u.
 */
struct usa {
	socklen_t len;
	int domain;
	int type;
	union {
		struct sockaddr sa;
		struct sockaddr_un sun;
		struct sockaddr_in sin;
		struct sockaddr_nl snl;
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
	int		proto;		/* Communication protocol 	*/
	int		backlog;	/* Listening queue length 	*/
	struct usa	lsa;		/* Local socket address         */
	struct usa	rsa;		/* Remote socket address        */
	bool		need_unlink;	/* Need to unlink socket node 	*/
	bool		need_delete;	/* Need to delete socket node 	*/
	char *		buffer;
	int		buffer_len;
};

#endif
