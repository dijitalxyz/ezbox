/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-igrs.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-29   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_IGRS_H_
#define _EZCFG_IGRS_H_

#include "ezcfg.h"

struct ezcfg_igrs_msg_op {
	char *name;
	bool (*build_fn)(struct ezcfg_igrs *igrs);
	int (*write_fn)(struct ezcfg_igrs *igrs, char *buf, int len);
	int (*handle_fn)(struct ezcfg_igrs *igrs);
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
	unsigned int target_client_id; /* 0 reserved */
	unsigned int target_service_id; /* 0 reserved */
	unsigned int source_service_id; /* 0 reserved */
	unsigned int sequence_id; /* 0 reserved */
	unsigned int acknowledge_id; /* 0 reserved */
	unsigned short return_code; /* 0 reserved */

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

/* igrs/igrs_create_session.c */
bool ezcfg_igrs_build_create_session_request(struct ezcfg_igrs *igrs);
int ezcfg_igrs_write_create_session_request(struct ezcfg_igrs *igrs, char *buf, int len);
int ezcfg_igrs_handle_create_session_request(struct ezcfg_igrs *igrs);

/* igrs/igrs_invoke_service.c */
bool ezcfg_igrs_build_invoke_service_request(struct ezcfg_igrs *igrs);
int ezcfg_igrs_write_invoke_service_request(struct ezcfg_igrs *igrs, char *buf, int len);
int ezcfg_igrs_handle_invoke_service_request(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_build_invoke_service_response(struct ezcfg_igrs *igrs);
int ezcfg_igrs_write_invoke_service_response(struct ezcfg_igrs *igrs, char *buf, int len);
int ezcfg_igrs_handle_invoke_service_response(struct ezcfg_igrs *igrs);

#endif

