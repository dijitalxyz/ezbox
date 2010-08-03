/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-12   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _LIBEZCFG_H_
#define _LIBEZCFG_H_

#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EZCFG_INVALID_SOCKET	-1

#define EZCFG_NVRAM_SPACE	0x20000 /* 128K Bytes */
#define EZCFG_SOCKET_DIR	"/tmp/ezcfg"
#define EZCFG_SOCKET_PATH	"/tmp/ezcfg/ezcfg.sock"
//#define EZCFG_CTRL_SOCK_PATH	"@/org/kernel/ezcfg/ctrl"
#define EZCFG_CTRL_SOCK_PATH	"/tmp/ezcfg/ctrl"
//#define EZCFG_MASTER_SOCK_PATH	"@/org/kernel/ezcfg/master"
#define EZCFG_MASTER_SOCK_PATH	"/tmp/ezcfg/master"
#define EZCFG_MASTER_SOCKET_QUEUE_LENGTH	20

/* ezcfg supported protocols */
#define EZCFG_PROTO_UNKNOWN	0
#define EZCFG_PROTO_HTTP	1
#define EZCFG_PROTO_IGRS	2
#define EZCFG_PROTO_ISDP	3

/* ezcfg xml definitions */
#define EZCFG_XML_MAX_ELEMENTS	128
#define EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL 0
#define EZCFG_XML_ELEMENT_ATTRIBUTE_HEAD 1

/* ezcfg soap element names */
#define EZCFG_SOAP_ENV_ELEMENT_NAME	"SOAP-ENV:Envelope"
#define EZCFG_SOAP_BODY_ELEMENT_NAME	"SOAP-ENV:Body"
#define EZCFG_SOAP_ENV_NS_NAME		"xmlns:SOAP-ENV"
#define EZCFG_SOAP_ENV_NS_VALUE		"http://www.w3.org/2002/12/soap-envelope"
#define EZCFG_SOAP_ENV_ENC_NAME		"SOAP-ENV:encodingStyle"
#define EZCFG_SOAP_ENV_ENC_VALUE	"http://schemas.xmlsoap.org/soap/encoding/"

/*
 * ezcfg - library context
 *
 * load/save the ezbox config and system environment
 * allows custom logging
 */

/*
 * libezcfg.c
 * ezbox config context
 */
struct ezcfg;

void ezcfg_set_log_fn(struct ezcfg *ezcfg,
                      void (*log_fn)(struct ezcfg *ezcfg,
                                    int priority, const char *file, int line, const char *fn,
                                    const char *format, va_list args));
int ezcfg_get_log_priority(struct ezcfg *ezcfg);
void ezcfg_set_log_priority(struct ezcfg *ezcfg, int priority);

struct ezcfg *ezcfg_new(void);
void ezcfg_delete(struct ezcfg *ezcfg);

/*
 * libezcfg-list.c
 * ezcfg_list
 *
 * access to libezcfg generated lists
 */
struct ezcfg_list_entry;
struct ezcfg_list_entry *ezcfg_list_entry_get_next(struct ezcfg_list_entry *list_entry);
struct ezcfg_list_entry *ezcfg_list_entry_get_by_name(struct ezcfg_list_entry *list_entry, const char *name);
const char *ezcfg_list_entry_get_name(struct ezcfg_list_entry *list_entry);
const char *ezcfg_list_entry_get_value(struct ezcfg_list_entry *list_entry);

/**
 * ezcfg_list_entry_foreach:
 * @list_entry: entry to store the current position
 * @first_entry: first entry to start with
 *
 * Helper to iterate over all entries of a list.
 */
#define ezcfg_list_entry_foreach(list_entry, first_entry) \
	for (list_entry = first_entry; \
	     list_entry != NULL; \
	     list_entry = ezcfg_list_entry_get_next(list_entry))

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
