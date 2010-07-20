/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg-http.c
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

struct http_header {
	char *name; /* HTTP header name */
	char *value; /* HTTP header value */
};

struct ezcfg_http {
	char *request_method; /* "GET", "POST", etc */
	char *uri; /* URL-decoded URI */
	char *http_version; /* E.g. "1.0", "1.1" */
	char *query_string; /* \0 - terminated */
	char *post_data; /* POST data buffer */
	char *remote_user; /* Authenticated user */
	char *log_message; /* error log message */
	char *remote_address; /* Client's network address (path or ip/port) */
	int post_data_len; /* POST buffer length */
	int status_code; /* HTTP status code */
	bool is_ssl; /* true if SSL-ed, false if not */
	int num_headers; /* Number of headers */
	struct http_header headers[64]; /* Maximum 64 headers */
};

// Skip the characters until one of the delimiters characters found.
// 0-terminate resulting word. Skip the rest of the delimiters if any.
// Advance pointer to buffer to the next word. Return found 0-terminated word.
static char *skip(char **buf, const char *delimiters)
{
	char *p, *begin_word, *end_word, *end_delimiters;

	begin_word = *buf;
	end_word = begin_word + strcspn(begin_word, delimiters);
	end_delimiters = end_word + strspn(end_word, delimiters);

	for (p = end_word; p < end_delimiters; p++) {
		*p = '\0';
	}

	*buf = end_delimiters;

	return begin_word;
}

/**
 * ezcfg_http_delete:
 *
 * Delete ezcfg http info structure.
 *
 * Returns:
 **/
void ezcfg_http_delete(struct ezcfg_http *http)
{
	if (http == NULL)
		return ;
	free(http);
}

/**
 * ezcfg_http_new:
 *
 * Create ezcfg http info structure.
 *
 * Returns: a new ezcfg http info structure
 **/
struct ezcfg_http *ezcfg_http_new(struct ezcfg *ezcfg)
{
	struct ezcfg_http *http = NULL;

	/* initialize http info structure */
	if ((http = calloc(1, sizeof(struct ezcfg_http))) == NULL)
		return NULL;

	memset(http, 0x00, sizeof(struct ezcfg_http));
	return http;
}

void ezcfg_http_delete_remote_user(struct ezcfg_http *http)
{
	if (http == NULL)
		return;
	if (http->remote_user != NULL) {
		free(http->remote_user);
		http->remote_user = NULL;
	}
}

void ezcfg_http_delete_post_data(struct ezcfg_http *http)
{
	if (http == NULL)
		return;
	if (http->post_data != NULL) {
		free(http->post_data);
		http->post_data = NULL;
	}
}

void ezcfg_http_reset_attributes(struct ezcfg_http *http)
{
	if (http == NULL)
		return;
	memset(http, 0, sizeof(struct ezcfg_http));
}

// Parse HTTP headers from the given buffer, advance buffer to the point
// where parsing stopped.
static void parse_http_headers(struct ezcfg_http *http, char **buf)
{
	int i;

	for (i = 0; i < (int)ARRAY_SIZE(http->headers); i++) {
		http->headers[i].name = skip(buf, ": ");
		http->headers[i].value = skip(buf, "\r\n");
		if (http->headers[i].name[0] == '\0')
			break;
		http->num_headers = i + 1;
	}
}

static bool is_valid_http_method(const char *method)
{
	return !strcmp(method, "GET")  ||
	       !strcmp(method, "POST") ||
	       !strcmp(method, "HEAD") ||
	       !strcmp(method, "PUT")  ||
	       !strcmp(method, "DELETE");
}

bool ezcfg_http_parse_request(struct ezcfg_http *http, char *buf)
{
	bool status = false;
	http->request_method = skip(&buf, " ");
	http->uri = skip(&buf, " ");
	http->http_version = skip(&buf, "\r\n");

	if (is_valid_http_method(http->request_method) &&
	    http->uri[0] == '/' &&
	    strncmp(http->http_version, "HTTP/", 5) == 0) {
		http->http_version += 5; /* Skip "HTTP/" */
		parse_http_headers(http, &buf);
		status = true;
	}

	return status;
}

char *ezcfg_http_get_version(struct ezcfg_http *http)
{
	return http->http_version;
}

void ezcfg_http_set_status_code(struct ezcfg_http *http, int status_code)
{
	http->status_code = status_code;
}

void ezcfg_http_set_post_data(struct ezcfg_http *http, char *data)
{
	http->post_data = data;
}

void ezcfg_http_set_post_data_len(struct ezcfg_http *http, int len)
{
	http->post_data_len = len;
}
