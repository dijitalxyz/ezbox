/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/http_html_index.c
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
#include "ezcfg-html.h"

/**
 * Private functions
 **/

static int build_home_index_response(struct ezcfg_http *http, struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;
	struct ezcfg_html *html = NULL;
	int head_index, body_index, child_index;
	char *msg = NULL;
	int msg_len;
	int n;
	int rc = 0;
	
	ASSERT(http != NULL);
	ASSERT(nvram != NULL);

	ezcfg = http->ezcfg;

	html = ezcfg_html_new(ezcfg);

	if (html == NULL) {
		err(ezcfg, "can not alloc html.\n");
		goto exit;
	}

	/* clean HTML structure info */
	ezcfg_html_reset_attributes(html);

	/* build HTML */
	ezcfg_html_set_version_major(html, 4);
	ezcfg_html_set_version_minor(html, 1);

	/* HTML root */
	ezcfg_html_set_root(html, EZCFG_HTML_HTML_ELEMENT_NAME);

	/* HTML Head */
	head_index = ezcfg_html_set_head(html, EZCFG_HTML_HEAD_ELEMENT_NAME);

	/* HTML Meta charset */
	child_index = ezcfg_html_add_head_child(html, head_index, -1, EZCFG_HTML_META_ELEMENT_NAME, NULL);
	ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_HTTP_EQUIV_ATTRIBUTE_NAME, EZCFG_HTTP_HEADER_CONTENT_TYPE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_CONTENT_ATTRIBUTE_NAME, "text/html; charset=UTF-8", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* HTML Title */
	child_index = ezcfg_html_add_head_child(html, head_index, child_index, EZCFG_HTML_TITLE_ELEMENT_NAME, "Welcome");

	/* HTML Body */
	body_index = ezcfg_html_set_body(html, EZCFG_HTML_BODY_ELEMENT_NAME);

	/* HTML P */
	child_index = ezcfg_html_add_body_child(html, body_index, -1, EZCFG_HTML_P_ELEMENT_NAME, "Hello World!");

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

static int build_redirect_to_home_response(struct ezcfg_http *http, struct ezcfg_nvram *nvram)
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

	ezcfg = http->ezcfg;

	html = ezcfg_html_new(ezcfg);

	if (html == NULL) {
		err(ezcfg, "can not alloc html.\n");
		goto exit;
	}

	/* clean HTML structure info */
	ezcfg_html_reset_attributes(html);

	/* build HTML */
	ezcfg_html_set_version_major(html, 4);
	ezcfg_html_set_version_minor(html, 1);

	/* HTML root */
	ezcfg_html_set_root(html, EZCFG_HTML_HTML_ELEMENT_NAME);

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

int ezcfg_http_handle_index_request(struct ezcfg_http *http, struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;
	char *request_uri;
	int ret = -1;

	ASSERT(http != NULL);
	ASSERT(nvram != NULL);

	ezcfg = http->ezcfg;

	request_uri = ezcfg_http_get_request_uri(http);

	if (strcmp(request_uri, EZCFG_HTTP_HTML_HOME_INDEX_URI) == 0) {
		/* home index uri=[/] */
		ret = build_home_index_response(http, nvram);
	}
	else {
		/* redirect to home index */
		ret = build_redirect_to_home_response(http, nvram);
	}
	return ret;
}
