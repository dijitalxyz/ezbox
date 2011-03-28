/* ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : api-nvram.c
 *
 * Description  : ezcfg API for nvram manipulate
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-09-17   0.1       Write it from scratch
 * ============================================================================
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-soap_http.h"

#include "ezcfg-api.h"

static bool debug = false;

static void log_fn(struct ezcfg *ezcfg, int priority,
                   const char *file, int line, const char *fn,
                   const char *format, va_list args)
{
	if (debug) {
		char buf[1024];
		struct timeval tv;
		struct timezone tz;

		vsnprintf(buf, sizeof(buf), format, args);
		gettimeofday(&tv, &tz);
		fprintf(stderr, "%llu.%06u [%u] %s(%d): %s",
		        (unsigned long long) tv.tv_sec, (unsigned int) tv.tv_usec,
		        (int) getpid(), fn, line, buf);
	} else {
		vsyslog(priority, format, args);
	}
}

/**
 * ezcfg_api_nvram_get:
 * @name: nvram name
 * @value: buffer to store nvram value
 * @len: buffer size
 *
 **/
int ezcfg_api_nvram_get(const char *name, char *value, size_t len)
{
	char buf[1024];
	char *msg = NULL;
	int msg_len;
	struct ezcfg *ezcfg = NULL;
	struct ezcfg_ctrl *ezctrl = NULL;
	struct ezcfg_soap_http *sh = NULL;
	struct ezcfg_soap *soap = NULL;
	struct ezcfg_http *http = NULL;
	struct ezcfg_socket *sp = NULL;
	int body_index, child_index, getnv_index;
	char *res_name, *res_value;
	char *p;
	int header_len;
	int n;
	int rc = 0;

	if (name == NULL || value == NULL || len < 1) {
		return -EZCFG_E_ARGUMENT ;
	}

	ezcfg = ezcfg_new();
	if (ezcfg == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

        ezcfg_log_init("nvram_get");
        ezcfg_set_log_fn(ezcfg, log_fn);

	sh = ezcfg_soap_http_new(ezcfg);
	if (sh == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	soap = ezcfg_soap_http_get_soap(sh);
	http = ezcfg_soap_http_get_http(sh);

	/* build HTTP request line */
	ezcfg_http_set_request_method(http, EZCFG_SOAP_HTTP_METHOD_GET);
	snprintf(buf, sizeof(buf), "%s?name=%s", EZCFG_SOAP_HTTP_NVRAM_GET_URI, name);
	ezcfg_http_set_request_uri(http, buf);
	ezcfg_http_set_version_major(http, 1);
	ezcfg_http_set_version_minor(http, 1);
	ezcfg_http_set_state_request(http);

	/* build HTTP headers */
	snprintf(buf, sizeof(buf), "%s", "127.0.0.1");
	ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_HOST, buf);
	snprintf(buf, sizeof(buf), "%s", "application/soap+xml");
	ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_ACCEPT, buf);

	msg_len = EZCFG_BUFFER_SIZE;
	msg = (char *)malloc(msg_len);
	if (msg == NULL) {
		rc = -EZCFG_E_SPACE ;
		goto exit;
	}
	memset(msg, 0, msg_len);
	ezcfg_soap_http_write_message(sh, msg, msg_len);

	snprintf(buf, sizeof(buf), "%s-%d", EZCFG_NVRAM_SOCK_PATH, getpid());
	ezctrl = ezcfg_ctrl_new_from_socket(ezcfg, AF_LOCAL, EZCFG_PROTO_SOAP_HTTP, buf, EZCFG_NVRAM_SOCK_PATH);

	if (ezctrl == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	if (ezcfg_ctrl_connect(ezctrl) < 0) {
		rc = -EZCFG_E_CONNECTION ;
		goto exit;
	}

	if (ezcfg_ctrl_write(ezctrl, msg, msg_len, 0) < 0) {
		rc = -EZCFG_E_WRITE ;
		goto exit;
	}

	ezcfg_soap_http_reset_attributes(sh);

	n = 0;
	sp = ezcfg_ctrl_get_socket(ezctrl);
	header_len = ezcfg_socket_read_http_header(sp, http, msg, msg_len, &n);

	if (header_len <= 0) {
		rc = -EZCFG_E_READ ;
		goto exit;
	}

	ezcfg_http_set_state_response(http);
	if (ezcfg_soap_http_parse_header(sh, msg, header_len) == false) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	p = ezcfg_socket_read_http_content(sp, http, msg, header_len, &msg_len, &n);
	if ((p == NULL) || (n <= header_len)){
		rc = -EZCFG_E_READ ;
		goto exit;
	}
	msg = p;

	ezcfg_soap_http_set_message_body(sh, msg + header_len, n - header_len);
	if (ezcfg_soap_http_parse_message_body(sh) == false) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	/* get getNvramResponse part */
	body_index = ezcfg_soap_get_body_index(soap);
	getnv_index = ezcfg_soap_get_element_index(soap, body_index, -1, EZCFG_SOAP_NVRAM_GETNV_RESPONSE_ELEMENT_NAME);
	if (getnv_index < 2) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	/* get nvram node name */
	child_index = ezcfg_soap_get_element_index(soap, getnv_index, -1, EZCFG_SOAP_NVRAM_NAME_ELEMENT_NAME);
	if (child_index < 2) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	res_name = ezcfg_soap_get_element_content_by_index(soap, child_index);
	if (res_name == NULL) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	/* get nvram entry value */
	child_index = ezcfg_soap_get_element_index(soap, getnv_index, -1, EZCFG_SOAP_NVRAM_VALUE_ELEMENT_NAME);
	if (child_index < 2) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	res_value = ezcfg_soap_get_element_content_by_index(soap, child_index);

	if (res_value == NULL) {
		/* nvram value is empty */
		res_value = "";
	}

	if (len < strlen(res_value)+1) {
		rc = -EZCFG_E_SPACE ;
		goto exit;
	}

	rc = snprintf(value, len, "%s", res_value);
exit:
	if (msg != NULL) {
		free(msg);
	}

        if (sh != NULL) {
                ezcfg_soap_http_delete(sh);
	}

        if (ezctrl != NULL) {
                ezcfg_ctrl_delete(ezctrl);
	}

        if (ezcfg != NULL) {
                ezcfg_delete(ezcfg);
	}

	return rc;
}

/**
 * ezcfg_api_nvram_set:
 * @name: nvram name
 * @value: buffer stored nvram value
 *
 **/
int ezcfg_api_nvram_set(const char *name, const char *value)
{
	char buf[1024];
	char *msg = NULL;
	int msg_len;
	struct ezcfg *ezcfg = NULL;
	struct ezcfg_ctrl *ezctrl = NULL;
	struct ezcfg_soap_http *sh = NULL;
	struct ezcfg_soap *soap = NULL;
	struct ezcfg_http *http = NULL;
	struct ezcfg_socket *sp = NULL;
	int body_index, child_index, setnv_index;
	char *result;
	char *p;
	int header_len;
	int n;
	int rc = 0;

	if (name == NULL || value == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	ezcfg = ezcfg_new();
	if (ezcfg == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

        ezcfg_log_init("nvram_set");
        ezcfg_set_log_fn(ezcfg, log_fn);

	sh = ezcfg_soap_http_new(ezcfg);
	if (sh == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	soap = ezcfg_soap_http_get_soap(sh);
	http = ezcfg_soap_http_get_http(sh);

	/* build SOAP */
	ezcfg_soap_set_version_major(soap, 1);
	ezcfg_soap_set_version_minor(soap, 2);

	/* SOAP Envelope */
	ezcfg_soap_set_envelope(soap, EZCFG_SOAP_ENVELOPE_ELEMENT_NAME);
	ezcfg_soap_add_envelope_attribute(soap, EZCFG_SOAP_ENVELOPE_ATTR_NS_NAME, EZCFG_SOAP_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* SOAP Body */
	body_index = ezcfg_soap_set_body(soap, EZCFG_SOAP_BODY_ELEMENT_NAME);

	/* Body child setNvram part */
	setnv_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_SOAP_NVRAM_SETNV_ELEMENT_NAME, NULL);
	ezcfg_soap_add_body_child_attribute(soap, setnv_index, EZCFG_SOAP_NVRAM_ATTR_NS_NAME, EZCFG_SOAP_NVRAM_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* nvram name part */
	child_index = ezcfg_soap_add_body_child(soap, setnv_index, -1, EZCFG_SOAP_NVRAM_NAME_ELEMENT_NAME, name);

	/* nvram value part */
	child_index = ezcfg_soap_add_body_child(soap, setnv_index, -1, EZCFG_SOAP_NVRAM_VALUE_ELEMENT_NAME, value);

	/* build HTTP message body */
	msg_len = ezcfg_soap_get_message_length(soap);
	msg_len += strlen("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	msg_len++; /* '\n' */
	msg_len++; /* '\0' */
	msg = (char *)malloc(msg_len);
	if (msg == NULL) {
		rc = -EZCFG_E_SPACE ;
		goto exit;
	}
	memset(msg, 0, msg_len);

	snprintf(msg, msg_len, "%s\n", "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	n = strlen(msg);
	n += ezcfg_soap_write_message(soap, msg + n, msg_len - n);
	ezcfg_http_set_message_body(http, msg, n);

	/* build HTTP request line */
	ezcfg_http_set_request_method(http, EZCFG_SOAP_HTTP_METHOD_POST);
	snprintf(buf, sizeof(buf), "%s", EZCFG_SOAP_HTTP_NVRAM_SET_URI);
	ezcfg_http_set_request_uri(http, buf);
	ezcfg_http_set_version_major(http, 1);
	ezcfg_http_set_version_minor(http, 1);
	ezcfg_http_set_state_request(http);

	/* build HTTP headers */
	snprintf(buf, sizeof(buf), "%s", "127.0.0.1");
	ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_HOST, buf);
	snprintf(buf, sizeof(buf), "%s", "application/soap+xml");
	ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_ACCEPT, buf);

	msg_len = ezcfg_soap_http_get_message_length(sh);
	p = (char *)realloc(msg, msg_len);
	if (p == NULL) {
		rc = -EZCFG_E_SPACE ;
		goto exit;
	}
	msg = p;
	memset(msg, 0, msg_len);
	n = ezcfg_soap_http_write_message(sh, msg, msg_len);

	snprintf(buf, sizeof(buf), "%s-%d", EZCFG_NVRAM_SOCK_PATH, getpid());
	ezctrl = ezcfg_ctrl_new_from_socket(ezcfg, AF_LOCAL, EZCFG_PROTO_SOAP_HTTP, buf, EZCFG_NVRAM_SOCK_PATH);

	if (ezctrl == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	if (ezcfg_ctrl_connect(ezctrl) < 0) {
		rc = -EZCFG_E_CONNECTION ;
		goto exit;
	}

	if (ezcfg_ctrl_write(ezctrl, msg, msg_len, 0) < 0) {
		rc = -EZCFG_E_WRITE ;
		goto exit;
	}

	ezcfg_soap_http_reset_attributes(sh);

	n = 0;
	sp = ezcfg_ctrl_get_socket(ezctrl);
	header_len = ezcfg_socket_read_http_header(sp, http, msg, msg_len, &n);

	if (header_len <= 0) {
		rc = -EZCFG_E_READ ;
		goto exit;
	}

	ezcfg_http_set_state_response(http);
	if (ezcfg_soap_http_parse_header(sh, msg, header_len) == false) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	p = ezcfg_socket_read_http_content(sp, http, msg, header_len, &msg_len, &n);
	if ((p == NULL) || (n <= header_len)){
		rc = -EZCFG_E_READ ;
		goto exit;
	}
	msg = p;

	ezcfg_soap_http_set_message_body(sh, msg + header_len, n - header_len);
	if (ezcfg_soap_http_parse_message_body(sh) == false) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	/* get setNvramResponse part */
	body_index = ezcfg_soap_get_body_index(soap);
	setnv_index = ezcfg_soap_get_element_index(soap, body_index, -1, EZCFG_SOAP_NVRAM_SETNV_RESPONSE_ELEMENT_NAME);
	if (setnv_index < 2) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	/* get nvram result part */
	child_index = ezcfg_soap_get_element_index(soap, setnv_index, -1, EZCFG_SOAP_NVRAM_RESULT_ELEMENT_NAME);
	if (child_index < 2) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	result = ezcfg_soap_get_element_content_by_index(soap, child_index);
	if (result == NULL) {
		rc = -EZCFG_E_RESULT ;
		goto exit;
	}

	if (strcmp(result, EZCFG_SOAP_NVRAM_RESULT_VALUE_OK) == 0) {
		rc = 0;
	}
	else {
		rc = -EZCFG_E_RESULT ;
	}

exit:
	if (msg != NULL) {
		free(msg);
	}

        if (sh != NULL) {
                ezcfg_soap_http_delete(sh);
	}

        if (ezctrl != NULL) {
                ezcfg_ctrl_delete(ezctrl);
	}

        if (ezcfg != NULL) {
                ezcfg_delete(ezcfg);
	}

	return rc;
}

/**
 * ezcfg_api_nvram_unset:
 * @name: nvram name
 *
 **/
int ezcfg_api_nvram_unset(const char *name)
{
	char buf[1024];
	char *msg = NULL;
	int msg_len;
	struct ezcfg *ezcfg = NULL;
	struct ezcfg_ctrl *ezctrl = NULL;
	struct ezcfg_soap_http *sh = NULL;
	struct ezcfg_soap *soap = NULL;
	struct ezcfg_http *http = NULL;
	struct ezcfg_socket *sp = NULL;
	int body_index, child_index, unsetnv_index;
	char *result;
	char *p;
	int header_len;
	int n;
	int rc = 0;

	if (name == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	ezcfg = ezcfg_new();
	if (ezcfg == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

        ezcfg_log_init("nvram_unset");
        ezcfg_set_log_fn(ezcfg, log_fn);

	sh = ezcfg_soap_http_new(ezcfg);
	if (sh == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	soap = ezcfg_soap_http_get_soap(sh);
	http = ezcfg_soap_http_get_http(sh);

	/* build HTTP request line */
	ezcfg_http_set_request_method(http, EZCFG_SOAP_HTTP_METHOD_GET);
	snprintf(buf, sizeof(buf), "%s?name=%s", EZCFG_SOAP_HTTP_NVRAM_UNSET_URI, name);
	ezcfg_http_set_request_uri(http, buf);
	ezcfg_http_set_version_major(http, 1);
	ezcfg_http_set_version_minor(http, 1);
	ezcfg_http_set_state_request(http);

	/* build HTTP headers */
	snprintf(buf, sizeof(buf), "%s", "127.0.0.1");
	ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_HOST, buf);
	snprintf(buf, sizeof(buf), "%s", "application/soap+xml");
	ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_ACCEPT, buf);

	msg_len = EZCFG_BUFFER_SIZE;
	msg = (char *)malloc(msg_len);
	if (msg == NULL) {
		rc = -EZCFG_E_SPACE ;
		goto exit;
	}
	memset(msg, 0, msg_len);
	ezcfg_soap_http_write_message(sh, msg, msg_len);

	snprintf(buf, sizeof(buf), "%s-%d", EZCFG_NVRAM_SOCK_PATH, getpid());
	ezctrl = ezcfg_ctrl_new_from_socket(ezcfg, AF_LOCAL, EZCFG_PROTO_SOAP_HTTP, buf, EZCFG_NVRAM_SOCK_PATH);

	if (ezctrl == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	if (ezcfg_ctrl_connect(ezctrl) < 0) {
		rc = -EZCFG_E_CONNECTION ;
		goto exit;
	}

	if (ezcfg_ctrl_write(ezctrl, msg, msg_len, 0) < 0) {
		rc = -EZCFG_E_WRITE ;
		goto exit;
	}

	ezcfg_soap_http_reset_attributes(sh);

	n = 0;
	sp = ezcfg_ctrl_get_socket(ezctrl);
	header_len = ezcfg_socket_read_http_header(sp, http, msg, msg_len, &n);

	if (header_len <= 0) {
		rc = -EZCFG_E_READ ;
		goto exit;
	}

	ezcfg_http_set_state_response(http);
	if (ezcfg_soap_http_parse_header(sh, msg, header_len) == false) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	p = ezcfg_socket_read_http_content(sp, http, msg, header_len, &msg_len, &n);
	if ((p == NULL) || (n <= header_len)){
		rc = -EZCFG_E_READ ;
		goto exit;
	}
	msg = p;

	ezcfg_soap_http_set_message_body(sh, msg + header_len, n - header_len);
	if (ezcfg_soap_http_parse_message_body(sh) == false) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	/* get unsetNvramResponse part */
	body_index = ezcfg_soap_get_body_index(soap);
	unsetnv_index = ezcfg_soap_get_element_index(soap, body_index, -1, EZCFG_SOAP_NVRAM_UNSETNV_RESPONSE_ELEMENT_NAME);
	if (unsetnv_index < 2) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	/* get nvram result part */
	child_index = ezcfg_soap_get_element_index(soap, unsetnv_index, -1, EZCFG_SOAP_NVRAM_RESULT_ELEMENT_NAME);
	if (child_index < 2) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	result = ezcfg_soap_get_element_content_by_index(soap, child_index);
	if (result == NULL) {
		rc = -EZCFG_E_RESULT ;
		goto exit;
	}

	if (strcmp(result, EZCFG_SOAP_NVRAM_RESULT_VALUE_OK) == 0) {
		rc = 0;
	}
	else {
		rc = -EZCFG_E_RESULT ;
	}

exit:
	if (msg != NULL) {
		free(msg);
	}

        if (sh != NULL) {
                ezcfg_soap_http_delete(sh);
	}

        if (ezctrl != NULL) {
                ezcfg_ctrl_delete(ezctrl);
	}

        if (ezcfg != NULL) {
                ezcfg_delete(ezcfg);
	}

	return rc;
}

/**
 * ezcfg_api_nvram_list:
 * @list: buffer to store nvram value
 * @len: buffer size
 *
 **/
int ezcfg_api_nvram_list(char *list, size_t len)
{
	char buf[1024];
	char *msg;
	int msg_len;
	struct ezcfg *ezcfg = NULL;
	struct ezcfg_ctrl *ezctrl = NULL;
	struct ezcfg_soap_http *sh = NULL;
	struct ezcfg_soap *soap = NULL;
	struct ezcfg_http *http = NULL;
	struct ezcfg_socket *sp = NULL;
	int body_index, child_index, listnv_index, nvnode_index;
	char *res_name, *res_value;
	char *p;
	int header_len;
	int l, n;
	int rc = 0;

	if (list == NULL || len < 1) {
		return -EZCFG_E_ARGUMENT ;
	}

	ezcfg = ezcfg_new();
	if (ezcfg == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

        ezcfg_log_init("nvram_list");
        ezcfg_set_log_fn(ezcfg, log_fn);

	sh = ezcfg_soap_http_new(ezcfg);
	if (sh == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	soap = ezcfg_soap_http_get_soap(sh);
	http = ezcfg_soap_http_get_http(sh);

	/* build HTTP request line */
	ezcfg_http_set_request_method(http, EZCFG_SOAP_HTTP_METHOD_GET);
	snprintf(buf, sizeof(buf), "%s", EZCFG_SOAP_HTTP_NVRAM_LIST_URI);
	ezcfg_http_set_request_uri(http, buf);
	ezcfg_http_set_version_major(http, 1);
	ezcfg_http_set_version_minor(http, 1);
	ezcfg_http_set_state_request(http);

	/* build HTTP headers */
	snprintf(buf, sizeof(buf), "%s", "127.0.0.1");
	ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_HOST, buf);
	snprintf(buf, sizeof(buf), "%s", "application/soap+xml");
	ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_ACCEPT, buf);

	msg_len = EZCFG_BUFFER_SIZE;
	msg = (char *)malloc(msg_len);
	if (msg == NULL) {
		rc = -EZCFG_E_SPACE ;
		goto exit;
	}
	memset(msg, 0, msg_len);
	ezcfg_soap_http_write_message(sh, msg, msg_len);

	snprintf(buf, sizeof(buf), "%s-%d", EZCFG_NVRAM_SOCK_PATH, getpid());
	ezctrl = ezcfg_ctrl_new_from_socket(ezcfg, AF_LOCAL, EZCFG_PROTO_SOAP_HTTP, buf, EZCFG_NVRAM_SOCK_PATH);

	if (ezctrl == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	if (ezcfg_ctrl_connect(ezctrl) < 0) {
		rc = -EZCFG_E_CONNECTION ;
		goto exit;
	}

	if (ezcfg_ctrl_write(ezctrl, msg, msg_len, 0) < 0) {
		rc = -EZCFG_E_WRITE ;
		goto exit;
	}

	ezcfg_soap_http_reset_attributes(sh);

	n = 0;
	sp = ezcfg_ctrl_get_socket(ezctrl);
	header_len = ezcfg_socket_read_http_header(sp, http, msg, msg_len, &n);

	if (header_len <= 0) {
		rc = -EZCFG_E_READ ;
		goto exit;
	}

	ezcfg_http_set_state_response(http);
	if (ezcfg_soap_http_parse_header(sh, msg, header_len) == false) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	p = ezcfg_socket_read_http_content(sp, http, msg, header_len, &msg_len, &n);
	if ((p == NULL) || (n <= header_len)){
		rc = -EZCFG_E_READ ;
		goto exit;
	}
	msg = p;

	ezcfg_soap_http_set_message_body(sh, msg + header_len, n - header_len);
	if (ezcfg_soap_http_parse_message_body(sh) == false) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	/* get listNvramResponse part */
	body_index = ezcfg_soap_get_body_index(soap);
	listnv_index = ezcfg_soap_get_element_index(soap, body_index, -1, EZCFG_SOAP_NVRAM_LISTNV_RESPONSE_ELEMENT_NAME);
	if (listnv_index < 2) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	/* get nvram node index */
	nvnode_index = ezcfg_soap_get_element_index(soap, listnv_index, -1, EZCFG_SOAP_NVRAM_NVRAM_ELEMENT_NAME);
	p = list;
	l = len;
	while (nvnode_index > 0) {
		/* get nvram node name */
		child_index = ezcfg_soap_get_element_index(soap, nvnode_index, -1, EZCFG_SOAP_NVRAM_NAME_ELEMENT_NAME);
		if (child_index < 2) {
			rc = -EZCFG_E_PARSE ;
			goto exit;
		}

		res_name = ezcfg_soap_get_element_content_by_index(soap, child_index);
		if (res_name == NULL) {
			rc = -EZCFG_E_PARSE ;
			goto exit;
		}

		/* get nvram node value */
		child_index = ezcfg_soap_get_element_index(soap, nvnode_index, -1, EZCFG_SOAP_NVRAM_VALUE_ELEMENT_NAME);
		if (child_index < 2) {
			rc = -EZCFG_E_PARSE ;
			goto exit;
		}

		res_value = ezcfg_soap_get_element_content_by_index(soap, child_index);
		if (res_value == NULL) {
			/* nvram value is empty */
			res_value = "";
		}

		/* construct list buffer */
		n = snprintf(p, l, "%s=%s\n", res_name, res_value);
		p += n;
		l -= n;

		if (l == 0) {
			rc = -EZCFG_E_SPACE ;
			goto exit;
		}

		nvnode_index =  ezcfg_soap_get_element_index(soap, listnv_index, nvnode_index, EZCFG_SOAP_NVRAM_NVRAM_ELEMENT_NAME);
	}

	rc = 0;
exit:
        if (sh != NULL) {
                ezcfg_soap_http_delete(sh);
	}

        if (ezctrl != NULL) {
                ezcfg_ctrl_delete(ezctrl);
	}

        if (ezcfg != NULL) {
                ezcfg_delete(ezcfg);
	}

	return rc;
}

/**
 * ezcfg_api_nvram_info:
 * @info: buffer to store nvram dump info
 * @len: buffer size
 *
 **/
int ezcfg_api_nvram_info(char *info, size_t len)
{
	char buf[1024];
	char *msg;
	int msg_len;
	struct ezcfg *ezcfg = NULL;
	struct ezcfg_ctrl *ezctrl = NULL;
	struct ezcfg_soap_http *sh = NULL;
	struct ezcfg_soap *soap = NULL;
	struct ezcfg_http *http = NULL;
	struct ezcfg_socket *sp = NULL;
	int body_index, child_index, infonv_index, nvnode_index;
	char *res_name, *res_value;
	char *p;
	int header_len;
	int l, n;
	int rc = 0;

	if (info == NULL || len < 1) {
		return -EZCFG_E_ARGUMENT ;
	}

	ezcfg = ezcfg_new();
	if (ezcfg == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

        ezcfg_log_init("nvram_info");
        ezcfg_set_log_fn(ezcfg, log_fn);

	sh = ezcfg_soap_http_new(ezcfg);
	if (sh == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	soap = ezcfg_soap_http_get_soap(sh);
	http = ezcfg_soap_http_get_http(sh);

	/* build HTTP request line */
	ezcfg_http_set_request_method(http, EZCFG_SOAP_HTTP_METHOD_GET);
	snprintf(buf, sizeof(buf), "%s", EZCFG_SOAP_HTTP_NVRAM_INFO_URI);
	ezcfg_http_set_request_uri(http, buf);
	ezcfg_http_set_version_major(http, 1);
	ezcfg_http_set_version_minor(http, 1);
	ezcfg_http_set_state_request(http);

	/* build HTTP headers */
	snprintf(buf, sizeof(buf), "%s", "127.0.0.1");
	ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_HOST, buf);
	snprintf(buf, sizeof(buf), "%s", "application/soap+xml");
	ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_ACCEPT, buf);

	msg_len = EZCFG_BUFFER_SIZE;
	msg = (char *)malloc(msg_len);
	if (msg == NULL) {
		rc = -EZCFG_E_SPACE ;
		goto exit;
	}
	memset(msg, 0, msg_len);
	ezcfg_soap_http_write_message(sh, msg, msg_len);

	snprintf(buf, sizeof(buf), "%s-%d", EZCFG_NVRAM_SOCK_PATH, getpid());
	ezctrl = ezcfg_ctrl_new_from_socket(ezcfg, AF_LOCAL, EZCFG_PROTO_SOAP_HTTP, buf, EZCFG_NVRAM_SOCK_PATH);

	if (ezctrl == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	if (ezcfg_ctrl_connect(ezctrl) < 0) {
		rc = -EZCFG_E_CONNECTION ;
		goto exit;
	}

	if (ezcfg_ctrl_write(ezctrl, msg, msg_len, 0) < 0) {
		rc = -EZCFG_E_WRITE ;
		goto exit;
	}

	ezcfg_soap_http_reset_attributes(sh);

	n = 0;
	sp = ezcfg_ctrl_get_socket(ezctrl);
	header_len = ezcfg_socket_read_http_header(sp, http, msg, msg_len, &n);

	if (header_len <= 0) {
		rc = -EZCFG_E_READ ;
		goto exit;
	}

	ezcfg_http_set_state_response(http);
	if (ezcfg_soap_http_parse_header(sh, msg, header_len) == false) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	p = ezcfg_socket_read_http_content(sp, http, msg, header_len, &msg_len, &n);
	if ((p == NULL) || (n <= header_len)){
		rc = -EZCFG_E_READ ;
		goto exit;
	}
	msg = p;

	ezcfg_soap_http_set_message_body(sh, msg + header_len, n - header_len);
	if (ezcfg_soap_http_parse_message_body(sh) == false) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	/* get infoNvramResponse part */
	body_index = ezcfg_soap_get_body_index(soap);
	infonv_index = ezcfg_soap_get_element_index(soap, body_index, -1, EZCFG_SOAP_NVRAM_INFONV_RESPONSE_ELEMENT_NAME);
	if (infonv_index < 2) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	/* get nvram info node index */
	nvnode_index = ezcfg_soap_get_element_index(soap, infonv_index, -1, EZCFG_SOAP_NVRAM_NVRAM_ELEMENT_NAME);
	p = info;
	l = len;
	while (nvnode_index > 0) {
		/* get nvram node name */
		child_index = ezcfg_soap_get_element_index(soap, nvnode_index, -1, EZCFG_SOAP_NVRAM_NAME_ELEMENT_NAME);
		if (child_index < 2) {
			rc = -EZCFG_E_PARSE ;
			goto exit;
		}

		res_name = ezcfg_soap_get_element_content_by_index(soap, child_index);
		if (res_name == NULL) {
			rc = -EZCFG_E_PARSE ;
			goto exit;
		}

		/* get nvram node value */
		child_index = ezcfg_soap_get_element_index(soap, nvnode_index, -1, EZCFG_SOAP_NVRAM_VALUE_ELEMENT_NAME);
		if (child_index < 2) {
			rc = -EZCFG_E_PARSE ;
			goto exit;
		}

		res_value = ezcfg_soap_get_element_content_by_index(soap, child_index);
		if (res_value == NULL) {
			/* nvram value is empty */
			res_value = "";
		}

		/* construct info buffer */
		n = snprintf(p, l, "%s=%s\n", res_name, res_value);
		p += n;
		l -= n;

		if (l == 0) {
			rc = -EZCFG_E_SPACE ;
			goto exit;
		}

		nvnode_index =  ezcfg_soap_get_element_index(soap, infonv_index, nvnode_index, EZCFG_SOAP_NVRAM_NVRAM_ELEMENT_NAME);
	}

	rc = 0;
exit:
        if (sh != NULL) {
                ezcfg_soap_http_delete(sh);
	}

        if (ezctrl != NULL) {
                ezcfg_ctrl_delete(ezctrl);
	}

        if (ezcfg != NULL) {
                ezcfg_delete(ezcfg);
	}

	return rc;
}

/**
 * ezcfg_api_nvram_commit:
 *
 **/
int ezcfg_api_nvram_commit(void)
{
	char buf[1024];
	char *msg;
	int msg_len;
	struct ezcfg *ezcfg = NULL;
	struct ezcfg_ctrl *ezctrl = NULL;
	struct ezcfg_soap_http *sh = NULL;
	struct ezcfg_soap *soap = NULL;
	struct ezcfg_http *http = NULL;
	struct ezcfg_socket *sp = NULL;
	int body_index, child_index, commitnv_index;
	char *result;
	char *p;
	int header_len;
	int n;
	int rc = 0;

	ezcfg = ezcfg_new();
	if (ezcfg == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

        ezcfg_log_init("nvram_commit");
        ezcfg_set_log_fn(ezcfg, log_fn);

	sh = ezcfg_soap_http_new(ezcfg);
	if (sh == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	soap = ezcfg_soap_http_get_soap(sh);
	http = ezcfg_soap_http_get_http(sh);

	/* build HTTP request line */
	ezcfg_http_set_request_method(http, EZCFG_SOAP_HTTP_METHOD_GET);
	snprintf(buf, sizeof(buf), "%s", EZCFG_SOAP_HTTP_NVRAM_COMMIT_URI);
	ezcfg_http_set_request_uri(http, buf);
	ezcfg_http_set_version_major(http, 1);
	ezcfg_http_set_version_minor(http, 1);
	ezcfg_http_set_state_request(http);

	/* build HTTP headers */
	snprintf(buf, sizeof(buf), "%s", "127.0.0.1");
	ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_HOST, buf);
	snprintf(buf, sizeof(buf), "%s", "application/soap+xml");
	ezcfg_http_add_header(http, EZCFG_SOAP_HTTP_HEADER_ACCEPT, buf);

	msg_len = EZCFG_BUFFER_SIZE;
	msg = (char *)malloc(msg_len);
	if (msg == NULL) {
		rc = -EZCFG_E_SPACE ;
		goto exit;
	}
	memset(msg, 0, msg_len);
	ezcfg_soap_http_write_message(sh, msg, msg_len);

	snprintf(buf, sizeof(buf), "%s-%d", EZCFG_NVRAM_SOCK_PATH, getpid());
	ezctrl = ezcfg_ctrl_new_from_socket(ezcfg, AF_LOCAL, EZCFG_PROTO_SOAP_HTTP, buf, EZCFG_NVRAM_SOCK_PATH);

	if (ezctrl == NULL) {
		rc = -EZCFG_E_RESOURCE ;
		goto exit;
	}

	if (ezcfg_ctrl_connect(ezctrl) < 0) {
		rc = -EZCFG_E_CONNECTION ;
		goto exit;
	}

	if (ezcfg_ctrl_write(ezctrl, msg, msg_len, 0) < 0) {
		rc = -EZCFG_E_WRITE ;
		goto exit;
	}

	ezcfg_soap_http_reset_attributes(sh);

	n = 0;
	sp = ezcfg_ctrl_get_socket(ezctrl);
	header_len = ezcfg_socket_read_http_header(sp, http, msg, msg_len, &n);

	if (header_len <= 0) {
		rc = -EZCFG_E_READ ;
		goto exit;
	}

	ezcfg_http_set_state_response(http);
	if (ezcfg_soap_http_parse_header(sh, msg, header_len) == false) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	p = ezcfg_socket_read_http_content(sp, http, msg, header_len, &msg_len, &n);
	if ((p == NULL) || (n <= header_len)){
		rc = -EZCFG_E_READ ;
		goto exit;
	}
	msg = p;

	ezcfg_soap_http_set_message_body(sh, msg + header_len, n - header_len);
	if (ezcfg_soap_http_parse_message_body(sh) == false) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	/* get commitNvramResponse part */
	body_index = ezcfg_soap_get_body_index(soap);
	commitnv_index = ezcfg_soap_get_element_index(soap, body_index, -1, EZCFG_SOAP_NVRAM_COMMITNV_RESPONSE_ELEMENT_NAME);
	if (commitnv_index < 2) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	/* get nvram result part */
	child_index = ezcfg_soap_get_element_index(soap, commitnv_index, -1, EZCFG_SOAP_NVRAM_RESULT_ELEMENT_NAME);
	if (child_index < 2) {
		rc = -EZCFG_E_PARSE ;
		goto exit;
	}

	result = ezcfg_soap_get_element_content_by_index(soap, child_index);
	if (result == NULL) {
		rc = -EZCFG_E_RESULT ;
		goto exit;
	}

	if (strcmp(result, EZCFG_SOAP_NVRAM_RESULT_VALUE_OK) == 0) {
		rc = 0;
	}
	else {
		rc = -EZCFG_E_RESULT ;
	}

exit:
        if (sh != NULL) {
                ezcfg_soap_http_delete(sh);
	}

        if (ezctrl != NULL) {
                ezcfg_ctrl_delete(ezctrl);
	}

        if (ezcfg != NULL) {
                ezcfg_delete(ezcfg);
	}

	return rc;
}

void ezcfg_api_nvram_set_debug(bool enable_debug)
{
	debug = enable_debug;
}
