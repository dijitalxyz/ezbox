/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-ezcfg.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010-2011 by ezbox-project
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

#define EZCFG_EZCFG_COMMON_LOG_LEVEL        "common.log_level"
#define EZCFG_EZCFG_COMMON_RULES_PATH       "common.rules_path"
#define EZCFG_EZCFG_COMMON_SOCKET_NUMBER    "common.socket_number"
#define EZCFG_EZCFG_COMMON_LOCALE           "common.locale"

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

#endif
