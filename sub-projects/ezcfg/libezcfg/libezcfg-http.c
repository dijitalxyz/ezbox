/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg-http.c
 *
 * Description  : implement HTTP/1.1 protocol (RFC 2616)
 *                and it's extension (RFC 2774)
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
	struct ezcfg *ezcfg;

	unsigned char num_methods; /* Number of supported methods */
	const char **method_strings;
	unsigned char method_index; /* index of method_strings, should be > 0, 0 means error */

	char *request_uri; /* URL-decoded URI */
	char *query_string; /* \0 - terminated, use same buf with request_uri !!! */

	unsigned short version_major;
	unsigned short version_minor;

	unsigned short status_code; /* HTTP response status code */
	char *reason_phrase; /* HTTP response Reason-Phrase */

	unsigned char max_headers; /* Max number of headers */
	unsigned char num_headers; /* Number of headers */
	struct http_header *headers; /* HTTP header array */

	int message_body_len; /* HTTP message body length */
	char *message_body; /* HTTP message bosy buffer */

	bool is_ssl; /* true if SSL-ed, false if not */
	char *remote_user; /* Authenticated user */
	char *log_message; /* error log message */
	char *remote_address; /* Client's network address (path or ip/port) */
};

static const char *default_method_strings[] = {
	/* bad method string */
	NULL,
	/* HTTP/1.1 defined methods */
	"OPTIONS",
	"GET",
	"HEAD",
	"POST",
	"PUT",
	"DELETE",
	"TRACE",
	"CONNECT",
};


/**
 * Skip the characters until one of the delimiters characters found.
 * 0-terminate resulting word. Skip the rest of the delimiters if any.
 * Advance pointer to buffer to the next word. Return found 0-terminated word.
 **/
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

static void clear_http_headers(struct ezcfg_http *http)
{
	struct ezcfg *ezcfg;
	int i;

	assert(http != NULL);

	ezcfg = http->ezcfg;
	for (i=0; i<(int)(http->max_headers); i++) {
		if ( http->headers[i].name != NULL) {
			free(http->headers[i].name);
			http->headers[i].name = NULL;
		}
		if ( http->headers[i].value != NULL) {
			free(http->headers[i].value);
			http->headers[i].value = NULL;
		}
	}
	http->num_headers = 0;
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
	assert(http != NULL);
	assert(http->headers != NULL);

	clear_http_headers(http);
	free(http->headers);

	if (http->request_uri != NULL) {
		free(http->request_uri);
	}
	if (http->message_body != NULL) {
		free(http->message_body);
	}
	if (http->remote_user != NULL) {
		free(http->remote_user);
	}
	if (http->log_message != NULL) {
		free(http->log_message);
	}
	if (http->remote_address != NULL) {
		free(http->remote_address);
	}

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
	struct ezcfg_http *http;

	assert(ezcfg != NULL);

	/* initialize http info structure */
	http = calloc(1, sizeof(struct ezcfg_http));
	if (http == NULL) {
		return NULL;
	}

	memset(http, 0, sizeof(struct ezcfg_http));

	http->max_headers = EZCFG_HTTP_MAX_HEADERS;
	http->headers = calloc(http->max_headers, sizeof(struct http_header));
	if (http->headers == NULL) {
		free(http);
		return NULL;
	}

	http->ezcfg = ezcfg;
	http->num_methods = ARRAY_SIZE(default_method_strings) - 1; /* first item is NULL */
	http->method_strings = default_method_strings;

	return http;
}

void ezcfg_http_delete_remote_user(struct ezcfg_http *http)
{
	struct ezcfg *ezcfg;

	assert(http != NULL);

	ezcfg = http->ezcfg;

	if (http->remote_user != NULL) {
		free(http->remote_user);
		http->remote_user = NULL;
	}
}

void ezcfg_http_reset_attributes(struct ezcfg_http *http)
{
	struct ezcfg *ezcfg;

	assert(http != NULL);

	ezcfg = http->ezcfg;

	clear_http_headers(http);

	if (http->request_uri != NULL) {
		free(http->request_uri);
		http->request_uri = NULL;
	}

	if (http->message_body != NULL) {
		free(http->message_body);
		http->message_body = NULL;
		http->message_body_len = 0;
	}

	if (http->remote_user != NULL) {
		free(http->remote_user);
		http->remote_user = NULL;
	}
	if (http->log_message != NULL) {
		free(http->log_message);
		http->log_message = NULL;
	}
	if (http->remote_address != NULL) {
		free(http->remote_address);
		http->remote_address = NULL;
	}
}

/**
 * Parse HTTP headers from the given buffer, advance buffer to the point
 * where parsing stopped.
 **/
static bool parse_http_headers(struct ezcfg_http *http, char **buf)
{

	struct ezcfg *ezcfg;
	char *name, *value;
	int i;

	assert(http != NULL);
	assert(buf != NULL);

	ezcfg = http->ezcfg;

	for (i=0; i<(int)(http->max_headers); i++) {
		name = skip(buf, ":");
		value = skip(buf, "\r\n");
		while (*value == ' ') value++;
		if (name[0] == '\0')
			break;
		http->headers[i].name = strdup(name);
		http->headers[i].value = strdup(value);
		if ((http->headers[i].name == NULL) || (http->headers[i].value == NULL)) {
			clear_http_headers(http);
			return false;
		}
	}
	http->num_headers = i;
	return true;
}

static unsigned char find_method_index(struct ezcfg_http *http, const char *method)
{
	struct ezcfg *ezcfg;
	int i;

	assert(http != NULL);
	assert(method != NULL);

	ezcfg = http->ezcfg;

	for (i = http->num_methods; i > 0; i--) {
		if (strcmp(http->method_strings[i], method) == 0)
			return i;
	}
	return 0;
}

bool ezcfg_http_parse_request(struct ezcfg_http *http, char *buf)
{
	struct ezcfg *ezcfg;
	char *p;
	char *method, *uri, *version;

	assert(http != NULL);
	assert(buf != NULL);

	ezcfg = http->ezcfg;

	/* split HTTP Request-Line (RFC2616 section 5.1) */
	method = skip(&buf, "\r\n");
	dbg(ezcfg, "request_line=[%s]\n", method);

	/* get request method string */
	p = strchr(method, ' ');
	if (p == NULL)
		return false;

	/* 0-terminated method string */
	*p = '\0';
	http->method_index = find_method_index(http, method);
	if (http->method_index == 0)
		return false;

	/* restore the SP charactor */
	*p = ' ';

	/* get uri string */
	uri = p+1;
	if (uri[0] != '/' && uri[0] != '*')
		return false;

	p = strchr(uri, ' ');
	if (p == NULL)
		return false;

	/* 0-terminated method string */
	*p = '\0';
	http->request_uri = strdup(uri);
	if (http->request_uri == NULL)
		return false;

	/* restore the SP charactor */
	*p = ' ';

	/* get http version */
	version = p+1;
	if (strncmp(version, "HTTP/", 5) != 0)
		return false;

	p = version+5;
	if (sscanf(p, "%hd.%hd",
	           &http->version_major,
	            &http->version_minor) != 2)
		return false;

	/* parse http headers */
	return parse_http_headers(http, &buf);
}

unsigned short ezcfg_http_get_version_major(struct ezcfg_http *http)
{
	struct ezcfg *ezcfg;

	assert(http != NULL);

	ezcfg = http->ezcfg;

	return http->version_major;
}

unsigned short ezcfg_http_get_version_minor(struct ezcfg_http *http)
{
	struct ezcfg *ezcfg;

	assert(http != NULL);

	ezcfg = http->ezcfg;

	return http->version_minor;
}

bool ezcfg_http_set_version_major(struct ezcfg_http *http, unsigned short major)
{
	struct ezcfg *ezcfg;

	assert(http != NULL);

	ezcfg = http->ezcfg;

	http->version_major = major;

	return true;
}

bool ezcfg_http_set_version_minor(struct ezcfg_http *http, unsigned short minor)
{
	struct ezcfg *ezcfg;

	assert(http != NULL);

	ezcfg = http->ezcfg;

	http->version_minor = minor;

	return true;
}

void ezcfg_http_set_status_code(struct ezcfg_http *http, int status_code)
{
	struct ezcfg *ezcfg;

	assert(http != NULL);

	ezcfg = http->ezcfg;

	http->status_code = status_code;
}

char *ezcfg_http_get_header(struct ezcfg_http *http, char *name)
{
	int i;
	struct ezcfg *ezcfg;

	assert(http != NULL);

	ezcfg = http->ezcfg;

	for (i = 0; i < http->num_headers; i++)
		if (!strcasecmp(name, http->headers[i].name))
			return http->headers[i].value;

	return NULL;
}

void ezcfg_http_dump(struct ezcfg_http *http)
{
	struct ezcfg *ezcfg;
	int i;

	assert(http != NULL);

	ezcfg = http->ezcfg;

	info(ezcfg, "request_method=[%s]\n", http->method_strings[http->method_index]);
	info(ezcfg, "uri=[%s]\n", http->request_uri);
	info(ezcfg, "query_string=[%s]\n", http->query_string);
	info(ezcfg, "http_version=[%d.%d]\n", http->version_major, http->version_minor);
	info(ezcfg, "message_body_len=[%d]\n", http->message_body_len);
	info(ezcfg, "message_body=[%s]\n", http->message_body);
	info(ezcfg, "remote_user=[%s]\n", http->remote_user);
	info(ezcfg, "remote_address=[%s]\n", http->remote_address);
	info(ezcfg, "status_code=[%d]\n", http->status_code);
	info(ezcfg, "is_ssl=[%d]\n", http->is_ssl);
	info(ezcfg, "max_headers=[%d]\n", http->max_headers);
	info(ezcfg, "num_headers=[%d]\n", http->num_headers);
	for(i=0; i<http->num_headers; i++)
		info(ezcfg, "header[%d]=[(%s),(%s)]\n", i, http->headers[i].name, http->headers[i].value);
}

bool ezcfg_http_set_method_strings(struct ezcfg_http *http, const char **method_strings, unsigned char num_methods)
{
	struct ezcfg *ezcfg;

	assert(http != NULL);
	assert(method_strings != NULL);

	ezcfg = http->ezcfg;

	http->num_methods = num_methods;
	http->method_strings = method_strings;

	return true;
}

unsigned char ezcfg_http_set_request_method(struct ezcfg_http *http, const char *method)
{
	struct ezcfg *ezcfg;
	int i;

	assert(http != NULL);
	assert(method != NULL);

	ezcfg = http->ezcfg;

	for (i = http->num_methods; i > 0; i++) {
		if (strcmp(http->method_strings[i], method) == 0)
			break;
	}

	return i;
}

bool ezcfg_http_set_request_uri(struct ezcfg_http *http, const char *uri)
{
	struct ezcfg *ezcfg;
	char *request_uri;

	assert(http != NULL);

	ezcfg = http->ezcfg;

	request_uri = strdup(uri);
	if (request_uri == NULL) {
		return false;
	}

	if (http->request_uri != NULL) {
		free(http->request_uri);
	}

	http->request_uri = request_uri;

	return true;
}

bool ezcfg_http_set_message_body(struct ezcfg_http *http, const char *body, int len)
{
	struct ezcfg *ezcfg;
	char *message_body;

	assert(http != NULL);

	ezcfg = http->ezcfg;

	message_body = calloc(len, sizeof(char));
	if (message_body == NULL) {
		return false;
	}
	memcpy(message_body, body, len);

	if (http->message_body != NULL) {
		free(http->message_body);
	}

	http->message_body = message_body;
	http->message_body_len = len;

	return true;
}
