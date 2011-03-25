/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/http_html_admin.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-03-25   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-http.h"

/**
 * Private functions
 **/

static int build_admin_response(struct ezcfg_http *http, struct ezcfg_nvram *nvram, char *uri)
{
	struct ezcfg *ezcfg;
	struct ezcfg_html *html = NULL;
	int head_index, body_index;
	char *msg = NULL;
	int msg_len;
	int n;
	int rc = 0;
	
	ASSERT(http != NULL);
	ASSERT(nvram != NULL);
	ASSERT(uri != NULL);

	ezcfg = http->ezcfg;

	html = ezcfg_html_new(ezcfg);

	if (html == NULL) {
		err(ezcfg, "can not alloc html.\n");
		goto exit;
	}

	/* clean HTML structure info */
	ezcfg_html_reset_attributes(html);

	/* build HTML */
	ezcfg_html_set_version_major(html, 1);
	ezcfg_html_set_version_minor(html, 2);

	/* HTML root */
	ezcfg_html_set_root(html, EZCFG_HTML_ROOT_ELEMENT_NAME);

	/* HTML Head */
	head_index = ezcfg_html_set_head(html, EZCFG_HTML_HEAD_ELEMENT_NAME);

	/* HTML Body */
	body_index = ezcfg_html_set_body(html, EZCFG_HTML_BODY_ELEMENT_NAME);

	msg_len = ezcfg_html_get_message_length(html);
	if (msg_len < 0) {
		err(ezcfg, "ezcfg_html_get_message_length\n");
		rc = -1;
		goto exit;
	}
	msg_len++; /* one more for '\0' */
	msg = (char *)malloc(msg_len);
	if (msg == NULL) {
		err(ezcfg, "malloc error.\n");
		rc = -1;
		goto exit;
	}

	memset(msg, 0, msg_len);
	n = ezcfg_html_write_message(html, msg, msg_len);
	if (n < 0) {
		err(ezcfg, "ezcfg_html_write_message\n");
		rc = -1;
		goto exit;
	}

	/* FIXME: name point to http->request_uri !!!
         * never reset http before using name */
	/* clean http structure info */
	ezcfg_http_reset_attributes(http);
	ezcfg_http_set_status_code(http, 200);
	ezcfg_http_set_state_response(http);

	if (ezcfg_http_set_message_body(http, msg, n) == NULL) {
		err(ezcfg, "ezcfg_http_set_message_body\n");
		rc = -1;
		goto exit;
	}

	/* set return value */
	rc = 0;
exit:
	if (html != NULL)
		ezcfg_html_delete(html);

	if (msg != NULL)
		free(msg);

	return rc;
}

/**
 * Public functions
 **/

int ezcfg_http_handle_admin_request(struct ezcfg_http *http, struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;
	char *request_uri;
	char *section;
	int ret = -1;

	ASSERT(http != NULL);
	ASSERT(nvram != NULL);

	ezcfg = http->ezcfg;

	request_uri = ezcfg_http_get_request_uri(http);

	if (strncmp(request_uri, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI, strlen(EZCFG_HTTP_HTML_ADMIN_PREFIX_URI)) == 0) {
		/* admin prefix uri=[/admin/] */
		section = request_uri+strlen(EZCFG_HTTP_HTML_ADMIN_PREFIX_URI);
		ret = build_admin_response(http, nvram, section);
	}
	return ret;
}
