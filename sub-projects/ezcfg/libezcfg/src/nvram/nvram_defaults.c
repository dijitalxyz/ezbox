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

ezcfg_nv_pair_t default_nvram_settings[] = {
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

	/*--------------------*/
	/* EZCFG parameters */
	/*--------------------*/
	/* EZCFG common log level */
	{ NVRAM_SERVICE_OPTION(EZCFG, COMMON_LOG_LEVEL), EZCFG_COMMON_LOG_LEVEL_ERR_STRING }, /* err | info | debug */
	/* EZCFG common rules path */
	{ NVRAM_SERVICE_OPTION(EZCFG, COMMON_RULES_PATH), EZCFG_COMMON_DEFAULT_RULES_PATH },
	/* EZCFG common socket number */
	{ NVRAM_SERVICE_OPTION(EZCFG, COMMON_SOCKET_NUMBER), "1" },
	/* EZCFG common locale */
	{ NVRAM_SERVICE_OPTION(EZCFG, COMMON_LOCALE), EZCFG_COMMON_DEFAULT_LOCALE_STRING },
	/* EZCFG common authentication number */
	{ NVRAM_SERVICE_OPTION(EZCFG, COMMON_AUTH_NUMBER), "1" },
	/* EZCFG NVRAM buffer size */
	{ NVRAM_SERVICE_OPTION(EZCFG, NVRAM_0_BUFFER_SIZE), EZCFG_NVRAM_BUFFER_SIZE_STRING },
	/* EZCFG NVRAM backend type */
	{ NVRAM_SERVICE_OPTION(EZCFG, NVRAM_0_BACKEND_TYPE), EZCFG_NVRAM_BACKEND_FILE_STRING }, /* 0: none, 1: file, 2: flash, 3: hdd */
	{ NVRAM_SERVICE_OPTION(EZCFG, NVRAM_0_CODING_TYPE), EZCFG_NVRAM_CODING_NONE_STRING }, /* 0: none, 1: gzip */
	/* EZCFG NVRAM storage path */
	{ NVRAM_SERVICE_OPTION(EZCFG, NVRAM_0_STORAGE_PATH), EZCFG_NVRAM_STORAGE_PATH },
#if (1 < EZCFG_NVRAM_STORAGE_NUM)
	/* EZCFG NVRAM buffer size */
	{ NVRAM_SERVICE_OPTION(EZCFG, NVRAM_1_BUFFER_SIZE), EZCFG_NVRAM_BUFFER_SIZE_STRING },
	/* EZCFG NVRAM backend type */
	{ NVRAM_SERVICE_OPTION(EZCFG, NVRAM_1_BACKEND_TYPE), EZCFG_NVRAM_BACKEND_FILE_STRING }, /* 0: none, 1: file, 2: flash, 3: hdd */
	{ NVRAM_SERVICE_OPTION(EZCFG, NVRAM_1_CODING_TYPE), EZCFG_NVRAM_CODING_NONE_STRING }, /* 0: none, 1: gzip */
	/* EZCFG NVRAM storage path */
	{ NVRAM_SERVICE_OPTION(EZCFG, NVRAM_1_STORAGE_PATH), EZCFG_NVRAM_BACKUP_STORAGE_PATH },
#endif
	/* EZCFG socket domain */
	{ NVRAM_SERVICE_OPTION(EZCFG, SOCKET_0_DOMAIN), EZCFG_SOCKET_DOMAIN_INET_STRING },
	/* EZCFG socket type */
	{ NVRAM_SERVICE_OPTION(EZCFG, SOCKET_0_TYPE), EZCFG_SOCKET_TYPE_STREAM_STRING },
	/* EZCFG socket protocol */
	{ NVRAM_SERVICE_OPTION(EZCFG, SOCKET_0_PROTOCOL), EZCFG_SOCKET_PROTO_HTTP_STRING },
	/* EZCFG socket address */
	{ NVRAM_SERVICE_OPTION(EZCFG, SOCKET_0_ADDRESS), "192.168.1.1:80" },

	/* EZCFG authentication type */
	{ NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_TYPE), EZCFG_AUTH_TYPE_HTTP_BASIC_STRING },
	/* EZCFG authentication username */
	{ NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_USER), EZCFG_AUTH_USER_ADMIN_STRING },
	/* EZCFG authentication realm */
	{ NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_REALM), EZCFG_AUTH_REALM_ADMIN_STRING },
	/* EZCFG authentication domain */
	{ NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_DOMAIN), EZCFG_AUTH_DOMAIN_ADMIN_STRING },
	/* EZCFG authentication secret */
	{ NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_SECRET), EZCFG_AUTH_SECRET_ADMIN_STRING },

	/*--------------------*/
	/* LAN H/W parameters */
	/*--------------------*/
	/* LAN interface name */
	{ NVRAM_SERVICE_OPTION(LAN, IFNAME), "eth0" },
	/* Enslaved LAN interfaces */
	{ NVRAM_SERVICE_OPTION(LAN, IFNAMES), "" },
	/* LAN driver names (e.g. 8139too) */
	{ NVRAM_SERVICE_OPTION(LAN, HWNAME), "" },
	/* LAN interface MAC address */
	{ NVRAM_SERVICE_OPTION(LAN, HWADDR), "" },
	/* LAN PHY port mode [auto|10half|10full|100half|100full|1000full] */
	{ NVRAM_SERVICE_OPTION(LAN, PHYMODE), "auto" },

	/* LAN TCP/IP parameters */
	/* LAN size DHCP server [1|0] */
	{ NVRAM_SERVICE_OPTION(LAN, DHCPD_ENABLE), "1" },
	/* LAN size PPPoE server [1|0] */
	{ NVRAM_SERVICE_OPTION(LAN, PPPOED_ENABLE), "0" },
	/* LAN IP address */
	{ NVRAM_SERVICE_OPTION(LAN, IPADDR), "192.168.1.1" },
	/* LAN netmask */
	{ NVRAM_SERVICE_OPTION(LAN, NETMASK), "255.255.255.0" },
	/* LAN side WINS server ip list [x.x.x.x x.x.x.x ...] */
	{ NVRAM_SERVICE_OPTION(LAN, WINS), "" },
	/* LAN domain name */
	{ NVRAM_SERVICE_OPTION(LAN, DOMAIN), "" },
	/* LAN lease time in seconds */
	{ NVRAM_SERVICE_OPTION(LAN, LEASE), "86400" },
	/* LAN spanning tree protocol */
	{ NVRAM_SERVICE_OPTION(LAN, STP_ENABLE), "0" },

	/* WAN H/W parameters */
	/* WAN interface name */
	{ NVRAM_SERVICE_OPTION(WAN, IFNAME), "eth1" },
	/* WAN interface names */
	{ NVRAM_SERVICE_OPTION(WAN, IFNAMES), "" },
	/* WAN driver name (e.g. 8139cp) */
	{ NVRAM_SERVICE_OPTION(WAN, HWNAME), "" },
	/* WAN interface MAC address */
	{ NVRAM_SERVICE_OPTION(WAN, HWADDR), "" },
	/* WAN PHY port mode [auto|10half|10full|100half|100full|1000full] */
	{ NVRAM_SERVICE_OPTION(WAN, PHYMODE), "auto" },
	/* WAN interface MAC clone [0|1] */
	{ NVRAM_SERVICE_OPTION(WAN, MACCLONE_ENABLE), "0" },
	/* WAN interface MAC clone address */
	{ NVRAM_SERVICE_OPTION(WAN, CLONE_HWADDR), "" },

        /* WAN TCP/IP parameters */
	/* WAN connection type [static|dhcp|pppoe|pptp|l2tp|disabled] */
	{ NVRAM_SERVICE_OPTION(WAN, TYPE), "dhcp" },
	/* WAN connection release [1|0] */
	{ NVRAM_SERVICE_OPTION(WAN, RELEASE), "0" },
	/* WAN IP address */
	{ NVRAM_SERVICE_OPTION(WAN, IPADDR), "0.0.0.0" },
	/* WAN netmask */
	{ NVRAM_SERVICE_OPTION(WAN, NETMASK), "0.0.0.0" },
	/* WAN gateway */
	{ NVRAM_SERVICE_OPTION(WAN, GATEWAY), "0.0.0.0" },
	/* WAN DNS server IP address [x.x.x.x x.x.x.x ...] */
	{ NVRAM_SERVICE_OPTION(WAN, DNS), "" },
	/* WAN WINS server IP address [x.x.x.x x.x.x.x ...] */
	{ NVRAM_SERVICE_OPTION(WAN, WINS), "" },
	/* WAN hostname */
	{ NVRAM_SERVICE_OPTION(WAN, HOSTNAME), "" },
	/* WAN domain name */
	{ NVRAM_SERVICE_OPTION(WAN, DOMAIN), "" },
	/* WAN lease time in seconds */
	{ NVRAM_SERVICE_OPTION(WAN, LEASE), "86400" },
	/* WAN MTU [1|0] */
	{ NVRAM_SERVICE_OPTION(WAN, MTU_ENABLE), "0" },
	/* Negotiate MTU to the smaller of this value or the peer MRU */
	{ NVRAM_SERVICE_OPTION(WAN, MTU), "1500" },

	/* WAN static IP parameters */
	/* WAN IP address */
	{ NVRAM_SERVICE_OPTION(WAN, STATIC_IPADDR), "0.0.0.0" },
	/* WAN netmask */
	{ NVRAM_SERVICE_OPTION(WAN, STATIC_NETMASK), "0.0.0.0" },
	/* WAN gateway */
	{ NVRAM_SERVICE_OPTION(WAN, STATIC_GATEWAY), "0.0.0.0" },
	/* WAN DNS server IP address [x.x.x.x x.x.x.x ...] */
	{ NVRAM_SERVICE_OPTION(WAN, STATIC_DNS), "" },

	/* WAN DHCP Client parameters */
	/* WAN DHCP gateway set enable [0|1] */
	{ NVRAM_SERVICE_OPTION(WAN, DHCP_GATEWAY_ENABLE), "0" },
	/* WAN DHCP gateway MAC address */
	{ NVRAM_SERVICE_OPTION(WAN, DHCP_GATEWAY_MAC), "FF:FF:FF:FF:FF:FF" },

	/* PPPoE parameters */
	/* PPPoE enslaved interface */
	{ NVRAM_SERVICE_OPTION(WAN, PPPOE_IFNAME), "ppp0" },
	/* Restore link automatically [0|1] */
	{ NVRAM_SERVICE_OPTION(WAN, PPPOE_KEEP_ENABLE), "1" },
	/* PPP username */
	{ NVRAM_SERVICE_OPTION(WAN, PPPOE_PPP_USERNAME), "" },
	/* PPP password */
	{ NVRAM_SERVICE_OPTION(WAN, PPPOE_PPP_PASSWD), "" },
	/* Dial on demand max idle time (seconds) */
	{ NVRAM_SERVICE_OPTION(WAN, PPPOE_PPP_IDLE), "60" },
	/* PPP Redial Period (seconds) */
	{ NVRAM_SERVICE_OPTION(WAN, PPPOE_PPP_LCP_ECHO_INTERVAL), "60" },
	/* PPP Redial Failure (times) */
	{ NVRAM_SERVICE_OPTION(WAN, PPPOE_PPP_LCP_ECHO_FAILURE), "3" },
	/* Negotiate MRU to this value */
	{ NVRAM_SERVICE_OPTION(WAN, PPPOE_MRU), "1492" },
	/* Negotiate MTU to the smaller of this value or the peer MRU */
	{ NVRAM_SERVICE_OPTION(WAN, PPPOE_MTU), "1492" },
	/* PPPoE service name */
	{ NVRAM_SERVICE_OPTION(WAN, PPPOE_SERVICE), "" },
	/* PPPoE access concentrator name */
	{ NVRAM_SERVICE_OPTION(WAN, PPPOE_ACNAME), "" },

	/* PPTP parameters */
	/* PPTP enslaved interface */
	{ NVRAM_SERVICE_OPTION(WAN, PPTP_IFNAME), "ppp0" },
	/* PPTP assigned IP address */
	{ NVRAM_SERVICE_OPTION(WAN, PPTP_IPADDR), "0.0.0.0" },
	/* PPTP assigned netmask */
	{ NVRAM_SERVICE_OPTION(WAN, PPTP_NETMASK), "0.0.0.0" },
	/* PPTP assigned gateway */
	{ NVRAM_SERVICE_OPTION(WAN, PPTP_GATEWAY), "0.0.0.0" },
	/* PPTP DNS server IP address [x.x.x.x x.x.x.x ...] */
	{ NVRAM_SERVICE_OPTION(WAN, PPTP_DNS), "" },
	/* PPTP server IP address */
	{ NVRAM_SERVICE_OPTION(WAN, PPTP_SERVER_IPADDR), "0.0.0.0" },
	/* Restore link automatically */
	{ NVRAM_SERVICE_OPTION(WAN, PPTP_KEEP_ENABLE), "1" },
	/* PPP username */
	{ NVRAM_SERVICE_OPTION(WAN, PPTP_PPP_USERNAME), "" },
	/* PPP password */
	{ NVRAM_SERVICE_OPTION(WAN, PPTP_PPP_PASSWD), "" },
	/* Dial on demand max idle time (seconds) */
	{ NVRAM_SERVICE_OPTION(WAN, PPTP_PPP_IDLE), "60" },
	/* PPP Redial Period  (seconds)*/
	{ NVRAM_SERVICE_OPTION(WAN, PPTP_PPP_LCP_ECHO_INTERVAL), "60" },
	/* PPP Redial Failure  (times) */
	{ NVRAM_SERVICE_OPTION(WAN, PPTP_PPP_LCP_ECHO_FAILURE), "3" },
	/* Negotiate MRU to this value */
	{ NVRAM_SERVICE_OPTION(WAN, PPTP_MRU), "1460" },
	/* Negotiate MTU to the smaller of this value or the peer MRU */
	{ NVRAM_SERVICE_OPTION(WAN, PPTP_MTU), "1460" },

	/* L2TP parameters */
	/* L2TP enslaved interface */
	{ NVRAM_SERVICE_OPTION(WAN, L2TP_IFNAME), "ppp0" },
	/* L2TP server IP address */
	{ NVRAM_SERVICE_OPTION(WAN, L2TP_SERVER_IPADDR), "0.0.0.0" },
	/* Restore link automatically */
	{ NVRAM_SERVICE_OPTION(WAN, L2TP_KEEP_ENABLE), "1" },
	/* PPP username */
	{ NVRAM_SERVICE_OPTION(WAN, L2TP_PPP_USERNAME), "" },
	/* PPP password */
	{ NVRAM_SERVICE_OPTION(WAN, L2TP_PPP_PASSWD), "" },
	/* Dial on demand max idle time (seconds) */
	{ NVRAM_SERVICE_OPTION(WAN, L2TP_PPP_IDLE), "60" },
	/* PPP Redial Period  (seconds) */
	{ NVRAM_SERVICE_OPTION(WAN, L2TP_PPP_LCP_ECHO_INTERVAL), "60" },
	/* PPP Redial Failure  (times) */
	{ NVRAM_SERVICE_OPTION(WAN, L2TP_PPP_LCP_ECHO_FAILURE), "3" },
	/* Negotiate MRU to this value */
	{ NVRAM_SERVICE_OPTION(WAN, L2TP_MRU), "1460" },
	/* Negotiate MTU to the smaller of this value or the peer MRU */
	{ NVRAM_SERVICE_OPTION(WAN, L2TP_MTU), "1460" },

	/* Static Route */
	{ "static_route", "" },	/* Static routes (ipaddr:netmask:gateway:metric:ifname ...) */

	/* LAN DHCP server */
	/* First assignable DHCP address */
	{ NVRAM_SERVICE_OPTION(LAN, DHCPD_START), "192.168.1.100" },
	/* Last assignable DHCP address */
	{ NVRAM_SERVICE_OPTION(LAN, DHCPD_END), "192.168.1.149" },
	/* LAN lease time in minutes */ /* Add */
	{ NVRAM_SERVICE_OPTION(LAN, DHCPD_LEASE), "60" },
	/* LAN DHCP gateway IP address */
	{ NVRAM_SERVICE_OPTION(LAN, DHCPD_GATEWAY), "" },
	/* LAN DNS server IP address */
	{ NVRAM_SERVICE_OPTION(LAN, DHCPD_DNS), "" },
	/* Use WAN domain name first if available (wan|lan) */
	{ NVRAM_SERVICE_OPTION(LAN, DHCPD_DOMAIN), "wan" },
	/* Use WAN WINS first if available (wan|lan) */
	{ NVRAM_SERVICE_OPTION(LAN, DHCPD_WINS), "wan" },

	/* rc services controller */
	/* syslog disabled */
	{ NVRAM_SERVICE_OPTION(RC, SYSLOG_ENABLE), "0" },
#if (EZCFG_EZBOX_DISTRO == EZCFG_EZBOX_DISTRO_KUAFU)
	/* telnetd enabled */
	{ NVRAM_SERVICE_OPTION(RC, TELNETD_ENABLE), "1" },
#else
	/* telnetd disabled */
	{ NVRAM_SERVICE_OPTION(RC, TELNETD_ENABLE), "0" },
#endif
#if (EZCFG_EZBOX_DISTRO == EZCFG_EZBOX_DISTRO_KUAFU)
	/* dnsmasq disabled */
	{ NVRAM_SERVICE_OPTION(RC, DNSMASQ_ENABLE), "0" },
#else
	/* dnsmasq enabled */
	{ NVRAM_SERVICE_OPTION(RC, DNSMASQ_ENABLE), "1" },
#endif
};

char *default_nvram_unsets[] = {
	/* WAN IP address */
	NVRAM_SERVICE_OPTION(WAN, IPADDR),
	/* WAN netmask */
	NVRAM_SERVICE_OPTION(WAN, NETMASK),
	/* WAN gateway */
	NVRAM_SERVICE_OPTION(WAN, GATEWAY),
	/* WAN DNS server IP address [x.x.x.x x.x.x.x ...] */
	NVRAM_SERVICE_OPTION(WAN, DNS),
	/* WAN domain name */
	NVRAM_SERVICE_OPTION(WAN, DOMAIN),
	/* WAN lease time in seconds */
	NVRAM_SERVICE_OPTION(WAN, LEASE),
};

/* Public functions */
int ezcfg_nvram_get_num_default_nvram_settings(void)
{
	return ARRAY_SIZE(default_nvram_settings);
}

int ezcfg_nvram_get_num_default_nvram_unsets(void)
{
	return ARRAY_SIZE(default_nvram_unsets);
}
