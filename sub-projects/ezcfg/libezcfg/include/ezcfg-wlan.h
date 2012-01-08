/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-wlan.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2011-08-02   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_WLAN_H_
#define _EZCFG_WLAN_H_

/* ezcfg nvram name prefix */
#define EZCFG_WLAN_NVRAM_PREFIX            "wlan_"

/* ezcfg wlan control names */
/* WLAN H/W parameters */
#if 0
#define EZCFG_WLAN_IFNAME                 "ifname"
#define EZCFG_WLAN_IFNAMES                "ifnames"
#endif
#define EZCFG_WLAN_HWNAME                 "hwname"
#define EZCFG_WLAN_HWADDR                 "hwaddr"
#define EZCFG_WLAN_PHYMODE                "phymode"
#define EZCFG_WLAN_NODE_TYPE              "node_type"

#endif
