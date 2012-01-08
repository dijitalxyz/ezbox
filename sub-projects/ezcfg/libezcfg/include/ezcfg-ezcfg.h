/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-ezcfg.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2011-03-03   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_EZCFG_H_
#define _EZCFG_EZCFG_H_

/* ezcfg config name prefix */
#define EZCFG_EZCFG_NVRAM_PREFIX            "ezcfg_"

/* ezcfg file trailing charlist */
#define EZCFG_EZCFG_TRAILING_CHARLIST       "\r\n\t "

/* ezcfg config [common] section */
#define EZCFG_EZCFG_SECTION_COMMON          "common"

#define EZCFG_EZCFG_KEYWORD_LOG_LEVEL       "log_level"
#define EZCFG_EZCFG_KEYWORD_RULES_PATH      "rules_path"
#define EZCFG_EZCFG_KEYWORD_SOCKET_NUMBER   "socket_number"
#define EZCFG_EZCFG_KEYWORD_LOCALE          "locale"
#define EZCFG_EZCFG_KEYWORD_AUTH_NUMBER     "auth_number"
#define EZCFG_EZCFG_KEYWORD_IGRS_NUMBER     "igrs_number"
#define EZCFG_EZCFG_KEYWORD_UPNP_NUMBER     "upnp_number"

#define EZCFG_EZCFG_COMMON_LOG_LEVEL        "common.log_level"
#define EZCFG_EZCFG_COMMON_RULES_PATH       "common.rules_path"
#define EZCFG_EZCFG_COMMON_SOCKET_NUMBER    "common.socket_number"
#define EZCFG_EZCFG_COMMON_LOCALE           "common.locale"
#define EZCFG_EZCFG_COMMON_AUTH_NUMBER      "common.auth_number"
#define EZCFG_EZCFG_COMMON_IGRS_NUMBER      "common.igrs_number"
#define EZCFG_EZCFG_COMMON_UPNP_NUMBER      "common.upnp_number"

/* ezcfg config [nvram] section */
#define EZCFG_EZCFG_SECTION_NVRAM           "nvram"

#define EZCFG_EZCFG_KEYWORD_BUFFER_SIZE     "buffer_size"
#define EZCFG_EZCFG_KEYWORD_BACKEND_TYPE    "backend_type"
#define EZCFG_EZCFG_KEYWORD_CODING_TYPE     "coding_type"
#define EZCFG_EZCFG_KEYWORD_STORAGE_PATH    "storage_path"

#define EZCFG_EZCFG_NVRAM_0_BUFFER_SIZE     "nvram.0.buffer_size"
#define EZCFG_EZCFG_NVRAM_0_BACKEND_TYPE    "nvram.0.backend_type"
#define EZCFG_EZCFG_NVRAM_0_CODING_TYPE     "nvram.0.coding_type"
#define EZCFG_EZCFG_NVRAM_0_STORAGE_PATH    "nvram.0.storage_path"

#define EZCFG_EZCFG_NVRAM_1_BUFFER_SIZE     "nvram.1.buffer_size"
#define EZCFG_EZCFG_NVRAM_1_BACKEND_TYPE    "nvram.1.backend_type"
#define EZCFG_EZCFG_NVRAM_1_CODING_TYPE     "nvram.1.coding_type"
#define EZCFG_EZCFG_NVRAM_1_STORAGE_PATH    "nvram.1.storage_path"

/* ezcfg config [socket] section */
#define EZCFG_EZCFG_SECTION_SOCKET          "socket"

#define EZCFG_EZCFG_KEYWORD_DOMAIN          "domain"
#define EZCFG_EZCFG_KEYWORD_TYPE            "type"
#define EZCFG_EZCFG_KEYWORD_PROTOCOL        "protocol"
#define EZCFG_EZCFG_KEYWORD_ADDRESS         "address"

#define EZCFG_EZCFG_SOCKET_0_DOMAIN         "socket.0.domain"
#define EZCFG_EZCFG_SOCKET_0_TYPE           "socket.0.type"
#define EZCFG_EZCFG_SOCKET_0_PROTOCOL       "socket.0.protocol"
#define EZCFG_EZCFG_SOCKET_0_ADDRESS        "socket.0.address"

/* ezcfg config [auth] section */
#define EZCFG_EZCFG_SECTION_AUTH            "auth"

//#define EZCFG_EZCFG_KEYWORD_TYPE            "type"
#define EZCFG_EZCFG_KEYWORD_USER            "user"
#define EZCFG_EZCFG_KEYWORD_REALM           "realm"
//#define EZCFG_EZCFG_KEYWORD_DOMAIN          "domain"
#define EZCFG_EZCFG_KEYWORD_SECRET          "secret"

#define EZCFG_EZCFG_AUTH_0_TYPE             "auth.0.type"
#define EZCFG_EZCFG_AUTH_0_USER             "auth.0.user"
#define EZCFG_EZCFG_AUTH_0_REALM            "auth.0.realm"
#define EZCFG_EZCFG_AUTH_0_DOMAIN           "auth.0.domain"
#define EZCFG_EZCFG_AUTH_0_SECRET           "auth.0.secret"
#define EZCFG_EZCFG_AUTH_0_SECRET2          "auth.0.secret2"

/* ezcfg config [igrs] section */
#define EZCFG_EZCFG_SECTION_IGRS            "igrs"

/* ezcfg config [upnp] section */
#define EZCFG_EZCFG_SECTION_UPNP            "upnp"

#define EZCFG_EZCFG_KEYWORD_ROLE            "role"
#define EZCFG_EZCFG_KEYWORD_DEVICE_TYPE     "device_type"
#define EZCFG_EZCFG_KEYWORD_DESCRIPTION_PATH    "description_path"
#define EZCFG_EZCFG_KEYWORD_INTERFACE       "interface"

#define EZCFG_EZCFG_UPNP_0_ROLE             "upnp.0.role"
#define EZCFG_EZCFG_UPNP_0_DEVICE_TYPE      "upnp.0.device_type"
#define EZCFG_EZCFG_UPNP_0_DESCRIPTION_PATH "upnp.0.description_path"
#define EZCFG_EZCFG_UPNP_0_INTERFACE        "upnp.0.interface"

#define EZCFG_EZCFG_UPNP_1_ROLE             "upnp.1.role"
#define EZCFG_EZCFG_UPNP_1_DEVICE_TYPE      "upnp.1.device_type"
#define EZCFG_EZCFG_UPNP_1_DESCRIPTION_PATH "upnp.1.description_path"
#define EZCFG_EZCFG_UPNP_1_INTERFACE        "upnp.1.interface"

#define EZCFG_EZCFG_HTTPD_ENABLE            "httpd_enable"
#define EZCFG_EZCFG_HTTPD_HTTP              "httpd_http"
#define EZCFG_EZCFG_HTTPD_HTTPS             "httpd_https"
#define EZCFG_EZCFG_HTTPD_BINDING           "httpd_binding"

#define EZCFG_EZCFG_UPNPD_ENABLE            "upnpd_enable"
#define EZCFG_EZCFG_UPNPD_BINDING           "upnpd_binding"

#define EZCFG_EZCFG_UPNPD_IGD1_ENABLE       "upnpd_igd1_enable"

#define EZCFG_EZCFG_IGRSD_ENABLE            "igrsd_enable"
#define EZCFG_EZCFG_IGRSD_BINDING           "igrsd_binding"

#endif
