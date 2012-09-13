/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-pppoe_server.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2012-09-13   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_PPPOE_SERVER_H_
#define _EZCFG_PPPOE_SERVER_H_

/* ezcfg nvram name prefix */
#define EZCFG_PPPOE_SERVER_NVRAM_PREFIX     "pppoe_server_"

#define EZCFG_PPPOE_SERVER_IFNAME           "ifname"
#define EZCFG_PPPOE_SERVER_LOCAL_IPADDR     "local_ipaddr"
#define EZCFG_PPPOE_SERVER_REMOTE_IPADDR    "remote_ipaddr"
#define EZCFG_PPPOE_SERVER_SESSION_NUMBER   "session_number"

#define EZCFG_PPPOE_SERVER_DEFAULT_LOCAL_IPADDR \
	"10.0.0.1"
#define EZCFG_PPPOE_SERVER_DEFAULT_REMOTE_IPADDR \
	"10.67.15.1"
#endif
