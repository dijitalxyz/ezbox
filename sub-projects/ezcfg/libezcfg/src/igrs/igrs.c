/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : igrs/igrs.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-26   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

struct ezcfg_igrs_msg_op {
	char *name;
	bool (*build_fn)(struct ezcfg_igrs *igrs);
	int (*write_fn)(struct ezcfg_igrs *igrs, char *buf, int len);
};

struct ezcfg_igrs {
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_soap *soap;

	/* IGRS info */
	unsigned short version_major; /* IGRS major version, must be 1 */
	unsigned short version_minor; /* IGRS minor version, must be 0 */

	unsigned short num_message_types; /* Number of supported message types */
	const struct ezcfg_igrs_msg_op *message_type_ops;
	unsigned short message_type_index; /* index for message type string */

	char source_device_id[EZCFG_UUID_STRING_LEN+1]; /* +1 for \0-terminated */
	char target_device_id[EZCFG_UUID_STRING_LEN+1]; /* +1 for \0-terminated */

	unsigned int source_client_id; /* 0 reserved */
	unsigned int target_service_id; /* 0 reserved */
	unsigned int sequence_id; /* 0 reserved */

	char *source_user_id; /* string, max length is 127 */
	char *service_security_id;

	char *invoke_args;

	char *host; /* Multicast channel and port reserved for ISDP */
	/* NOTIFY headers */
	char *cache_control; /* Used in advertisement mechanisms */
	char *location; /* A URL */
	char *nt; /* Notification Type */
	char *nts; /* Notification Sub Type. Single URI */
	char *server;
	char *usn; /* Unique Service Name */
	/* M-SEARCH headers */
	char *man; /* "ssdp:discover" */
	char *mx; /* Maximum wait time in seconds */
	char *st; /* Search Target */
};

/* for HTTP/1.1 request methods */
static const char *igrs_method_strings[] = {
	/* bad method string */
	NULL ,
	/* IGRS used methods */
	EZCFG_IGRS_METHOD_POST_EXT ,
};

/* for HTTP/1.1 known header */
static const char *igrs_header_strings[] = {
	/* bad header string */
	NULL ,
	/* IGRS known headers */
	EZCFG_IGRS_HEADER_HOST ,
	EZCFG_IGRS_HEADER_CONTENT_TYPE ,
	EZCFG_IGRS_HEADER_CONTENT_LENGTH ,
	EZCFG_IGRS_HEADER_MAN ,
	EZCFG_IGRS_HEADER_01_IGRS_VERSION ,
	EZCFG_IGRS_HEADER_01_IGRS_MESSAGE_TYPE ,
	EZCFG_IGRS_HEADER_01_TARGET_DEVICE_ID ,
	EZCFG_IGRS_HEADER_01_SOURCE_DEVICE_ID ,
	EZCFG_IGRS_HEADER_01_SEQUENCE_ID ,
	EZCFG_IGRS_HEADER_02_SOAP_ACTION ,
};

static bool build_create_session_request(struct ezcfg_igrs *igrs);
static int write_create_session_request(struct ezcfg_igrs *igrs, char *buf, int len);

static bool build_invoke_service_request(struct ezcfg_igrs *igrs);
static int write_invoke_service_request(struct ezcfg_igrs *igrs, char *buf, int len);

static const struct ezcfg_igrs_msg_op default_message_type_ops[] = {
	/* bad Message Type string */
	{ NULL, NULL, NULL },
	/* 9.1 Device Advertisement */
	{ EZCFG_IGRS_MSG_DEVICE_ONLINE_ADVERTISEMENT ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_DEVICE_OFFLINE_ADVERTISEMENT ,
	  NULL, NULL },
	/* 9.2 Device Pipe Management */
	{ EZCFG_IGRS_MSG_CREATE_PIPE_REQUEST ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_CREATE_PIPE_RESPONSE ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_AUTHENTICATE_REQUEST ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_AUTHENTICATE_RESPONSE ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_AUTHENTICATE_RESULT_REQUEST ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_AUTHENTICATE_RESULT_RESPONSE ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_CREATE_PIPE_RESULT_REQUEST ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_CREATE_PIPE_RESULT_RESPONSE ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_DETACH_PIPE_NOTIFY ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_DEVICE_ONLINE_DETECTION_REQUEST ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_DEVICE_ONLINE_DETECTION_RESPONSE ,
	  NULL, NULL },
	/* 9.3 Detaild Device Description Document Retrieval */
	{ EZCFG_IGRS_MSG_GET_DEVICE_DESCRIPTION_REQUEST ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_GET_DEVICE_DESCRIPTION_RESPONSE ,
	  NULL, NULL },
	/* 9.4 Retrieve Detailed Device Description Document Based on Non-Secure Pipe */
	/* 9.5 Device Group Setup */
	{ EZCFG_IGRS_MSG_PEER_DEVICE_GROUP_ADVERTISEMENT ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_QUIT_PEER_DEVICE_GROUP_NOTIFY ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_CENTRALISED_DEVICE_GROUP_ADVERTISEMENT ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_JOIN_CENTRALISED_DEVICE_GROUP_REQUEST ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_JOIN_CENTRALISED_DEVICE_GROUP_RESPONSE ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_QUIT_CENTRALISED_DEVICE_GROUP_ADVERTISEMENT ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_QUIT_CENTRALISED_DEVICE_GROUP_NOTIFY ,
	  NULL, NULL },
	/* 9.6 Device Search */
	{ EZCFG_IGRS_MSG_SEARCH_DEVICE_REQUEST ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_SEARCH_DEVICE_RESPONSE ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_SEARCH_DEVICE_REQUEST_ON_DEVICE ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_SEARCH_DEVICE_RESPONSE_ON_DEVICE ,
	  NULL, NULL },
	/* 9.7 Device Online/Offline Event Subscription */
	{ EZCFG_IGRS_MSG_SUBSCRIBE_DEVICE_EVENT_REQUEST ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_RENEW_SUBSCRIPTION_DEVICE_EVENT_REQUEST ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_SUBSCRIBE_DEVICE_EVENT_RESPONSE ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_UNSUBSCRIBE_DEVICE_EVENT_NOTIFY ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_NOTIFY_DEVICE_EVENT ,
	  NULL, NULL },
	/* 9.8 Device Group Search */
	{ EZCFG_IGRS_MSG_SEARCH_DEVICE_GROUP_REQUEST ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_SEARCH_DEVICE_GROUP_RESPONSE ,
	  NULL, NULL },
	/* 10.1 Service Online/Offline Advertisement */
	{ EZCFG_IGRS_MSG_SERVICE_ONLINE_ADVERTISEMENT ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_SERVICE_OFFLINE_ADVERTISEMENT ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_REGISTER_SERVICE_NOTIFY ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_UNREGISTER_SERVICE_NOTIFY ,
	  NULL, NULL },
	/* 10.2 Service Search */
	{ EZCFG_IGRS_MSG_SEARCH_SERVICE_REQUEST ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_SEARCH_SERVICE_RESPONSE ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_SEARCH_SERVICE_REQUEST_ON_DEVICE ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_SEARCH_SERVICE_RESPONSE_ON_DEVICE ,
	  NULL, NULL },
	/* 10.3 Service Online/Offline Event Subscription */
	{ EZCFG_IGRS_MSG_SUBSCRIBE_SERVICE_EVENT_REQUEST ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_RENEW_SUBSCRIPTION_SERVICE_EVENT_REQUEST ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_SUBSCRIBE_SERVICE_EVENT_RESPONSE ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_UNSUBSCRIBE_DEVICE_EVENT_NOTIFY ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_NOTIFY_SERVICE_EVENT ,
	  NULL, NULL },
	/* 10.4 Service Description Document Retrieval */
	{ EZCFG_IGRS_MSG_GET_SERVICE_DESCRIPTION_REQUEST ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_GET_SERVICE_DESCRIPTION_RESPONSE ,
	  NULL, NULL },
	/* 10.5 Session */
	{ EZCFG_IGRS_MSG_CREATE_SESSION_REQUEST ,
	  build_create_session_request, write_create_session_request },
	{ EZCFG_IGRS_MSG_CREATE_SESSION_RESPONSE ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_DESTROY_SESSION_NOTIFY ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_APPLY_SESSION_KEY_REQUEST ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_APPLY_SESSION_KEY_RESPONSE ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_TRANSFER_SESSION_KEY_REQUEST ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_TRANSFER_SESSION_KEY_RESPONSE ,
	  NULL, NULL },
	/* 10.6 Service Invocation */
	{ EZCFG_IGRS_MSG_INVOKE_SERVICE_REQUEST ,
	  build_invoke_service_request, write_invoke_service_request },
	{ EZCFG_IGRS_MSG_INVOKE_SERVICE_RESPONSE ,
	  NULL, NULL },
	{ EZCFG_IGRS_MSG_SEND_NOTIFICATION ,
	  NULL, NULL },
};

/**
 * private functions
 **/
static bool build_create_session_request(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_soap *soap;
	char buf[1024];
	char *msg;
	int msg_len;
	int n;
	int body_index, session_index, userinfo_index, child_index;

	ASSERT(igrs != NULL);
	ASSERT(igrs->http != NULL);
	ASSERT(igrs->soap != NULL);

	ezcfg = igrs->ezcfg;
	http = igrs->http;
	soap = igrs->soap;

	/* build SOAP */
	ezcfg_soap_set_version_major(soap, 1);
	ezcfg_soap_set_version_minor(soap, 2);

	/* SOAP Envelope */
	ezcfg_soap_set_envelope(soap, EZCFG_IGRS_ENVELOPE_ELEMENT_NAME);
	ezcfg_soap_add_envelope_attribute(soap, EZCFG_IGRS_ENVELOPE_ATTR_NS_NAME, EZCFG_IGRS_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_soap_add_envelope_attribute(soap, EZCFG_IGRS_ENVELOPE_ATTR_ENC_NAME, EZCFG_IGRS_ENVELOPE_ATTR_ENC_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* SOAP Body */
	body_index = ezcfg_soap_set_body(soap, EZCFG_IGRS_BODY_ELEMENT_NAME);

	/* Body child Session part */
	session_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_IGRS_SESSION_ELEMENT_NAME, NULL);
	ezcfg_soap_add_body_child_attribute(soap, session_index, EZCFG_IGRS_SESSION_ATTR_NS_NAME, EZCFG_IGRS_SESSION_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	child_index = -1;
	/* SourceCleintId part */
	snprintf(buf, sizeof(buf), "%u", igrs->source_client_id);
	child_index = ezcfg_soap_add_body_child(soap, session_index, child_index, EZCFG_IGRS_SOURCE_CLIENT_ID_ELEMENT_NAME, buf);

	/* TargetServiceId part */
	snprintf(buf, sizeof(buf), "%u", igrs->target_service_id);
	child_index = ezcfg_soap_add_body_child(soap, session_index, child_index, EZCFG_IGRS_TARGET_SERVICE_ID_ELEMENT_NAME, buf);

	/* SequenceId part */
	snprintf(buf, sizeof(buf), "%u", igrs->sequence_id);
	child_index = ezcfg_soap_add_body_child(soap, session_index, child_index, EZCFG_IGRS_SEQUENCE_ID_ELEMENT_NAME, buf);

	/* UserInfo part */
	userinfo_index = ezcfg_soap_add_body_child(soap, session_index, child_index, EZCFG_IGRS_USER_INFO_ELEMENT_NAME, NULL);

	snprintf(buf, sizeof(buf), "%s", igrs->source_user_id ? igrs->source_user_id : "NULL");
	child_index = ezcfg_soap_add_body_child(soap, userinfo_index, -1, EZCFG_IGRS_SOURCE_USER_ID_ELEMENT_NAME, buf);

	snprintf(buf, sizeof(buf), "urn:IGRS:ServiceSecurity:%s", igrs->service_security_id ? igrs->service_security_id : "NULL");
	child_index = ezcfg_soap_add_body_child(soap, userinfo_index, child_index, EZCFG_IGRS_SERVICE_SECURITY_ID_ELEMENT_NAME, buf);

	child_index = ezcfg_soap_add_body_child(soap, userinfo_index, child_index, EZCFG_IGRS_TOKEN_ELEMENT_NAME, "");

	msg_len = ezcfg_soap_get_length(soap);
	dbg(ezcfg, "msg_len=[%d]\n", msg_len);
	if (msg_len < 0) {
		err(ezcfg, "ezcfg_soap_get_length\n");
		return false;
	}
	msg_len++; /* one more for \0 */
	if (msg_len <= sizeof(buf)) {
		msg = buf;
	}
	else {
		msg = (char *)malloc(msg_len);
		if (msg == NULL) {
			err(ezcfg, "msg malloc\n");
			return false;
		}
	}

	memset(msg, '\0', msg_len);
	n = ezcfg_soap_write(soap, msg, msg_len);
	if (n < 0) {
		err(ezcfg, "ezcfg_soap_write\n");
		if (msg != buf) { free(msg); }
		return false;
	}
	ezcfg_http_set_message_body(http, msg, n);
	if (msg != buf) { free(msg); }

	/* build HTTP request line */
	ezcfg_http_set_request_method(http, EZCFG_IGRS_METHOD_POST_EXT);
	ezcfg_http_set_request_uri(http, "/IGRS");
	ezcfg_http_set_version_major(http, 1);
	ezcfg_http_set_version_minor(http, 1);

	/* build HTTP headers */
	snprintf(buf, sizeof(buf), "%s", "192.168.1.1:3880");
	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_HOST , buf);

	snprintf(buf, sizeof(buf), "\"%s\";ns=01", EZCFG_IGRS_SESSION_ATTR_NS_VALUE);
	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_MAN , buf);

	snprintf(buf, sizeof(buf), "IGRS/%d.%d", igrs->version_major, igrs->version_minor);
	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_01_IGRS_VERSION , buf);

	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_01_IGRS_MESSAGE_TYPE , EZCFG_IGRS_MSG_CREATE_SESSION_REQUEST);

	snprintf(buf, sizeof(buf), "urn:IGRS:Device:DeviceId:%s", igrs->target_device_id);
	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_01_TARGET_DEVICE_ID , buf);

	snprintf(buf, sizeof(buf), "urn:IGRS:Device:DeviceId:%s", igrs->source_device_id);
	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_01_SOURCE_DEVICE_ID , buf);

	snprintf(buf, sizeof(buf), "%u", igrs->sequence_id);
	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_01_SEQUENCE_ID , buf);

	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_CONTENT_TYPE , "text/xml;charset=utf-8");

	snprintf(buf, sizeof(buf), "%u", ezcfg_http_get_message_body_len(http));
	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_CONTENT_LENGTH , buf);

	snprintf(buf, sizeof(buf), "\"%s\";ns=02", EZCFG_IGRS_ENVELOPE_ATTR_NS_VALUE);
	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_MAN , buf);

	snprintf(buf, sizeof(buf), "\"%s\"", EZCFG_IGRS_SOAP_ACTION_CREATE_SESSION_REQUEST);
	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_02_SOAP_ACTION , buf);

	return true;
}

static int write_create_session_request(struct ezcfg_igrs *igrs, char *buf, int len)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_soap *soap;

	char *p;
	int n;

	ASSERT(igrs != NULL);
	ASSERT(igrs->http != NULL);
	ASSERT(igrs->soap != NULL);
	ASSERT(buf != NULL);
	ASSERT(len > 0);

	ezcfg = igrs->ezcfg;
	http = igrs->http;
	soap = igrs->soap;

	p = buf; n = 0;
	n = ezcfg_http_write_request_line(http, p, len);
	if (n < 0) {
		err(ezcfg, "ezcfg_http_write_request_line\n");
		return n;
	}
	p += n;	len -= n;

	n = ezcfg_http_write_headers(http, p, len);
	if (n < 0) {
		err(ezcfg, "ezcfg_http_write_headers\n");
		return n;
	}
	p += n;	len -= n;

	n = ezcfg_http_write_crlf(http, p, len);
	if (n < 0) {
		err(ezcfg, "ezcfg_http_write_crlf\n");
		return n;
	}
	p += n;	len -= n;

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

static bool build_invoke_service_request(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_soap *soap;
	char buf[1024];
	char *msg;
	int msg_len;
	int n;
	int body_index, session_index, child_index;

	ASSERT(igrs != NULL);
	ASSERT(igrs->http != NULL);
	ASSERT(igrs->soap != NULL);

	ezcfg = igrs->ezcfg;
	http = igrs->http;
	soap = igrs->soap;

	/* build SOAP */
	ezcfg_soap_set_version_major(soap, 1);
	ezcfg_soap_set_version_minor(soap, 2);

	/* SOAP Envelope */
	ezcfg_soap_set_envelope(soap, EZCFG_IGRS_ENVELOPE_ELEMENT_NAME);
	ezcfg_soap_add_envelope_attribute(soap, EZCFG_IGRS_ENVELOPE_ATTR_NS_NAME, EZCFG_IGRS_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_soap_add_envelope_attribute(soap, EZCFG_IGRS_ENVELOPE_ATTR_ENC_NAME, EZCFG_IGRS_ENVELOPE_ATTR_ENC_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* SOAP Body */
	body_index = ezcfg_soap_set_body(soap, EZCFG_IGRS_BODY_ELEMENT_NAME);

	/* Body child Session part */
	session_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_IGRS_SESSION_ELEMENT_NAME, NULL);
	ezcfg_soap_add_body_child_attribute(soap, session_index, EZCFG_IGRS_SESSION_ATTR_NS_NAME, EZCFG_IGRS_SESSION_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* SourceCleintId part */
	child_index = -1;
	snprintf(buf, sizeof(buf), "%u", igrs->source_client_id);
	child_index = ezcfg_soap_add_body_child(soap, session_index, child_index, EZCFG_IGRS_SOURCE_CLIENT_ID_ELEMENT_NAME, buf);

	/* TargetServiceId part */
	snprintf(buf, sizeof(buf), "%u", igrs->target_service_id);
	child_index = ezcfg_soap_add_body_child(soap, session_index, child_index, EZCFG_IGRS_TARGET_SERVICE_ID_ELEMENT_NAME, buf);

	/* SequenceId part */
	snprintf(buf, sizeof(buf), "%u", igrs->sequence_id);
	child_index = ezcfg_soap_add_body_child(soap, session_index, child_index, EZCFG_IGRS_SEQUENCE_ID_ELEMENT_NAME, buf);

	/* The specific invocation acknowledgement message */
	if (igrs->invoke_args != NULL) {
		child_index = ezcfg_soap_add_body_child(soap, session_index, child_index, EZCFG_IGRS_INVOKE_ARGS_ELEMENT_NAME, igrs->invoke_args);
	}

	msg_len = ezcfg_soap_get_length(soap);
	if (msg_len < 0) {
		err(ezcfg, "ezcfg_soap_get_length\n");
		return false;
	}
	msg_len++; /* one more for \0 */
	if (msg_len <= sizeof(buf)) {
		msg = buf;
	}
	else {
		msg = (char *)malloc(msg_len);
		if (msg == NULL) {
			err(ezcfg, "msg malloc\n");
			return false;
		}
	}

	memset(msg, '\0', msg_len);
	n = ezcfg_soap_write(soap, msg, msg_len);
	if (n < 0) {
		err(ezcfg, "ezcfg_soap_write\n");
		if (msg != buf) { free(msg); }
		return false;
	}
	ezcfg_http_set_message_body(http, msg, n);
	if (msg != buf) { free(msg); }

	/* build HTTP request line */
	ezcfg_http_set_request_method(http, EZCFG_IGRS_METHOD_POST_EXT);
	ezcfg_http_set_request_uri(http, "/IGRS");
	ezcfg_http_set_version_major(http, 1);
	ezcfg_http_set_version_minor(http, 1);

	/* build HTTP headers */
	snprintf(buf, sizeof(buf), "%s", "192.168.1.1:3880");
	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_HOST , buf);

	snprintf(buf, sizeof(buf), "\"%s\";ns=01", EZCFG_IGRS_SESSION_ATTR_NS_VALUE);
	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_MAN , buf);

	snprintf(buf, sizeof(buf), "IGRS/%d.%d", igrs->version_major, igrs->version_minor);
	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_01_IGRS_VERSION , buf);

	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_01_IGRS_MESSAGE_TYPE , EZCFG_IGRS_MSG_INVOKE_SERVICE_REQUEST);

	snprintf(buf, sizeof(buf), "urn:IGRS:Device:DeviceId:%s", igrs->target_device_id);
	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_01_TARGET_DEVICE_ID , buf);

	snprintf(buf, sizeof(buf), "urn:IGRS:Device:DeviceId:%s", igrs->source_device_id);
	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_01_SOURCE_DEVICE_ID , buf);

	snprintf(buf, sizeof(buf), "%u", igrs->sequence_id);
	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_01_SEQUENCE_ID , buf);

	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_CONTENT_TYPE , "text/xml;charset=utf-8");

	snprintf(buf, sizeof(buf), "%u", ezcfg_http_get_message_body_len(http));
	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_CONTENT_LENGTH , buf);

	snprintf(buf, sizeof(buf), "\"%s\";ns=02", EZCFG_IGRS_ENVELOPE_ATTR_NS_VALUE);
	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_MAN , buf);

	snprintf(buf, sizeof(buf), "\"%s\"", EZCFG_IGRS_SOAP_ACTION_INVOKE_SERVICE_REQUEST);
	ezcfg_http_add_header(http, EZCFG_IGRS_HEADER_02_SOAP_ACTION , buf);

	return true;
}

static int write_invoke_service_request(struct ezcfg_igrs *igrs, char *buf, int len)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_soap *soap;

	char *p;
	int n;

	ASSERT(igrs != NULL);
	ASSERT(igrs->http != NULL);
	ASSERT(igrs->soap != NULL);
	ASSERT(buf != NULL);
	ASSERT(len > 0);

	ezcfg = igrs->ezcfg;
	http = igrs->http;
	soap = igrs->soap;

	p = buf; n = 0;
	n = ezcfg_http_write_request_line(http, p, len);
	if (n < 0) {
		err(ezcfg, "ezcfg_http_write_request_line\n");
		return n;
	}
	p += n;	len -= n;

	n = ezcfg_http_write_headers(http, p, len);
	if (n < 0) {
		err(ezcfg, "ezcfg_http_write_headers\n");
		return n;
	}
	p += n;	len -= n;

	n = ezcfg_http_write_crlf(http, p, len);
	if (n < 0) {
		err(ezcfg, "ezcfg_http_write_crlf\n");
		return n;
	}
	p += n;	len -= n;

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

/**
 * Public functions
 **/
void ezcfg_igrs_delete(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;

	ASSERT(igrs != NULL);

	ezcfg = igrs->ezcfg;

	if (igrs->http != NULL) {
		ezcfg_http_delete(igrs->http);
	}

	if (igrs->soap != NULL) {
		ezcfg_soap_delete(igrs->soap);
	}

	if (igrs->source_user_id != NULL) {
		free(igrs->source_user_id);
	}

	if (igrs->service_security_id != NULL) {
		free(igrs->service_security_id);
	}

	if (igrs->invoke_args != NULL) {
		free(igrs->invoke_args);
	}

	free(igrs);
}

/**
 * ezcfg_igrs_new:
 * Create ezcfg igrs protocol data structure
 * Returns: a new ezcfg igrs protocol data structure
 **/
struct ezcfg_igrs *ezcfg_igrs_new(struct ezcfg *ezcfg)
{
	struct ezcfg_igrs *igrs;

	ASSERT(ezcfg != NULL);

	/* initialize igrs protocol data structure */
	igrs = calloc(1, sizeof(struct ezcfg_igrs));
	if (igrs == NULL) {
		return NULL;
	}

	memset(igrs, 0, sizeof(struct ezcfg_igrs));

	igrs->http = ezcfg_http_new(ezcfg);
	if (igrs->http == NULL) {
		ezcfg_igrs_delete(igrs);
		return NULL;
	}

	igrs->soap = ezcfg_soap_new(ezcfg);
	if (igrs->soap == NULL) {
		ezcfg_igrs_delete(igrs);
		return NULL;
	}

	igrs->ezcfg = ezcfg;
	ezcfg_http_set_method_strings(igrs->http, igrs_method_strings, ARRAY_SIZE(igrs_method_strings) - 1);
	ezcfg_http_set_known_header_strings(igrs->http, igrs_header_strings, ARRAY_SIZE(igrs_header_strings) - 1);
	ezcfg_igrs_set_message_type_ops(igrs, default_message_type_ops, ARRAY_SIZE(default_message_type_ops) - 1);
	return igrs;
}

void ezcfg_igrs_dump(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;

	ASSERT(igrs != NULL);

	ezcfg = igrs->ezcfg;
}

int ezcfg_igrs_get_message_length(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;
        struct ezcfg_http *http;
	int count, n;

        ASSERT(igrs != NULL);
        ASSERT(igrs->http != NULL);

        ezcfg = igrs->ezcfg;
        http = igrs->http;

	count = 0;
	n = ezcfg_http_get_start_line_length(http);
	if (n < 0) {
		return -1;
	}
	count += n;

	n = ezcfg_http_get_headers_length(http);
	if (n < 0) {
		return -1;
	}
	count += n;

	n = ezcfg_http_get_crlf_length(http);
	if (n < 0) {
		return -1;
	}
	count += n;

	n = ezcfg_http_get_message_body_len(http);
	if (n < 0) {
		return -1;
	}
	count += n;
	return count;
}

bool ezcfg_igrs_set_message_type_ops(struct ezcfg_igrs *igrs, const struct ezcfg_igrs_msg_op *message_type_ops, unsigned short num_message_types)
{
	struct ezcfg *ezcfg;

	ASSERT(igrs != NULL);
	ASSERT(message_type_ops != NULL);

	ezcfg = igrs->ezcfg;

	igrs->num_message_types = num_message_types;
	igrs->message_type_ops = message_type_ops;

	return true;
}

unsigned short ezcfg_igrs_get_version_major(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;

	ASSERT(igrs != NULL);

	ezcfg = igrs->ezcfg;

	return igrs->version_major;
}

unsigned short ezcfg_igrs_get_version_minor(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;

	ASSERT(igrs != NULL);

	ezcfg = igrs->ezcfg;

	return igrs->version_minor;
}

bool ezcfg_igrs_set_version_major(struct ezcfg_igrs *igrs, unsigned short major)
{
	struct ezcfg *ezcfg;

	ASSERT(igrs != NULL);

	ezcfg = igrs->ezcfg;

	igrs->version_major = major;

	return true;
}

bool ezcfg_igrs_set_version_minor(struct ezcfg_igrs *igrs, unsigned short minor)
{
	struct ezcfg *ezcfg;

	ASSERT(igrs != NULL);

	ezcfg = igrs->ezcfg;

	igrs->version_minor = minor;

	return true;
}

unsigned short ezcfg_igrs_set_message_type(struct ezcfg_igrs *igrs, const char *type)
{
	struct ezcfg *ezcfg;
	const struct ezcfg_igrs_msg_op *op;
	unsigned short i;

	ASSERT(igrs != NULL);
	ASSERT(type != NULL);

	ezcfg = igrs->ezcfg;

	i = igrs->num_message_types;
	op = &(igrs->message_type_ops[i]);
	while ((i > 0) && (strcmp(op->name, type) != 0)) {
		i--;
		op = &(igrs->message_type_ops[i]);
	}
	igrs->message_type_index = i;
	return i;
}

bool ezcfg_igrs_set_source_device_id(struct ezcfg_igrs *igrs, const char *uuid_str)
{
	struct ezcfg *ezcfg;

	ASSERT(igrs != NULL);
	ASSERT(uuid_str != NULL);

	ezcfg = igrs->ezcfg;

	snprintf(igrs->source_device_id, EZCFG_UUID_STRING_LEN+1, "%s", uuid_str);
	return true;
}

char *ezcfg_igrs_get_source_device_id(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;

	ASSERT(igrs != NULL);

	ezcfg = igrs->ezcfg;

	return igrs->source_device_id;
}

bool ezcfg_igrs_set_target_device_id(struct ezcfg_igrs *igrs, const char *uuid_str)
{
	struct ezcfg *ezcfg;

	ASSERT(igrs != NULL);
	ASSERT(uuid_str != NULL);

	ezcfg = igrs->ezcfg;

	snprintf(igrs->target_device_id, EZCFG_UUID_STRING_LEN+1, "%s", uuid_str);
	return true;
}

char *ezcfg_igrs_get_target_device_id(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;

	ASSERT(igrs != NULL);

	ezcfg = igrs->ezcfg;

	return igrs->target_device_id;
}

bool ezcfg_igrs_set_source_user_id(struct ezcfg_igrs *igrs, const char *user_id)
{
	struct ezcfg *ezcfg;
	char *s;

	ASSERT(igrs != NULL);
	ASSERT(user_id != NULL);

	ezcfg = igrs->ezcfg;

	s = strdup(user_id);
	if (s == NULL) {
		err(ezcfg, "no enough memory for user id\n");
		return false;
	}

	if (igrs->source_user_id != NULL) {
		free(igrs->source_user_id);
	}
	igrs->source_user_id = s;
	return true;
}

char *ezcfg_igrs_get_source_user_id(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;

	ASSERT(igrs != NULL);

	ezcfg = igrs->ezcfg;

	return igrs->source_user_id;
}

bool ezcfg_igrs_set_service_security_id(struct ezcfg_igrs *igrs, const char *security_id)
{
	struct ezcfg *ezcfg;
	char *s;

	ASSERT(igrs != NULL);

	ezcfg = igrs->ezcfg;

	if (security_id == NULL) {
		s = NULL;
	} else {
		s = strdup(security_id);
		if (s == NULL) {
			err(ezcfg, "no enough memory for service security id\n");
			return false;
		}
	}

	if (igrs->service_security_id != NULL) {
		free(igrs->service_security_id);
	}
	igrs->service_security_id = s;
	return true;
}

char *ezcfg_igrs_get_service_security_id(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;

	ASSERT(igrs != NULL);

	ezcfg = igrs->ezcfg;

	return igrs->service_security_id;
}

bool ezcfg_igrs_set_invoke_args(struct ezcfg_igrs *igrs, const char *invoke_args)
{
	struct ezcfg *ezcfg;
	char *s;

	ASSERT(igrs != NULL);

	ezcfg = igrs->ezcfg;

	if (invoke_args == NULL) {
		s = NULL;
	} else {
		s = strdup(invoke_args);
		if (s == NULL) {
			err(ezcfg, "no enough memory for service invoke args\n");
			return false;
		}
	}

	if (igrs->invoke_args != NULL) {
		free(igrs->invoke_args);
	}
	igrs->invoke_args = s;
	return true;
}

char *ezcfg_igrs_get_invoke_args(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;

	ASSERT(igrs != NULL);

	ezcfg = igrs->ezcfg;

	return igrs->invoke_args;
}

bool ezcfg_igrs_set_sequence_id(struct ezcfg_igrs *igrs, unsigned int seq_id)
{
	struct ezcfg *ezcfg;

	ASSERT(igrs != NULL);
	ASSERT(seq_id > 0);

	ezcfg = igrs->ezcfg;

	igrs->sequence_id = seq_id;
	return true;
}

unsigned int ezcfg_igrs_get_sequence_id(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;

	ASSERT(igrs != NULL);

	ezcfg = igrs->ezcfg;

	return igrs->sequence_id;
}

bool ezcfg_igrs_set_source_client_id(struct ezcfg_igrs *igrs, unsigned int client_id)
{
	struct ezcfg *ezcfg;

	ASSERT(igrs != NULL);
	ASSERT(client_id > 0);

	ezcfg = igrs->ezcfg;

	igrs->source_client_id = client_id;
	return true;
}

unsigned int ezcfg_igrs_get_source_client_id(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;

	ASSERT(igrs != NULL);

	ezcfg = igrs->ezcfg;

	return igrs->source_client_id;
}

bool ezcfg_igrs_set_target_service_id(struct ezcfg_igrs *igrs, unsigned int service_id)
{
	struct ezcfg *ezcfg;

	ASSERT(igrs != NULL);
	ASSERT(service_id > 0);

	ezcfg = igrs->ezcfg;

	igrs->target_service_id = service_id;
	return true;
}

unsigned int ezcfg_igrs_get_target_service_id(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;

	ASSERT(igrs != NULL);

	ezcfg = igrs->ezcfg;

	return igrs->target_service_id;
}

bool ezcfg_igrs_build_message(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;
	const struct ezcfg_igrs_msg_op *op;

	ASSERT(igrs != NULL);

	ezcfg = igrs->ezcfg;

	if (igrs->message_type_index == 0) {
		err(ezcfg, "build message fail with unknown message type\n");
		return false;
	}

	op = &(igrs->message_type_ops[igrs->message_type_index]);
	if (op->build_fn == NULL) {
		err(ezcfg, "message no build callback function\n");
		return false;
	}
	return op->build_fn(igrs);
}

int ezcfg_igrs_write_message(struct ezcfg_igrs *igrs, char *buf, int len)
{
	struct ezcfg *ezcfg;
	const struct ezcfg_igrs_msg_op *op;

	ASSERT(igrs != NULL);
	ASSERT(buf != NULL);
	ASSERT(len > 0);

	ezcfg = igrs->ezcfg;

	dbg(ezcfg, "igrs->message_type_index=[%d]\n", igrs->message_type_index);
	if (igrs->message_type_index == 0) {
		err(ezcfg, "unknown igrs message type\n");
		return -1;
	}

	op = &(igrs->message_type_ops[igrs->message_type_index]);

	if (op->write_fn != NULL) {
		info(ezcfg, "write %s\n", op->name);
		return op->write_fn(igrs, buf, len);
	}
	return -1;
}

char *ezcfg_igrs_get_http_header_value(struct ezcfg_igrs *igrs, char *name)
{
        struct ezcfg *ezcfg;
        struct ezcfg_http *http;

        ASSERT(igrs != NULL);
        ASSERT(igrs->http != NULL);

        ezcfg = igrs->ezcfg;
        http = igrs->http;

	return ezcfg_http_get_header_value(http, name);
}

void ezcfg_igrs_reset_attributes(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_soap *soap;

	ASSERT(igrs != NULL);
	ASSERT(igrs->http != NULL);
	ASSERT(igrs->soap != NULL);

	ezcfg = igrs->ezcfg;
	http = igrs->http;
	soap = igrs->soap;

	ezcfg_http_reset_attributes(http);
	//ezcfg_soap_reset_attributes(soap);

	if (igrs->source_user_id != NULL) {
		free(igrs->source_user_id);
		igrs->source_user_id = NULL;
	}

	if (igrs->service_security_id != NULL) {
		free(igrs->service_security_id);
		igrs->service_security_id = NULL;
	}
}

bool ezcfg_igrs_parse_request(struct ezcfg_igrs *igrs, char *buf, int len)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	char *s;
	int n;

	ASSERT(igrs != NULL);

	ezcfg = igrs->ezcfg;
	http = igrs->http;

	if (ezcfg_http_parse_request(http, buf, len) == false) {
		return false;
	}

	/* check IGRS version */
	s = ezcfg_http_get_header_value(http, EZCFG_IGRS_HEADER_01_IGRS_VERSION);
	if (s == NULL) {
		err(ezcfg, "no igrs version in header\n");
		return false;
	}
	n = sscanf(s, "IGRS/%hd.%hd", &(igrs->version_major), &(igrs->version_minor));
	if (n != 2) {
		err(ezcfg, "igrs version format in header is invalid\n");
		return false;
	}

	/* check IGRS message type */
	s = ezcfg_http_get_header_value(http, EZCFG_IGRS_HEADER_01_IGRS_MESSAGE_TYPE);
	if (s == NULL) {
		err(ezcfg, "no igrs message type in header\n");
		return false;
	}
	if (ezcfg_igrs_set_message_type(igrs, s) == 0) {
		err(ezcfg, "igrs message type is unknown\n");
		return false;
	}
	
	return true;
}

bool ezcfg_igrs_parse_response(struct ezcfg_igrs *igrs, char *buf, int len)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	char *s;
	int n;

	ASSERT(igrs != NULL);

	ezcfg = igrs->ezcfg;
	http = igrs->http;

	if (ezcfg_http_parse_response(http, buf, len) == false) {
		return false;
	}

	/* check IGRS version */
	s = ezcfg_http_get_header_value(http, EZCFG_IGRS_HEADER_01_IGRS_VERSION);
	if (s == NULL) {
		err(ezcfg, "no igrs version in header\n");
		return false;
	}
	n = sscanf(s, "IGRS/%hd.%hd", &(igrs->version_major), &(igrs->version_minor));
	if (n != 2) {
		err(ezcfg, "igrs version format in header is invalid\n");
		return false;
	}

	/* check IGRS message type */
	s = ezcfg_http_get_header_value(http, EZCFG_IGRS_HEADER_01_IGRS_MESSAGE_TYPE);
	if (s == NULL) {
		err(ezcfg, "no igrs message type in header\n");
		return false;
	}
	if (ezcfg_igrs_set_message_type(igrs, s) == 0) {
		err(ezcfg, "igrs message type is unknown\n");
		return false;
	}
	
	return true;
}

char *ezcfg_igrs_set_message_body(struct ezcfg_igrs *igrs, const char *body, int len)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;

	ASSERT(igrs != NULL);

	ezcfg = igrs->ezcfg;
	http = igrs->http;

	return ezcfg_http_set_message_body(http, body, len);
}
