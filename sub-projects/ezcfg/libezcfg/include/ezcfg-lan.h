/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-lan.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-18   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_LAN_H_
#define _EZCFG_LAN_H_

/* ezcfg nvram name prefix */
#define EZCFG_LAN_NVRAM_PREFIX            "lan_"

/* ezcfg lan control names */
/* LAN H/W parameters */
#define EZCFG_LAN_IFNAME                 "ifname"
#define EZCFG_LAN_IFNAMES                "ifnames"
#define EZCFG_LAN_HWNAME                 "hwname"
#define EZCFG_LAN_HWADDR                 "hwaddr"
#define EZCFG_LAN_PHYMODE                "phymode"

/* LAN TCP/IP parameters */
#define EZCFG_LAN_DHCPD_ENABLE           "dhcpd_enable"
#define EZCFG_LAN_PPPOED_ENABLE          "pppoed_enable"
#define EZCFG_LAN_IPADDR                 "ipaddr"
#define EZCFG_LAN_NETMASK                "netmask"
#define EZCFG_LAN_WINS                   "wins"
#define EZCFG_LAN_DOMAIN                 "domain"
#define EZCFG_LAN_LEASE                  "lease"
#define EZCFG_LAN_STP_ENABLE             "stp_enable"

/* LAN DHCP server parameters */
#define EZCFG_LAN_DHCPD_START            "dhcpd_start"
#define EZCFG_LAN_DHCPD_END              "dhcpd_end"
#define EZCFG_LAN_DHCPD_LEASE            "dhcpd_lease"
#define EZCFG_LAN_DHCPD_GATEWAY          "dhcpd_gateway"
#define EZCFG_LAN_DHCPD_DNS              "dhcpd_dns"
#define EZCFG_LAN_DHCPD_DOMAIN           "dhcpd_domain"
#define EZCFG_LAN_DHCPD_WINS             "dhcpd_wins"

#endif
