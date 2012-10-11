/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-freeradius2_server.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2012-10-11   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_FREERADIUS2_SERVER_H_
#define _EZCFG_FREERADIUS2_SERVER_H_

/* ezcfg nvram name prefix */
#define EZCFG_FREERADIUS2_SERVER_NVRAM_PREFIX   "pppoe_server_"

/* ezcfg freeradius2 genenal options names */
#define EZCFG_FREERADIUS2_OPT_KEYWORD_NAME      "name"

/* ezcfg freeradius2 server nvram names */
#define EZCFG_FREERADIUS2_SERVER_IPADDR         "ipaddr"
#define EZCFG_FREERADIUS2_SERVER_PORT           "port"
#define EZCFG_FREERADIUS2_SERVER_NAME           "name"

#define EZCFG_FREERADIUS2_SERVER_DEFAULT_IPADDR "192.168.1.1"
#define EZCFG_FREERADIUS2_SERVER_DEFAULT_PORT   "1812,1813"

#endif
