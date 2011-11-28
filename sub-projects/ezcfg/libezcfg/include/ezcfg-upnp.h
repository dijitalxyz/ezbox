/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-upnp.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-09   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_UPNP_H_
#define _EZCFG_UPNP_H_

/* ezcfg upnp http methods */
#define EZCFG_UPNP_HTTP_METHOD_POST         "POST"
#define EZCFG_UPNP_HTTP_METHOD_POST_EXT     "M-POST"
/* ezcfg upnp http headers */
#define EZCFG_UPNP_HTTP_HEADER_HOST                 "Host"
#define EZCFG_UPNP_HTTP_HEADER_CONTENT_TYPE         "Content-Type"
#define EZCFG_UPNP_HTTP_HEADER_CONTENT_LENGTH       "Content-Length"
#define EZCFG_UPNP_HTTP_HEADER_MAN                  "Man"
#define EZCFG_UPNP_HTTP_HEADER_01_UPNP_VERSION      "01-UPNPVersion"
#define EZCFG_UPNP_HTTP_HEADER_01_UPNP_MESSAGE_TYPE "01-UPNPMessageType"
#define EZCFG_UPNP_HTTP_HEADER_01_TARGET_DEVICE_ID  "01-TargetDeviceId"
#define EZCFG_UPNP_HTTP_HEADER_01_SOURCE_DEVICE_ID  "01-SourceDeviceId"
#define EZCFG_UPNP_HTTP_HEADER_01_SEQUENCE_ID       "01-SequenceId"
#define EZCFG_UPNP_HTTP_HEADER_02_SOAP_ACTION       "02-SoapAction"

#endif /* _EZCFG_UPNP_H_ */
