/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg-worker.c
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

#define INVALID_SOCKET	-1
#define BUFFER_SIZE	4096
#define MAX_REQUEST_SIZE 8192

/*
 * ezcfg_worker:
 *
 * Opaque object handling one event source.
 * Multi-threads model - worker part.
 */
struct ezcfg_worker {
	struct ezcfg *ezcfg;
	struct ezcfg_master *master;
	struct ezcfg_socket *client;
	struct ezcfg_http *http_info;
	time_t birth_time;
	bool free_post_data;
	int64_t num_bytes_sent;
};

static void reset_per_request_attributes(struct ezcfg_worker *worker)
{
	ezcfg_http_delete_remote_user(worker->http_info);

	if (worker->free_post_data == true) {
		ezcfg_http_delete_post_data(worker->http_info);
	}
}

static int set_non_blocking_mode(int sock)
{
	int flags;

	flags = fcntl(sock, F_GETFL, 0);
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);

	return 0;
}

static void close_socket_gracefully(int sock) {
	char buf[BUFFER_SIZE];
	int n;

	// Send FIN to the client
	shutdown(sock, 1);
	set_non_blocking_mode(sock);

	// Read and discard pending data. If we do not do that and close the
	// socket, the data in the send buffer may be discarded. This
	// behaviour is seen on Windows, when client keeps sending data
	// when server decide to close the connection; then when client
	// does recv() it gets no data back.
	do {
		n = recv(sock, buf, sizeof(buf), 0);
	} while (n > 0);

	// Now we know that our FIN is ACK-ed, safe to close
	close(sock);
}

static void close_connection(struct ezcfg_worker *worker)
{
	int sock;
	reset_per_request_attributes(worker);
	sock = ezcfg_socket_get_sock(worker->client);
	if (sock != INVALID_SOCKET) {
		close_socket_gracefully(sock);
	}
}

static void reset_connection_attributes(struct ezcfg_worker *worker) {
	reset_per_request_attributes(worker);
	worker->free_post_data = false;
	worker->num_bytes_sent = 0;
	ezcfg_http_reset_attributes(worker->http_info);
}

// Return content length of the request, or -1 constant if
// Content-Length header is not set.
static int get_content_length(const struct ezcfg_worker *worker) {
	const char *cl = ezcfg_http_get_header(worker->http_info, "Content-Length");
	return cl == NULL ? -1 : strtol(cl, NULL, 10);
}


// Check whether full request is buffered. Return:
//   -1  if request is malformed
//    0  if request is not yet fully buffered
//   >0  actual request length, including last \r\n\r\n
static int get_request_len(const char *buf, size_t buflen)
{
	const char *s, *e;
	int len = 0;

	for (s = buf, e = s + buflen - 1; len <= 0 && s < e; s++)
		// Control characters are not allowed but >=128 is.
		if (!isprint(* (unsigned char *) s) && *s != '\r' &&
		    *s != '\n' && * (unsigned char *) s < 128) {
			len = -1;
		} else if (s[0] == '\n' && s[1] == '\n') {
			len = (int) (s - buf) + 2;
		} else if (s[0] == '\n' && &s[1] < e &&
			   s[1] == '\r' && s[2] == '\n') {
			len = (int) (s - buf) + 3;
		}

	return len;
}

// Keep reading the input (either opened file descriptor fd, or socket sock,
// or SSL descriptor ssl) into buffer buf, until \r\n\r\n appears in the
// buffer (which marks the end of HTTP request). Buffer buf may already
// have some data. The length of the data is stored in nread.
// Upon every read operation, increase nread by the number of bytes read.
static int read_request(int sock, char *buf, int bufsiz, int *nread)
{
	int n, request_len;

	request_len = 0;
	while (*nread < bufsiz && request_len == 0) {
		n = recv(sock, buf + *nread, bufsiz - *nread, 0);
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

static int worker_write(struct ezcfg_worker *worker, const char *buf, int len)
{
	int sent;
	int n, k;
	int sock;

	sent = 0;
	sock = ezcfg_socket_get_sock(worker->client);
	while (sent < len) {
		/* How many bytes we send in this iteration */
		k = len - sent > INT_MAX ? INT_MAX : (int) (len - sent);
		n = send(sock, buf + sent, k, 0);
		if (n < 0)
			break;
		sent += n;
	}
	return sent;
}

static int worker_printf(struct ezcfg_worker *worker, const char *fmt, ...)
{
	char buf[MAX_REQUEST_SIZE];
	int len;
	va_list ap;

	va_start(ap, fmt);
	len = vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	return worker_write(worker, buf, len);
}

static void send_http_error(struct ezcfg_worker *worker, int status,
                            const char *reason, const char *fmt, ...)
{
	char buf[BUFFER_SIZE];
	va_list ap;
	int len;
	bool handled;

	ezcfg_http_set_status_code(worker->http_info, status);
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

// This is the heart of the worker's logic.
// This function is called when the request is read, parsed and validated,
// and worker must decide what action to take: serve a file, or
// a directory, or call embedded function, etcetera.
static void handle_request(struct ezcfg_worker *worker)
{
	worker_printf(worker,
	             "HTTP/1.1 %d %s\r\n"
	             "\r\n", 200, "OK");
}

static void shift_to_next(struct ezcfg_worker *worker, char *buf, int req_len, int *nread)
{
	int cl;
	int over_len, body_len;

	cl = get_content_length(worker);
	over_len = *nread - req_len;
	assert(over_len >= 0);

	if (cl == -1) {
		body_len = 0;
	} else if (cl < (int64_t) over_len) {
		body_len = (int) cl;
	} else {
		body_len = over_len;
	}

	*nread -= req_len + body_len;
	memmove(buf, buf + req_len + body_len, *nread);
}

static void process_new_connection(struct ezcfg_worker *worker)
{
	int request_len, nread;
	char buf[MAX_REQUEST_SIZE];
	struct ezcfg *ezcfg;

	assert(worker != NULL);

	ezcfg = worker->ezcfg;

	dbg(ezcfg, "%d\n", __LINE__);

	nread = 0;
	reset_connection_attributes(worker);
	memset(buf, 0, sizeof(buf));

	/* If next request is not pipelined, read it */
	request_len = read_request(ezcfg_socket_get_sock(worker->client), buf, sizeof(buf), &nread);
	assert(nread >= request_len);
	if (request_len <= 0) {
		info(ezcfg, "%d remote end closed the connection\n", __LINE__);
		return; /* Remote end closed the connection */
	}

	dbg(ezcfg, "%d\n", __LINE__);

	/* 0-terminate the request: parse http request uses sscanf */
	buf[request_len - 1] = '\0';
	if (ezcfg_http_parse_request(worker->http_info, buf)) {
		char *http_version = ezcfg_http_get_version(worker->http_info);
		if (http_version == NULL) {
			send_http_error(worker, 505,
			                "HTTP version not supported",
			                "%s", "Weird HTTP version");
		} else if (strcmp(http_version, "1.0") != 0 &&
		           strcmp(http_version, "1.1") != 0) {
			send_http_error(worker, 505,
			                "HTTP version not supported",
			                "%s", "Weird HTTP version");
		} else {
			ezcfg_http_set_post_data(worker->http_info, buf + request_len);
			ezcfg_http_set_post_data_len(worker->http_info, nread - request_len);
			worker->birth_time = time(NULL);
			handle_request(worker);
			shift_to_next(worker, buf, request_len, &nread);
		}
	} else {
		/* Do not put garbage in the access log */
		send_http_error(worker, 400, "Bad Request", "Can not parse request: [%.*s]", nread, buf);
	}
	dbg(ezcfg, "%d\n", __LINE__);
}

struct ezcfg_worker *ezcfg_worker_new(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;
	struct ezcfg_worker *worker;
	struct ezcfg_socket *client;
	struct ezcfg_http *http_info;

	assert(master != NULL);

	worker = calloc(1, sizeof(struct ezcfg_worker));
	if (worker == NULL)
		return NULL;

	ezcfg = ezcfg_master_get_ezcfg(master);
	client = ezcfg_socket_calloc(ezcfg, 1);
	if (client == NULL) {
		free(worker);
		return NULL;
	}

	http_info = ezcfg_http_new(ezcfg);
	if (http_info == NULL) {
		free(worker);
		free(client);
		return NULL;
	}

	memset(worker, 0x00, sizeof(struct ezcfg_worker));
	worker->ezcfg = ezcfg;
	worker->master = master;
	worker->client = client;
	worker->http_info = http_info;
	return worker;
}

void ezcfg_worker_thread(struct ezcfg_worker *worker) 
{
	struct ezcfg *ezcfg;

	assert(worker != NULL);

	ezcfg = worker->ezcfg;

	dbg(ezcfg, "%d stop_flag=[%d]\n", __LINE__, ezcfg_master_is_stop(worker->master));

	while ((ezcfg_master_is_stop(worker->master) == false) &&
	       (ezcfg_master_get_socket(worker->master, worker->client) == true)) {
		dbg(ezcfg, "%d\n", __LINE__);
		worker->birth_time = time(NULL);
		process_new_connection(worker);
		close_connection(worker);
	}

	dbg(ezcfg, "%d\n", __LINE__);
	// Signal master that we're done with connection and exiting
	ezcfg_master_stop_worker(worker->master);
}
