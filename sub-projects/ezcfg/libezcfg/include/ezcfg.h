/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-12   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_H_
#define _EZCFG_H_

#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <syslog.h>
#include <sys/stat.h>

#include "ezcfg-ezbox_distro.h"
#include "ezcfg-nvram_defaults.h"

#ifdef EZCFG_DEBUG
#include <assert.h>
#define ASSERT(exp) assert(exp)   
#else
#define ASSERT(exp) do {} while(0) 
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define EZBOX_PROJECT_HOME_PAGE_URI	"http://code.google.com/p/ezbox/"

#define EZCFG_INVALID_SOCKET      -1
#define EZCFG_BUFFER_SIZE         8192
#define EZCFG_PATH_MAX            128
#define EZCFG_LOCALE_MAX          32

#define EZCFG_CONFIG_FILE_PATH	SYSCONFDIR "/ezcfg.conf"
#define EZCFG_NVRAM_CONFIG_FILE_PATH	SYSCONFDIR "/nvram.conf"

#define EZCFG_ROOT_PATH		"/var/ezcfg"
/* semaphore path */
#define EZCFG_SEM_ROOT_PATH	EZCFG_ROOT_PATH "/sem"
#define EZCFG_SEM_EZCFG_PATH	EZCFG_SEM_ROOT_PATH "/ezcfg"
#define EZCFG_SEM_PROJID_EZCFG	'e'

#define EZCFG_SEM_NVRAM_INDEX	0
#define EZCFG_SEM_RC_INDEX	1
#define EZCFG_SEM_NUMBER	2

/* socket path */
#define EZCFG_SOCK_ROOT_PATH	"@/org/ezbox/ezcfg/sock"
#define EZCFG_SOCK_CTRL_PATH	EZCFG_SOCK_ROOT_PATH "/ctrl"
#define EZCFG_SOCK_NVRAM_PATH	EZCFG_SOCK_ROOT_PATH "/nvram"
#define EZCFG_SOCK_UEVENT_PATH	EZCFG_SOCK_ROOT_PATH "/uevent"
#define EZCFG_SOCK_MASTER_PATH	EZCFG_SOCK_ROOT_PATH "/master"

/* thread config definitions */
#define EZCFG_MASTER_SOCKET_QUEUE_LENGTH	20
#define EZCFG_MASTER_WAIT_TIME	5
#define EZCFG_WORKER_WAIT_TIME	1

/* ezcfg common definitions */
#define EZCFG_COMMON_LOG_LEVEL_ERR_STRING     "err"
#define EZCFG_COMMON_LOG_LEVEL_INFO_STRING    "info"
#define EZCFG_COMMON_LOG_LEVEL_DEBUG_STRING   "debug"
#define EZCFG_COMMON_DEFAULT_RULES_PATH       SYSCONFDIR "/ezcfg.rules"
#define EZCFG_COMMON_DEFAULT_LOCALE_STRING    "zh_CN.UTF-8"

/* ezcfg nvram definitions */
#define EZCFG_NVRAM_BUFFER_SIZE            0x10000 /* 64K Bytes */
#define EZCFG_NVRAM_BUFFER_SIZE_STRING     "65536" /* 64K Bytes */
#define EZCFG_NVRAM_BACKEND_NONE           0
#define EZCFG_NVRAM_BACKEND_NONE_STRING    "0"
#define EZCFG_NVRAM_BACKEND_FILE           1
#define EZCFG_NVRAM_BACKEND_FILE_STRING    "1"
#define EZCFG_NVRAM_BACKEND_FLASH          2
#define EZCFG_NVRAM_BACKEND_FLASH_STRING   "2"
#define EZCFG_NVRAM_BACKEND_HDD            3
#define EZCFG_NVRAM_BACKEND_HDD_STRING     "3"
#define EZCFG_NVRAM_CODING_NONE            0
#define EZCFG_NVRAM_CODING_NONE_STRING     "0"
#define EZCFG_NVRAM_CODING_GZIP            1
#define EZCFG_NVRAM_CODING_GZIP_STRING     "1"
#define EZCFG_NVRAM_STORAGE_PATH           EZCFG_ROOT_PATH "/nvram.bin"
#define EZCFG_NVRAM_BACKUP_STORAGE_PATH    EZCFG_ROOT_PATH "/nvram_backup.bin"
#define EZCFG_NVRAM_STORAGE_NUM            2

/* ezcfg authentication type string */
#define EZCFG_AUTH_TYPE_HTTP_BASIC_STRING  "http-basic"
#define EZCFG_AUTH_TYPE_HTTP_DIGEST_STRING "http-digest"


/* ezcfg rc act string */
#define EZCFG_RC_ACT_START                 "start"
#define EZCFG_RC_ACT_STOP                  "stop"
#define EZCFG_RC_ACT_RESTART               "restart"
#define EZCFG_RC_ACT_RELOAD                "reload"


/* ezcfg rc service string */
#define EZCFG_RC_SERVICE_SYSTEM            "system"
#define EZCFG_RC_SERVICE_LOGIN             "login"
#define EZCFG_RC_SERVICE_EZCFG_HTTPD       "ezcfg_httpd"
#define EZCFG_RC_SERVICE_TELNETD           "telnetd"
#define EZCFG_RC_SERVICE_EMC2              "emc2"
#define EZCFG_RC_SERVICE_EMC2_LATENCY_TEST "emc2_latency_test"

/* ezcfg service binding type */
#define EZCFG_SERVICE_BINDING_UNKNOWN	0
#define EZCFG_SERVICE_BINDING_LAN	1
#define EZCFG_SERVICE_BINDING_WAN	2


/* ezcfg socket domain string */
#define EZCFG_SOCKET_DOMAIN_LOCAL_STRING   "local"
#define EZCFG_SOCKET_DOMAIN_INET_STRING    "inet"
#define EZCFG_SOCKET_DOMAIN_INET6_STRING   "inet6"

/* ezcfg socket type string */
#define EZCFG_SOCKET_TYPE_STREAM_STRING    "stream"
#define EZCFG_SOCKET_TYPE_DGRAM_STRING     "dgram"
#define EZCFG_SOCKET_TYPE_RAW_STRING       "raw"

/* ezcfg supported protocols */
#define EZCFG_PROTO_UNKNOWN	0
#define EZCFG_PROTO_HTTP	1
#define EZCFG_PROTO_SOAP_HTTP	2
#define EZCFG_PROTO_IGRS	3
#define EZCFG_PROTO_ISDP	4
#define EZCFG_PROTO_UEVENT	5
#define EZCFG_SOCKET_PROTO_UNKNOWN_STRING         "0"
#define EZCFG_SOCKET_PROTO_HTTP_STRING            "1"
#define EZCFG_SOCKET_PROTO_SOAP_HTTP_STRING       "2"
#define EZCFG_SOCKET_PROTO_IGRS_STRING            "3"
#define EZCFG_SOCKET_PROTO_ISDP_STRING            "4"
#define EZCFG_SOCKET_PROTO_UEVENT_STRING          "5"

#define EZCFG_PROTO_HTTP_PORT_NUMBER              80
#define EZCFG_PROTO_HTTP_PORT_NUMBER_STRING       "80"


/* ezcfg xml definitions */
#define EZCFG_XML_MAX_ELEMENTS	         128
#define EZCFG_XML_ENLARGE_SIZE           16
#define EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL 0
#define EZCFG_XML_ELEMENT_ATTRIBUTE_HEAD 1


/* ezcfg css definitions */
#define EZCFG_CSS_RULE_TYPE_RULE_SET        0
#define EZCFG_CSS_RULE_TYPE_AT_RULE         1


/* ezcfg soap element names */
#define EZCFG_SOAP_ENVELOPE_ELEMENT_NAME    "env:Envelope"
#define EZCFG_SOAP_HEADER_ELEMENT_NAME      "env:Header"
#define EZCFG_SOAP_BODY_ELEMENT_NAME        "env:Body"
#define EZCFG_SOAP_FAULT_ELEMENT_NAME       "env:Fault"
#define EZCFG_SOAP_CODE_ELEMENT_NAME        "env:Code"
#define EZCFG_SOAP_VALUE_ELEMENT_NAME       "env:Value"
#define EZCFG_SOAP_VALUE_ELEMENT_VALUE      "env:Sender"
#define EZCFG_SOAP_SUBCODE_ELEMENT_NAME     "env:Subcode"
#define EZCFG_SOAP_REASON_ELEMENT_NAME      "env:Reason"
#define EZCFG_SOAP_TEXT_ELEMENT_NAME        "env:Text"
#define EZCFG_SOAP_DETAIL_ELEMENT_NAME      "env:Detail"
#define EZCFG_SOAP_NODE_ELEMENT_NAME        "env:Node"
#define EZCFG_SOAP_ROLE_ELEMENT_NAME        "env:Role"
#define EZCFG_SOAP_ENVELOPE_ATTR_NS_NAME    "xmlns:env"
#define EZCFG_SOAP_ENVELOPE_ATTR_NS_VALUE   "http://www.w3.org/2003/05/soap-envelope"
#define EZCFG_SOAP_ENVELOPE_ATTR_ENC_NAME   "env:encodingStyle"
#define EZCFG_SOAP_ENVELOPE_ATTR_ENC_VALUE  "http://www.w3.org/2003/05/soap-encoding"

#define EZCFG_SOAP_NVRAM_GETNV_ELEMENT_NAME              "nvns:getNvram"
#define EZCFG_SOAP_NVRAM_GETNV_RESPONSE_ELEMENT_NAME     "nvns:getNvramResponse"
#define EZCFG_SOAP_NVRAM_SETNV_ELEMENT_NAME              "nvns:setNvram"
#define EZCFG_SOAP_NVRAM_SETNV_RESPONSE_ELEMENT_NAME     "nvns:setNvramResponse"
#define EZCFG_SOAP_NVRAM_UNSETNV_ELEMENT_NAME            "nvns:unsetNvram"
#define EZCFG_SOAP_NVRAM_UNSETNV_RESPONSE_ELEMENT_NAME   "nvns:unsetNvramResponse"
#define EZCFG_SOAP_NVRAM_SETMNV_ELEMENT_NAME             "nvns:setMultiNvram"
#define EZCFG_SOAP_NVRAM_SETMNV_RESPONSE_ELEMENT_NAME    "nvns:setMultiNvramResponse"
#define EZCFG_SOAP_NVRAM_LISTNV_ELEMENT_NAME             "nvns:listNvram"
#define EZCFG_SOAP_NVRAM_LISTNV_RESPONSE_ELEMENT_NAME    "nvns:listNvramResponse"
#define EZCFG_SOAP_NVRAM_COMMITNV_ELEMENT_NAME           "nvns:commitNvram"
#define EZCFG_SOAP_NVRAM_COMMITNV_RESPONSE_ELEMENT_NAME  "nvns:commitNvramResponse"
#define EZCFG_SOAP_NVRAM_INFONV_ELEMENT_NAME             "nvns:infoNvram"
#define EZCFG_SOAP_NVRAM_INFONV_RESPONSE_ELEMENT_NAME    "nvns:infoNvramResponse"
#define EZCFG_SOAP_NVRAM_INSERT_SOCKET_ELEMENT_NAME      "nvns:insertSocket"
#define EZCFG_SOAP_NVRAM_INSERT_SOCKET_RESPONSE_ELEMENT_NAME  \
	"nvns:insertSocketNvramResponse"
#define EZCFG_SOAP_NVRAM_REMOVE_SOCKET_ELEMENT_NAME      "nvns:removeSocket"
#define EZCFG_SOAP_NVRAM_REMOVE_SOCKET_RESPONSE_ELEMENT_NAME  \
	"nvns:removeSocketNvramResponse"
#define EZCFG_SOAP_NVRAM_NAME_ELEMENT_NAME               "nvns:name"
#define EZCFG_SOAP_NVRAM_VALUE_ELEMENT_NAME              "nvns:value"
#define EZCFG_SOAP_NVRAM_RESULT_ELEMENT_NAME             "nvns:result"
#define EZCFG_SOAP_NVRAM_NVRAM_ELEMENT_NAME              "nvns:nvram"
#define EZCFG_SOAP_NVRAM_ATTR_NS_NAME                    "xmlns:nvns"
#define EZCFG_SOAP_NVRAM_ATTR_NS_VALUE                   "http://www.ezidc.net/ezcfg/nvram/schemas"
#define EZCFG_SOAP_NVRAM_INVALID_NAME_FAULT_VALUE        "NVRAM Name is Invalid"
#define EZCFG_SOAP_NVRAM_INVALID_VALUE_FAULT_VALUE       "NVRAM Value is Invalid"
#define EZCFG_SOAP_NVRAM_OPERATION_FAIL_FAULT_VALUE      "NVRAM Operation Fail"
#define EZCFG_SOAP_NVRAM_RESULT_VALUE_OK                 "OK"
#define EZCFG_SOAP_NVRAM_RESULT_VALUE_ERROR              "ERROR"


/* ezcfg http definitions */
#define EZCFG_HTTP_MAX_HEADERS         64 /* must be less than 256 */
#define EZCFG_HTTP_CHUNK_SIZE          20480
#define EZCFG_HTTP_MAX_CHUNK_NUM       32
#define EZCFG_HTTP_MAX_REQUEST_SIZE    (EZCFG_HTTP_CHUNK_SIZE * EZCFG_HTTP_MAX_CHUNK_NUM)
#define EZCFG_HTTP_MAX_RESPONSE_SIZE   EZCFG_HTTP_MAX_REQUEST_SIZE
/* ezcfg http request/response mode */
#define EZCFG_HTTP_MODE_REQUEST        0
#define EZCFG_HTTP_MODE_RESPONSE       1


/* ezcfg HTTP html request/response mode */
#define EZCFG_HTTP_HTML_HOME_INDEX_URI      "/"
#define EZCFG_HTTP_HTML_LANG_DIR            DATADIR "/ezcfg/html/lang"
#define EZCFG_HTTP_HTML_INDEX_DOMAIN        "index"


/* ezcfg HTTP html admin request/response mode */
#define EZCFG_HTTP_HTML_ADMIN_PREFIX_URI    "/admin/"

/* ezcfg HTTP html admin authentication default string */
#define EZCFG_AUTH_USER_ADMIN_STRING       "root"
#define EZCFG_AUTH_REALM_ADMIN_STRING      "ezbox"
#define EZCFG_AUTH_DOMAIN_ADMIN_STRING     EZCFG_HTTP_HTML_ADMIN_PREFIX_URI
#define EZCFG_AUTH_SECRET_ADMIN_STRING     "admin"


/* ezcfg uuid definitions */
#define EZCFG_UUID_BINARY_LEN	16 /* 128/8 */
#define EZCFG_UUID_STRING_LEN	36 /* 8+1+4+1+4+1+4+1+12 */
#define EZCFG_UUID_NIL_STRING	"00000000-0000-0000-0000-000000000000"


/* ezcfg nvram SOAP/HTTP binding */


/* ezcfg SOAP/HTTP request/response mode */
#define EZCFG_SOAP_HTTP_MODE_REQUEST        EZCFG_HTTP_MODE_REQUEST
#define EZCFG_SOAP_HTTP_MODE_RESPONSE       EZCFG_HTTP_MODE_RESPONSE
#define EZCFG_SOAP_HTTP_CHUNK_SIZE          20480
#define EZCFG_SOAP_HTTP_MAX_CHUNK_NUM       32
/* ezcfg SOAP/HTTP request size */
#define EZCFG_SOAP_HTTP_MAX_REQUEST_SIZE    (EZCFG_SOAP_HTTP_CHUNK_SIZE * EZCFG_SOAP_HTTP_MAX_CHUNK_NUM)
/* ezcfg SOAP/HTTP response size */
#define EZCFG_SOAP_HTTP_MAX_RESPONSE_SIZE   EZCFG_SOAP_HTTP_MAX_REQUEST_SIZE


/* ezcfg igrs request/response mode */
#define EZCFG_IGRS_HTTP_MODE_REQUEST        EZCFG_HTTP_MODE_REQUEST
#define EZCFG_IGRS_HTTP_MODE_RESPONSE       EZCFG_HTTP_MODE_RESPONSE
#define EZCFG_IGRS_HTTP_CHUNK_SIZE          20480
#define EZCFG_IGRS_HTTP_MAX_CHUNK_NUM       32
/* ezcfg igrs/HTTP request size */
#define EZCFG_IGRS_HTTP_MAX_REQUEST_SIZE    (EZCFG_IGRS_HTTP_CHUNK_SIZE * EZCFG_IGRS_HTTP_MAX_CHUNK_NUM)
/* ezcfg igrs/HTTP response size */
#define EZCFG_IGRS_HTTP_MAX_RESPONSE_SIZE   EZCFG_IGRS_HTTP_MAX_REQUEST_SIZE


/*
 * ezcfg - library context
 *
 * load/save the ezbox config and system environment
 * allows custom logging
 */

/*
 * common/ezcfg.c
 * ezbox config context
 */
struct ezcfg;

void ezcfg_log(struct ezcfg *ezcfg,
               int priority, const char *file, int line, const char *fn,
               const char *format, ...)
               __attribute__((format(printf, 6, 7)));

static inline void __attribute__((always_inline, format(printf, 2, 3)))
ezcfg_log_null(struct ezcfg *ezcfg, const char *format, ...) {}

#define ezcfg_log_cond(ezcfg, prio, arg...) \
  do { \
    if (ezcfg_common_get_log_priority(ezcfg) >= prio) \
      ezcfg_log(ezcfg, prio, __FILE__, __LINE__, __FUNCTION__, ## arg); \
  } while (0)

/* FIXME: remove it later */
#define ENABLE_LOGGING	1
#define ENABLE_DEBUG	1

#ifdef ENABLE_LOGGING
#  ifdef ENABLE_DEBUG
#    define dbg(ezcfg, arg...) ezcfg_log_cond(ezcfg, LOG_DEBUG, ## arg)
#  else
#    define dbg(ezcfg, arg...) ezcfg_log_null(ezcfg, ## arg)
#  endif
#  define info(ezcfg, arg...) ezcfg_log_cond(ezcfg, LOG_INFO, ## arg)
#  define err(ezcfg, arg...) ezcfg_log_cond(ezcfg, LOG_ERR, ## arg)
#else
#  define dbg(ezcfg, arg...) ezcfg_log_null(ezcfg, ## arg)
#  define info(ezcfg, arg...) ezcfg_log_null(ezcfg, ## arg)
#  define err(ezcfg, arg...) ezcfg_log_null(ezcfg, ## arg)
#endif


static inline void ezcfg_log_init(const char *program_name)
{
	openlog(program_name, LOG_PID | LOG_CONS, LOG_DAEMON);
}

static inline void ezcfg_log_close(void)
{
	closelog();
}

void ezcfg_common_set_log_fn(struct ezcfg *ezcfg,
                      void (*log_fn)(struct ezcfg *ezcfg,
                                    int priority, const char *file, int line, const char *fn,
                                    const char *format, va_list args));
int ezcfg_common_get_log_priority(struct ezcfg *ezcfg);
void ezcfg_common_set_log_priority(struct ezcfg *ezcfg, int priority);

struct ezcfg *ezcfg_new(char *path);
void ezcfg_delete(struct ezcfg *ezcfg);

/*
 * commom/list.c
 * ezcfg_list
 *
 * access to ezcfg generated lists
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

/* thread/master.c */
struct ezcfg_master;
struct ezcfg_master *ezcfg_master_start(struct ezcfg *ezcfg);
void ezcfg_master_stop(struct ezcfg_master *master);
void ezcfg_master_reload(struct ezcfg_master *master);
void ezcfg_master_set_threads_max(struct ezcfg_master *master, int threads_max);

/* uevent/uevent.c */
struct ezcfg_uevent;
void ezcfg_uevent_delete(struct ezcfg_uevent *uevent);
struct ezcfg_uevent *ezcfg_uevent_new(struct ezcfg *ezcfg);

/* igrs/igrs.c */
struct ezcfg_igrs;
void ezcfg_igrs_delete(struct ezcfg_igrs *igrs);
struct ezcfg_igrs *ezcfg_igrs_new(struct ezcfg *ezcfg);
bool ezcfg_igrs_set_version_major(struct ezcfg_igrs *igrs, unsigned short major);
bool ezcfg_igrs_set_version_minor(struct ezcfg_igrs *igrs, unsigned short minor);
int ezcfg_igrs_get_message_length(struct ezcfg_igrs *igrs);
unsigned short ezcfg_igrs_set_message_type(struct ezcfg_igrs *igrs, const char *type);
bool ezcfg_igrs_set_source_device_id(struct ezcfg_igrs *igrs, const char *uuid_str);
char *ezcfg_igrs_get_source_device_id(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_target_device_id(struct ezcfg_igrs *igrs, const char *uuid_str);
char *ezcfg_igrs_get_target_device_id(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_sequence_id(struct ezcfg_igrs *igrs, unsigned int seq_id);
unsigned int ezcfg_igrs_get_sequence_id(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_source_user_id(struct ezcfg_igrs *igrs, const char *user_id);
char *ezcfg_igrs_get_source_user_id(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_service_security_id(struct ezcfg_igrs *igrs, const char *security_id);
char *ezcfg_igrs_get_service_security_id(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_source_client_id(struct ezcfg_igrs *igrs, unsigned int client_id);
unsigned int ezcfg_igrs_get_source_client_id(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_source_service_id(struct ezcfg_igrs *igrs, unsigned int service_id);
unsigned int ezcfg_igrs_get_source_service_id(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_target_service_id(struct ezcfg_igrs *igrs, unsigned int service_id);
unsigned int ezcfg_igrs_get_target_service_id(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_invoke_args(struct ezcfg_igrs *igrs, const char *invoke_args);
char *ezcfg_igrs_get_invoke_args(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_build_message(struct ezcfg_igrs *igrs);
int ezcfg_igrs_write_message(struct ezcfg_igrs *igrs, char *buf, int len);

/* uuid/uuid.c */
struct ezcfg_uuid;
struct ezcfg_uuid *ezcfg_uuid_new(struct ezcfg *ezcfg, int version);
bool ezcfg_uuid_delete(struct ezcfg_uuid *uuid);
bool ezcfg_uuid_generate(struct ezcfg_uuid *uuid);
bool ezcfg_uuid_export_str(struct ezcfg_uuid *uuid, char *buf, int len);

/* ctrl/ctrl.c - daemon runtime setup */
struct ezcfg_ctrl;
struct ezcfg_ctrl *ezcfg_ctrl_new_from_socket(struct ezcfg *ezcfg, const int family, const int proto, const char *local_socket_path, const char *remote_socket_path);
void ezcfg_ctrl_delete(struct ezcfg_ctrl *ezctrl);
int ezcfg_ctrl_connect(struct ezcfg_ctrl *ezctrl);
int ezcfg_ctrl_read(struct ezcfg_ctrl *ezctrl, void *buf, int len, int flags);
int ezcfg_ctrl_write(struct ezcfg_ctrl *ezctrl, const void *buf, int len, int flags);

#endif /* _EZCFG_H_ */

