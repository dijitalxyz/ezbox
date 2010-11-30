/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-http.h
 *
 * Description  : implement HTTP/1.1 protocol (RFC 2616)
 *                and it's extension (RFC 2774)
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-30   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_HTTP_H_
#define _EZCFG_HTTP_H_

#include "ezcfg.h"

typedef enum {
	HTTP_REQUEST = 0,
	HTTP_RESPONSE
} http_state_t;

struct http_header {
	char *name; /* HTTP header name */
	char *value; /* HTTP header value */
	struct http_header *next;
	bool is_known_header; /* true if name in header_strings */
};

struct status_code_reason_phrase_map {
	unsigned short status_code;
	char *reason_phrase;
};

struct ezcfg_http {
	struct ezcfg *ezcfg;

	http_state_t state;

	unsigned char num_methods; /* Number of supported methods */
	const char **method_strings;
	unsigned char method_index; /* index of method_strings, should be > 0, 0 means error */

	char *request_uri; /* URL-decoded URI */

	unsigned short version_major;
	unsigned short version_minor;

	unsigned short num_status_codes; /* Number of supported status codes */
	const struct status_code_reason_phrase_map *status_code_maps;
	unsigned short status_code_index; /* index of status_code_reason_phrase_map, should be > 0 */

	unsigned char num_known_headers; /* Number of supported headers */
	const char **known_header_strings;

	struct http_header *header_head; /* HTTP header list head */
	struct http_header *header_tail; /* HTTP header list tail */

	int message_body_len; /* HTTP message body length */
	char *message_body; /* HTTP message bosy buffer */

	bool is_ssl; /* true if SSL-ed, false if not */
	bool is_extension; /* true if HTTP extension framework */
};

#endif
