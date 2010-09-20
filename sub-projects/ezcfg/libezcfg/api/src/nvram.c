/* ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : nvram.c
 *
 * Description  : ezcfg API for nvram manipulate
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-09-17   0.1       Write it from scratch
 * ============================================================================
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "libezcfg.h"
#include "libezcfg-private.h"

#include "ezcfg-api.h"

/* Check whether full response is buffered. Return:
 *   -1  if response is malformed
 *    0  if response is not yet fully buffered
 *   >0  actual response length, including last \r\n\r\n
 */
static int get_response_len(const char *buf, size_t buflen)
{
	const char *s, *e;
	int len = 0;

	for (s = buf, e = s + buflen - 1; len <= 0 && s < e; s++) {
		/* Control characters are not allowed but >=128 is. */
		if (!isprint(* (unsigned char *) s) && *s != '\r' &&
		    *s != '\n' && * (unsigned char *) s < 128) {
			len = -1;
		} else if (s[0] == '\n' && s[1] == '\n') {
			len = (int) (s - buf) + 2;
		} else if (s[0] == '\n' && &s[1] < e &&
		           s[1] == '\r' && s[2] == '\n') {
			len = (int) (s - buf) + 3;
		}
	}

	return len;
}


/**
 * Keep reading the input into buffer buf, until \r\n\r\n appears in the
 * buffer (which marks the end of HTTP request). Buffer buf may already
 * have some data. The length of the data is stored in nread.
 * Upon every read operation, increase nread by the number of bytes read.
 **/
static int read_response(struct ezcfg_ctrl *ezctrl, char *buf, int bufsiz, int *nread)
{
	int n, response_len;

	ASSERT(ezctrl != NULL);

        response_len = 0;

	while (*nread < bufsiz && response_len == 0) {
		n = ezcfg_socket_read(ezcfg_ctrl_get_socket(ezctrl), buf + *nread, bufsiz - *nread, 0);
		if (n <= 0) {
			break;
		} else {
			*nread += n;
			response_len = get_response_len(buf, (size_t) *nread);
                }
        }

        return response_len;
}



/**
 * ezcfg_api_nvram_get:
 * @name: nvram name
 * @value: buffer to store nvram value
 * @len: buffer size
 *
 **/
int ezcfg_api_nvram_get(const char *name, char *value, size_t len)
{
	char buf[1024];
	char msg[EZCFG_SOAP_HTTP_MAX_REQUEST_SIZE];
	int msg_len;
	struct ezcfg *ezcfg = NULL;
	struct ezcfg_ctrl *ezctrl = NULL;
	struct ezcfg_soap_http *sh = NULL;
	struct ezcfg_soap *soap = NULL;
	struct ezcfg_http *http = NULL;
	//int body_index, child_index;
	int n;
	int rc = 0;

	if (name == NULL || value == NULL || len < 1) {
		return -EZCFG_E_ARGUMENT ;
	}

	ezcfg = ezcfg_new();
	if (ezcfg == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	sh = ezcfg_soap_http_new(ezcfg);
	if (sh == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	soap = ezcfg_soap_http_get_soap(sh);
	http = ezcfg_soap_http_get_http(sh);

	/* build HTTP request line */
	ezcfg_http_set_request_method(http, EZCFG_SOAP_HTTP_METHOD_GET);
	snprintf(buf, sizeof(buf), "%s?name=%s", EZCFG_SOAP_HTTP_NVRAM_GET_URI, name);
	ezcfg_http_set_request_uri(http, buf);
	ezcfg_http_set_version_major(http, 1);
	ezcfg_http_set_version_minor(http, 1);

	/* build HTTP headers */
	snprintf(buf, sizeof(buf), "%s", "127.0.0.1");
	ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_HOST, buf);
	snprintf(buf, sizeof(buf), "%s", "application/soap+xml");
	ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_ACCEPT, buf);

	memset(msg, 0, sizeof(msg));
	msg_len = ezcfg_soap_http_write_message(sh, msg, sizeof(msg), EZCFG_SOAP_HTTP_MODE_REQUEST);

	snprintf(buf, sizeof(buf), "%s-%d", EZCFG_NVRAM_SOCK_PATH, getpid());
	ezctrl = ezcfg_ctrl_new_from_socket(ezcfg, AF_LOCAL, EZCFG_PROTO_SOAP_HTTP, buf, EZCFG_NVRAM_SOCK_PATH);

	if (ezctrl == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	if (ezcfg_ctrl_connect(ezctrl) < 0) {
		rc = -EZCFG_E_CONNECTION ;
		goto exit;
	}

	if (ezcfg_ctrl_write(ezctrl, msg, msg_len, 0) < 0) {
		rc = -EZCFG_E_WRITE ;
		goto exit;
	}

	memset(msg, 0, sizeof(msg));
	n = 0;
	msg_len = read_response(ezctrl, msg, sizeof(msg), &n);

	if (msg_len <= 0) {
		rc = -EZCFG_E_READ ;
		goto exit;
	}

	if (n > msg_len) {
		ezcfg_soap_http_set_http_message_body(sh, msg + msg_len, n - msg_len);
	}

	if (ezcfg_soap_http_parse_response(sh, msg, msg_len) == false) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	//handle_soap_http_request(sh);
#if 0
	if (ezcfg_ctrl_read(ezctrl, msg, sizeof(msg), 0) < 0) {
		rc = -EZCFG_E_READ ;
		goto exit;
	}
#endif

exit:
        if (sh != NULL)
                ezcfg_soap_http_delete(sh);

        if (ezctrl != NULL)
                ezcfg_ctrl_delete(ezctrl);

        if (ezcfg != NULL)
                ezcfg_delete(ezcfg);

	return rc;
}

