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

	ASSERT(sh != NULL);
	ASSERT(sh->soap != NULL);

	ezcfg = sh->ezcfg;

	return ezcfg_soap_get_version_major(sh->soap);
}

unsigned short ezcfg_soap_http_get_soap_version_minor(struct ezcfg_soap_http *sh)
{
	struct ezcfg *ezcfg;

	ASSERT(sh != NULL);
	ASSERT(sh->soap != NULL);

	ezcfg = sh->ezcfg;

	return ezcfg_soap_get_version_minor(sh->soap);
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

unsigned short ezcfg_soap_http_get_http_version_major(struct ezcfg_soap_http *sh)
{
	struct ezcfg *ezcfg;

	ASSERT(sh != NULL);
	ASSERT(sh->http != NULL);

	ezcfg = sh->ezcfg;

	return ezcfg_http_get_version_major(sh->http);
}

unsigned short ezcfg_soap_http_get_http_version_minor(struct ezcfg_soap_http *sh)
{
	struct ezcfg *ezcfg;

	ASSERT(sh != NULL);
	ASSERT(sh->http != NULL);

	ezcfg = sh->ezcfg;

	return ezcfg_http_get_version_minor(sh->http);
}

bool ezcfg_soap_http_set_http_version_major(struct ezcfg_soap_http *sh, unsigned short major)
{
	struct ezcfg *ezcfg;

	ASSERT(sh != NULL);
	ASSERT(sh->http != NULL);

	ezcfg = sh->ezcfg;

	return ezcfg_http_set_version_major(sh->http, major);
}

bool ezcfg_soap_http_set_http_version_minor(struct ezcfg_soap_http *sh, unsigned short minor)
{
	struct ezcfg *ezcfg;

	ASSERT(sh != NULL);
	ASSERT(sh->http != NULL);

	ezcfg = sh->ezcfg;

	return ezcfg_http_set_version_minor(sh->http, minor);
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

	ASSERT(sh != NULL);
	ASSERT(sh->http != NULL);

	ezcfg = sh->ezcfg;

        return ezcfg_http_get_header_value(sh->http, name);
}

void ezcfg_soap_http_reset_attributes(struct ezcfg_soap_http *sh)
{
	struct ezcfg *ezcfg;

	ASSERT(sh != NULL);
	ASSERT(sh->http != NULL);
	ASSERT(sh->soap != NULL);

	ezcfg = sh->ezcfg;

	ezcfg_http_reset_attributes(sh->http);
	ezcfg_soap_reset_attributes(sh->soap);
}

void ezcfg_soap_http_dump(struct ezcfg_soap_http *sh)
{
	struct ezcfg *ezcfg;

	ASSERT(sh != NULL);

	ezcfg = sh->ezcfg;
}

bool ezcfg_soap_http_parse_request(struct ezcfg_soap_http *sh, char *buf, int len)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_soap *soap;
	char *msg_body;
	int msg_body_len;

	ASSERT(sh != NULL);
	ASSERT(sh->http != NULL);
	ASSERT(sh->soap != NULL);

	ezcfg = sh->ezcfg;
	http = sh->http;
	soap = sh->soap;

	if (ezcfg_http_parse_request(http, buf, len) == false) {
		return false;
	}

	msg_body = ezcfg_http_get_message_body(http);
	msg_body_len = ezcfg_http_get_message_body_len(http);

	if (msg_body != NULL && msg_body_len > 0) {
		if (ezcfg_soap_parse_request(soap, msg_body, msg_body_len) == false) {
			return false;
		}
	}

	return true;
}

char *ezcfg_soap_http_set_http_message_body(struct ezcfg_soap_http *sh, const char *body, int len)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;

	ASSERT(sh != NULL);
	ASSERT(sh->http != NULL);

	ezcfg = sh->ezcfg;
	http = sh->http;

	return ezcfg_http_set_message_body(http, body, len);
}

int ezcfg_soap_http_write_message(struct ezcfg_soap_http *sh, char *buf, int len, int mode)
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
	if (mode == EZCFG_SOAP_HTTP_MODE_REQUEST) {
		n = ezcfg_http_write_request_line(http, p, len);
		if (n < 0) {
			err(ezcfg, "ezcfg_http_write_request_line\n");
			return n;
		}
	}
	else {
		n = ezcfg_http_write_status_line(http, p, len);
		if (n < 0) {
			err(ezcfg, "ezcfg_http_write_status_line\n");
			return n;
		}
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

	if (ezcfg_http_get_message_body(http) != NULL) {
		n = ezcfg_http_write_message_body(http, p, len);
		if (n < 0) {
			err(ezcfg, "ezcfg_http_write_message_body\n");
			return n;
		}
		p += n; len -= n;
	}

	return (p-buf);
}

static void build_nvram_get_response(struct ezcfg_soap_http *sh, const char *name, const char *value)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_soap *soap;
	char buf[EZCFG_SOAP_HTTP_MAX_RESPONSE_SIZE];
	int body_index, child_index;
	int n;
	
	ASSERT(sh != NULL);
	ASSERT(sh->http != NULL);
	ASSERT(sh->soap != NULL);
	ASSERT(name != NULL);

	ezcfg = sh->ezcfg;
	http = sh->http;
	soap = sh->soap;

	if (value != NULL) {
		int getnv_index;

		/* clean SOAP structure info */
		ezcfg_soap_reset_attributes(soap);

		/* build SOAP */
		ezcfg_soap_set_version_major(soap, 1);
		ezcfg_soap_set_version_minor(soap, 2);

		/* SOAP Envelope */
		ezcfg_soap_set_envelope(soap, EZCFG_SOAP_ENVELOPE_ELEMENT_NAME);
		ezcfg_soap_add_envelope_attribute(soap, EZCFG_SOAP_ENVELOPE_ATTR_NS_NAME, EZCFG_SOAP_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

		/* SOAP Body */
		body_index = ezcfg_soap_set_body(soap, EZCFG_SOAP_BODY_ELEMENT_NAME);

		/* Body child getNvram part */
		getnv_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_SOAP_NVRAM_GETNV_RESPONSE_ELEMENT_NAME, NULL);
		ezcfg_soap_add_body_child_attribute(soap, getnv_index, EZCFG_SOAP_NVRAM_ATTR_NS_NAME, EZCFG_SOAP_NVRAM_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

		/* nvram name part */
		child_index = ezcfg_soap_add_body_child(soap, getnv_index, -1, EZCFG_SOAP_NVRAM_NAME_ELEMENT_NAME, name);
		buf[0] = '\0';
		n = ezcfg_soap_write(soap, buf, sizeof(buf));

		/* nvram value part */
		child_index = ezcfg_soap_add_body_child(soap, getnv_index, -1, EZCFG_SOAP_NVRAM_VALUE_ELEMENT_NAME, value);

		snprintf(buf, sizeof(buf), "%s\n", "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
		n = strlen(buf);
		n += ezcfg_soap_write(soap, buf + n, sizeof(buf) - n);

		/* FIXME: name point to http->request_uri !!!
	         * never reset http before using name */
		/* clean http structure info */
		ezcfg_http_reset_attributes(http);
		ezcfg_http_set_status_code(http, 200);

		ezcfg_http_set_message_body(http, buf, n);

		snprintf(buf, sizeof(buf), "%s", "application/soap+xml; charset=\"utf-8\"");
		ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_TYPE , buf);

		snprintf(buf, sizeof(buf), "%u", ezcfg_http_get_message_body_len(http));
		ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_LENGTH , buf);
	}
	else {
		int fault_index;
		int code_index;
		int reason_index;

		err(ezcfg, "no nvram [%s]\n", name);

		/* clean SOAP structure info */
		ezcfg_soap_reset_attributes(soap);

		/* build SOAP */
		ezcfg_soap_set_version_major(soap, 1);
		ezcfg_soap_set_version_minor(soap, 2);

		/* SOAP Envelope */
		ezcfg_soap_set_envelope(soap, EZCFG_SOAP_ENVELOPE_ELEMENT_NAME);
		ezcfg_soap_add_envelope_attribute(soap, EZCFG_SOAP_ENVELOPE_ATTR_NS_NAME, EZCFG_SOAP_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

		/* SOAP Body */
		body_index = ezcfg_soap_set_body(soap, EZCFG_SOAP_BODY_ELEMENT_NAME);

		/* SOAP Fault part */
		fault_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_SOAP_FAULT_ELEMENT_NAME, NULL);

		/* SOAP Fault Code part */
		code_index = ezcfg_soap_add_body_child(soap, fault_index, -1, EZCFG_SOAP_CODE_ELEMENT_NAME, NULL);

		/* SOAP Fault Code value part */
		child_index = ezcfg_soap_add_body_child(soap, code_index, -1, EZCFG_SOAP_VALUE_ELEMENT_NAME, EZCFG_SOAP_VALUE_ELEMENT_VALUE);

		/* SOAP Fault Reason part */
		reason_index = ezcfg_soap_add_body_child(soap, fault_index, -1, EZCFG_SOAP_REASON_ELEMENT_NAME, NULL);

		/* SOAP Fault Reason Text part */
		child_index = ezcfg_soap_add_body_child(soap, reason_index, -1, EZCFG_SOAP_TEXT_ELEMENT_NAME, EZCFG_SOAP_NVRAM_INVALID_NAME_FAULT_VALUE);

		snprintf(buf, sizeof(buf), "%s\n", "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
		n = strlen(buf);
		n += ezcfg_soap_write(soap, buf + n, sizeof(buf) - n);

		/* FIXME: name point to http->request_uri !!!
	         * never reset http before using name */
		/* clean http structure info */
		ezcfg_http_reset_attributes(http);
		ezcfg_http_set_status_code(http, 200);

		ezcfg_http_set_message_body(http, buf, n);

		snprintf(buf, sizeof(buf), "%s", "application/soap+xml; charset=\"utf-8\"");
		ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_TYPE , buf);

		snprintf(buf, sizeof(buf), "%u", ezcfg_http_get_message_body_len(http));
		ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_LENGTH , buf);
	}
}

static void build_nvram_set_response(struct ezcfg_soap_http *sh, const char *name, const char *result)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_soap *soap;
	char buf[EZCFG_SOAP_HTTP_MAX_RESPONSE_SIZE];
	int body_index, child_index;
	int n;
	
	ASSERT(sh != NULL);
	ASSERT(sh->http != NULL);
	ASSERT(sh->soap != NULL);

	ezcfg = sh->ezcfg;
	http = sh->http;
	soap = sh->soap;

	if (result != NULL) {
		int setnv_index;

		/* clean SOAP structure info */
		ezcfg_soap_reset_attributes(soap);

		/* build SOAP */
		ezcfg_soap_set_version_major(soap, 1);
		ezcfg_soap_set_version_minor(soap, 2);

		/* SOAP Envelope */
		ezcfg_soap_set_envelope(soap, EZCFG_SOAP_ENVELOPE_ELEMENT_NAME);
		ezcfg_soap_add_envelope_attribute(soap, EZCFG_SOAP_ENVELOPE_ATTR_NS_NAME, EZCFG_SOAP_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

		/* SOAP Body */
		body_index = ezcfg_soap_set_body(soap, EZCFG_SOAP_BODY_ELEMENT_NAME);

		/* Body child unsetNvram part */
		setnv_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_SOAP_NVRAM_SETNV_RESPONSE_ELEMENT_NAME, NULL);
		ezcfg_soap_add_body_child_attribute(soap, setnv_index, EZCFG_SOAP_NVRAM_ATTR_NS_NAME, EZCFG_SOAP_NVRAM_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

		/* nvram result part */
		child_index = ezcfg_soap_add_body_child(soap, setnv_index, -1, EZCFG_SOAP_NVRAM_RESULT_ELEMENT_NAME, result);

		snprintf(buf, sizeof(buf), "%s\n", "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
		n = strlen(buf);
		n += ezcfg_soap_write(soap, buf + n, sizeof(buf) - n);

		/* FIXME: name point to http->request_uri !!!
	         * never reset http before using name */
		/* clean http structure info */
		ezcfg_http_reset_attributes(http);
		ezcfg_http_set_status_code(http, 200);

		ezcfg_http_set_message_body(http, buf, n);

		snprintf(buf, sizeof(buf), "%s", "application/soap+xml; charset=\"utf-8\"");
		ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_TYPE , buf);

		snprintf(buf, sizeof(buf), "%u", ezcfg_http_get_message_body_len(http));
		ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_LENGTH , buf);
	}
	else {
		int fault_index;
		int code_index;
		int reason_index;

		err(ezcfg, "no nvram [%s]\n", name);

		/* clean SOAP structure info */
		ezcfg_soap_reset_attributes(soap);

		/* build SOAP */
		ezcfg_soap_set_version_major(soap, 1);
		ezcfg_soap_set_version_minor(soap, 2);

		/* SOAP Envelope */
		ezcfg_soap_set_envelope(soap, EZCFG_SOAP_ENVELOPE_ELEMENT_NAME);
		ezcfg_soap_add_envelope_attribute(soap, EZCFG_SOAP_ENVELOPE_ATTR_NS_NAME, EZCFG_SOAP_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

		/* SOAP Body */
		body_index = ezcfg_soap_set_body(soap, EZCFG_SOAP_BODY_ELEMENT_NAME);

		/* SOAP Fault part */
		fault_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_SOAP_FAULT_ELEMENT_NAME, NULL);

		/* SOAP Fault Code part */
		code_index = ezcfg_soap_add_body_child(soap, fault_index, -1, EZCFG_SOAP_CODE_ELEMENT_NAME, NULL);

		/* SOAP Fault Code value part */
		child_index = ezcfg_soap_add_body_child(soap, code_index, -1, EZCFG_SOAP_VALUE_ELEMENT_NAME, EZCFG_SOAP_VALUE_ELEMENT_VALUE);

		/* SOAP Fault Reason part */
		reason_index = ezcfg_soap_add_body_child(soap, fault_index, -1, EZCFG_SOAP_REASON_ELEMENT_NAME, NULL);

		/* SOAP Fault Reason Text part */
		child_index = ezcfg_soap_add_body_child(soap, reason_index, -1, EZCFG_SOAP_TEXT_ELEMENT_NAME, EZCFG_SOAP_NVRAM_INVALID_VALUE_FAULT_VALUE);

		snprintf(buf, sizeof(buf), "%s\n", "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
		n = strlen(buf);
		n += ezcfg_soap_write(soap, buf + n, sizeof(buf) - n);

		/* FIXME: name point to http->request_uri !!!
	         * never reset http before using name */
		/* clean http structure info */
		ezcfg_http_reset_attributes(http);
		ezcfg_http_set_status_code(http, 200);

		ezcfg_http_set_message_body(http, buf, n);

		snprintf(buf, sizeof(buf), "%s", "application/soap+xml; charset=\"utf-8\"");
		ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_TYPE , buf);

		snprintf(buf, sizeof(buf), "%u", ezcfg_http_get_message_body_len(http));
		ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_LENGTH , buf);
	}
}

static void build_nvram_unset_response(struct ezcfg_soap_http *sh, const char *name, const char *result)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_soap *soap;
	char buf[EZCFG_SOAP_HTTP_MAX_RESPONSE_SIZE];
	int body_index, child_index;
	int n;
	
	ASSERT(sh != NULL);
	ASSERT(sh->http != NULL);
	ASSERT(sh->soap != NULL);
	ASSERT(name != NULL);

	ezcfg = sh->ezcfg;
	http = sh->http;
	soap = sh->soap;

	if (result != NULL) {
		int unsetnv_index;

		/* clean SOAP structure info */
		ezcfg_soap_reset_attributes(soap);

		/* build SOAP */
		ezcfg_soap_set_version_major(soap, 1);
		ezcfg_soap_set_version_minor(soap, 2);

		/* SOAP Envelope */
		ezcfg_soap_set_envelope(soap, EZCFG_SOAP_ENVELOPE_ELEMENT_NAME);
		ezcfg_soap_add_envelope_attribute(soap, EZCFG_SOAP_ENVELOPE_ATTR_NS_NAME, EZCFG_SOAP_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

		/* SOAP Body */
		body_index = ezcfg_soap_set_body(soap, EZCFG_SOAP_BODY_ELEMENT_NAME);

		/* Body child unsetNvram part */
		unsetnv_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_SOAP_NVRAM_UNSETNV_RESPONSE_ELEMENT_NAME, NULL);
		ezcfg_soap_add_body_child_attribute(soap, unsetnv_index, EZCFG_SOAP_NVRAM_ATTR_NS_NAME, EZCFG_SOAP_NVRAM_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

		/* nvram result part */
		child_index = ezcfg_soap_add_body_child(soap, unsetnv_index, -1, EZCFG_SOAP_NVRAM_RESULT_ELEMENT_NAME, result);

		snprintf(buf, sizeof(buf), "%s\n", "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
		n = strlen(buf);
		n += ezcfg_soap_write(soap, buf + n, sizeof(buf) - n);

		/* FIXME: name point to http->request_uri !!!
	         * never reset http before using name */
		/* clean http structure info */
		ezcfg_http_reset_attributes(http);
		ezcfg_http_set_status_code(http, 200);

		ezcfg_http_set_message_body(http, buf, n);

		snprintf(buf, sizeof(buf), "%s", "application/soap+xml; charset=\"utf-8\"");
		ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_TYPE , buf);

		snprintf(buf, sizeof(buf), "%u", ezcfg_http_get_message_body_len(http));
		ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_LENGTH , buf);
	}
	else {
		int fault_index;
		int code_index;
		int reason_index;

		err(ezcfg, "no nvram [%s]\n", name);

		/* clean SOAP structure info */
		ezcfg_soap_reset_attributes(soap);

		/* build SOAP */
		ezcfg_soap_set_version_major(soap, 1);
		ezcfg_soap_set_version_minor(soap, 2);

		/* SOAP Envelope */
		ezcfg_soap_set_envelope(soap, EZCFG_SOAP_ENVELOPE_ELEMENT_NAME);
		ezcfg_soap_add_envelope_attribute(soap, EZCFG_SOAP_ENVELOPE_ATTR_NS_NAME, EZCFG_SOAP_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

		/* SOAP Body */
		body_index = ezcfg_soap_set_body(soap, EZCFG_SOAP_BODY_ELEMENT_NAME);

		/* SOAP Fault part */
		fault_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_SOAP_FAULT_ELEMENT_NAME, NULL);

		/* SOAP Fault Code part */
		code_index = ezcfg_soap_add_body_child(soap, fault_index, -1, EZCFG_SOAP_CODE_ELEMENT_NAME, NULL);

		/* SOAP Fault Code value part */
		child_index = ezcfg_soap_add_body_child(soap, code_index, -1, EZCFG_SOAP_VALUE_ELEMENT_NAME, EZCFG_SOAP_VALUE_ELEMENT_VALUE);

		/* SOAP Fault Reason part */
		reason_index = ezcfg_soap_add_body_child(soap, fault_index, -1, EZCFG_SOAP_REASON_ELEMENT_NAME, NULL);

		/* SOAP Fault Reason Text part */
		child_index = ezcfg_soap_add_body_child(soap, reason_index, -1, EZCFG_SOAP_TEXT_ELEMENT_NAME, EZCFG_SOAP_NVRAM_INVALID_NAME_FAULT_VALUE);

		snprintf(buf, sizeof(buf), "%s\n", "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
		n = strlen(buf);
		n += ezcfg_soap_write(soap, buf + n, sizeof(buf) - n);

		/* FIXME: name point to http->request_uri !!!
	         * never reset http before using name */
		/* clean http structure info */
		ezcfg_http_reset_attributes(http);
		ezcfg_http_set_status_code(http, 200);

		ezcfg_http_set_message_body(http, buf, n);

		snprintf(buf, sizeof(buf), "%s", "application/soap+xml; charset=\"utf-8\"");
		ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_TYPE , buf);

		snprintf(buf, sizeof(buf), "%u", ezcfg_http_get_message_body_len(http));
		ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_CONTENT_LENGTH , buf);
	}
}

void ezcfg_soap_http_handle_nvram_request(struct ezcfg_soap_http *sh, struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_soap *soap;
	char *request_uri;
	char *name, *value, *result;
	int body_index, child_index;

	ASSERT(sh != NULL);
	ASSERT(sh->http != NULL);
	ASSERT(sh->soap != NULL);
	ASSERT(nvram != NULL);

	ezcfg = sh->ezcfg;
	http = sh->http;
	soap = sh->soap;
	result = NULL;

	request_uri = ezcfg_http_get_request_uri(http);

	if (strncmp(request_uri, EZCFG_SOAP_HTTP_NVRAM_GET_URI, strlen(EZCFG_SOAP_HTTP_NVRAM_GET_URI)) == 0) {
		/* nvram get uri=[/ezcfg/nvram/soap-http/getNvram?name=xxx] */
		name = request_uri + strlen(EZCFG_SOAP_HTTP_NVRAM_GET_URI) + 6;

		/* get nvram node value, must release value!!! */
		value = ezcfg_nvram_get_node_value(nvram, name);

		build_nvram_get_response(sh, name, value);

		if (value != NULL) {
			free(value);
		}
	}
	else if (strcmp(request_uri, EZCFG_SOAP_HTTP_NVRAM_SET_URI) == 0) {
		/* nvram get uri=[/ezcfg/nvram/soap-http/setNvram] */

		int setnv_index;

		/* get setNvram part */
		body_index = ezcfg_soap_get_body_index(soap);
		setnv_index = ezcfg_soap_get_element_index(soap, body_index, EZCFG_SOAP_NVRAM_SETNV_ELEMENT_NAME);

		/* get nvram node name */
		child_index = ezcfg_soap_get_element_index(soap, setnv_index, EZCFG_SOAP_NVRAM_NAME_ELEMENT_NAME);
		name = ezcfg_soap_get_element_content_by_index(soap, child_index);
		
		/* get nvram node value */
		child_index = ezcfg_soap_get_element_index(soap, setnv_index, EZCFG_SOAP_NVRAM_VALUE_ELEMENT_NAME);
		value = ezcfg_soap_get_element_content_by_index(soap, child_index);

		if (name != NULL && value != NULL) {
			dbg(ezcfg, "name=[%s], value=[%s]\n", name, value);
			if (ezcfg_nvram_set_node_value(nvram, name, value) == true) {
				result = EZCFG_SOAP_NVRAM_RESULT_VALUE_OK;
			}
			else {
				result = EZCFG_SOAP_NVRAM_RESULT_VALUE_FAIL;
			}
		}

		build_nvram_set_response(sh, name, result);
	}
	else if (strncmp(request_uri, EZCFG_SOAP_HTTP_NVRAM_UNSET_URI, strlen(EZCFG_SOAP_HTTP_NVRAM_UNSET_URI)) == 0) {
		/* nvram get uri=[/ezcfg/nvram/soap-http/unsetNvram?name=xxx] */
		name = request_uri + strlen(EZCFG_SOAP_HTTP_NVRAM_UNSET_URI) + 6;
		
		/* get nvram node value, must release value!!! */
		value = ezcfg_nvram_get_node_value(nvram, name);

		if (value != NULL) {
			ezcfg_nvram_unset_node_value(nvram, name);
			result = EZCFG_SOAP_NVRAM_RESULT_VALUE_OK;
		}

		build_nvram_unset_response(sh, name, result);

		if (value != NULL) {
			free(value);
		}
	}
}
