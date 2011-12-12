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

/* ezcfg nvram name prefix */
#define EZCFG_UPNP_NVRAM_PREFIX            "upnp_"

/* ezcfg UPnP role string */
#define EZCFG_UPNP_ROLE_UNKNOWN         0
#define EZCFG_UPNP_ROLE_DEVICE          1
#define EZCFG_UPNP_ROLE_CONTROL_POINT   2

#define EZCFG_UPNP_ROLE_DEVICE_STRING   "device"
#define EZCFG_UPNP_ROLE_CONTROL_POINT_STRING "control-point"


/* ezcfg UPnP type string */
#define EZCFG_UPNP_TYPE_UNKNOWN         0
#define EZCFG_UPNP_TYPE_IGD1            1

#define EZCFG_UPNP_TYPE_IGD1_STRING     "igd1"


/* IGD1 config nvram name */
#define EZCFG_UPNP_IGD1_DESCRIPTION_PATH   "igd1_description_path"
#define EZCFG_UPNP_IGD1_ROOT_DEVICE_XML_FILE	EZCFG_UPNP_IGD1_CONF_PATH "/InternetGatewayDevice1.xml"


/* UPnP Description XML string */
#define EZCFG_UPNP_DESC_SPEC_VERSION_ELEMENT_NAME       "specVersion"
#define EZCFG_UPNP_DESC_MAJOR_ELEMENT_NAME              "major"
#define EZCFG_UPNP_DESC_MINOR_ELEMENT_NAME              "minor"
#define EZCFG_UPNP_DESC_URL_BASE_ELEMENT_NAME           "URLBase"

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
