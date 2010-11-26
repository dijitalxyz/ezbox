/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/worker.c
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
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

/*
 * ezcfg_worker:
 * Opaque object handling one event source.
 * Multi-threads model - worker part.
 */
struct ezcfg_worker {
	struct ezcfg *ezcfg;
	struct ezcfg_master *master;
	struct ezcfg_socket *client;
	unsigned char proto;
	void *proto_data;
	time_t birth_time;
	int64_t num_bytes_sent;
};

static void reset_connection_attributes(struct ezcfg_worker *worker) {
	struct ezcfg *ezcfg;

	ASSERT(worker != NULL);

	ezcfg = worker->ezcfg;

	switch(worker->proto) {
	case EZCFG_PROTO_HTTP :
		ezcfg_http_reset_attributes(worker->proto_data);
		break;
	case EZCFG_PROTO_SOAP_HTTP :
		ezcfg_soap_http_reset_attributes(worker->proto_data);
		break;
	case EZCFG_PROTO_IGRS :
		ezcfg_igrs_reset_attributes(worker->proto_data);
		break;
	default :
		err(ezcfg, "unknown protocol\n");
	}

	worker->num_bytes_sent = 0;
}

static void close_connection(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;

	ASSERT(worker != NULL);

	ezcfg = worker->ezcfg;

	ezcfg_socket_close_sock(worker->client);
}

/* Return content length of the request, or -1 constant if
 * Content-Length header is not set.
 */
static int get_content_length(const struct ezcfg_worker *worker) {
	const char *cl = NULL;
	switch(worker->proto) {
	case EZCFG_PROTO_HTTP :
		cl = ezcfg_http_get_header_value(worker->proto_data, EZCFG_HTTP_HEADER_CONTENT_LENGTH);
		break;
	case EZCFG_PROTO_SOAP_HTTP :
		cl = ezcfg_soap_http_get_http_header_value(worker->proto_data, EZCFG_SOAP_HTTP_HEADER_CONTENT_LENGTH);
		break;
	case EZCFG_PROTO_IGRS :
		cl = ezcfg_igrs_get_http_header_value(worker->proto_data, EZCFG_IGRS_HEADER_CONTENT_LENGTH);
		break;
	default :
		break;
	}
	return cl == NULL ? -1 : strtol(cl, NULL, 10);
}


/* Check whether full request is buffered. Return:
 *   -1  if request is malformed
 *    0  if request is not yet fully buffered
 *   >0  actual request length, including last \r\n\r\n
 */
static int get_request_len(const char *buf, size_t buflen)
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
static int read_request(struct ezcfg_worker *worker, char *buf, int bufsiz, int *nread)
{
	struct ezcfg *ezcfg;
	int n, request_len;

	ASSERT(worker != NULL);

	ezcfg = worker->ezcfg;

	request_len = 0;

	while (*nread < bufsiz && request_len == 0) {
		n = ezcfg_socket_read(worker->client, buf + *nread, bufsiz - *nread, 0);
		if (n <= 0) {
			break;
		} else {
			*nread += n;
			request_len = get_request_len(buf, (size_t) *nread);
		}
	}

	return request_len;
}

static bool error_handler(struct ezcfg_worker *worker)
{
	return false;
}

static int worker_printf(struct ezcfg_worker *worker, const char *fmt, ...)
{
	char buf[EZCFG_MAX_REQUEST_SIZE];
	int len;
	va_list ap;

	va_start(ap, fmt);
	len = vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	return ezcfg_socket_write(worker->client, buf, len, 0);
}

static int worker_write(struct ezcfg_worker *worker, const char *buf, int len)
{
	return ezcfg_socket_write(worker->client, buf, len, 0);
}

static void send_http_error(struct ezcfg_worker *worker, int status,
                            const char *reason, const char *fmt, ...)
{
	char buf[EZCFG_BUFFER_SIZE];
	va_list ap;
	int len;
	bool handled;

	ezcfg_http_set_status_code(worker->proto_data, status);
	handled = error_handler(worker);

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
			worker->num_bytes_sent = len;
		}
		worker_printf(worker,
		              "HTTP/1.1 %d %s\r\n"
		              "Content-Type: text/plain\r\n"
		              "Content-Length: %d\r\n"
		              "Connection: close\r\n"
		              "\r\n%s", status, reason, len, buf);
	}
}

static void send_soap_http_error(struct ezcfg_worker *worker, int status,
                            const char *reason, const char *fmt, ...)
{
	char buf[EZCFG_BUFFER_SIZE];
	va_list ap;
	int len;
	bool handled;

	//ezcfg_http_set_status_code(worker->proto_data, status);
	handled = error_handler(worker);

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
			worker->num_bytes_sent = len;
		}
		worker_printf(worker,
		              "HTTP/1.1 %d %s\r\n"
		              "Content-Type: text/plain\r\n"
		              "Content-Length: %d\r\n"
		              "Connection: close\r\n"
		              "\r\n%s", status, reason, len, buf);
	}
}

static void send_igrs_error(struct ezcfg_worker *worker, int status,
                            const char *reason, const char *fmt, ...)
{
	char buf[EZCFG_BUFFER_SIZE];
	va_list ap;
	int len;
	bool handled;

	//ezcfg_http_set_status_code(worker->proto_data, status);
	handled = error_handler(worker);

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
			worker->num_bytes_sent = len;
		}
		worker_printf(worker,
		              "HTTP/1.1 %d %s\r\n"
		              "Content-Type: text/plain\r\n"
		              "Content-Length: %d\r\n"
		              "Connection: close\r\n"
		              "\r\n%s", status, reason, len, buf);
	}
}

static void handle_http_request(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;

	ASSERT(worker != NULL);

	ezcfg = worker->ezcfg;

	worker_printf(worker,
	             "HTTP/1.1 %d %s\r\n"
	             "\r\n", 200, "OK");
}

static bool is_soap_http_nvram_request(const char *uri)
{
	if (strncmp(uri, EZCFG_SOAP_HTTP_NVRAM_GET_URI"?name=", strlen(EZCFG_SOAP_HTTP_NVRAM_GET_URI) + 6) == 0) {
		return true;
	}
	else if (strcmp(uri, EZCFG_SOAP_HTTP_NVRAM_SET_URI) == 0) {
		return true;
	}
	else if (strncmp(uri, EZCFG_SOAP_HTTP_NVRAM_UNSET_URI"?name=", strlen(EZCFG_SOAP_HTTP_NVRAM_UNSET_URI) + 6) == 0) {
		return true;
	}
	else if (strcmp(uri, EZCFG_SOAP_HTTP_NVRAM_LIST_URI) == 0) {
		return true;
	}
	else if (strcmp(uri, EZCFG_SOAP_HTTP_NVRAM_COMMIT_URI) == 0) {
		return true;
	}
	else {
		return false;
	}
}

static void handle_soap_http_request(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;
	struct ezcfg_soap_http *sh;
	struct ezcfg_http *http;
	struct ezcfg_soap *soap;
	struct ezcfg_nvram *nvram;
	char *request_uri;
	char *buf;
	int len, buf_len;

	ASSERT(worker != NULL);
	ASSERT(worker->proto_data != NULL);

	sh = worker->proto_data;

	ezcfg = worker->ezcfg;
	http = ezcfg_soap_http_get_http(sh);
	soap = ezcfg_soap_http_get_soap(sh);
	nvram = ezcfg_master_get_nvram(worker->master);

	buf_len = EZCFG_SOAP_HTTP_MAX_REQUEST_SIZE ;

	buf = calloc(buf_len, sizeof(char));
	if (buf == NULL) {
		err(ezcfg, "not enough memory for handling SOAP/HTTP request\n");
		return;
	}

	request_uri = ezcfg_http_get_request_uri(http);
	if (request_uri == NULL) {
		err(ezcfg, "no request uri for SOAP/HTTP binding GET method.\n");

		/* clean http structure info */
		ezcfg_http_reset_attributes(http);
		ezcfg_http_set_status_code(http, 400);

		/* build SOAP/HTTP binding error response */
		buf[0] = '\0';
		len = ezcfg_soap_http_write_message(sh, buf, buf_len, EZCFG_SOAP_HTTP_MODE_RESPONSE);

		worker_write(worker, buf, len);
		goto exit;
	}

	if (is_soap_http_nvram_request(request_uri) == true) {
		ezcfg_soap_http_handle_nvram_request(sh, nvram);

		/* build SOAP/HTTP binding response */
		buf[0] = '\0';
		len = ezcfg_soap_http_write_message(sh, buf, buf_len, EZCFG_SOAP_HTTP_MODE_RESPONSE);

		worker_write(worker, buf, len);
	}
exit:
	free(buf);
}

static void handle_igrs_request(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;
	struct ezcfg_igrs *igrs;
	struct ezcfg_http *http;
	struct ezcfg_soap *soap;
	char *request_uri;
	char *msg = NULL;
	int msg_len;

	ASSERT(worker != NULL);
	ASSERT(worker->proto_data != NULL);

	igrs = worker->proto_data;

	ezcfg = worker->ezcfg;
	http = ezcfg_igrs_get_http(igrs);
	soap = ezcfg_igrs_get_soap(igrs);

	dbg(ezcfg, "igrs=[%s]\n", igrs);
	dbg(ezcfg, "http=[%x]\n", http);
	dbg(ezcfg, "soap=[%x]\n", soap);
	request_uri = ezcfg_http_get_request_uri(http);
	if (request_uri == NULL) {
		err(ezcfg, "no request uri for IGRS action.\n");

		/* clean http structure info */
		ezcfg_http_reset_attributes(http);
		ezcfg_http_set_status_code(http, 400);
		ezcfg_http_set_state_response(http);

		/* build IGRS error response */
		msg_len = ezcfg_igrs_http_get_message_length(igrs);
		if (msg_len < 0) {
			err(ezcfg, "ezcfg_igrs_get_message_length error.\n");
			goto exit;
		}
		msg_len++; /* one more for '\0' */
		info(ezcfg, "msg_len=[%d]\n", msg_len);
		msg = (char *)malloc(msg_len);
		if (msg == NULL) {
			err(ezcfg, "malloc msg error.\n");
			goto exit;
		}
		memset(msg, 0, msg_len);
		msg_len = ezcfg_igrs_http_write_message(igrs, msg, msg_len);
		worker_write(worker, msg, msg_len);
		goto exit;
	}

	if (ezcfg_igrs_handle_message(igrs) < 0) {
		dbg(ezcfg, "%s(%d)\n", __func__, __LINE__);
		/* clean http structure info */
		ezcfg_http_reset_attributes(http);
		ezcfg_http_set_status_code(http, 400);
		ezcfg_http_set_state_response(http);

		/* build IGRS error response */
		msg_len = ezcfg_igrs_http_get_message_length(igrs);
		if (msg_len < 0) {
			err(ezcfg, "ezcfg_igrs_get_message_length error.\n");
			goto exit;
		}
		msg_len++; /* one more for '\0' */
		info(ezcfg, "msg_len=[%d]\n", msg_len);
		msg = (char *)malloc(msg_len);
		if (msg == NULL) {
			err(ezcfg, "malloc msg error.\n");
			goto exit;
		}
		memset(msg, 0, msg_len);
		msg_len = ezcfg_igrs_http_write_message(igrs, msg, msg_len);
		worker_write(worker, msg, msg_len);
	}
	else {
		/* build IGRS response */
		ezcfg_igrs_build_message(igrs);
		msg_len = ezcfg_igrs_get_message_length(igrs);
		if (msg_len < 0) {
			err(ezcfg, "ezcfg_igrs_get_message_length error.\n");
			goto exit;
		}
		msg_len++; /* one more for '\0' */
		info(ezcfg, "msg_len=[%d]\n", msg_len);
		msg = (char *)malloc(msg_len);
		if (msg == NULL) {
			err(ezcfg, "malloc msg error.\n");
			goto exit;
		}
		memset(msg, 0, msg_len);
		msg_len = ezcfg_igrs_write_message(igrs, msg, msg_len);
		worker_write(worker, msg, msg_len);
	}
exit:
	if (msg != NULL)
		free(msg);
}

static void process_http_new_connection(struct ezcfg_worker *worker)
{
	int request_len, nread;
	char *buf;
	int buf_len;
	struct ezcfg *ezcfg;

	ASSERT(worker != NULL);

	ezcfg = worker->ezcfg;
	buf_len = EZCFG_HTTP_MAX_REQUEST_SIZE ;

	buf = calloc(buf_len, sizeof(char));
	if (buf == NULL) {
		err(ezcfg, "not enough memory for processing http new connection\n");
		return;
	}
	nread = 0;
	request_len = read_request(worker, buf, buf_len, &nread);

	ASSERT(nread >= request_len);

	if (request_len <= 0) {
		err(ezcfg, "request error\n");
		free(buf);
		return; /* Request is too large or format is not correct */
	}

	if (ezcfg_http_parse_request(worker->proto_data, buf, nread) == true) {
		unsigned short major, minor;
		major = ezcfg_http_get_version_major(worker->proto_data);
		minor = ezcfg_http_get_version_minor(worker->proto_data);
		if (major != 1 || minor != 1) {
			send_http_error(worker, 505,
			                "HTTP version not supported",
			                "%s", "Weird HTTP version");
		} else {
			if (nread > request_len) {
				ezcfg_http_set_message_body(worker->proto_data, buf + request_len, nread - request_len);
			}
			worker->birth_time = time(NULL);
			handle_http_request(worker);
		}
	} else {
		/* Do not put garbage in the access log */
		send_http_error(worker, 400, "Bad Request", "Can not parse request: %.*s", nread, buf);
	}

	/* release buf memory */
	free(buf);
}

static void process_soap_http_new_connection(struct ezcfg_worker *worker)
{
	int request_len, nread;
	char *buf;
	int buf_len;
	struct ezcfg *ezcfg;

	ASSERT(worker != NULL);

	ezcfg = worker->ezcfg;
	buf_len = EZCFG_SOAP_HTTP_MAX_REQUEST_SIZE ;

	buf = calloc(buf_len, sizeof(char));
	if (buf == NULL) {
		err(ezcfg, "not enough memory for processing SOAP/HTTP new connection\n");
		return;
	}
	nread = 0;
	request_len = read_request(worker, buf, buf_len, &nread);

	ASSERT(nread >= request_len);

	if (request_len <= 0) {
		err(ezcfg, "request error\n");
		free(buf);
		return; /* Request is too large or format is not correct */
	}

	/* first setup message body info */
	if (nread > request_len) {
		ezcfg_soap_http_set_http_message_body(worker->proto_data, buf + request_len, nread - request_len);	
	}

	/* 0-terminate the request: parse http request uses sscanf
	 * !!! never, be careful not mangle the "\r\n\r\n" string!!!
	 */
	//buf[request_len - 1] = '\0';
	if (ezcfg_soap_http_parse_request(worker->proto_data, buf, request_len) == true) {
		unsigned short major, minor;
		major = ezcfg_soap_http_get_http_version_major(worker->proto_data);
		minor = ezcfg_soap_http_get_http_version_minor(worker->proto_data);
		if ((major == 1) && (minor == 1)) {
			worker->birth_time = time(NULL);
			handle_soap_http_request(worker);
		} else {
			send_soap_http_error(worker, 505,
			                "SOAP/HTTP binding version not supported",
			                "%s", "Weird HTTP version");
		}
	} else {
		/* Do not put garbage in the access log */
		send_soap_http_error(worker, 400, "Bad Request", "Can not parse request: %.*s", nread, buf);
	}

	/* release buf memory */
	free(buf);
}

static void process_igrs_new_connection(struct ezcfg_worker *worker)
{
	int request_len, nread;
	char *buf;
	int buf_len;
	struct ezcfg *ezcfg;

	ASSERT(worker != NULL);

	ezcfg = worker->ezcfg;
	buf_len = EZCFG_IGRS_MAX_REQUEST_SIZE ;

	buf = calloc(buf_len, sizeof(char));
	if (buf == NULL) {
		err(ezcfg, "not enough memory for processing igrs new connection\n");
		return;
	}
	memset(buf, 0, buf_len);
	nread = 0;
	request_len = read_request(worker, buf, buf_len, &nread);

	ASSERT(nread >= request_len);

	if (request_len <= 0) {
		err(ezcfg, "request error\n");
		free(buf);
		return; /* Request is too large or format is not correct */
	}

	/* first setup message body info */
	dbg(ezcfg, "nread=[%d], request_len=[%d]\n", nread, request_len);
	if (nread > request_len) {
		ezcfg_igrs_set_message_body(worker->proto_data, buf + request_len, nread - request_len);
	}
	dbg(ezcfg, "message_body=[%s]\n", buf + request_len);

	/* 0-terminate the request: parse http request uses sscanf
	 * !!! never, be careful not mangle the "\r\n\r\n" string!!!
	 */
	//buf[request_len - 1] = '\0';
	if (ezcfg_igrs_parse_request(worker->proto_data, buf, request_len) == true) {
		unsigned short major, minor;
		major = ezcfg_igrs_get_version_major(worker->proto_data);
		minor = ezcfg_igrs_get_version_minor(worker->proto_data);
		if ((major == 1) && (minor == 0)) {
#if 0
			dbg(ezcfg, "nread=[%d], request_len=[%d]\n", nread, request_len);
			if (nread > request_len) {
				ezcfg_igrs_set_message_body(worker->proto_data, buf + request_len, nread - request_len);
			}
#endif
			worker->birth_time = time(NULL);
			dbg(ezcfg, "message_body=[%s]\n", buf + request_len);
			handle_igrs_request(worker);
			dbg(ezcfg, "%s(%d)\n", __func__, __LINE__);
		} else {
			send_igrs_error(worker, 505,
			                "IGRS version not supported",
			                "%s", "Weird IGRS version");
		}
	} else {
		/* Do not put garbage in the access log */
		dbg(ezcfg, "ezcfg_igrs_parse_request=false\n");
		send_igrs_error(worker, 400, "Bad Request-1", "Can not parse request: %.*s", nread, buf);
	}

	/* release buf memory */
	free(buf);
}

static void init_protocol_data(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;

	ASSERT(worker != NULL);
	/* proto_data should be empty before init */
	ASSERT(worker->proto_data == NULL);

	ezcfg = worker->ezcfg;

	/* set communication protocol */
	worker->proto = ezcfg_socket_get_proto(worker->client);

	/* initialize protocol data structure */
	switch(worker->proto) {
	case EZCFG_PROTO_HTTP :
		worker->proto_data = ezcfg_http_new(ezcfg);
		break;
	case EZCFG_PROTO_SOAP_HTTP :
		worker->proto_data = ezcfg_soap_http_new(ezcfg);
		break;
	case EZCFG_PROTO_IGRS :
		worker->proto_data = ezcfg_igrs_new(ezcfg);
		break;
	case EZCFG_PROTO_ISDP :
		//worker->proto_data = ezcfg_isdp_new(ezcfg);
		break;
	default :
		info(ezcfg, "unknown protocol\n");
	}
}

static void process_new_connection(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;

	ASSERT(worker != NULL);

	ezcfg = worker->ezcfg;

	reset_connection_attributes(worker);

	/* dispatch protocol handler */
	switch(worker->proto) {
	case EZCFG_PROTO_HTTP :
		process_http_new_connection(worker);
		break;
	case EZCFG_PROTO_SOAP_HTTP :
		process_soap_http_new_connection(worker);
		break;
	case EZCFG_PROTO_IGRS :
		process_igrs_new_connection(worker);
		break;
	case EZCFG_PROTO_ISDP :
		//process_isdp_new_connection(worker);
		break;
	default :
		err(ezcfg, "unknown protocol\n");
	}
}
 
static void release_protocol_data(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;

	ASSERT(worker != NULL);

	ezcfg = worker->ezcfg;

	/* release protocol data */
	switch(worker->proto) {
	case EZCFG_PROTO_HTTP :
		ezcfg_http_delete(worker->proto_data);
		worker->proto_data = NULL;
		break;
	case EZCFG_PROTO_SOAP_HTTP :
		ezcfg_soap_http_delete(worker->proto_data);
		worker->proto_data = NULL;
		break;
	case EZCFG_PROTO_IGRS :
		ezcfg_igrs_delete(worker->proto_data);
		worker->proto_data = NULL;
		break;
	case EZCFG_PROTO_ISDP :
		//ezcfg_isdp_delete(worker->proto_data);
		//worker->proto_data = NULL;
		break;
	default :
		err(ezcfg, "unknown protocol\n");
	}
}

/**
 * Public functions
 **/
void ezcfg_worker_delete(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;

	ASSERT(worker != NULL);

	ezcfg = worker->ezcfg;

	if (worker->client != NULL) {
		info(ezcfg, "delete worker client\n");
		ezcfg_socket_delete(worker->client);
	}

	free(worker);
}

struct ezcfg_worker *ezcfg_worker_new(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;
	struct ezcfg_worker *worker = NULL;
	struct ezcfg_socket *client = NULL;

	ASSERT(master != NULL);
	worker = calloc(1, sizeof(struct ezcfg_worker));
	if (worker == NULL)
		return NULL;

	ezcfg = ezcfg_master_get_ezcfg(master);
	client = ezcfg_socket_calloc(ezcfg, 1);
	if (client == NULL) {
		free(worker);
		return NULL;
	}

	memset(worker, 0, sizeof(struct ezcfg_worker));

	worker->ezcfg = ezcfg;
	worker->master = master;
	worker->client = client;
	worker->proto = EZCFG_PROTO_UNKNOWN;
	worker->proto_data = NULL;
	return worker;

}

void ezcfg_worker_thread(struct ezcfg_worker *worker) 
{
	struct ezcfg *ezcfg;
	struct ezcfg_master *master;

	ASSERT(worker != NULL);

	ezcfg = worker->ezcfg;
	master = worker->master;

	info(ezcfg, "enter worker thread.\n");

	while ((ezcfg_master_is_stop(worker->master) == false) &&
	       (ezcfg_master_get_socket(worker->master, worker->client) == true)) {

		/* record start working time */
		worker->birth_time = time(NULL);

		/* initialize protocol data */
		init_protocol_data(worker);

		/* process the connection */
		if (worker->proto_data != NULL) {
			process_new_connection(worker);
		}

		/* close connection */
		close_connection(worker);

		/* release protocol data */
		if (worker->proto_data != NULL) {
			release_protocol_data(worker);
		}
	}

	/* clean worker resource */
	/* do it in ezcfg_master_stop_worker */
	//ezcfg_worker_delete(worker);

	/* Signal master that we're done with connection and exiting */
	ezcfg_master_stop_worker(master, worker);

	info(ezcfg, "exit worker thread.\n");
}
