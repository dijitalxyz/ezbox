/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/master_upnp_ssdp.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-01   0.1       Split it from master.c
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

#if (HAVE_EZBOX_SERVICE_EZCFG_IGRSD == 1)
#include "ezcfg-igrs.h"
#endif

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

bool ezcfg_master_handle_upnp_ssdp_socket(
	struct ezcfg_master *master,
	struct ezcfg_socket *listener,
	struct ezcfg_socket *accepted)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	char buf[EZCFG_UPNP_SSDP_MAX_MESSAGE_SIZE];
	int len;
	bool ret = false;

	ASSERT(master != NULL);
	ASSERT(listener != NULL);
	ASSERT(accepted != NULL);

	ezcfg = ezcfg_master_get_ezcfg(master);

	http = ezcfg_http_new(ezcfg);
	if (http == NULL) {
		err(ezcfg, "not enough memory to handle SSDP.\n");
		return false;
	}

	len = ezcfg_socket_read(listener, buf, sizeof(buf), 0);
	if (len < 1) {
		err(ezcfg, "read SSDP socket error.\n");
		goto func_out;
	}

	buf[len - 1] = '\0';

	if (ezcfg_http_parse_header(http, buf, len) == false) {
		err(ezcfg, "SSDP packet format error.\n");
                goto func_out;
        }

#if (HAVE_EZBOX_SERVICE_EZCFG_IGRSD == 1)
	/* check legacy IGRS ssdp package */
	if (ezcfg_http_get_header_value(http, EZCFG_IGRS_HTTP_HEADER_01_IGRS_VERSION) != NULL) {
		/* it's a legacy IGRS ssdp package */
		ezcfg_socket_set_proto(accepted, EZCFG_PROTO_IGRS_ISDP);
	}
#endif

	if (ezcfg_socket_set_buffer(accepted, buf, len) == false) {
		err(ezcfg, "set SSDP socket buffer error.\n");
		goto func_out;
	}

	ret = true;

func_out:
	if (http != NULL)
		ezcfg_http_delete(http);

	return ret;
}
