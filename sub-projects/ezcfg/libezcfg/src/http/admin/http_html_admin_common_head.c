/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/admin/http_html_admin_common_head.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-12   0.1       Write it from scratch
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

#include <locale.h>
#include <libintl.h>

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-http.h"
#include "ezcfg-html.h"

/**
 * Private functions
 **/

/**
 * Public functions
 **/

int ezcfg_http_html_admin_set_html_common_head(struct ezcfg_html *html, int pi, int si)
{
	struct ezcfg *ezcfg;
	int child_index;
	char buf[1024];

	ASSERT(html != NULL);
	ASSERT(pi > 0);

	ezcfg = html->ezcfg;

	/* HTML Meta charset */
	child_index = ezcfg_html_add_head_child(html, pi, si, EZCFG_HTML_META_ELEMENT_NAME, NULL);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child err.\n");
		goto func_exit;
	}
	/* HTML http-equiv content-type */
	ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_HTTP_EQUIV_ATTRIBUTE_NAME, EZCFG_HTTP_HEADER_CONTENT_TYPE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	snprintf(buf, sizeof(buf), "%s; %s=%s", EZCFG_HTTP_MIME_TEXT_HTML, EZCFG_HTTP_CHARSET_NAME, EZCFG_HTTP_CHARSET_UTF8);
	ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_CONTENT_ATTRIBUTE_NAME, buf, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* HTML Meta Cache-control */
	child_index = ezcfg_html_add_head_child(html, pi, child_index, EZCFG_HTML_META_ELEMENT_NAME, NULL);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child err.\n");
		goto func_exit;
	}
	/* HTML http-equiv cache-control */
	ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_HTTP_EQUIV_ATTRIBUTE_NAME, EZCFG_HTTP_HEADER_CACHE_CONTROL, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_CONTENT_ATTRIBUTE_NAME, EZCFG_HTTP_CACHE_REQUEST_NO_CACHE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* HTML Meta expires */
	child_index = ezcfg_html_add_head_child(html, pi, child_index, EZCFG_HTML_META_ELEMENT_NAME, NULL);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child err.\n");
		goto func_exit;
	}
	/* HTML http-equiv expires */
	ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_HTTP_EQUIV_ATTRIBUTE_NAME, EZCFG_HTTP_HEADER_EXPIRES, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_CONTENT_ATTRIBUTE_NAME, "0", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* HTML Meta pragma */
	child_index = ezcfg_html_add_head_child(html, pi, child_index, EZCFG_HTML_META_ELEMENT_NAME, NULL);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child err.\n");
		goto func_exit;
	}
	/* HTML http-equiv pragma */
	ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_HTTP_EQUIV_ATTRIBUTE_NAME, EZCFG_HTTP_HEADER_PRAGMA, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_head_child_attribute(html, child_index, EZCFG_HTML_CONTENT_ATTRIBUTE_NAME, EZCFG_HTTP_PRAGMA_NO_CACHE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

func_exit:
	return child_index;
}

bool ezcfg_http_html_admin_set_http_common_header(struct ezcfg_http *http)
{
	struct ezcfg *ezcfg;
	char buf[1024];
	bool ret;

	ASSERT(http);

	ezcfg = http->ezcfg;

	/* HTML http-equiv content-type */
	snprintf(buf, sizeof(buf), "%s; %s=%s", EZCFG_HTTP_MIME_TEXT_HTML, EZCFG_HTTP_CHARSET_NAME, EZCFG_HTTP_CHARSET_UTF8);
	ret = ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CONTENT_TYPE, buf);
	if (ret == false) {
		return false;
	}

	/* HTML http-equiv cache-control */
	ret = ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CACHE_CONTROL, EZCFG_HTTP_CACHE_REQUEST_NO_CACHE);
	if (ret == false) {
		goto func_exit;
	}

	/* HTML http-equiv expires */
	ret = ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_EXPIRES, "0");
	if (ret == false) {
		goto func_exit;
	}

	/* HTML http-equiv pragma */
	ret = ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_PRAGMA, EZCFG_HTTP_PRAGMA_NO_CACHE);
	if (ret == false) {
		goto func_exit;
	}

	snprintf(buf, sizeof(buf), "%u", ezcfg_http_get_message_body_len(http));
	ret = ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CONTENT_LENGTH , buf);
	if (ret == false) {
		goto func_exit;
	}

func_exit:
	return ret;
}

