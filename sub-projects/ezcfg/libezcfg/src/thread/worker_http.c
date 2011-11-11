/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/worker.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-12   0.1       Split it from worker.c
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
#include "ezcfg-soap_http.h"

#if 1
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

#define handle_error_en(en, msg) \
	do { errno = en; perror(msg); } while (0)

static bool http_error_handler(struct ezcfg_worker *worker)
{
	return false;
}

static void send_http_error(struct ezcfg_worker *worker, int status,
                            const char *reason, const char *fmt, ...)
{
	struct ezcfg_http *http;
	char buf[EZCFG_BUFFER_SIZE];
	va_list ap;
	int len;
	bool handled;

	http = (struct ezcfg_http *)ezcfg_worker_get_proto_data(worker);
	ezcfg_http_set_status_code(http, status);
	handled = http_error_handler(worker);

	if (handled == false) {
		buf[0] = '\0';
		len = 0;

		/* Errors 1xx, 204 and 304 MUST NOT send a body */
		if (status > 199 && status != 204 && status != 304) {
			len = snprintf(buf, sizeof(buf),
			               "Error %d: %s\n", status, reason);
			va_start(ap, fmt);
			len += vsnprintf(buf + len, sizeof(buf) - len, fmt, ap);
			va_end(ap);
			ezcfg_worker_set_num_bytes_sent(worker, len);
		}
		ezcfg_worker_printf(worker,
		              "HTTP/1.1 %d %s\r\n"
		              "Content-Type: text/plain\r\n"
		              "Content-Length: %d\r\n"
		              "Connection: close\r\n"
		              "\r\n%s", status, reason, len, buf);
	}
}

static bool is_http_html_admin_request(const char *uri)
{
	if (strncmp(uri, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI, strlen(EZCFG_HTTP_HTML_ADMIN_PREFIX_URI)) == 0) {
		return true;
	}
	else {
		return false;
	}
}

static void handle_http_request(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_master *master;
	struct ezcfg_nvram *nvram;
	char *request_uri;
	char *msg = NULL;
	int msg_len;

	ASSERT(worker != NULL);

	http = (struct ezcfg_http *)ezcfg_worker_get_proto_data(worker);
	ASSERT(http != NULL);

	ezcfg = ezcfg_worker_get_ezcfg(worker);
	master = ezcfg_worker_get_master(worker);
	nvram = ezcfg_master_get_nvram(master);

	request_uri = ezcfg_http_get_request_uri(http);
	if (request_uri == NULL) {
		err(ezcfg, "no request uri for HTTP GET method.\n");

		/* clean http structure info */
		ezcfg_http_reset_attributes(http);
		ezcfg_http_set_status_code(http, 400);
		ezcfg_http_set_state_response(http);

		/* build HTTP error response */
		msg_len = ezcfg_http_get_message_length(http);
		if (msg_len < 0) {
			err(ezcfg, "ezcfg_http_get_message_length error.\n");
			goto func_exit;
		}
		msg_len++; /* one more for '\0' */
		msg = (char *)malloc(msg_len);
		if (msg == NULL) {
			err(ezcfg, "malloc msg error.\n");
			goto func_exit;
		}
		memset(msg, 0, msg_len);
		msg_len = ezcfg_http_write_message(http, msg, msg_len);
		ezcfg_worker_write(worker, msg, msg_len);
		goto func_exit;
	}

	if (is_http_html_admin_request(request_uri) == true) {
		struct ezcfg_auth *auth, *auths;
		bool ret;
		char buf[1024];
		char *p;

		auth = ezcfg_auth_new(ezcfg);
		if (auth == NULL) {
			err(ezcfg, "ezcfg_auth_new error.\n");
			goto func_exit;
		}

		if (ezcfg_http_parse_auth(http, auth) == false) {
			err(ezcfg, "ezcfg_http_parse_auth error.\n");
			goto need_authenticate;
		}

		ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_REALM), &p);
		if (p == NULL) {
			err(ezcfg, "ezcfg_nvram_get_entry_value error.\n");
			goto need_authenticate;
		}
		ret = ezcfg_auth_set_realm(auth, p);
		free(p);
		if (ret == false) {
			err(ezcfg, "ezcfg_auth_set_realm error.\n");
			goto need_authenticate;
		}

		ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_DOMAIN), &p);
		if (p == NULL) {
			err(ezcfg, "ezcfg_nvram_get_entry_value error.\n");
			goto need_authenticate;
		}
		ret = ezcfg_auth_set_domain(auth, p);
		free(p);
		if (ret == false) {
			err(ezcfg, "ezcfg_auth_set_domain error.\n");
			goto need_authenticate;
		}

		/* lock auths */
		ezcfg_master_auth_mutex_lock(master);
		auths = ezcfg_master_get_auths(master);
		ret = ezcfg_auth_check_authorized(&auths, auth);
		/* unlock auths */
		ezcfg_master_auth_mutex_unlock(master);

		if (ret == false) {
need_authenticate:
			/* clean http structure info */
			ezcfg_http_reset_attributes(http);
			ezcfg_http_set_status_code(http, 401);
			ezcfg_http_set_state_response(http);

			/* http WWW-Authenticate */
			ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_REALM), &p);
			if (p == NULL) {
				err(ezcfg, "ezcfg_nvram_get_entry_value error.\n");
				goto func_exit;
			}
			snprintf(buf, sizeof(buf), "Basic realm=\"%s\"", p);
			free(p);
			ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_WWW_AUTHENTICATE, buf);

			/* build HTTP Unauthorized response */
			msg_len = ezcfg_http_get_message_length(http);
			if (msg_len < 0) {
				err(ezcfg, "ezcfg_http_get_message_length error.\n");
				goto func_exit;
			}
			msg_len++; /* one more for '\0' */
			msg = (char *)malloc(msg_len);
			if (msg == NULL) {
				err(ezcfg, "malloc msg error.\n");
				goto func_exit;
			}
			memset(msg, 0, msg_len);
			msg_len = ezcfg_http_write_message(http, msg, msg_len);
			ezcfg_worker_write(worker, msg, msg_len);
			goto func_exit;
		}

		if (ezcfg_http_handle_admin_request(http, nvram) < 0) {
			/* clean http structure info */
			ezcfg_http_reset_attributes(http);
			ezcfg_http_set_status_code(http, 400);
			ezcfg_http_set_state_response(http);

			/* build HTTP error response */
			msg_len = ezcfg_http_get_message_length(http);
			if (msg_len < 0) {
				err(ezcfg, "ezcfg_http_get_message_length error.\n");
				goto func_exit;
			}
			msg_len++; /* one more for '\0' */
			msg = (char *)malloc(msg_len);
			if (msg == NULL) {
				err(ezcfg, "malloc msg error.\n");
				goto func_exit;
			}
			memset(msg, 0, msg_len);
			msg_len = ezcfg_http_write_message(http, msg, msg_len);
			ezcfg_worker_write(worker, msg, msg_len);
			goto func_exit;
		}
		else {
			/* build HTTP response */
			msg_len = ezcfg_http_get_message_length(http);
			if (msg_len < 0) {
				err(ezcfg, "ezcfg_http_get_message_length error.\n");
				goto func_exit;
			}
			msg_len++; /* one more for '\0' */
			msg = (char *)malloc(msg_len);
			if (msg == NULL) {
				err(ezcfg, "malloc msg error.\n");
				goto func_exit;
			}
			memset(msg, 0, msg_len);
			msg_len = ezcfg_http_write_message(http, msg, msg_len);
			ezcfg_worker_write(worker, msg, msg_len);
			goto func_exit;
		}
	}
	else {
		/* will always return index page if not find the uri */
		if (ezcfg_http_handle_index_request(http, nvram) < 0) {
			/* clean http structure info */
			ezcfg_http_reset_attributes(http);
			ezcfg_http_set_status_code(http, 400);
			ezcfg_http_set_state_response(http);

			/* build HTTP error response */
			msg_len = ezcfg_http_get_message_length(http);
			if (msg_len < 0) {
				err(ezcfg, "ezcfg_http_get_message_length error.\n");
				goto func_exit;
			}
			msg_len++; /* one more for '\0' */
			msg = (char *)malloc(msg_len);
			if (msg == NULL) {
				err(ezcfg, "malloc msg error.\n");
				goto func_exit;
			}
			memset(msg, 0, msg_len);
			msg_len = ezcfg_http_write_message(http, msg, msg_len);
			ezcfg_worker_write(worker, msg, msg_len);
			goto func_exit;
		}
		else {
			/* build HTTP response */
			msg_len = ezcfg_http_get_message_length(http);
			if (msg_len < 0) {
				err(ezcfg, "ezcfg_http_get_message_length error.\n");
				goto func_exit;
			}
			msg_len++; /* one more for '\0' */
			msg = (char *)malloc(msg_len);
			if (msg == NULL) {
				err(ezcfg, "malloc msg error.\n");
				goto func_exit;
			}
			memset(msg, 0, msg_len);
			msg_len = ezcfg_http_write_message(http, msg, msg_len);
			ezcfg_worker_write(worker, msg, msg_len);
			goto func_exit;
		}
	}
func_exit:
	if (msg != NULL)
		free(msg);
}

void ezcfg_worker_process_http_new_connection(struct ezcfg_worker *worker)
{
	int header_len, nread;
	char *buf;
	int buf_len;
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;

	ASSERT(worker != NULL);

	http = (struct ezcfg_http *)ezcfg_worker_get_proto_data(worker);
	ASSERT(http != NULL);

	ezcfg = ezcfg_worker_get_ezcfg(worker);
	buf_len = EZCFG_HTTP_CHUNK_SIZE;

	buf = calloc(buf_len+1, sizeof(char)); /* +1 for \0 */
	if (buf == NULL) {
		err(ezcfg, "not enough memory for processing http new connection\n");
		return;
	}
	nread = 0;
	header_len = ezcfg_socket_read_http_header(ezcfg_worker_get_client(worker), http, buf, buf_len, &nread);

	ASSERT(nread >= header_len);

	if (header_len <= 0) {
		err(ezcfg, "request error\n");
		free(buf);
		return; /* Request is too large or format is not correct */
	}

	/* 0-terminate the request: parse http request uses sscanf
	 * !!! never, be careful not mangle the "\r\n\r\n" string!!!
	 */
	//buf[header_len - 1] = '\0';
	ezcfg_http_set_state_request(http);
	if (ezcfg_http_parse_header(http, buf, header_len) == true) {
		unsigned short major, minor;
		char *p;
		time_t t;
		major = ezcfg_http_get_version_major(http);
		minor = ezcfg_http_get_version_minor(http);
		if (major != 1 || minor != 1) {
			send_http_error(worker, 505,
			                "HTTP version not supported",
			                "%s", "Weird HTTP version");
			goto exit;
		}
		p = ezcfg_socket_read_http_content(ezcfg_worker_get_client(worker), http, buf, header_len, &buf_len, &nread);
		if (p == NULL) {
			send_http_error(worker, 400, "Bad Request", "");
			goto exit;
		}
		buf = p;
		if (nread > header_len) {
			ezcfg_http_set_message_body(http, buf + header_len, nread - header_len);
		}
		if (time(&t) > 0) {
			ezcfg_worker_set_birth_time(worker, t);
		}
		handle_http_request(worker);
	} else {
		/* Do not put garbage in the access log */
		send_http_error(worker, 400, "Bad Request", "Can not parse request: %.*s", nread, buf);
	}

exit:
	/* release buf memory */
	free(buf);
}
