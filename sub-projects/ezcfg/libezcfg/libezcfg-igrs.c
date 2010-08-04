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

static bool build_create_session_request(struct ezcfg_igrs *igrs);

static const struct ezcfg_igrs_msg_op default_message_type_ops[] = {
	/* bad Message Type string */
	{ NULL, NULL },
	/* 9.1 Device Advertisement */
	{ "DeviceOnlineAdvertisement", NULL },
	{ "DeviceOfflineAdvertisement", NULL },
	/* 9.2 Device Pipe Management */
	{ "CreatePipeRequest", NULL },
	{ "CreatePipeResponse", NULL },
	{ "AuthenticateRequest", NULL },
	{ "AuthenticateResponse", NULL },
	{ "AuthenticateResultRequest", NULL },
	{ "AuthenticateResultResponse", NULL },
	{ "CreatePipeResultRequest", NULL },
	{ "CreatePipeResultResponse", NULL },
	{ "DetachPipeNotify", NULL },
	{ "DeviceOnlineDetectionRequest", NULL },
	{ "DeviceOnlineDetectionResponse", NULL },
	/* 9.3 Detaild Device Description Document Retrieval */
	{ "GetDeviceDescriptionRequest", NULL },
	{ "GetDeviceDescriptionResponse", NULL },
	/* 9.4 Retrieve Detailed Device Description Document Based on Non-Secure Pipe */
	/* 9.5 Device Group Setup */
	{ "PeerDeviceGroupAdvertisement", NULL },
	{ "QuitPeerDeviceGroupNotify", NULL },
	{ "CentralisedDeviceGroupAdvertisement", NULL },
	{ "JoinCentralisedDeviceGroupRequest", NULL },
	{ "JoinCentralisedDeviceGroupResponse", NULL },
	{ "QuitCentralisedDeviceGroupAdvertisement", NULL },
	{ "QuitCentralisedDeviceGroupNotify", NULL },
	/* 9.6 Device Search */
	{ "SearchDeviceRequest", NULL },
	{ "SearchDeviceResponse", NULL },
	{ "SearchDeviceRequestOnDevice", NULL },
	{ "SearchDeviceResponseOnDevice", NULL },
	/* 9.7 Device Online/Offline Event Subscription */
	{ "SubscribeDeviceEventRequest", NULL },
	{ "RenewSubscriptionDeviceEventRequest", NULL },
	{ "SubscribeDeviceEventResponse", NULL },
	{ "UnSubscribeDeviceEventNotify", NULL },
	{ "NotifyDeviceEvent", NULL },
	/* 9.8 Device Group Search */
	{ "SearchDeviceGroupRequest", NULL },
	{ "SearchDeviceGroupResponse", NULL },
	/* 10.1 Service Online/Offline Advertisement */
	{ "ServiceOnlineAdvertisement", NULL },
	{ "ServiceOfflineAdvertisement", NULL },
	{ "RegisterServiceNotify", NULL },
	{ "UnRegisterServiceNotify", NULL },
	/* 10.2 Service Search */
	{ "SearchServiceRequest", NULL },
	{ "SearchServiceResponse", NULL },
	{ "SearchServiceRequestOnDevice", NULL },
	{ "SearchServiceResponseOnDevice", NULL },
	/* 10.3 Service Online/Offline Event Subscription */
	{ "SubscribeServiceEventRequest", NULL },
	{ "RenewSubscriptionServiceEventRequest", NULL },
	{ "SubscribeServiceEventResponse", NULL },
	{ "UnSubscribeDeviceEventNotify", NULL },
	{ "NotifyServiceEvent", NULL },
	/* 10.4 Service Description Document Retrieval */
	{ "GetServiceDescriptionRequest", NULL },
	{ "GetServiceDescriptionResponse", NULL },
	/* 10.5 Session */
	{ "CreateSessionRequest", build_create_session_request },
	{ "CreateSessionResponse", NULL },
	{ "DestroySessionNotify", NULL },
	{ "ApplySessionKeyRequest", NULL },
	{ "ApplySessionKeyResponse", NULL },
	{ "TransferSessionKeyRequest", NULL },
	{ "TransferSessionKeyResponse", NULL },
	/* 10.6 Service Invocation */
	{ "InvokeServiceRequest", NULL },
	{ "InvokeServiceResponse", NULL },
	{ "SendNotification", NULL },
};

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
 *
 * Create ezcfg igrs protocol data structure
 *
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
	ezcfg_igrs_set_message_type_ops(igrs, default_message_type_ops, ARRAY_SIZE(default_message_type_ops) - 1);
	return igrs;
}

void ezcfg_igrs_dump(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;

	assert(igrs != NULL);

	ezcfg = igrs->ezcfg;

}

static bool build_create_session_request(struct ezcfg_igrs *igrs)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_soap *soap;

	assert(igrs != NULL);
	assert(igrs->http != NULL);
	assert(igrs->soap != NULL);

	ezcfg = igrs->ezcfg;
	http = igrs->http;
	soap = igrs->soap;

	ezcfg_http_set_request_method(http, "M-POST");
	ezcfg_http_set_request_uri(http, "/IGRS");
	ezcfg_http_set_version_major(http, 1);
	ezcfg_http_set_version_minor(http, 1);

	return true;
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
				return op->builder(igrs);
			}
		}
	}
	return false;
}
