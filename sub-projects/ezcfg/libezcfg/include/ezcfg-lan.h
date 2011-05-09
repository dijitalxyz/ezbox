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
#define EZCFG_LAN_HTTPD_ENABLE           "httpd_enable"
#define EZCFG_LAN_DHCPD_ENABLE           "dhcpd_enable"
#define EZCFG_LAN_PPPOED_ENABLE          "pppoed_enable"
#define EZCFG_LAN_IPADDR                 "ipaddr"
#define EZCFG_LAN_NETMASK                "netmask"
#define EZCFG_LAN_WINS                   "wins"
#define EZCFG_LAN_DOMAIN                 "domain"
#define EZCFG_LAN_LEASE                  "lease"
#define EZCFG_LAN_STP_ENABLE             "stp_enable"

/* LAN DHCP server parameters */
#define EZCFG_LAN_DHCPD_START_IPADDR     "dhcpd_start_ipaddr"
#define EZCFG_LAN_DHCPD_END_IPADDR       "dhcpd_end_ipaddr"
#define EZCFG_LAN_DHCPD_LEASE            "dhcpd_lease"
#define EZCFG_LAN_DHCPD_GATEWAY          "dhcpd_gateway"
#define EZCFG_LAN_DHCPD_DNS1             "dhcpd_dns1"
#define EZCFG_LAN_DHCPD_DNS2             "dhcpd_dns2"
#define EZCFG_LAN_DHCPD_DNS3             "dhcpd_dns3"
#define EZCFG_LAN_DHCPD_DOMAIN           "dhcpd_domain"
#define EZCFG_LAN_DHCPD_WINS             "dhcpd_wins"

/* LAN HTTP server parameters */
#define EZCFG_LAN_HTTPD_HTTP             "httpd_http"
#define EZCFG_LAN_HTTPD_HTTPS            "httpd_https"

/* default values */
#define EZCFG_LAN_DEFAULT_IPADDR         "192.168.1.1"
#define EZCFG_LAN_DEFAULT_NETMASK        "255.255.255.0"
#define EZCFG_LAN_DEFAULT_DHCPD_START_IPADDR     "192.168.1.100"
#define EZCFG_LAN_DEFAULT_DHCPD_END_IPADDR       "192.168.1.100"

#endif
