/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-loopback.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-12   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_LOOPBACK_H_
#define _EZCFG_LOOPBACK_H_

/* ezcfg nvram name prefix */
#define EZCFG_LOOPBACK_NVRAM_PREFIX            "loopback_"

/* ezcfg loopback control names */
/* Loopback H/W parameters */
#define EZCFG_LOOPBACK_IFNAME                 "ifname"
#define EZCFG_LOOPBACK_IFNAMES                "ifnames"
#define EZCFG_LOOPBACK_HWNAME                 "hwname"
#define EZCFG_LOOPBACK_HWADDR                 "hwaddr"
#define EZCFG_LOOPBACK_PHYMODE                "phymode"

/* Loopback TCP/IP parameters */
#define EZCFG_LOOPBACK_IPADDR                 "ipaddr"
#define EZCFG_LOOPBACK_NETMASK                "netmask"

/* default values */
#define EZCFG_LOOPBACK_DEFAULT_IPADDR         "127.0.0.1"
#define EZCFG_LOOPBACK_DEFAULT_NETMASK        "255.0.0.0"

#endif
