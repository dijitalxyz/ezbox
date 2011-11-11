/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/master.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-10-10   0.1       Split it from master.c
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
#include <sys/ipc.h>
#include <sys/sem.h>
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

#if 0
#define DBG(format, args...) do { \
	pid_t pid; \
	char path[256]; \
	FILE *fp; \
	pid = getpid(); \
	snprintf(path, 256, "/tmp/%d-debug.txt", pid); \
	fp = fopen(path, "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

/* don't remove ctrl, nvram and uevent socket */
void ezcfg_master_load_socket_conf(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;
	struct ezcfg_socket *sp;
	struct ezcfg_socket *ctrl_sp, *nvram_sp, *uevent_sp;
	struct ezcfg_socket **psp;
	char *p = NULL;
	int domain, type, proto;
	char address[256];
	int i;
	int socket_number = -1;

	if (master == NULL)
		return ;

	//ezcfg = master->ezcfg;
	ezcfg = ezcfg_master_get_ezcfg(master);

	/* ctrl socket and nvram socket */
	ctrl_sp = ezcfg_socket_new(ezcfg, AF_LOCAL, SOCK_STREAM, EZCFG_PROTO_IGRS, EZCFG_SOCK_CTRL_PATH);
	if (ctrl_sp == NULL) {
		err(ezcfg, "ezcfg_socket_new(ctrl_sp)\n");
		return ;
	}

	nvram_sp = ezcfg_socket_new(ezcfg, AF_LOCAL, SOCK_STREAM, EZCFG_PROTO_SOAP_HTTP, EZCFG_SOCK_NVRAM_PATH);
	if (nvram_sp == NULL) {
		err(ezcfg, "ezcfg_socket_new(nvram_sp)\n");
		return ;
	}

	uevent_sp = ezcfg_socket_new(ezcfg, AF_NETLINK, SOCK_DGRAM, EZCFG_PROTO_UEVENT, "kernel");
	if (uevent_sp == NULL) {
		err(ezcfg, "ezcfg_socket_new(uevent_sp)\n");
		return ;
	}

	/* tag listening_sockets to need_delete = true; */
	//sp = master->listening_sockets;
	sp = ezcfg_master_get_listening_sockets(master);
	while(sp != NULL) {
		if((ezcfg_socket_compare(sp, ctrl_sp) == false) &&
		   (ezcfg_socket_compare(sp, nvram_sp) == false) &&
		   (ezcfg_socket_compare(sp, uevent_sp) == false)) {
			ezcfg_socket_set_need_delete(sp, true);
		}
		sp = ezcfg_socket_get_next(sp);
	}

	/* delete unused sp */
	ezcfg_socket_delete(ctrl_sp);
	ezcfg_socket_delete(nvram_sp);
	ezcfg_socket_delete(uevent_sp);

	/* first get the socket number */
	p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_SOCKET_NUMBER);
	if (p != NULL) {
		socket_number = atoi(p);
		free(p);
	}
	for (i = 0; i < socket_number; i++) {

		/* initialize */
		domain = -1;
		type = -1;
		proto = EZCFG_PROTO_UNKNOWN;
		address[0] = '\0';
		sp = NULL;

		/* socket domain */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_SOCKET, i, EZCFG_EZCFG_KEYWORD_DOMAIN);
		if (p != NULL) {
			if (strcmp(p, EZCFG_SOCKET_DOMAIN_LOCAL_STRING) == 0) {
				domain = AF_LOCAL;
			}
			else if (strcmp(p, EZCFG_SOCKET_DOMAIN_INET_STRING) == 0) {
				domain = AF_INET;
			}
			else if (strcmp(p, EZCFG_SOCKET_DOMAIN_INET6_STRING) == 0) {
				domain = AF_INET6;
			}
			free(p);
		}

		/* socket type */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_SOCKET, i, EZCFG_EZCFG_KEYWORD_TYPE);
		if (p != NULL) {
			if (strcmp(p, EZCFG_SOCKET_TYPE_STREAM_STRING) == 0) {
				type = SOCK_STREAM;
			}
			else if (strcmp(p, EZCFG_SOCKET_TYPE_DGRAM_STRING) == 0) {
				type = SOCK_DGRAM;
			}
			else if (strcmp(p, EZCFG_SOCKET_TYPE_RAW_STRING) == 0) {
				type = SOCK_RAW;
			}
			free(p);
		}

		/* socket protocol */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_SOCKET, i, EZCFG_EZCFG_KEYWORD_PROTOCOL);
		if (p != NULL) {
			proto = atoi(p);
			free(p);
		}

		/* socket address */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_SOCKET, i, EZCFG_EZCFG_KEYWORD_ADDRESS);
		if (p != NULL) {
			snprintf(address, sizeof(address), "%s", p);
			free(p);
			p = NULL;
		}
		if ((domain < 0) ||
		    (type < 0) ||
		    (proto == EZCFG_PROTO_UNKNOWN) ||
		    (address[0] == '\0')) {
			err(ezcfg, "socket setting error\n");
			continue;
		}

		sp = ezcfg_socket_new(ezcfg, domain, type, proto, address);
		if (sp == NULL) {
			err(ezcfg, "init socket fail: %m\n");
			continue;
		}

	    	//if (ezcfg_socket_list_in(&(master->listening_sockets), sp) == true) {
	    	psp = ezcfg_master_get_p_listening_sockets(master);
	    	if (ezcfg_socket_list_in(psp, sp) == true) {
			info(ezcfg, "socket already up\n");
			/* don't delete this socket in listening_sockets */
			//ezcfg_socket_list_set_need_delete(&(master->listening_sockets), sp, false);
			ezcfg_socket_list_set_need_delete(psp, sp, false);
			ezcfg_socket_delete(sp);
			continue;
		}

		if ((domain == AF_LOCAL) &&
		    (address[0] != '@')) {
			ezcfg_socket_set_need_unlink(sp, true);
		}

		//if (ezcfg_socket_list_insert(&(master->listening_sockets), sp) < 0) {
	    	psp = ezcfg_master_get_p_listening_sockets(master);
		if (ezcfg_socket_list_insert(psp, sp) < 0) {
			err(ezcfg, "insert listener socket fail: %m\n");
			ezcfg_socket_delete(sp);
			continue;
		}

		if (ezcfg_socket_enable_receiving(sp) < 0) {
			err(ezcfg, "enable socket [%s] receiving fail: %m\n", address);
			//ezcfg_socket_list_delete_socket(&(master->listening_sockets), sp);
	    		psp = ezcfg_master_get_p_listening_sockets(master);
			ezcfg_socket_list_delete_socket(psp, sp);
			continue;
		}

		//if (ezcfg_socket_enable_listening(sp, master->sq_len) < 0) {
		if (ezcfg_socket_enable_listening(sp, ezcfg_master_get_sq_len(master)) < 0) {
			err(ezcfg, "enable socket [%s] listening fail: %m\n", address);
			//ezcfg_socket_list_delete_socket(&(master->listening_sockets), sp);
	    		psp = ezcfg_master_get_p_listening_sockets(master);
			ezcfg_socket_list_delete_socket(psp, sp);
			continue;
		}

		ezcfg_socket_set_close_on_exec(sp);
	}

	/* delete all sockets taged need_delete = true in need_listening_sockets */
	//sp = master->listening_sockets;
	sp = ezcfg_master_get_listening_sockets(master);
	while(sp != NULL) {
		if(ezcfg_socket_get_need_delete(sp) == true) {
			//ezcfg_socket_list_delete_socket(&(master->listening_sockets), sp);
	    		psp = ezcfg_master_get_p_listening_sockets(master);
			ezcfg_socket_list_delete_socket(psp, sp);
			//sp = master->listening_sockets;
			sp = ezcfg_master_get_listening_sockets(master);
		}
		else {
			sp = ezcfg_socket_get_next(sp);
		}
	}
}
