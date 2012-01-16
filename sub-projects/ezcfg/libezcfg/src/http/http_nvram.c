/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/http_nvram.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2012-01-13   0.1       Write it from scratch
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

struct ezcfg_http_nvram {
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_nvram *nvram;
	int content_type;
	char *root;
	char *path;
};

/**
 * Private functions
 **/
static int build_http_nvram_response(struct ezcfg_http_nvram *hn)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_nvram *nvram;
	int rc = 0;
	
	ASSERT(hn != NULL);

	ezcfg = hn->ezcfg;
	http = hn->http;
	nvram = hn->nvram;

	/* FIXME: name point to http->request_uri !!!
         * never reset http before using name */
	/* clean http structure info */
	ezcfg_http_reset_attributes(http);
	ezcfg_http_set_status_code(http, 200);
	ezcfg_http_set_state_response(http);

	/* set return value */
	rc = 0;
	return rc;
}

/**
 * Public functions
 **/
void ezcfg_http_nvram_delete(struct ezcfg_http_nvram *hn)
{
	ASSERT(hn != NULL);

	if (hn->root != NULL)
		free(hn->root);

	if (hn->path != NULL)
		free(hn->path);

	free(hn);
}

struct ezcfg_http_nvram *ezcfg_http_nvram_new(struct ezcfg *ezcfg)
{
	struct ezcfg_http_nvram *hn;

	ASSERT(ezcfg != NULL);

	hn = malloc(sizeof(struct ezcfg_http_nvram));
	if (hn != NULL) {
		memset(hn, 0, sizeof(struct ezcfg_http_nvram));
		hn->ezcfg = ezcfg;
        }
	return hn;
}

bool ezcfg_http_nvram_set_http(struct ezcfg_http_nvram *hn, struct ezcfg_http *http)
{
	ASSERT(hn != NULL);
	ASSERT(http != NULL);

	hn->http = http;
	return true;
}

bool ezcfg_http_nvram_set_nvram(struct ezcfg_http_nvram *hn, struct ezcfg_nvram *nvram)
{
	ASSERT(hn != NULL);
	ASSERT(nvram != NULL);

	hn->nvram = nvram;
	return true;
}

int ezcfg_http_nvram_get_content_type(struct ezcfg_http_nvram *hn)
{
	ASSERT(hn != NULL);

	return hn->content_type;
}

bool ezcfg_http_nvram_set_content_type(struct ezcfg_http_nvram *hn, const int type)
{
	ASSERT(hn != NULL);
	ASSERT(type >= 0);

	hn->content_type = type;
	return true;
}

bool ezcfg_http_nvram_set_root(struct ezcfg_http_nvram *hn, const char *root)
{
	char *p;
	ASSERT(hn != NULL);
	ASSERT(root != NULL);

	p = strdup(root);
	if (p == NULL)
		return false;

	if (hn->root != NULL)
		free(hn->root);

	hn->root = p;
	return true;
}

bool ezcfg_http_nvram_set_path(struct ezcfg_http_nvram *hn, const char *path)
{
	char *p;
	ASSERT(hn != NULL);
	ASSERT(path != NULL);

	p = strdup(path);
	if (p == NULL)
		return false;

	if (hn->path != NULL)
		free(hn->path);

	hn->path = p;
	return true;
}

int ezcfg_http_handle_nvram_request(struct ezcfg_http_nvram *hn)
{
	struct ezcfg *ezcfg;

	ASSERT(hn != NULL);
	ASSERT(hn->http != NULL);
	ASSERT(hn->nvram != NULL);

	ezcfg = hn->ezcfg;

	return build_http_nvram_response(hn);
}
