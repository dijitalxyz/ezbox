/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg-soap_http.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-09-07   0.1       Write it from scratch
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

#include "libezcfg.h"
#include "libezcfg-private.h"

struct ezcfg_soap_http {
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_soap *soap;
};

/* for SOAP/HTTP binding request methods */
static const char *soap_http_method_strings[] = {
	/* bad method string */
	NULL ,
	/* SOAP/HTTP binding used methods */
	EZCFG_SOAP_HTTP_METHOD_GET ,
	EZCFG_SOAP_HTTP_METHOD_POST ,
};

/* for SOAP/HTTP binding known header */
static const char *soap_http_header_strings[] = {
	/* bad header string */
	NULL ,
	/* SOAP/HTTP binding known headers */
	EZCFG_SOAP_HTTP_HEADER_HOST ,
	EZCFG_SOAP_HTTP_HEADER_CONTENT_TYPE ,
	EZCFG_SOAP_HTTP_HEADER_CONTENT_LENGTH ,
};

/**
 * Public functions
 **/
void ezcfg_soap_http_delete(struct ezcfg_soap_http *sh)
{
	struct ezcfg *ezcfg;

	ASSERT(sh != NULL);

	ezcfg = sh->ezcfg;

	if (sh->soap != NULL)
		ezcfg_soap_delete(sh->soap);

	if (sh->http != NULL)
		ezcfg_http_delete(sh->http);

	free(sh);
}

/**
 * ezcfg_soap_http_new:
 * Create ezcfg SOAP/HTTP info builder data structure
 * Returns: a new ezcfg soap http binding info builder data structure
 **/
struct ezcfg_soap_http *ezcfg_soap_http_new(struct ezcfg *ezcfg)
{
	struct ezcfg_soap_http *sh;

	ASSERT(ezcfg != NULL);

	/* initialize soap http binding info builder data structure */
	sh = calloc(1, sizeof(struct ezcfg_soap_http));
	if (sh == NULL) {
		err(ezcfg, "initialize soap http binding builder error.\n");
		return NULL;
	}

	memset(sh, 0, sizeof(struct ezcfg_soap_http));

	sh->soap = ezcfg_soap_new(ezcfg);
	if (sh->soap == NULL) {
		ezcfg_soap_http_delete(sh);
		return NULL;
	}

	sh->http = ezcfg_http_new(ezcfg);
	if (sh->http == NULL) {
		ezcfg_soap_http_delete(sh);
		return NULL;
	}

	sh->ezcfg = ezcfg;
	ezcfg_http_set_method_strings(sh->http, soap_http_method_strings, ARRAY_SIZE(soap_http_method_strings) - 1);
	ezcfg_http_set_known_header_strings(sh->http, soap_http_header_strings, ARRAY_SIZE(soap_http_header_strings) - 1);

	return sh;
}

unsigned short ezcfg_soap_http_get_soap_version_major(struct ezcfg_soap_http *sh)
{
	struct ezcfg *ezcfg;
	struct ezcfg_soap *soap;

	ASSERT(sh != NULL);
	ASSERT(sh->soap != NULL);

	ezcfg = sh->ezcfg;
	soap = sh->soap;

	return ezcfg_soap_get_version_major(soap);
}

unsigned short ezcfg_soap_http_get_soap_version_minor(struct ezcfg_soap_http *sh)
{
	struct ezcfg *ezcfg;
	struct ezcfg_soap *soap;

	ASSERT(sh != NULL);
	ASSERT(sh->soap != NULL);

	ezcfg = sh->ezcfg;
	soap = sh->soap;

	return ezcfg_soap_get_version_minor(soap);
}

bool ezcfg_soap_http_set_soap_version_major(struct ezcfg_soap_http *sh, unsigned short major)
{
	struct ezcfg *ezcfg;

	ASSERT(sh != NULL);
	ASSERT(sh->soap != NULL);

	ezcfg = sh->ezcfg;

	return ezcfg_soap_set_version_major(sh->soap, major);
}

bool ezcfg_soap_http_set_soap_version_minor(struct ezcfg_soap_http *sh, unsigned short minor)
{
	struct ezcfg *ezcfg;

	ASSERT(sh != NULL);
	ASSERT(sh->soap != NULL);

	ezcfg = sh->ezcfg;

	return ezcfg_soap_set_version_minor(sh->soap, minor);
}

struct ezcfg_soap *ezcfg_soap_http_get_soap(struct ezcfg_soap_http *sh)
{
	struct ezcfg *ezcfg;

	ASSERT(sh != NULL);
	ASSERT(sh->soap != NULL);

	ezcfg = sh->ezcfg;

	return sh->soap;
}

struct ezcfg_http *ezcfg_soap_http_get_http(struct ezcfg_soap_http *sh)
{
	struct ezcfg *ezcfg;

	ASSERT(sh != NULL);
	ASSERT(sh->http != NULL);

	ezcfg = sh->ezcfg;

	return sh->http;
}

char *ezcfg_soap_http_get_http_header_value(struct ezcfg_soap_http *sh, char *name)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;

	ASSERT(sh != NULL);
	ASSERT(sh->http != NULL);

	ezcfg = sh->ezcfg;
	http = sh->http;

        return ezcfg_http_get_header_value(http, name);
}

void ezcfg_soap_http_reset_attributes(struct ezcfg_soap_http *sh)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_soap *soap;

	ASSERT(sh != NULL);
	ASSERT(sh->http != NULL);
	ASSERT(sh->soap != NULL);

	ezcfg = sh->ezcfg;
	http = sh->http;
	soap = sh->soap;

	ezcfg_http_reset_attributes(http);
	//ezcfg_soap_reset_attributes(soap);
}

void ezcfg_soap_http_dump(struct ezcfg_soap_http *sh)
{
	struct ezcfg *ezcfg;

	ASSERT(sh != NULL);

	ezcfg = sh->ezcfg;
}

bool ezcfg_soap_http_parse_request(struct ezcfg_soap_http *sh, char *buf)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;

	ASSERT(sh != NULL);
	ASSERT(sh->http != NULL);

	ezcfg = sh->ezcfg;
	http = sh->http;

	if (ezcfg_http_parse_request(http, buf) == false) {
		return false;
	}

	return true;
}

char *ezcfg_soap_http_set_message_body(struct ezcfg_soap_http *sh, const char *body, int len)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;

	ASSERT(sh != NULL);
	ASSERT(sh->http != NULL);

	ezcfg = sh->ezcfg;
	http = sh->http;

	return ezcfg_http_set_message_body(http, body, len);
}

int ezcfg_soap_http_write_message(struct ezcfg_soap_http *sh, char *buf, int len)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_soap *soap;

	char *p;
	int n;

	ASSERT(sh != NULL);
	ASSERT(sh->http != NULL);
	ASSERT(sh->soap != NULL);
	ASSERT(buf != NULL);
	ASSERT(len > 0);

	ezcfg = sh->ezcfg;
	http = sh->http;
	soap = sh->soap;

	p = buf; n = 0;
	n = ezcfg_http_write_request_line(http, p, len);
	if (n < 0) {
		err(ezcfg, "ezcfg_http_write_request_line\n");
		return n;
	}
	p += n; len -= n;

	n = ezcfg_http_write_headers(http, p, len);
	if (n < 0) {
		err(ezcfg, "ezcfg_http_write_headers\n");
		return n;
	}
	p += n; len -= n;

	n = ezcfg_http_write_crlf(http, p, len);
	if (n < 0) {
		err(ezcfg, "ezcfg_http_write_crlf\n");
		return n;
	}
	p += n; len -= n;

	n = ezcfg_http_write_message_body(http, p, len);
	if (n < 0) {
		err(ezcfg, "ezcfg_http_write_message_body\n");
		return n;
	}

	return (p-buf)+n;
}

