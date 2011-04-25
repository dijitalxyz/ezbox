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
#include "ezcfg-http_html_admin.h"

struct http_html_admin_handler {
	char *section;
	int (*handler)(struct ezcfg_http_html_admin *admin);
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
	struct ezcfg_http_html_admin *admin = NULL;
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
				admin = ezcfg_http_html_admin_new(ezcfg, http, nvram);
				if (admin != NULL) {
					ret = ah->handler(admin);
				}
				break;
			}
			ah++;
		}
	}

	if (admin != NULL) {
		ezcfg_http_html_admin_delete(admin);
	}
	return ret;
}

struct ezcfg_http_html_admin *ezcfg_http_html_admin_new(
	struct ezcfg *ezcfg,
	struct ezcfg_http *http,
	struct ezcfg_nvram *nvram)
{
	struct ezcfg_http_html_admin *admin;

	ASSERT(ezcfg != NULL);
	ASSERT(http != NULL);
	ASSERT(nvram != NULL);

	admin = calloc(1, sizeof(struct ezcfg_http_html_admin));
	if (admin != NULL) {
		admin->ezcfg = ezcfg;
		admin->http = http;
		admin->nvram = nvram;
		admin->post_list = NULL;
	}
	return admin;
}

void ezcfg_http_html_admin_delete(struct ezcfg_http_html_admin *admin)
{
	ASSERT(admin != NULL);

	if (admin->post_list != NULL) {
		ezcfg_link_list_delete(admin->post_list);
	}

	free(admin);
}

bool ezcfg_http_html_admin_parse_post_data(struct ezcfg_http_html_admin *admin)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_link_list *list;

	ASSERT(admin != NULL);

	ezcfg = admin->ezcfg;
	http = admin->http;

	list = ezcfg_link_list_new(ezcfg);
	if (list == NULL) {
		return false;
	}
	admin->post_list = list;
	return ezcfg_http_parse_post_data(http, list);
}

bool ezcfg_http_html_admin_validate_post_data(struct ezcfg_http_html_admin *admin)
{
	struct ezcfg *ezcfg;
	struct ezcfg_link_list *list;
	struct ezcfg_nvram *nvram;
	int i;
	int list_length;
	char *name, *value;
	bool ret;

	ASSERT(admin != NULL);

	ezcfg = admin->ezcfg;
	nvram = admin->nvram;
	list = admin->post_list;

	list_length = ezcfg_link_list_get_length(list);
	for(i = 1; i < list_length+1; i++) {
		name = ezcfg_link_list_get_node_name_by_index(list, i);
		value = ezcfg_link_list_get_node_value_by_index(list, i);
		ret = ezcfg_nvram_is_valid_entry_value(nvram, name, value);
		if (ret == false) {
			return false;
		}
	}
	return true;
}

int ezcfg_http_html_admin_get_action(struct ezcfg_http_html_admin *admin)
{
	struct ezcfg *ezcfg;
	struct ezcfg_link_list *list;

	ASSERT(admin != NULL);

	ezcfg = admin->ezcfg;

	list = admin->post_list;

	if (list != NULL) {
		if (ezcfg_link_list_in(list, "act_save") == true)
			return HTTP_HTML_ADMIN_ACT_SAVE;
		else if (ezcfg_link_list_in(list, "act_refresh") == true)
			return HTTP_HTML_ADMIN_ACT_REFRESH;
		else
			return HTTP_HTML_ADMIN_ACT_CANCEL;
	}
	return HTTP_HTML_ADMIN_ACT_CANCEL;
}
