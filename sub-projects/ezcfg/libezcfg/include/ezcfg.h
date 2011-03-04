/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
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

#define EZCFG_INVALID_SOCKET      -1
#define EZCFG_BUFFER_SIZE         8192

#define EZCFG_CONFIG_FILE_PATH	SYSCONFDIR "/ezcfg.conf"

#define EZCFG_CTRL_SOCK_PATH	"@/org/kernel/ezcfg/ctrl"
//#define EZCFG_CTRL_SOCK_PATH	"/tmp/ezcfg/ctrl.sock"
#define EZCFG_NVRAM_SOCK_PATH	"@/org/kernel/ezcfg/nvram"
//#define EZCFG_NVRAM_SOCK_PATH	"/tmp/ezcfg/nvram.sock"
#define EZCFG_MASTER_SOCK_PATH	"@/org/kernel/ezcfg/master"
//#define EZCFG_MASTER_SOCK_PATH	"/tmp/ezcfg/master.sock"
#define EZCFG_MASTER_SOCKET_QUEUE_LENGTH	20

/* ezcfg nvram definitions */
#define EZCFG_NVRAM_BUFFER_SIZE            0x10000 /* 64K Bytes */
#define EZCFG_NVRAM_BUFFER_SIZE_STRING     "65536" /* 64K Bytes */
#define EZCFG_NVRAM_BACKEND_NONE           0
#define EZCFG_NVRAM_BACKEND_FILE           1
#define EZCFG_NVRAM_BACKEND_FLASH          2
#define EZCFG_NVRAM_BACKEND_HDD            3
#define EZCFG_NVRAM_STORAGE_PATH           "/var/ezcfg/nvram.bin"

/* ezcfg supported protocols */
#define EZCFG_PROTO_UNKNOWN	0
#define EZCFG_PROTO_HTTP	1
#define EZCFG_PROTO_SOAP_HTTP	2
#define EZCFG_PROTO_IGRS	3
#define EZCFG_PROTO_ISDP	4


/* ezcfg xml definitions */
#define EZCFG_XML_MAX_ELEMENTS	         128
#define EZCFG_XML_ENLARGE_SIZE           16
#define EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL 0
#define EZCFG_XML_ELEMENT_ATTRIBUTE_HEAD 1


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
#define EZCFG_SOAP_NVRAM_LISTNV_ELEMENT_NAME             "nvns:listNvram"
#define EZCFG_SOAP_NVRAM_LISTNV_RESPONSE_ELEMENT_NAME    "nvns:listNvramResponse"
#define EZCFG_SOAP_NVRAM_COMMITNV_ELEMENT_NAME           "nvns:commitNvram"
#define EZCFG_SOAP_NVRAM_COMMITNV_RESPONSE_ELEMENT_NAME  "nvns:commitNvramResponse"
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

/* ezcfg http define strings */
#define EZCFG_HTTP_SP_STRING           " "
#define EZCFG_HTTP_CRLF_STRING         "\r\n"

/* HTTP/1.1 (RFC2616) defined methods */
#define EZCFG_HTTP_METHOD_OPTIONS  "OPTIONS"
#define EZCFG_HTTP_METHOD_GET      "GET"
#define EZCFG_HTTP_METHOD_HEAD     "HEAD"
#define EZCFG_HTTP_METHOD_POST     "POST"
#define EZCFG_HTTP_METHOD_PUT      "PUT"
#define EZCFG_HTTP_METHOD_DELETE   "DELETE"
#define EZCFG_HTTP_METHOD_TRACE    "TRACE"
#define EZCFG_HTTP_METHOD_CONNECT  "CONNECT"
/* HTTP/1.1 extension (RFC2774) defined methods */
#define EZCFG_HTTP_METHOD_OPTIONS_EXT  "M-OPTIONS"
#define EZCFG_HTTP_METHOD_GET_EXT      "M-GET"
#define EZCFG_HTTP_METHOD_HEAD_EXT     "M-HEAD"
#define EZCFG_HTTP_METHOD_POST_EXT     "M-POST"
#define EZCFG_HTTP_METHOD_PUT_EXT      "M-PUT"
#define EZCFG_HTTP_METHOD_DELETE_EXT   "M-DELETE"
#define EZCFG_HTTP_METHOD_TRACE_EXT    "M-TRACE"
#define EZCFG_HTTP_METHOD_CONNECT_EXT  "M-CONNECT"

/* HTTP/1.1 (RFC2616) General headers */
#define EZCFG_HTTP_HEADER_CACHE_CONTROL       "Cache-Control"
#define EZCFG_HTTP_HEADER_CONNECTION          "Connection"
#define EZCFG_HTTP_HEADER_DATE                "Date"
#define EZCFG_HTTP_HEADER_PRAGMA              "Pragma"
#define EZCFG_HTTP_HEADER_TRAILER             "Trailer"
#define EZCFG_HTTP_HEADER_TRANSFER_ENCODING   "Transfer-Encoding"
#define EZCFG_HTTP_HEADER_UPGRADE             "Upgrade"
#define EZCFG_HTTP_HEADER_VIA                 "Via"
#define EZCFG_HTTP_HEADER_WARNING             "Warning"
/* HTTP/1.1 (RFC2616) request headers */
#define EZCFG_HTTP_HEADER_ACCEPT              "Accept"
#define EZCFG_HTTP_HEADER_ACCEPT_CHARSET      "Accept-Charset"
#define EZCFG_HTTP_HEADER_ACCEPT_ENCODING     "Accept-Encoding"
#define EZCFG_HTTP_HEADER_ACCEPT_LANGUAGE     "Accept-Language"
#define EZCFG_HTTP_HEADER_AUTHORIZATION       "Authorization"
#define EZCFG_HTTP_HEADER_EXPECT              "Expect"
#define EZCFG_HTTP_HEADER_FROM                "From"
#define EZCFG_HTTP_HEADER_HOST                "Host"
#define EZCFG_HTTP_HEADER_IF_MATCH            "If-Match"
#define EZCFG_HTTP_HEADER_IF_MODIFIED_SINCE   "If-Modified-Since"
#define EZCFG_HTTP_HEADER_IF_NONE_MATCH       "If-None-Match"
#define EZCFG_HTTP_HEADER_IF_RANGE            "If-Range"
#define EZCFG_HTTP_HEADER_IF_UNMODIFIED_SINCE "If-Unmodified-Since"
#define EZCFG_HTTP_HEADER_MAX_FORWARDS        "Max-Forwards"
#define EZCFG_HTTP_HEADER_PROXY_AUTHORIZATION "Proxy-Authorization"
#define EZCFG_HTTP_HEADER_RANGE               "Range"
#define EZCFG_HTTP_HEADER_REFERER             "Referer"
#define EZCFG_HTTP_HEADER_TE                  "TE"
#define EZCFG_HTTP_HEADER_USER_AGENT          "User-Agent"
/* HTTP/1.1 (RFC2616) response headers */
#define EZCFG_HTTP_HEADER_ACCEPT_RANGES       "Accept-Ranges"
#define EZCFG_HTTP_HEADER_AGE                 "Age"
#define EZCFG_HTTP_HEADER_ETAG                "ETag"
#define EZCFG_HTTP_HEADER_LOCATION            "Location"
#define EZCFG_HTTP_HEADER_PROXY_AUTHENTICATE  "Proxy-Authenticate"
#define EZCFG_HTTP_HEADER_RETRY_AFTER         "Retry-After"
#define EZCFG_HTTP_HEADER_SERVER              "Server"
#define EZCFG_HTTP_HEADER_VARY                "Vary"
#define EZCFG_HTTP_HEADER_WWW_AUTHENTICATE    "WWW-Authenticate"
/* HTTP/1.1 (RFC2616) entity headers */
#define EZCFG_HTTP_HEADER_ALLOW               "Allow"
#define EZCFG_HTTP_HEADER_CONTENT_ENCODING    "Content-Encoding"
#define EZCFG_HTTP_HEADER_CONTENT_LANGUAGE    "Content-Language"
#define EZCFG_HTTP_HEADER_CONTENT_LENGTH      "Content-Length"
#define EZCFG_HTTP_HEADER_CONTENT_LOCATION    "Content-Location"
#define EZCFG_HTTP_HEADER_CONTENT_MD5         "Content-MD5"
#define EZCFG_HTTP_HEADER_CONTENT_RANGE       "Content-Range"
#define EZCFG_HTTP_HEADER_CONTENT_TYPE        "Content-Type"
#define EZCFG_HTTP_HEADER_EXPIRES             "Expires"
#define EZCFG_HTTP_HEADER_LAST_MODIFIED       "Last-Modified"
/* HTTP/1.1 extension (RFC2774) headers */
#define EZCFG_HTTP_HEADER_MAN                 "Man"
#define EZCFG_HTTP_HEADER_OPT                 "Opt"
#define EZCFG_HTTP_HEADER_C_MAN               "C-Man"
#define EZCFG_HTTP_HEADER_C_OPT               "C-Opt"
/* HTTP/1.1 extension (RFC2774) responese headers */
#define EZCFG_HTTP_HEADER_EXT                 "Ext"
#define EZCFG_HTTP_HEADER_C_EXT               "C-Ext"
/* HTTP/1.1 (RFC2616) status code reason phrase section 10 */
#define EZCFG_HTTP_REASON_PHRASE_1XX          "Informational"
#define EZCFG_HTTP_REASON_PHRASE_100          "Continue"
#define EZCFG_HTTP_REASON_PHRASE_101          "Switching Protocols"
/* HTTP Extensions for Distributed Authoring -- WEBDAV (RFC2518) status code 102 */
#define EZCFG_HTTP_REASON_PHRASE_102          "Processing"
#define EZCFG_HTTP_REASON_PHRASE_2XX          "Successful"
#define EZCFG_HTTP_REASON_PHRASE_200          "OK"
#define EZCFG_HTTP_REASON_PHRASE_201          "Created"
#define EZCFG_HTTP_REASON_PHRASE_202          "Accepted"
#define EZCFG_HTTP_REASON_PHRASE_203          "Non-Authoritative Information"
#define EZCFG_HTTP_REASON_PHRASE_204          "No Content"
#define EZCFG_HTTP_REASON_PHRASE_205          "Reset Content"
#define EZCFG_HTTP_REASON_PHRASE_206          "Partial Content"
/* HTTP Extensions for Distributed Authoring -- WEBDAV (RFC2518) status code 207 */
#define EZCFG_HTTP_REASON_PHRASE_207          "Multi-Status"
#define EZCFG_HTTP_REASON_PHRASE_3XX          "Redirection"
#define EZCFG_HTTP_REASON_PHRASE_300          "Multiple Choices"
#define EZCFG_HTTP_REASON_PHRASE_301          "Moved Permanently"
#define EZCFG_HTTP_REASON_PHRASE_302          "Found"
#define EZCFG_HTTP_REASON_PHRASE_303          "See Other"
#define EZCFG_HTTP_REASON_PHRASE_304          "Not Modified"
#define EZCFG_HTTP_REASON_PHRASE_305          "Use Proxy"
#define EZCFG_HTTP_REASON_PHRASE_306          "(Unused)"
#define EZCFG_HTTP_REASON_PHRASE_307          "Temporary Redirect"
#define EZCFG_HTTP_REASON_PHRASE_4XX          "Client Error"
#define EZCFG_HTTP_REASON_PHRASE_400          "Bad Request"
#define EZCFG_HTTP_REASON_PHRASE_401          "Unauthorized"
#define EZCFG_HTTP_REASON_PHRASE_402          "Payment Required"
#define EZCFG_HTTP_REASON_PHRASE_403          "Forbidden"
#define EZCFG_HTTP_REASON_PHRASE_404          "Not Found"
#define EZCFG_HTTP_REASON_PHRASE_405          "Method Not Allowed"
#define EZCFG_HTTP_REASON_PHRASE_406          "Not Acceptable"
#define EZCFG_HTTP_REASON_PHRASE_407          "Proxy Authentication Required"
#define EZCFG_HTTP_REASON_PHRASE_408          "Request Timeout"
#define EZCFG_HTTP_REASON_PHRASE_409          "Conflict"
#define EZCFG_HTTP_REASON_PHRASE_410          "Gone"
#define EZCFG_HTTP_REASON_PHRASE_411          "Length Required"
#define EZCFG_HTTP_REASON_PHRASE_412          "Precondition Failed"
#define EZCFG_HTTP_REASON_PHRASE_413          "Request Entity Too Large"
#define EZCFG_HTTP_REASON_PHRASE_414          "Request-URI Too Long"
#define EZCFG_HTTP_REASON_PHRASE_415          "Unsupported Media Type"
#define EZCFG_HTTP_REASON_PHRASE_416          "Requested Range Not Satisfiable"
#define EZCFG_HTTP_REASON_PHRASE_417          "Expectation Failed"
/* HTTP Extensions for Distributed Authoring -- WEBDAV (RFC2518) for status code 422-424 */
#define EZCFG_HTTP_REASON_PHRASE_422          "Unprocessable Entity"
#define EZCFG_HTTP_REASON_PHRASE_423          "Locked"
#define EZCFG_HTTP_REASON_PHRASE_424          "Failed Dependency"
/* Upgrading to TLS Within HTTP/1.1 (RFC2817) for status code 426 */
#define EZCFG_HTTP_REASON_PHRASE_426          "Upgrade Required"
#define EZCFG_HTTP_REASON_PHRASE_5XX          "Server Error"
#define EZCFG_HTTP_REASON_PHRASE_500          "Internal Server Error"
#define EZCFG_HTTP_REASON_PHRASE_501          "Not Implemented"
#define EZCFG_HTTP_REASON_PHRASE_502          "Bad Gateway"
#define EZCFG_HTTP_REASON_PHRASE_503          "Service Unavailable"
#define EZCFG_HTTP_REASON_PHRASE_504          "Gateway Timeout"
#define EZCFG_HTTP_REASON_PHRASE_505          "HTTP Version Not Supported"
/* HTTP Extensions for Distributed Authoring -- WEBDAV (RFC2518) status code 507 */
#define EZCFG_HTTP_REASON_PHRASE_507          "Insufficient Storage"


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
/* ezcfg SOAP/HTTP http methods */
#define EZCFG_SOAP_HTTP_METHOD_GET          "GET"
#define EZCFG_SOAP_HTTP_METHOD_POST         "POST"
/* ezcfg SOAP/HTTP http headers */
#define EZCFG_SOAP_HTTP_HEADER_HOST                 "Host"
#define EZCFG_SOAP_HTTP_HEADER_CONTENT_TYPE         "Content-Type"
#define EZCFG_SOAP_HTTP_HEADER_CONTENT_LENGTH       "Content-Length"
#define EZCFG_SOAP_HTTP_HEADER_ACCEPT               "Accept"
/* ezcfg SOAP/HTTP nvram handler */
#define EZCFG_SOAP_HTTP_NVRAM_GET_URI               "/ezcfg/nvram/soap-http/getNvram"
#define EZCFG_SOAP_HTTP_NVRAM_SET_URI               "/ezcfg/nvram/soap-http/setNvram"
#define EZCFG_SOAP_HTTP_NVRAM_UNSET_URI             "/ezcfg/nvram/soap-http/unsetNvram"
#define EZCFG_SOAP_HTTP_NVRAM_LIST_URI              "/ezcfg/nvram/soap-http/listNvram"
#define EZCFG_SOAP_HTTP_NVRAM_COMMIT_URI            "/ezcfg/nvram/soap-http/commitNvram"

/* ezcfg igrs request/response mode */
#define EZCFG_IGRS_HTTP_MODE_REQUEST        EZCFG_HTTP_MODE_REQUEST
#define EZCFG_IGRS_HTTP_MODE_RESPONSE       EZCFG_HTTP_MODE_RESPONSE
#define EZCFG_IGRS_HTTP_CHUNK_SIZE          20480
#define EZCFG_IGRS_HTTP_MAX_CHUNK_NUM       32
/* ezcfg igrs/HTTP request size */
#define EZCFG_IGRS_HTTP_MAX_REQUEST_SIZE    (EZCFG_IGRS_HTTP_CHUNK_SIZE * EZCFG_IGRS_HTTP_MAX_CHUNK_NUM)
/* ezcfg igrs/HTTP response size */
#define EZCFG_IGRS_HTTP_MAX_RESPONSE_SIZE   EZCFG_IGRS_HTTP_MAX_REQUEST_SIZE
/* ezcfg igrs http methods */
#define EZCFG_IGRS_HTTP_METHOD_POST         "POST"
#define EZCFG_IGRS_HTTP_METHOD_POST_EXT     "M-POST"
/* ezcfg igrs http headers */
#define EZCFG_IGRS_HTTP_HEADER_HOST                 "Host"
#define EZCFG_IGRS_HTTP_HEADER_CONTENT_TYPE         "Content-Type"
#define EZCFG_IGRS_HTTP_HEADER_CONTENT_LENGTH       "Content-Length"
#define EZCFG_IGRS_HTTP_HEADER_MAN                  "Man"
#define EZCFG_IGRS_HTTP_HEADER_01_IGRS_VERSION      "01-IGRSVersion"
#define EZCFG_IGRS_HTTP_HEADER_01_IGRS_MESSAGE_TYPE "01-IGRSMessageType"
#define EZCFG_IGRS_HTTP_HEADER_01_TARGET_DEVICE_ID  "01-TargetDeviceId"
#define EZCFG_IGRS_HTTP_HEADER_01_SOURCE_DEVICE_ID  "01-SourceDeviceId"
#define EZCFG_IGRS_HTTP_HEADER_01_SEQUENCE_ID       "01-SequenceId"
#define EZCFG_IGRS_HTTP_HEADER_02_SOAP_ACTION       "02-SoapAction"

/* ezcfg igrs soap element names */
#define EZCFG_IGRS_SOAP_ENVELOPE_ELEMENT_NAME             "SOAP-ENV:Envelope"
#define EZCFG_IGRS_SOAP_BODY_ELEMENT_NAME                 "SOAP-ENV:Body"
#define EZCFG_IGRS_SOAP_SESSION_ELEMENT_NAME              "Session"
#define EZCFG_IGRS_SOAP_SOURCE_CLIENT_ID_ELEMENT_NAME     "SourceClientId"
#define EZCFG_IGRS_SOAP_TARGET_CLIENT_ID_ELEMENT_NAME     "TargetClientId"
#define EZCFG_IGRS_SOAP_SOURCE_SERVICE_ID_ELEMENT_NAME    "SourceServiceId"
#define EZCFG_IGRS_SOAP_TARGET_SERVICE_ID_ELEMENT_NAME    "TargetServiceId"
#define EZCFG_IGRS_SOAP_SEQUENCE_ID_ELEMENT_NAME          "SequenceId"
#define EZCFG_IGRS_SOAP_ACKNOWLEDGE_ID_ELEMENT_NAME       "AcknowledgeId"
#define EZCFG_IGRS_SOAP_RETURN_CODE_ELEMENT_NAME          "ReturnCode"
#define EZCFG_IGRS_SOAP_USER_INFO_ELEMENT_NAME            "UserInfo"
#define EZCFG_IGRS_SOAP_SOURCE_USER_ID_ELEMENT_NAME       "SourceUserId"
#define EZCFG_IGRS_SOAP_SERVICE_SECURITY_ID_ELEMENT_NAME  "ServiceSecurityId"
#define EZCFG_IGRS_SOAP_TOKEN_ELEMENT_NAME                "Token"
#define EZCFG_IGRS_SOAP_INVOKE_ARGS_ELEMENT_NAME          "InvokeArguments"

/* ezcfg igrs soap element attributes */
#define EZCFG_IGRS_SOAP_ENVELOPE_ATTR_NS_NAME	"xmlns:SOAP-ENV"
//#define EZCFG_IGRS_SOAP_ENVELOPE_ATTR_NS_VALUE	"http://schemas.xmlsoap.org/soap/envelope/"
#define EZCFG_IGRS_SOAP_ENVELOPE_ATTR_NS_VALUE	"http://www.w3.org/2002/12/soap-envelope"
#define EZCFG_IGRS_SOAP_ENVELOPE_ATTR_ENC_NAME	"SOAP-ENV:encodingStyle"
#define EZCFG_IGRS_SOAP_ENVELOPE_ATTR_ENC_VALUE	"http://schemas.xmlsoap.org/soap/encoding/"
#define EZCFG_IGRS_SOAP_SESSION_ATTR_NS_NAME		"xmlns"
#define EZCFG_IGRS_SOAP_SESSION_ATTR_NS_VALUE	"http://www.igrs.org/spec1.0"

/* ezcfg igrs message types */
/* 9.1 Device Advertisement */
#define EZCFG_IGRS_MSG_DEVICE_ONLINE_ADVERTISEMENT   "DeviceOnlineAdvertisement"
#define EZCFG_IGRS_MSG_DEVICE_OFFLINE_ADVERTISEMENT  "DeviceOfflineAdvertisement"
/* 9.2 Device Pipe Management */
#define EZCFG_IGRS_MSG_CREATE_PIPE_REQUEST               "CreatePipeRequest"
#define EZCFG_IGRS_MSG_CREATE_PIPE_RESPONSE              "CreatePipeResponse"
#define EZCFG_IGRS_MSG_AUTHENTICATE_REQUEST              "AuthenticateRequest"
#define EZCFG_IGRS_MSG_AUTHENTICATE_RESPONSE             "AuthenticateResponse"
#define EZCFG_IGRS_MSG_AUTHENTICATE_RESULT_REQUEST       "AuthenticateResultRequest"
#define EZCFG_IGRS_MSG_AUTHENTICATE_RESULT_RESPONSE      "AuthenticateResultResponse"
#define EZCFG_IGRS_MSG_CREATE_PIPE_RESULT_REQUEST        "CreatePipeResultRequest"
#define EZCFG_IGRS_MSG_CREATE_PIPE_RESULT_RESPONSE       "CreatePipeResultResponse"
#define EZCFG_IGRS_MSG_DETACH_PIPE_NOTIFY                "DetachPipeNotify"
#define EZCFG_IGRS_MSG_DEVICE_ONLINE_DETECTION_REQUEST   "DeviceOnlineDetectionRequest"
#define EZCFG_IGRS_MSG_DEVICE_ONLINE_DETECTION_RESPONSE  "DeviceOnlineDetectionResponse"
/* 9.3 Detaild Device Description Document Retrieval */
#define EZCFG_IGRS_MSG_GET_DEVICE_DESCRIPTION_REQUEST   "GetDeviceDescriptionRequest"
#define EZCFG_IGRS_MSG_GET_DEVICE_DESCRIPTION_RESPONSE  "GetDeviceDescriptionResponse"
/* 9.4 Retrieve Detailed Device Description Document Based on Non-Secure Pipe */
/* 9.5 Device Group Setup */
#define EZCFG_IGRS_MSG_PEER_DEVICE_GROUP_ADVERTISEMENT              "PeerDeviceGroupAdvertisement"
#define EZCFG_IGRS_MSG_QUIT_PEER_DEVICE_GROUP_NOTIFY                "QuitPeerDeviceGroupNotify"
#define EZCFG_IGRS_MSG_CENTRALISED_DEVICE_GROUP_ADVERTISEMENT       "CentralisedDeviceGroupAdvertisement"
#define EZCFG_IGRS_MSG_JOIN_CENTRALISED_DEVICE_GROUP_REQUEST        "JoinCentralisedDeviceGroupRequest"
#define EZCFG_IGRS_MSG_JOIN_CENTRALISED_DEVICE_GROUP_RESPONSE       "JoinCentralisedDeviceGroupResponse"
#define EZCFG_IGRS_MSG_QUIT_CENTRALISED_DEVICE_GROUP_ADVERTISEMENT  "QuitCentralisedDeviceGroupAdvertisement"
#define EZCFG_IGRS_MSG_QUIT_CENTRALISED_DEVICE_GROUP_NOTIFY         "QuitCentralisedDeviceGroupNotify"
/* 9.6 Device Search */
#define EZCFG_IGRS_MSG_SEARCH_DEVICE_REQUEST              "SearchDeviceRequest"
#define EZCFG_IGRS_MSG_SEARCH_DEVICE_RESPONSE             "SearchDeviceResponse"
#define EZCFG_IGRS_MSG_SEARCH_DEVICE_REQUEST_ON_DEVICE    "SearchDeviceRequestOnDevice"
#define EZCFG_IGRS_MSG_SEARCH_DEVICE_RESPONSE_ON_DEVICE   "SearchDeviceResponseOnDevice"
/* 9.7 Device Online/Offline Event Subscription */
#define EZCFG_IGRS_MSG_SUBSCRIBE_DEVICE_EVENT_REQUEST           "SubscribeDeviceEventRequest"
#define EZCFG_IGRS_MSG_RENEW_SUBSCRIPTION_DEVICE_EVENT_REQUEST  "RenewSubscriptionDeviceEventRequest"
#define EZCFG_IGRS_MSG_SUBSCRIBE_DEVICE_EVENT_RESPONSE          "SubscribeDeviceEventResponse"
#define EZCFG_IGRS_MSG_UNSUBSCRIBE_DEVICE_EVENT_NOTIFY          "UnSubscribeDeviceEventNotify"
#define EZCFG_IGRS_MSG_NOTIFY_DEVICE_EVENT                      "NotifyDeviceEvent"
/* 9.8 Device Group Search */
#define EZCFG_IGRS_MSG_SEARCH_DEVICE_GROUP_REQUEST    "SearchDeviceGroupRequest"
#define EZCFG_IGRS_MSG_SEARCH_DEVICE_GROUP_RESPONSE   "SearchDeviceGroupResponse"
/* 10.1 Service Online/Offline Advertisement */
#define EZCFG_IGRS_MSG_SERVICE_ONLINE_ADVERTISEMENT   "ServiceOnlineAdvertisement"
#define EZCFG_IGRS_MSG_SERVICE_OFFLINE_ADVERTISEMENT  "ServiceOfflineAdvertisement"
#define EZCFG_IGRS_MSG_REGISTER_SERVICE_NOTIFY        "RegisterServiceNotify"
#define EZCFG_IGRS_MSG_UNREGISTER_SERVICE_NOTIFY      "UnRegisterServiceNotify"
/* 10.2 Service Search */
#define EZCFG_IGRS_MSG_SEARCH_SERVICE_REQUEST             "SearchServiceRequest"
#define EZCFG_IGRS_MSG_SEARCH_SERVICE_RESPONSE            "SearchServiceResponse"
#define EZCFG_IGRS_MSG_SEARCH_SERVICE_REQUEST_ON_DEVICE   "SearchServiceRequestOnDevice"
#define EZCFG_IGRS_MSG_SEARCH_SERVICE_RESPONSE_ON_DEVICE  "SearchServiceResponseOnDevice"
/* 10.3 Service Online/Offline Event Subscription */
#define EZCFG_IGRS_MSG_SUBSCRIBE_SERVICE_EVENT_REQUEST           "SubscribeServiceEventRequest"
#define EZCFG_IGRS_MSG_RENEW_SUBSCRIPTION_SERVICE_EVENT_REQUEST  "RenewSubscriptionServiceEventRequest"
#define EZCFG_IGRS_MSG_SUBSCRIBE_SERVICE_EVENT_RESPONSE          "SubscribeServiceEventResponse"
#define EZCFG_IGRS_MSG_UNSUBSCRIBE_DEVICE_EVENT_NOTIFY           "UnSubscribeDeviceEventNotify"
#define EZCFG_IGRS_MSG_NOTIFY_SERVICE_EVENT                      "NotifyServiceEvent"
/* 10.4 Service Description Document Retrieval */
#define EZCFG_IGRS_MSG_GET_SERVICE_DESCRIPTION_REQUEST   "GetServiceDescriptionRequest"
#define EZCFG_IGRS_MSG_GET_SERVICE_DESCRIPTION_RESPONSE  "GetServiceDescriptionResponse"
/* 10.5 Session */
#define EZCFG_IGRS_MSG_CREATE_SESSION_REQUEST         "CreateSessionRequest"
#define EZCFG_IGRS_MSG_CREATE_SESSION_RESPONSE        "CreateSessionResponse"
#define EZCFG_IGRS_MSG_DESTROY_SESSION_NOTIFY         "DestroySessionNotify"
#define EZCFG_IGRS_MSG_APPLY_SESSION_KEY_REQUEST      "ApplySessionKeyRequest"
#define EZCFG_IGRS_MSG_APPLY_SESSION_KEY_RESPONSE     "ApplySessionKeyResponse"
#define EZCFG_IGRS_MSG_TRANSFER_SESSION_KEY_REQUEST   "TransferSessionKeyRequest"
#define EZCFG_IGRS_MSG_TRANSFER_SESSION_KEY_RESPONSE  "TransferSessionKeyResponse"
/* 10.6 Service Invocation */
#define EZCFG_IGRS_MSG_INVOKE_SERVICE_REQUEST   "InvokeServiceRequest"
#define EZCFG_IGRS_MSG_INVOKE_SERVICE_RESPONSE  "InvokeServiceResponse"
#define EZCFG_IGRS_MSG_SEND_NOTIFICATION        "SendNotification"


/* ezcfg igrs soap actions */
/* 9.3 Detaild Device Description Document Retrieval */
#define EZCFG_IGRS_SOAP_ACTION_GET_DEVICE_DESCRIPTION_REQUEST   "IGRS-GetDeviceDescription-Request"
#define EZCFG_IGRS_SOAP_ACTION_GET_DEVICE_DESCRIPTION_RESPONSE  "IGRS-GetDeviceDescription-Response"
/* 9.4 Retrieve Detailed Device Description Document Based on Non-Secure Pipe */
/* 9.5 Device Group Setup */
#define EZCFG_IGRS_SOAP_ACTION_PEER_DEVICE_GROUP_ADVERTISEMENT              "IGRS-PeerDeviceGroup-Advertisement"
#define EZCFG_IGRS_SOAP_ACTION_CENTRALISED_DEVICE_GROUP_ADVERTISEMENT       "IGRS-CentralisedDeviceGroup-Advertisement"
/* 9.6 Device Search */
#define EZCFG_IGRS_SOAP_ACTION_SEARCH_DEVICE_RESPONSE             "IGRS-SearchDevice-Response"
#define EZCFG_IGRS_SOAP_ACTION_SEARCH_DEVICE_REQUEST_ON_DEVICE    "IGRS-SearchDevice-Request-OnDevice"
#define EZCFG_IGRS_SOAP_ACTION_SEARCH_DEVICE_RESPONSE_ON_DEVICE   "IGRS-SearchDevice-Response-OnDevice"
/* 9.7 Device Online/Offline Event Subscription */
#define EZCFG_IGRS_SOAP_ACTION_SUBSCRIBE_DEVICE_EVENT_REQUEST           "IGRS-SubscribeDeviceEvent-Request"
#define EZCFG_IGRS_SOAP_ACTION_RENEW_SUBSCRIPTION_DEVICE_EVENT_REQUEST  "IGRS-RenewSubscriptionDeviceEvent-Request"
#define EZCFG_IGRS_SOAP_ACTION_SUBSCRIBE_DEVICE_EVENT_RESPONSE          "IGRS-SubscribeDeviceEvent-Response"
#define EZCFG_IGRS_SOAP_ACTION_UNSUBSCRIBE_DEVICE_EVENT_NOTIFY          "IGRS-UnSubscribeDeviceEvent-Notify"
#define EZCFG_IGRS_SOAP_ACTION_DEVICE_EVENT_NOTIFY                      "IGRS-DeviceEvent-Notify"
/* 9.8 Device Group Search */
#define EZCFG_IGRS_SOAP_ACTION_SEARCH_DEVICE_GROUP_RESPONSE  "IGRS-SearchDeviceGroup-Response"
/* 10.1 Service Online/Offline Advertisement */
#define EZCFG_IGRS_SOAP_ACTION_REGISTER_SERVICE_NOTIFY        "IGRS-RegisterService-Notify"
#define EZCFG_IGRS_SOAP_ACTION_UNREGISTER_SERVICE_NOTIFY      "IGRS-UnRegisterService-Notify"
/* 10.2 Service Search */
#define EZCFG_IGRS_SOAP_ACTION_SEARCH_SERVICE_RESPONSE            "IGRS-SearchService-Response"
#define EZCFG_IGRS_SOAP_ACTION_SEARCH_SERVICE_REQUEST_ON_DEVICE   "IGRS-SearchService-Request-OnDevice"
#define EZCFG_IGRS_SOAP_ACTION_SEARCH_SERVICE_RESPONSE_ON_DEVICE  "IGRS-SearchService-Response-OnDevice"
/* 10.3 Service Online/Offline Event Subscription */
#define EZCFG_IGRS_SOAP_ACTION_SUBSCRIBE_SERVICE_EVENT_REQUEST           "IGRS-SubscribeServiceEvent-Request"
#define EZCFG_IGRS_SOAP_ACTION_RENEW_SUBSCRIPTION_SERVICE_EVENT_REQUEST  "IGRS-RenewSubscriptionServiceEvent-Request"
#define EZCFG_IGRS_SOAP_ACTION_SUBSCRIBE_SERVICE_EVENT_RESPONSE          "IGRS-SubscribeServiceEvent-Response"
#define EZCFG_IGRS_SOAP_ACTION_UNSUBSCRIBE_DEVICE_EVENT_NOTIFY           "IGRS-UnSubscribeDeviceEvent-Notify"
#define EZCFG_IGRS_SOAP_ACTION_SERVICE_EVENT_NOTIFY                      "IGRS-ServiceEvent-Notify"
/* 10.4 Service Description Document Retrieval */
#define EZCFG_IGRS_SOAP_ACTION_GET_SERVICE_DESCRIPTION_REQUEST   "IGRS-GetServiceDescription-Request"
#define EZCFG_IGRS_SOAP_ACTION_GET_SERVICE_DESCRIPTION_RESPONSE  "IGRS-GetServiceDescription-Response"
/* 10.5 Session */
#define EZCFG_IGRS_SOAP_ACTION_CREATE_SESSION_REQUEST         "IGRS-CreateSession-Request"
#define EZCFG_IGRS_SOAP_ACTION_CREATE_SESSION_RESPONSE        "IGRS-CreateSession-Response"
#define EZCFG_IGRS_SOAP_ACTION_DESTROY_SESSION_NOTIFY         "IGRS-DestroySession-Notify"
#define EZCFG_IGRS_SOAP_ACTION_APPLY_SESSION_KEY_REQUEST      "IGRS-ApplySessionKey-Request"
#define EZCFG_IGRS_SOAP_ACTION_APPLY_SESSION_KEY_RESPONSE     "IGRS-ApplySessionKey-Response"
#define EZCFG_IGRS_SOAP_ACTION_TRANSFER_SESSION_KEY_REQUEST   "IGRS-TransferSessionKey-Request"
#define EZCFG_IGRS_SOAP_ACTION_TRANSFER_SESSION_KEY_RESPONSE  "IGRS-TransferSessionKey-Response"
/* 10.6 Service Invocation */
#define EZCFG_IGRS_SOAP_ACTION_INVOKE_SERVICE_REQUEST   "IGRS-InvokeService-Request"
#define EZCFG_IGRS_SOAP_ACTION_INVOKE_SERVICE_RESPONSE  "IGRS-InvokeService-Response"
#define EZCFG_IGRS_SOAP_ACTION_SEND_NOTIFICATION        "IGRS-Send-Notification"


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
    if (ezcfg_get_log_priority(ezcfg) >= prio) \
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

void ezcfg_set_log_fn(struct ezcfg *ezcfg,
                      void (*log_fn)(struct ezcfg *ezcfg,
                                    int priority, const char *file, int line, const char *fn,
                                    const char *format, va_list args));
int ezcfg_get_log_priority(struct ezcfg *ezcfg);
void ezcfg_set_log_priority(struct ezcfg *ezcfg, int priority);

struct ezcfg *ezcfg_new(void);
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
struct ezcfg_ctrl *ezcfg_ctrl_new_from_socket(struct ezcfg *ezcfg, const int family, const unsigned char proto, const char *local_socket_path, const char *remote_socket_path);
void ezcfg_ctrl_delete(struct ezcfg_ctrl *ezctrl);
int ezcfg_ctrl_connect(struct ezcfg_ctrl *ezctrl);
int ezcfg_ctrl_read(struct ezcfg_ctrl *ezctrl, void *buf, int len, int flags);
int ezcfg_ctrl_write(struct ezcfg_ctrl *ezctrl, const void *buf, int len, int flags);

#endif /* _EZCFG_H_ */

