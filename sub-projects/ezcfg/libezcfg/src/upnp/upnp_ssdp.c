/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : upnp/upnp_ssdp.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-09   0.1       Write it from scratch
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
#include "ezcfg-upnp.h"

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

struct ezcfg_upnp_ssdp {
	struct ezcfg *ezcfg;
	struct ezcfg_socket *socket;
	struct ezcfg_http *http;
	struct ezcfg_upnp *upnp;
};

/* for HTTP/1.1 request methods */
static const char *upnp_ssdp_method_strings[] = {
	/* bad method string */
	NULL ,
	/* UPnP SSDP used methods */
	EZCFG_UPNP_HTTP_METHOD_NOTIFY ,
	EZCFG_UPNP_HTTP_METHOD_MSEARCH ,
};

/* for HTTP/1.1 known header */
static const char *upnp_ssdp_header_strings[] = {
	/* bad header string */
	NULL ,
	/* UPnP SSDP known HTTP headers */
	EZCFG_UPNP_HTTP_HEADER_HOST ,
	EZCFG_UPNP_HTTP_HEADER_CACHE_CONTROL ,
	EZCFG_UPNP_HTTP_HEADER_LOCATION ,
	EZCFG_UPNP_HTTP_HEADER_NT ,
	EZCFG_UPNP_HTTP_HEADER_NTS ,
	EZCFG_UPNP_HTTP_HEADER_SERVER ,
	EZCFG_UPNP_HTTP_HEADER_USN ,
};

/**
 * private functions
 **/

/**
 * Public functions
 **/
void ezcfg_upnp_ssdp_delete(struct ezcfg_upnp_ssdp *ssdp)
{
	struct ezcfg *ezcfg;

	ASSERT(ssdp != NULL);

	ezcfg = ssdp->ezcfg;

	if (ssdp->http != NULL) {
		ezcfg_http_delete(ssdp->http);
	}

	free(ssdp);
}

/**
 * ezcfg_upnp_ssdp_new:
 * Create ezcfg ssdp protocol data structure
 * Returns: a new ezcfg ssdp protocol data structure
 **/
struct ezcfg_upnp_ssdp *ezcfg_upnp_ssdp_new(struct ezcfg *ezcfg)
{
	struct ezcfg_upnp_ssdp *ssdp;

	ASSERT(ezcfg != NULL);

	/* initialize ssdp protocol data structure */
	ssdp = calloc(1, sizeof(struct ezcfg_upnp_ssdp));
	if (ssdp == NULL) {
		return NULL;
	}

	memset(ssdp, 0, sizeof(struct ezcfg_upnp_ssdp));

	ssdp->http = ezcfg_http_new(ezcfg);
	if (ssdp->http == NULL) {
		goto fail_exit;
	}

	ssdp->ezcfg = ezcfg;
	ezcfg_http_set_method_strings(ssdp->http, upnp_ssdp_method_strings, ARRAY_SIZE(upnp_ssdp_method_strings) - 1);
	ezcfg_http_set_known_header_strings(ssdp->http, upnp_ssdp_header_strings, ARRAY_SIZE(upnp_ssdp_header_strings) - 1);

	return ssdp;

fail_exit:
	ezcfg_upnp_ssdp_delete(ssdp);
	return NULL;
}

/**
 * ezcfg_upnp_ssdp_set_upnp:
 **/
bool ezcfg_upnp_ssdp_set_upnp(struct ezcfg_upnp_ssdp *ssdp, struct ezcfg_upnp *upnp)
{
	ASSERT(ssdp != NULL);
	ASSERT(upnp != NULL);

	ssdp->upnp = upnp;

	return true;
}

/**
 * ezcfg_upnp_ssdp_advertise_alive:
 **/
bool ezcfg_upnp_ssdp_advertise_alive(struct ezcfg_upnp_ssdp *ssdp)
{
	struct ezcfg *ezcfg;
	struct ezcfg_upnp *upnp;
	struct ezcfg_http *http;
	struct ezcfg_socket *socket = NULL;
	int domain, type, proto;
	char socket_path[128];
	char buf[256];
	upnp_if_t *ifp;
	char ip[INET_ADDRSTRLEN];
	char *msg;
	int msg_len;

	ASSERT(ssdp != NULL);

	ezcfg = ssdp->ezcfg;
	upnp = ssdp->upnp;
	http = ssdp->http;

	domain = ezcfg_util_socket_domain_get_index(EZCFG_SOCKET_DOMAIN_INET_STRING);
	type = ezcfg_util_socket_type_get_index(EZCFG_SOCKET_TYPE_DGRAM_STRING);
	proto = ezcfg_util_socket_protocol_get_index(EZCFG_SOCKET_PROTO_UPNP_SSDP_STRING);

	ifp = upnp->ifs;
	while(ifp != NULL) {
		if (ezcfg_util_if_get_ipaddr(ifp->ifname, ip) == true) {
			snprintf(socket_path, sizeof(socket_path), "%s:%s@%s",
				EZCFG_PROTO_UPNP_SSDP_MCAST_IPADDR_STRING,
				EZCFG_PROTO_UPNP_SSDP_PORT_NUMBER_STRING, ip);

			socket = ezcfg_socket_new(ezcfg, domain, type, proto, socket_path);
			if (socket == NULL) {
				return false;
			}
			ezcfg_socket_enable_sending(socket);

			/* build HTTP request line */
			ezcfg_http_set_request_method(http, EZCFG_UPNP_HTTP_METHOD_NOTIFY);
			ezcfg_http_set_request_uri(http, "*");
			ezcfg_http_set_version_major(http, 1);
			ezcfg_http_set_version_minor(http, 1);
			ezcfg_http_set_state_request(http);

			/* Host: 239.255.255.250:1900 */
			snprintf(buf, sizeof(buf), "%s:%s",
			         EZCFG_PROTO_UPNP_SSDP_MCAST_IPADDR_STRING,
			         EZCFG_PROTO_UPNP_SSDP_PORT_NUMBER_STRING);
			if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_HOST, buf) == false) {
				err(ezcfg, "HTTP add header error.\n");
				return false;
			}

			/* Cache-Control: max-age=1800 */
			snprintf(buf, sizeof(buf), "max-age=%d", 1800);
			if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_CACHE_CONTROL, buf) == false) {
				err(ezcfg, "HTTP add header error.\n");
				return false;
			}

			/* Location: http://192.168.1.1:61900/igd1/InternetGatewayDevice1.xml */
			snprintf(buf, sizeof(buf), "http://%s:%s%s",
			         ip, EZCFG_PROTO_UPNP_GENA_PORT_NUMBER_STRING,
			         ezcfg_util_upnp_get_type_description_path(upnp->type));
			if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_LOCATION, buf) == false) {
				err(ezcfg, "HTTP add header error.\n");
				return false;
			}

			/* NT: Notification Type, for root device */
			snprintf(buf, sizeof(buf), "%s", "upnp:rootdevice");
			if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_NT, buf) == false) {
				err(ezcfg, "HTTP add header error.\n");
				return false;
			}

			/* NTS: Notification Sub Type, must be "ssdp:alive" */
			snprintf(buf, sizeof(buf), "%s", "ssdp:alive");
			if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_NTS, buf) == false) {
				err(ezcfg, "HTTP add header error.\n");
				return false;
			}

			/* Server: OS/version UPnP/1.0 product/version */
			snprintf(buf, sizeof(buf), "Linux/2.6 UPnP/%d.%d ezbox/1.0",
			         upnp->version_major, upnp->version_minor);
			if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_SERVER, buf) == false) {
				err(ezcfg, "HTTP add header error.\n");
				return false;
			}

			/* USN: Unique Service Name */
			snprintf(buf, sizeof(buf), "%s::%s", upnp->u.dev.UDN, "upnp:rootdevice");
			if (ezcfg_http_add_header(http, EZCFG_UPNP_HTTP_HEADER_NTS, buf) == false) {
				err(ezcfg, "HTTP add header error.\n");
				return false;
			}

			msg_len = ezcfg_http_get_message_length(http);
			if (msg_len < 0) {
				err(ezcfg, "HTTP message length error.\n");
				return false;
			}
			msg_len++; /* one more for '\0' */

			if (msg_len <= sizeof(buf)) {
				msg = buf;
			}
			else {
				msg = malloc(msg_len);
				if (msg == NULL) {
					err(ezcfg, "HTTP malloc msg buffer error.\n");
					return false;
				}
			}

			memset(msg, 0, msg_len);
			msg_len = ezcfg_http_write_message(http, msg, msg_len);

			ezcfg_socket_write(socket, msg, msg_len, 0);

			if (msg != buf) {
				free(msg);
			}

			ezcfg_socket_delete(socket);
			socket = NULL;
		}
		ifp = ifp->next;
	}

	return true;
}
