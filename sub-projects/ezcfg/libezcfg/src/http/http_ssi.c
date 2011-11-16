/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/http_ssi.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-16   0.1       Write it from scratch
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

struct ezcfg_http_ssi {
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_nvram *nvram;
	struct ezcfg_ssi *ssi;
};

/**
 * Private functions
 **/
static int build_http_ssi_response(struct ezcfg_http *http,
	struct ezcfg_nvram *nvram,
	struct ezcfg_ssi *ssi)
{
	struct ezcfg *ezcfg;
	char buf[1024];
	int rc = 0;
	
	ASSERT(http != NULL);
	ASSERT(nvram != NULL);
	ASSERT(ssi != NULL);

	ezcfg = http->ezcfg;

	/* FIXME: name point to http->request_uri !!!
         * never reset http before using name */
	/* clean http structure info */
	ezcfg_http_reset_attributes(http);
	ezcfg_http_set_status_code(http, 200);
	ezcfg_http_set_state_response(http);

	/* HTTP header content-type */
	snprintf(buf, sizeof(buf), "%s; %s=%s", EZCFG_HTTP_MIME_TEXT_HTML, EZCFG_HTTP_CHARSET_NAME, EZCFG_HTTP_CHARSET_UTF8);
	ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CONTENT_TYPE, buf);

	/* HTTP header cache-control */
	ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_CACHE_CONTROL, EZCFG_HTTP_CACHE_REQUEST_NO_CACHE);

	/* HTTP header expires */
	ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_EXPIRES, "0");

	/* HTTP header pragma */
	ezcfg_http_add_header(http, EZCFG_HTTP_HEADER_PRAGMA, EZCFG_HTTP_PRAGMA_NO_CACHE);

	/* set return value */
	rc = 0;
//func_exit:
	return rc;
}

/**
 * Public functions
 **/
void ezcfg_http_ssi_delete(struct ezcfg_http_ssi *hs)
{
	ASSERT(hs != NULL);

	free(hs);
}

struct ezcfg_http_ssi *ezcfg_http_ssi_new(
	struct ezcfg *ezcfg,
	struct ezcfg_http *http,
	struct ezcfg_nvram *nvram,
	struct ezcfg_ssi *ssi)
{
	struct ezcfg_http_ssi *hs;

	ASSERT(ezcfg != NULL);
	ASSERT(http != NULL);
	ASSERT(nvram != NULL);
	ASSERT(ssi != NULL);

	hs = calloc(1, sizeof(struct ezcfg_http_ssi));
	if (hs != NULL) {
		hs->ezcfg = ezcfg;
		hs->http = http;
		hs->nvram = nvram;
		hs->ssi = ssi;
        }
	return hs;
}

int ezcfg_http_handle_ssi_request(struct ezcfg_http *http,
	struct ezcfg_nvram *nvram,
	struct ezcfg_ssi *ssi)
{
	struct ezcfg *ezcfg;

	ASSERT(http != NULL);
	ASSERT(nvram != NULL);
	ASSERT(ssi != NULL);

	ezcfg = http->ezcfg;

	return build_http_ssi_response(http, nvram, ssi);
}
