/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : nvram/nvram_defaults.c
 *
 * Description  : implement Non-Volatile RAM default settings
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-08-20   0.1       Write it from scratch
 * ============================================================================
 */

#include "ezcfg-private.h"
#include "ezcfg.h"

struct ezcfg_nvram_pair default_nvram_settings[] = {
	/* System configuration */
	{ "sys_passwd", "admin" },	/* The root password */
	{ "sys_hostname", "ezbox" },	/* The system host name */
	{ "timer_interval", "3600" },	/* Timer interval in seconds */

	/* Restore defaults */
	{ "restore_defaults", "0" },	/* Set to 0 not restore defaults on boot */

	/* kernel modules */
	{ "mod_wait_time", "30" },	/* wait module up time */

	/* NTP */
	{ "ntp_server", "cn.pool.ntp.org hk.pool.ntp.org" }, /* NTP server */
	{ "ntp_mode", "auto" },		/* NTP server mode [manual | auto] */
	{ "ntp_enable", "1" },
	{ "ntp_timezone", "PRC" },

	/* Process Monitor */
	{ "process_monitor_interval", "3" },	/* process_monitor check interval, should be 1-10, default is 3 munites, 0 means not start process_monitor */
	{ "process_monitor_cron_period", "12" },/* cron task for checking process_monitor period, should be 3-30, default is 12 munites, 0 means not setup cron task */

	/* LAN H/W parameters */
	{ "lan_ifname", "eth0" },	/* LAN interface name */
	{ "lan_ifnames", "" },		/* Enslaved LAN interfaces */
	{ "lan_hwname", "" },		/* LAN driver names (e.g. et0) */
	{ "lan_hwaddr", "" },		/* LAN interface MAC address */
	{ "lan_phymode", "auto" },      /* LAN PHY port mode [auto|10half|10full|100half|100full|1000full] */

	/* LAN TCP/IP parameters */
	{ "lan_dhcp", "1" },			/* LAN size DHCP server [1|0] */
	{ "lan_pppoe", "0" },			/* LAN size PPPoE server [1|0] */
	{ "lan_ipaddr", "192.168.1.1" },	/* LAN IP address */
	{ "lan_netmask", "255.255.255.0" },	/* LAN netmask */
	{ "lan_wins", "" },			/* x.x.x.x x.x.x.x ... */
	{ "lan_domain", "" },			/* LAN domain name */
	{ "lan_lease", "86400" },		/* LAN lease time in seconds */
	{ "lan_stp", "0" },			/* LAN spanning tree protocol */

	/* WAN H/W parameters */
	{ "wan_ifname", "eth1" },	/* WAN interface name */
	{ "wan_hwname", "" },		/* WAN driver name (e.g. et1) */
	{ "wan_ifnames", "" },		/* WAN interface names */
	{ "wan_hwaddr", "" },		/* WAN interface MAC address */
	{ "wan_clone_hwaddr", "" },	/* WAN interface MAC address */
	{ "wan_macclone", "0" },	/* WAN interface MAC clone [0|1] */

        /* WAN TCP/IP parameters */
	{ "wan_proto", "dhcp" },	/* [static|dhcp|pppoe|pptp|l2tp|disabled] */
	{ "wan_release", "0" },		/* WAN connection release [1|0] */
	{ "wan_ipaddr", "0.0.0.0" },	/* WAN IP address */
	{ "wan_netmask", "0.0.0.0" },	/* WAN netmask */
	{ "wan_gateway", "0.0.0.0" },	/* WAN gateway */
	{ "wan_dns", "" },		/* x.x.x.x x.x.x.x ... */
	{ "wan_wins", "" },		/* x.x.x.x x.x.x.x ... */
	{ "wan_devicename", "ezbox" },
	{ "wan_hostname", "" },		/* WAN hostname */
	{ "wan_domain", "" },		/* WAN domain name */
	{ "wan_lease", "86400" },	/* WAN lease time in seconds */
	{ "wan_mtu_enable", "0" },	/* WAN MTU [1|0] */
	{ "wan_mtu", "1500" },		/* Negotiate MTU to the smaller of this value or the peer MRU */

	/* WAN static IP parameters */
	{ "wan_static_ipaddr", "0.0.0.0" },	/* WAN IP address */
	{ "wan_static_netmask", "0.0.0.0" },	/* WAN netmask */
	{ "wan_static_gateway", "0.0.0.0" },	/* WAN gateway */
	{ "wan_static_dns", "" },		/* x.x.x.x x.x.x.x ... */

	/* WAN DHCP Client parameters */
	{ "wan_dhcp_gw", "0" },
	{ "wan_dhcp_gateway", "FF:FF:FF:FF:FF:FF" },

	/* PPPoE parameters */
	{ "wan_pppoe_ifname", "ppp0" },	/* PPPoE enslaved interface */
	{ "wan_pppoe_username", "" },	/* PPP username */
	{ "wan_pppoe_passwd", "" },	/* PPP password */
	{ "wan_pppoe_demand", "0" },	/* Dial on demand */
	{ "wan_pppoe_keepalive", "1" },	/* Restore link automatically */
	{ "wan_pppoe_ppp_idle", "60" },	/* Dial on demand max idle time (seconds) */
	{ "wan_pppoe_ppp_lcp_echo_interval", "60" },	/* PPP Redial Period  (seconds) */
	{ "wan_pppoe_ppp_lcp_echo_failure", "3" },	/* PPP Redial Failure  (times) */
	{ "wan_pppoe_mru", "1492" },	/* Negotiate MRU to this value */
	{ "wan_pppoe_mtu", "1492" },	/* Negotiate MTU to the smaller of this value or the peer MRU */
	{ "wan_pppoe_service", "" },	/* PPPoE service name */
	{ "wan_pppoe_acname", "" },	/* PPPoE access concentrator name */

	/* PPTP parameters */
	{ "wan_pptp_ifname", "ppp0" },		/* PPTP enslaved interface */
	{ "wan_pptp_ipaddr", "0.0.0.0" },	/* PPTP assigned IP address */
	{ "wan_pptp_netmask", "0.0.0.0" },	/* PPTP assigned netmask */
	{ "wan_pptp_gateway", "0.0.0.0" },	/* PPTP assigned gateway */
	{ "wan_pptp_dns", "" },			/* x.x.x.x x.x.x.x ... */
	{ "wan_pptp_server_ipaddr", "0.0.0.0" },/* PPTP server IP address */
	{ "wan_pptp_demand", "0" },	/* Dial on demand */
	{ "wan_pptp_keepalive", "1" },	/* Restore link automatically */
	{ "wan_pptp_ppp_username", "" },/* PPP username */
	{ "wan_pptp_ppp_passwd", "" },	/* PPP password */
	{ "wan_pptp_ppp_idle", "60" },	/* Dial on demand max idle time (seconds) */
	{ "wan_pptp_ppp_lcp_echo_interval", "60" },	/* PPP Redial Period  (seconds)*/
	{ "wan_pptp_ppp_lcp_echo_failure", "3" },	/* PPP Redial Failure  (times) */
	{ "wan_pptp_mru", "1460" },	/* Negotiate MRU to this value */
	{ "wan_pptp_mtu", "1460" },	/* Negotiate MTU to the smaller of this value or the peer MRU */

	/* L2TP parameters */
	{ "wan_l2tp_ifname", "ppp0" },	/* L2TP enslaved interface */
	{ "wan_l2tp_server_ipaddr", "0.0.0.0" },	/* L2TP server IP address */
	{ "wan_l2tp_demand", "0" },	/* Dial on demand */
	{ "wan_l2tp_keepalive", "1" },	/* Restore link automatically */
	{ "wan_l2tp_ppp_username", "" },/* PPP username */
	{ "wan_l2tp_ppp_passwd", "" },	/* PPP password */
	{ "wan_l2tp_ppp_idle", "60" },	/* Dial on demand max idle time (seconds) */
	{ "wan_l2tp_ppp_lcp_echo_interval", "60" },	/* PPP Redial Period  (seconds) */
	{ "wan_l2tp_ppp_lcp_echo_failure", "3" },	/* PPP Redial Failure  (times) */
	{ "wan_l2tp_mru", "1460" },	/* Negotiate MRU to this value */
	{ "wan_l2tp_mtu", "1460" },	/* Negotiate MTU to the smaller of this value or the peer MRU */

	/* Static Route */
	{ "static_route", "" },	/* Static routes (ipaddr:netmask:gateway:metric:ifname ...) */

	/* LAN DHCP server */
	{ "lan_dhcp_start", "192.168.1.101" },	/* First assignable DHCP address */
	{ "lan_dhcp_end", "192.168.1.150" },	/* Last assignable DHCP address */
	{ "lan_dhcp_lease", "60" },	/* LAN lease time in minutes */ /* Add */
	{ "lan_dhcp_gateway", "" },	/* LAN DHCP gateway */
	{ "lan_dhcp_dns", "" },		/* LAN DHCP gateway */
	{ "lan_dhcp_domain", "wan" },	/* Use WAN domain name first if available (wan|lan) */
	{ "lan_dhcp_wins", "wan" },	/* Use WAN WINS first if available (wan|lan) */
};

/* Public functions */
int ezcfg_nvram_get_num_default_nvram_settings(void)
{
	return ARRAY_SIZE(default_nvram_settings);
}
