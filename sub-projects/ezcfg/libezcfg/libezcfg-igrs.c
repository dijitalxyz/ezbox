/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg-igrs.c
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
#include <assert.h>
#include <pthread.h>

#include "libezcfg.h"
#include "libezcfg-private.h"

struct ezcfg_igrs_msg_op {
	char *name;
	bool (*builder)(struct ezcfg_igrs *igrs);
	int (*writer)(struct ezcfg_igrs *igrs, char *buf, int len);
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

	unsigned int sequence_id; /* 0 reserved */

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
	NULL,
	/* IGRS used motheds */
	"M-POST",
};

/* for HTTP/1.1 known header */
static const char *igrs_header_strings[] = {
	/* bad header string */
	NULL,
	/* IGRS known headers */
	"Host",
	"Content-Type",
	"Content-Length",
	"Man",
	"01-IGRSVersion",
	"01-IGRSMessageType",
	"01-TargetDeviceId",
	"01-SourceDeviceId",
	"01-SequenceId",
	"02-SoapAction",
};

static bool build_create_session_request(struct ezcfg_igrs *igrs);
static int write_create_session_request(struct ezcfg_igrs *igrs, char *buf, int len);

static const struct ezcfg_igrs_msg_op default_message_type_ops[] = {
	/* bad Message Type string */
	{ NULL, NULL, NULL },
	/* 9.1 Device Advertisement */
	{ "DeviceOnlineAdvertisement", NULL, NULL },
	{ "DeviceOfflineAdvertisement", NULL, NULL },
	/* 9.2 Device Pipe Management */
	{ "CreatePipeRequest", NULL, NULL },
	{ "CreatePipeResponse", NULL, NULL },
	{ "AuthenticateRequest", NULL, NULL },
	{ "AuthenticateResponse", NULL, NULL },
	{ "AuthenticateResultRequest", NULL, NULL },
	{ "AuthenticateResultResponse", NULL, NULL },
	{ "CreatePipeResultRequest", NULL, NULL },
	{ "CreatePipeResultResponse", NULL, NULL },
	{ "DetachPipeNotify", NULL, NULL },
	{ "DeviceOnlineDetectionRequest", NULL, NULL },
	{ "DeviceOnlineDetectionResponse", NULL, NULL },
	/* 9.3 Detaild Device Description Document Retrieval */
	{ "GetDeviceDescriptionRequest", NULL, NULL },
	{ "GetDeviceDescriptionResponse", NULL, NULL },
	/* 9.4 Retrieve Detailed Device Description Document Based on Non-Secure Pipe */
	/* 9.5 Device Group Setup */
	{ "PeerDeviceGroupAdvertisement", NULL, NULL },
	{ "QuitPeerDeviceGroupNotify", NULL, NULL },
	{ "CentralisedDeviceGroupAdvertisement", NULL, NULL },
	{ "JoinCentralisedDeviceGroupRequest", NULL, NULL },
	{ "JoinCentralisedDeviceGroupResponse", NULL, NULL },
	{ "QuitCentralisedDeviceGroupAdvertisement", NULL, NULL },
	{ "QuitCentralisedDeviceGroupNotify", NULL, NULL },
	/* 9.6 Device Search */
	{ "SearchDeviceRequest", NULL, NULL },
	{ "SearchDeviceResponse", NULL, NULL },
	{ "SearchDeviceRequestOnDevice", NULL, NULL },
	{ "SearchDeviceResponseOnDevice", NULL, NULL },
	/* 9.7 Device Online/Offline Event Subscription */
	{ "SubscribeDeviceEventRequest", NULL, NULL },
	{ "RenewSubscriptionDeviceEventRequest", NULL, NULL },
	{ "SubscribeDeviceEventResponse", NULL, NULL },
	{ "UnSubscribeDeviceEventNotify", NULL, NULL },
	{ "NotifyDeviceEvent", NULL, NULL },
	/* 9.8 Device Group Search */
	{ "SearchDeviceGroupRequest", NULL, NULL },
	{ "SearchDeviceGroupResponse", NULL, NULL },
	/* 10.1 Service Online/Offline Advertisement */
	{ "ServiceOnlineAdvertisement", NULL, NULL },
	{ "ServiceOfflineAdvertisement", NULL, NULL },
	{ "RegisterServiceNotify", NULL, NULL },
	{ "UnRegisterServiceNotify", NULL, NULL },
	/* 10.2 Service Search */
	{ "SearchServiceRequest", NULL, NULL },
	{ "SearchServiceResponse", NULL, NULL },
	{ "SearchServiceRequestOnDevice", NULL, NULL },
	{ "SearchServiceResponseOnDevice", NULL, NULL },
	/* 10.3 Service Online/Offline Event Subscription */
	{ "SubscribeServiceEventRequest", NULL, NULL },
	{ "RenewSubscriptionServiceEventRequest", NULL, NULL },
	{ "SubscribeServiceEventResponse", NULL, NULL },
	{ "UnSubscribeDeviceEventNotify", NULL, NULL },
	{ "NotifyServiceEvent", NULL, NULL },
	/* 10.4 Service Description Document Retrieval */
	{ "GetServiceDescriptionRequest", NULL, NULL },
	{ "GetServiceDescriptionResponse", NULL, NULL },
	/* 10.5 Session */
	{ "CreateSessionRequest", build_create_session_request, write_create_session_request },
	{ "CreateSessionResponse", NULL, NULL },
	{ "DestroySessionNotify", NULL, NULL },
	{ "ApplySessionKeyRequest", NULL, NULL },
	{ "ApplySessionKeyResponse", NULL, NULL },
	{ "TransferSessionKeyRequest", NULL, NULL },
	{ "TransferSessionKeyResponse", NULL, NULL },
	/* 10.6 Service Invocation */
	{ "InvokeServiceRequest", NULL, NULL },
	{ "InvokeServiceResponse", NULL, NULL },
	{ "SendNotification", NULL, NULL },
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
	int n;
	int body_index, session_index, userinfo_index, child_index;

	assert(igrs != NULL);
	assert(igrs->http != NULL);
	assert(igrs->soap != NULL);

	ezcfg = igrs->ezcfg;
	http = igrs->http;
	soap = igrs->soap;

	/* build SOAP */
	ezcfg_soap_set_version_major(soap, 1);
	ezcfg_soap_set_version_minor(soap, 2);
	ezcfg_soap_set_envelope(soap, EZCFG_IGRS_ENVELOPE_ELEMENT_NAME);
	ezcfg_soap_add_envelope_attribute(soap, EZCFG_IGRS_ENVELOPE_ATTR_NS_NAME, EZCFG_IGRS_ENVELOPE_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_soap_add_envelope_attribute(soap, EZCFG_IGRS_ENVELOPE_ATTR_ENC_NAME, EZCFG_IGRS_ENVELOPE_ATTR_ENC_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	body_index = ezcfg_soap_set_body(soap, EZCFG_IGRS_BODY_ELEMENT_NAME);

	session_index = ezcfg_soap_add_body_child(soap, body_index, -1, EZCFG_IGRS_SESSION_ELEMENT_NAME, NULL);
	ezcfg_soap_add_body_child_attribute(soap, session_index, EZCFG_IGRS_SESSION_ATTR_NS_NAME, EZCFG_IGRS_SESSION_ATTR_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	child_index = ezcfg_soap_add_body_child(soap, session_index, -1, EZCFG_IGRS_SOURCE_CLIENT_ID_ELEMENT_NAME, "111111");
	child_index = ezcfg_soap_add_body_child(soap, session_index, child_index, EZCFG_IGRS_TARGET_SERVICE_ID_ELEMENT_NAME, "222222");
	child_index = ezcfg_soap_add_body_child(soap, session_index, child_index, EZCFG_IGRS_SEQUENCE_ID_ELEMENT_NAME, "333333");

	userinfo_index = ezcfg_soap_add_body_child(soap, session_index, child_index, EZCFG_IGRS_USER_INFO_ELEMENT_NAME, NULL);
	child_index = ezcfg_soap_add_body_child(soap, userinfo_index, -1, EZCFG_IGRS_SOURCE_USER_ID_ELEMENT_NAME, "igrs-tester");
	child_index = ezcfg_soap_add_body_child(soap, userinfo_index, child_index, EZCFG_IGRS_SERVICE_SECURITY_ID_ELEMENT_NAME, "urn:IGRS:ServiceSecurity:NULL");
	child_index = ezcfg_soap_add_body_child(soap, userinfo_index, child_index, EZCFG_IGRS_TOKEN_ELEMENT_NAME, "");

	buf[0] = '\0';
	n = ezcfg_soap_write(soap, buf, sizeof(buf));
	ezcfg_http_set_message_body(http, buf, n);

	/* build HTTP request line */
	ezcfg_http_set_request_method(http, "M-POST");
	ezcfg_http_set_request_uri(http, "/IGRS");
	ezcfg_http_set_version_major(http, 1);
	ezcfg_http_set_version_minor(http, 1);

	/* build HTTP headers */
	ezcfg_http_add_header(http, "HOST", "192.168.1.1:3880");
	ezcfg_http_add_header(http, "MAN", "\"http://www.igrs.org/session\";ns=01");

	snprintf(buf, sizeof(buf), "IGRS/%d.%d", igrs->version_major, igrs->version_minor);
	ezcfg_http_add_header(http, "01-IGRSVersion", buf);

	ezcfg_http_add_header(http, "01-IGRSMessageType", "CreateSessionRequest");

	snprintf(buf, sizeof(buf), "urn:IGRS:Device:DeviceId:%s", igrs->target_device_id);
	ezcfg_http_add_header(http, "01-TargetDeviceId", buf);

	snprintf(buf, sizeof(buf), "urn:IGRS:Device:DeviceId:%s", igrs->source_device_id);
	ezcfg_http_add_header(http, "01-SourceDeviceId", buf);

	snprintf(buf, sizeof(buf), "%u", igrs->sequence_id);
	ezcfg_http_add_header(http, "01-SequenceId", buf);

	ezcfg_http_add_header(http, "Content-type", "text/xml;charset=utf-8");

	snprintf(buf, sizeof(buf), "%u", ezcfg_http_get_message_body_len(http));
	ezcfg_http_add_header(http, "Content-length", buf);

	ezcfg_http_add_header(http, "MAN", "\"http://www.w3.org/2002/12/soap-envelope\";ns=02");
	ezcfg_http_add_header(http, "02-SoapAction", "\"IGRS-CreateSession-Request\"");

	return true;
}

static int write_create_session_request(struct ezcfg_igrs *igrs, char *buf, int len)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_soap *soap;

	char *p;
	int n;

	assert(igrs != NULL);
	assert(igrs->http != NULL);
	assert(igrs->soap != NULL);
	assert(buf != NULL);
	assert(len > 0);

	ezcfg = igrs->ezcfg;
	http = igrs->http;
	soap = igrs->soap;

	p = buf;
	n = 0;
	n = ezcfg_http_write_request_line(http, p, len);
	if (n < 0) {
		err(ezcfg, "ezcfg_http_write_request_line\n");
		return n;
	}
	p += n;
	len -= n;
	n = ezcfg_http_write_headers(http, p, len);
	if (n < 0) {
		err(ezcfg, "ezcfg_http_write_headers\n");
		return n;
	}
	p += n;
	len -= n;

	if (len < 2) {
		err(ezcfg, "buffer is to small for igrs message\n");
		return -1;
	}

	p[0] = '\r'; p[1] = '\n'; /* add CRLF for HTTP */
	p += 2;
	len -= 2;

	n = ezcfg_http_write_message_body(http, p, len);
	if (n < 0) {
		err(ezcfg, "ezcfg_http_write_message_body\n");
		return n;
	}

	return (p-buf)+n;
}

/**
 * Public functions
 **/
void ezcfg_igrs_delete(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;

	assert(igrs != NULL);

	ezcfg = igrs->ezcfg;

	if (igrs->http != NULL) {
		ezcfg_http_delete(igrs->http);
	}

	if (igrs->soap != NULL) {
		ezcfg_soap_delete(igrs->soap);
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

	assert(ezcfg != NULL);

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

	assert(igrs != NULL);

	ezcfg = igrs->ezcfg;

}

bool ezcfg_igrs_set_message_type_ops(struct ezcfg_igrs *igrs, const struct ezcfg_igrs_msg_op *message_type_ops, unsigned short num_message_types)
{
	struct ezcfg *ezcfg;

	assert(igrs != NULL);
	assert(message_type_ops != NULL);

	ezcfg = igrs->ezcfg;

	igrs->num_message_types = num_message_types;
	igrs->message_type_ops = message_type_ops;

	return true;
}

unsigned short ezcfg_igrs_get_version_major(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;

	assert(igrs != NULL);

	ezcfg = igrs->ezcfg;

	return igrs->version_major;
}

unsigned short ezcfg_igrs_get_version_minor(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;

	assert(igrs != NULL);

	ezcfg = igrs->ezcfg;

	return igrs->version_minor;
}

bool ezcfg_igrs_set_version_major(struct ezcfg_igrs *igrs, unsigned short major)
{
	struct ezcfg *ezcfg;

	assert(igrs != NULL);

	ezcfg = igrs->ezcfg;

	igrs->version_major = major;

	return true;
}

bool ezcfg_igrs_set_version_minor(struct ezcfg_igrs *igrs, unsigned short minor)
{
	struct ezcfg *ezcfg;

	assert(igrs != NULL);

	ezcfg = igrs->ezcfg;

	igrs->version_minor = minor;

	return true;
}

bool ezcfg_igrs_set_source_device_id(struct ezcfg_igrs *igrs, const char *uuid_str)
{
	struct ezcfg *ezcfg;

	assert(igrs != NULL);
	assert(uuid_str != NULL);

	ezcfg = igrs->ezcfg;

	snprintf(igrs->source_device_id, EZCFG_UUID_STRING_LEN+1, "%s", uuid_str);
	return true;
}

char *ezcfg_igrs_get_source_device_id(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;

	assert(igrs != NULL);

	ezcfg = igrs->ezcfg;

	return igrs->source_device_id;
}

bool ezcfg_igrs_set_target_device_id(struct ezcfg_igrs *igrs, const char *uuid_str)
{
	struct ezcfg *ezcfg;

	assert(igrs != NULL);
	assert(uuid_str != NULL);

	ezcfg = igrs->ezcfg;

	snprintf(igrs->target_device_id, EZCFG_UUID_STRING_LEN+1, "%s", uuid_str);
	return true;
}

char *ezcfg_igrs_get_target_device_id(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;

	assert(igrs != NULL);

	ezcfg = igrs->ezcfg;

	return igrs->target_device_id;
}

bool ezcfg_igrs_set_sequence_id(struct ezcfg_igrs *igrs, unsigned int seq_id)
{
	struct ezcfg *ezcfg;

	assert(igrs != NULL);
	assert(seq_id > 0);

	ezcfg = igrs->ezcfg;

	igrs->sequence_id = seq_id;
	return true;
}

unsigned int ezcfg_igrs_get_sequence_id(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;

	assert(igrs != NULL);

	ezcfg = igrs->ezcfg;

	return igrs->sequence_id;
}

bool ezcfg_igrs_build_message(struct ezcfg_igrs *igrs, const char *type)
{
	struct ezcfg *ezcfg;
	const struct ezcfg_igrs_msg_op *op;
	int i;

	assert(igrs != NULL);
	assert(type != NULL);

	ezcfg = igrs->ezcfg;

	dbg(ezcfg, "debug num_message_types=[%d]\n", igrs->num_message_types);
	for (i = igrs->num_message_types; i > 0; i--) {
	dbg(ezcfg, "debug i=[%d]\n", i);
		op = &(igrs->message_type_ops[i]);
		if ( strcmp(op->name, type) == 0) {
			if (op->builder != NULL) {
				igrs->message_type_index = i;
				return op->builder(igrs);
			}
		}
	}
	return false;
}

int ezcfg_igrs_write_message(struct ezcfg_igrs *igrs, char *buf, int len)
{
	struct ezcfg *ezcfg;
	const struct ezcfg_igrs_msg_op *op;

	assert(igrs != NULL);
	assert(buf != NULL);
	assert(len > 0);

	ezcfg = igrs->ezcfg;

	if (igrs->message_type_index == 0) {
		err(ezcfg, "unknown igrs message type\n");
		return -1;
	}

	op = &(igrs->message_type_ops[igrs->message_type_index]);

	if (op->writer != NULL) {
		info(ezcfg, "write %s\n", op->name);
		return op->writer(igrs, buf, len);
	}
	return -1;
}
