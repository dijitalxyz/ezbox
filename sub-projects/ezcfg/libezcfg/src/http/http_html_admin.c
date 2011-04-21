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

#include <locale.h>
#include <libintl.h>

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-http.h"
#include "ezcfg-html.h"

struct http_html_admin_handler {
	char *section;
	int (*handler)(struct ezcfg_http *http, struct ezcfg_nvram *nvram);
};

struct http_html_admin_handler adm_handlers[] = {
	{ "status_system", ezcfg_http_html_admin_status_system_handler },
	{ "setup_system", ezcfg_http_html_admin_setup_system_handler },
	{ "layout_css", ezcfg_http_html_admin_layout_css_handler },
	{ NULL, NULL }
};

/**
 * Private functions
 **/

/**
 * Public functions
 **/

int ezcfg_http_handle_admin_request(struct ezcfg_http *http, struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;
	char *request_uri;
	char *section;
	struct http_html_admin_handler *ah;
	int ret = -1;

	ASSERT(http != NULL);
	ASSERT(nvram != NULL);

	ezcfg = http->ezcfg;

	request_uri = ezcfg_http_get_request_uri(http);

	if (strncmp(request_uri, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI, strlen(EZCFG_HTTP_HTML_ADMIN_PREFIX_URI)) == 0) {
		/* admin prefix uri=[/admin/] */
		section = request_uri+strlen(EZCFG_HTTP_HTML_ADMIN_PREFIX_URI);
		if (*section == '\0') {
			if (ezcfg_http_set_request_uri(http, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI "status_system") == true) {
				request_uri = ezcfg_http_get_request_uri(http);
				section = request_uri+strlen(EZCFG_HTTP_HTML_ADMIN_PREFIX_URI);
			}
		}
		ah = adm_handlers;
		while(ah->section != NULL) {
			if (strcmp(ah->section, section) == 0) {
				ret = ah->handler(http, nvram);
				break;
			}
			ah++;
		}
	}
	return ret;
}

