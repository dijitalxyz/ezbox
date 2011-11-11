/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/worker.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
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

/*
 * ezcfg_worker:
 * Opaque object handling one event source.
 * Multi-threads model - worker part.
 */
struct ezcfg_worker {
	pthread_t thread_id;
	struct ezcfg *ezcfg;
	struct ezcfg_worker *next; /* Linkage */
	struct ezcfg_master *master;
	struct ezcfg_socket *client;
	unsigned char proto;
	void *proto_data;
	time_t birth_time;
	int64_t num_bytes_sent;
};

#define handle_error_en(en, msg) \
	do { errno = en; perror(msg); } while (0)

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
	case EZCFG_PROTO_UEVENT :
		info(ezcfg, "UEVENT reset attributes\n");
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

#if 0
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
		cl = ezcfg_igrs_get_http_header_value(worker->proto_data, EZCFG_IGRS_HTTP_HEADER_CONTENT_LENGTH);
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
static int http_read_request(struct ezcfg_worker *worker, struct ezcfg_http *http, char *buf, int bufsiz, int *nread)
{
	struct ezcfg *ezcfg;
	int n, request_len;

	ASSERT(worker != NULL);
	ASSERT(http != NULL);

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

/**
 * Keep reading the input into buffer buf, until reach max buffer size or error.
 * Buffer buf may already have some data. The length of the data is stored in nread.
 * Upon every read operation, increase nread by the number of bytes read.
 **/
static char * http_read_content(struct ezcfg_worker *worker, struct ezcfg_http *http, char *buf, int request_len, int *bufsiz, int *nread)
{
	struct ezcfg *ezcfg;
	int n;
	char *p, *q;
	int content_length, buf_len, chunk_size;

	ASSERT(worker != NULL);
	ASSERT(http != NULL);

	ezcfg = worker->ezcfg;
	buf_len = *bufsiz;

	if ((p = ezcfg_http_get_header_value(http, EZCFG_HTTP_HEADER_CONTENT_LENGTH)) != NULL) {
		content_length = atoi(p);
		if ((*nread - request_len) < content_length) {
			/* need to read more data from socket */
			buf_len += (content_length - (*nread - request_len));
			buf = realloc(buf, buf_len);
			if (buf == NULL) {
				return NULL;
			}
			*bufsiz = buf_len;
			while (*nread < buf_len) {
				n = ezcfg_socket_read(worker->client, buf + *nread, *bufsiz - *nread, 0);
				if (n <= 0) {
					break;
				}
				else {
					*nread += n;
				}
			}

			if ((*nread - request_len) < content_length) {
				/* can not read all content */
				return NULL;
			}
		}
	}
	else if ((p = ezcfg_http_get_header_value(http, EZCFG_HTTP_HEADER_TRANSFER_ENCODING)) != NULL) {
		if (strcmp(p, "chunked") != 0) {
			/* unknown Transfer-Encoding */
			return NULL;
		}

		do {
			if (buf_len <= *nread) {
				buf_len += EZCFG_BUFFER_SIZE;
			}
			if (buf_len > EZCFG_HTTP_MAX_REQUEST_SIZE) {
				/* too large for the request */
				return NULL;
			}
			buf = realloc(buf, buf_len);
			if (buf == NULL) {
				return NULL;
			}
			*bufsiz = buf_len;
			while (*nread < *bufsiz) {
				n = ezcfg_socket_read(worker->client, buf + *nread, *bufsiz - *nread, 0);
				if (n <= 0) {
					break;
				}
				else {
					*nread += n;
				}
			}
		} while (*nread == buf_len);

		/* concate chunk data */
		/* 0-terminate the content */
		buf[*nread] = '\0';
		/* point to chunk size */
		p = buf+request_len;
		content_length = 0;
		chunk_size = strtol(p, NULL, 16);
		while((chunk_size > 0) && (chunk_size <= EZCFG_HTTP_CHUNK_SIZE)) {
			q = strstr(p, EZCFG_HTTP_CRLF_STRING);
			if (q == NULL) {
				return NULL;
			}
			q += strlen(EZCFG_HTTP_CRLF_STRING);
			memmove(buf + request_len + content_length, q, chunk_size);
			content_length += chunk_size;
			p = q + chunk_size + strlen(EZCFG_HTTP_CRLF_STRING);
			chunk_size = strtol(p, NULL, 16);
		}
		if ((chunk_size < 0) || (chunk_size > EZCFG_HTTP_CHUNK_SIZE)) {
			return NULL;
		}
		/* update nread */
		*nread = request_len + content_length;
	}

	return buf;
}
#endif

static bool error_handler(struct ezcfg_worker *worker)
{
	return false;
}

static int worker_printf(struct ezcfg_worker *worker, const char *fmt, ...)
{
	char *buf;
	int buf_len;
	int len;
	int ret;
	va_list ap;

	buf_len = EZCFG_BUFFER_SIZE ;
	buf = (char *)malloc(buf_len);
	if (buf == NULL) {
		return -1;
	}

	va_start(ap, fmt);
	len = vsnprintf(buf, buf_len, fmt, ap);
	va_end(ap);

	ret = ezcfg_socket_write(worker->client, buf, len, 0);
	free(buf);
	return ret;
}

static int worker_write(struct ezcfg_worker *worker, const char *buf, int len)
{
	return ezcfg_socket_write(worker->client, buf, len, 0);
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

static void process_igrs_new_connection(struct ezcfg_worker *worker)
{
	int header_len, nread;
	char *buf;
	int buf_len;
	struct ezcfg *ezcfg;
	struct ezcfg_igrs *igrs;
	struct ezcfg_http *http;

	ASSERT(worker != NULL);

	ezcfg = worker->ezcfg;
	igrs = (struct ezcfg_igrs *)(worker->proto_data);
	http = ezcfg_igrs_get_http(igrs);
	buf_len = EZCFG_IGRS_HTTP_CHUNK_SIZE ;

	buf = calloc(buf_len+1, sizeof(char)); /* +1 for \0 */
	if (buf == NULL) {
		err(ezcfg, "not enough memory for processing igrs new connection\n");
		return;
	}
	memset(buf, 0, buf_len);
	nread = 0;
	header_len = ezcfg_socket_read_http_header(worker->client, http, buf, buf_len, &nread);

	ASSERT(nread >= header_len);

	if (header_len <= 0) {
		err(ezcfg, "request error\n");
		goto exit;
	}

	/* first setup message body info */
	if (nread > header_len) {
		ezcfg_igrs_set_message_body(igrs, buf + header_len, nread - header_len);
	}

	/* 0-terminate the request: parse http request uses sscanf
	 * !!! never, be careful not mangle the "\r\n\r\n" string!!!
	 */
	//buf[header_len - 1] = '\0';
	ezcfg_http_set_state_request(http);
	if (ezcfg_igrs_parse_header(igrs, buf, header_len) == true) {
		unsigned short major, minor;
		char *p;
		major = ezcfg_igrs_get_version_major(igrs);
		minor = ezcfg_igrs_get_version_minor(igrs);
		if ((major != 1) || (minor != 0)) {
			send_igrs_error(worker, 505,
			                "IGRS version not supported",
			                "%s", "Weird IGRS version");
			goto exit;
		}
		p = ezcfg_socket_read_http_content(worker->client, http, buf, header_len, &buf_len, &nread);
		if (p == NULL) {
			/* Do not put garbage in the access log */
			send_igrs_error(worker, 400, "Bad Request", "Can not parse request: %.*s", nread, buf);
			goto exit;
		}
		buf = p;
		if (nread > header_len) {
			ezcfg_igrs_set_message_body(igrs, buf + header_len, nread - header_len);
			ezcfg_igrs_parse_message_body(igrs);
		}
		worker->birth_time = time(NULL);
		handle_igrs_request(worker);
	} else {
		/* Do not put garbage in the access log */
		send_igrs_error(worker, 400, "Bad Request", "Can not parse request: %.*s", nread, buf);
	}

exit:
	/* release buf memory */
	if (buf != NULL)
		free(buf);
}

static void process_uevent_new_connection(struct ezcfg_worker *worker)
{
	char *buf;
	int buf_len;
	struct ezcfg *ezcfg;
	struct ezcfg_uevent *uevent;

	ASSERT(worker != NULL);

	ezcfg = worker->ezcfg;
	uevent = (struct ezcfg_uevent *)(worker->proto_data);
	buf = ezcfg_socket_get_buffer(worker->client);
	if (buf == NULL) {
		err(ezcfg, "not enough memory for processing ssdp new connection\n");
		return;
	}
	buf_len = ezcfg_socket_get_buffer_len(worker->client);

	info(ezcfg, "uevent=[%s], len=%d\n", buf, buf_len);
	return;
}

static void process_ssdp_new_connection(struct ezcfg_worker *worker)
{
	char *buf;
	int buf_len;
	struct ezcfg *ezcfg;
	struct ezcfg_ssdp *ssdp;

	ASSERT(worker != NULL);

	ezcfg = worker->ezcfg;
	ssdp = (struct ezcfg_ssdp *)(worker->proto_data);
	buf = ezcfg_socket_get_buffer(worker->client);
	if (buf == NULL) {
		err(ezcfg, "not enough memory for processing ssdp new connection\n");
		return;
	}
	buf_len = ezcfg_socket_get_buffer_len(worker->client);

	info(ezcfg, "ssdp=[%s], len=%d\n", buf, buf_len);
	return;
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
	case EZCFG_PROTO_UEVENT :
		worker->proto_data = ezcfg_uevent_new(ezcfg);
		info(ezcfg, "UEVENT protocol\n");
		break;
	case EZCFG_PROTO_SSDP :
		worker->proto_data = ezcfg_ssdp_new(ezcfg);
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
		//process_http_new_connection(worker);
		ezcfg_worker_process_http_new_connection(worker);
		break;
	case EZCFG_PROTO_SOAP_HTTP :
		//process_soap_http_new_connection(worker);
		ezcfg_worker_process_soap_http_new_connection(worker);
		break;
	case EZCFG_PROTO_IGRS :
		process_igrs_new_connection(worker);
		break;
	case EZCFG_PROTO_ISDP :
		//process_isdp_new_connection(worker);
		break;
	case EZCFG_PROTO_UEVENT :
		process_uevent_new_connection(worker);
		break;
	case EZCFG_PROTO_SSDP :
		process_ssdp_new_connection(worker);
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
	case EZCFG_PROTO_UEVENT :
		ezcfg_uevent_delete(worker->proto_data);
		worker->proto_data = NULL;
		break;
	case EZCFG_PROTO_SSDP :
		ezcfg_ssdp_delete(worker->proto_data);
		worker->proto_data = NULL;
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

pthread_t *ezcfg_worker_get_thread_id(struct ezcfg_worker *worker)
{
	return &(worker->thread_id);
}

struct ezcfg_worker *ezcfg_worker_get_next(struct ezcfg_worker *worker)
{
	ASSERT(worker != NULL);
	return worker->next;
}

bool ezcfg_worker_set_next(struct ezcfg_worker *worker, struct ezcfg_worker *next)
{
	ASSERT(worker != NULL);
	worker->next = next;
	return true;
}

void ezcfg_worker_close_connection(struct ezcfg_worker *worker)
{
	close_connection(worker);
}

void ezcfg_worker_thread(struct ezcfg_worker *worker) 
{
	struct ezcfg *ezcfg;
	struct ezcfg_master *master;
	//sigset_t sigset;
	//int s;

	ASSERT(worker != NULL);

	ezcfg = worker->ezcfg;
	master = worker->master;

	/* Block signal HUP, USR1 */
	/* do it in root thread */
#if 0
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGHUP);
	sigaddset(&sigset, SIGUSR1);
	s = pthread_sigmask(SIG_BLOCK, &sigset, NULL);
	if (s != 0) {
		handle_error_en(s, "pthread_sigmask");
		goto worker_exit;
	}
#endif

	while ((ezcfg_master_is_stop(worker->master) == false) &&
	       (ezcfg_master_get_socket(worker->master, worker->client, EZCFG_WORKER_WAIT_TIME) == true)) {

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
}

struct ezcfg *ezcfg_worker_get_ezcfg(struct ezcfg_worker *worker)
{
	return worker->ezcfg;
}

struct ezcfg_master *ezcfg_worker_get_master(struct ezcfg_worker *worker)
{
	return worker->master;
}

int ezcfg_worker_printf(struct ezcfg_worker *worker, const char *fmt, ...)
{
	char *buf;
	int buf_len;
	int len;
	int ret;
	va_list ap;

	buf_len = EZCFG_BUFFER_SIZE ;
	buf = (char *)malloc(buf_len);
	if (buf == NULL) {
		return -1;
	}

	va_start(ap, fmt);
	len = vsnprintf(buf, buf_len, fmt, ap);
	va_end(ap);

	ret = ezcfg_socket_write(worker->client, buf, len, 0);
	free(buf);
	return ret;
}

int ezcfg_worker_write(struct ezcfg_worker *worker, const char *buf, int len)
{
	return ezcfg_socket_write(worker->client, buf, len, 0);
}

void *ezcfg_worker_get_proto_data(struct ezcfg_worker *worker)
{
	return worker->proto_data;
}

struct ezcfg_socket *ezcfg_worker_get_client(struct ezcfg_worker *worker)
{
	return worker->client;
}

bool ezcfg_worker_set_num_bytes_sent(struct ezcfg_worker *worker, int64_t num)
{
	worker->num_bytes_sent = num;
	return true;
}

bool ezcfg_worker_set_birth_time(struct ezcfg_worker *worker, time_t t)
{
	worker->birth_time = t;
	return true;
}
