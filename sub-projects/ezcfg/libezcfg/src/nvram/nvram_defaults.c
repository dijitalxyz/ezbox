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
	{ NVRAM_SERVICE_OPTION(SYS, DEVICE_NAME), "ezbox" }, /* ezbox */
	{ NVRAM_SERVICE_OPTION(SYS, SERIAL_NUMBER), "012345678901234567" }, /* */
	{ NVRAM_SERVICE_OPTION(SYS, HARDWARE_VERSION), "1.0" }, /* 1.0 */
	{ NVRAM_SERVICE_OPTION(SYS, SOFTWARE_VERSION), "1.0" }, /* 1.0 */
	{ NVRAM_SERVICE_OPTION(SYS, LANGUAGE), "zh_CN" }, /* zh_CN */
	{ NVRAM_SERVICE_OPTION(SYS, TZ_AREA), "Asia" }, /* Asia */
	{ NVRAM_SERVICE_OPTION(SYS, TZ_LOCATION), "Shanghai" }, /* Shanghai */
	{ NVRAM_SERVICE_OPTION(SYS, RESTORE_DEFAULTS), "0" }, /* Set to 0 not restore defaults on boot */
#if (HAVE_EZBOX_LAN_NIC == 1)
	{ NVRAM_SERVICE_OPTION(SYS, LAN_NIC), "eth0" },
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
	{ NVRAM_SERVICE_OPTION(SYS, WAN_NIC), "eth1" },
#endif

#if 0
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
#endif

	/*--------------------*/
	/* EZCFG parameters */
	/*--------------------*/
	/* EZCFG common log level */
	{ NVRAM_SERVICE_OPTION(EZCFG, COMMON_LOG_LEVEL),
	  EZCFG_COMMON_LOG_LEVEL_ERR_STRING }, /* err | info | debug */
	/* EZCFG common rules path */
	{ NVRAM_SERVICE_OPTION(EZCFG, COMMON_RULES_PATH),
	  EZCFG_COMMON_DEFAULT_RULES_PATH },
	/* EZCFG common socket number */
	{ NVRAM_SERVICE_OPTION(EZCFG, COMMON_SOCKET_NUMBER), "1" },
	/* EZCFG common locale */
	{ NVRAM_SERVICE_OPTION(EZCFG, COMMON_LOCALE),
	  EZCFG_COMMON_DEFAULT_LOCALE_STRING },
	/* EZCFG common authentication number */
	{ NVRAM_SERVICE_OPTION(EZCFG, COMMON_AUTH_NUMBER), "1" },
	/* EZCFG NVRAM buffer size */
	{ NVRAM_SERVICE_OPTION(EZCFG, NVRAM_0_BUFFER_SIZE),
	  EZCFG_NVRAM_BUFFER_SIZE_STRING },
	/* EZCFG NVRAM backend type */
	{ NVRAM_SERVICE_OPTION(EZCFG, NVRAM_0_BACKEND_TYPE),
	  EZCFG_NVRAM_BACKEND_FILE_STRING }, /* 0: none, 1: file, 2: flash, 3: hdd */
	{ NVRAM_SERVICE_OPTION(EZCFG, NVRAM_0_CODING_TYPE),
	  EZCFG_NVRAM_CODING_NONE_STRING }, /* 0: none, 1: gzip */
	/* EZCFG NVRAM storage path */
	{ NVRAM_SERVICE_OPTION(EZCFG, NVRAM_0_STORAGE_PATH),
	  EZCFG_NVRAM_STORAGE_PATH },
#if (1 < EZCFG_NVRAM_STORAGE_NUM)
	/* EZCFG NVRAM buffer size */
	{ NVRAM_SERVICE_OPTION(EZCFG, NVRAM_1_BUFFER_SIZE),
	  EZCFG_NVRAM_BUFFER_SIZE_STRING },
	/* EZCFG NVRAM backend type */
	{ NVRAM_SERVICE_OPTION(EZCFG, NVRAM_1_BACKEND_TYPE),
	  EZCFG_NVRAM_BACKEND_FILE_STRING }, /* 0: none, 1: file, 2: flash, 3: hdd */
	{ NVRAM_SERVICE_OPTION(EZCFG, NVRAM_1_CODING_TYPE),
	  EZCFG_NVRAM_CODING_NONE_STRING }, /* 0: none, 1: gzip */
	/* EZCFG NVRAM storage path */
	{ NVRAM_SERVICE_OPTION(EZCFG, NVRAM_1_STORAGE_PATH),
	  EZCFG_NVRAM_BACKUP_STORAGE_PATH },
#endif
	/* EZCFG socket domain */
	{ NVRAM_SERVICE_OPTION(EZCFG, SOCKET_0_DOMAIN),
	  EZCFG_SOCKET_DOMAIN_INET_STRING },
	/* EZCFG socket type */
	{ NVRAM_SERVICE_OPTION(EZCFG, SOCKET_0_TYPE),
	  EZCFG_SOCKET_TYPE_STREAM_STRING },
	/* EZCFG socket protocol */
	{ NVRAM_SERVICE_OPTION(EZCFG, SOCKET_0_PROTOCOL),
	  EZCFG_SOCKET_PROTO_HTTP_STRING },
	/* EZCFG socket address */
	{ NVRAM_SERVICE_OPTION(EZCFG, SOCKET_0_ADDRESS),
	  EZCFG_LOOPBACK_DEFAULT_IPADDR
	  ":" EZCFG_PROTO_HTTP_PORT_NUMBER_STRING },

	/* EZCFG authentication type */
	{ NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_TYPE),
	  EZCFG_AUTH_TYPE_HTTP_BASIC_STRING },
	/* EZCFG authentication username */
	{ NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_USER),
	  EZCFG_AUTH_USER_ADMIN_STRING },
	/* EZCFG authentication realm */
	{ NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_REALM),
	  EZCFG_AUTH_REALM_ADMIN_STRING },
	/* EZCFG authentication domain */
	{ NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_DOMAIN),
	  EZCFG_AUTH_DOMAIN_ADMIN_STRING },
	/* EZCFG authentication secret */
	{ NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_SECRET),
	  EZCFG_AUTH_SECRET_ADMIN_STRING },

	/* ezcfg HTTP server [1|0] */
	{ NVRAM_SERVICE_OPTION(EZCFG, HTTPD_ENABLE), "1" },
	/* Support HTTP protocol */
	{ NVRAM_SERVICE_OPTION(EZCFG, HTTPD_HTTP), "1" },
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
	/* Support HTTPS protocol */
	{ NVRAM_SERVICE_OPTION(EZCFG, HTTPD_HTTPS), "1" },
#endif
	/* ezcfg HTTP server binding interface */
#if (HAVE_EZBOX_LAN_NIC == 1)
	{ NVRAM_SERVICE_OPTION(EZCFG, HTTPD_BINDING), "lan" },
#else
	{ NVRAM_SERVICE_OPTION(EZCFG, HTTPD_BINDING), "wan" },
#endif

	/*--------------------*/
	/* Loopback H/W parameters */
	/*--------------------*/
	/* Loopback interface name */
	{ NVRAM_SERVICE_OPTION(LOOPBACK, IFNAME), "lo" },

	/* Loopback TCP/IP parameters */
	/* Loopback IP address */
	{ NVRAM_SERVICE_OPTION(LOOPBACK, IPADDR),
	  EZCFG_LOOPBACK_DEFAULT_IPADDR },
	/* Loopback netmask */
	{ NVRAM_SERVICE_OPTION(LOOPBACK, NETMASK),
	  EZCFG_LOOPBACK_DEFAULT_NETMASK },

#if (HAVE_EZBOX_LAN_NIC == 1)
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
	/* LAN IP address */
	{ NVRAM_SERVICE_OPTION(LAN, IPADDR),
	  EZCFG_LAN_DEFAULT_IPADDR },
	/* LAN netmask */
	{ NVRAM_SERVICE_OPTION(LAN, NETMASK),
	  EZCFG_LAN_DEFAULT_NETMASK },
	/* LAN side WINS server ip list [x.x.x.x x.x.x.x ...] */
	{ NVRAM_SERVICE_OPTION(LAN, WINS), "" },
	/* LAN domain name */
	{ NVRAM_SERVICE_OPTION(LAN, DOMAIN), "" },
	/* LAN spanning tree protocol */
	{ NVRAM_SERVICE_OPTION(LAN, STP_ENABLE), "0" },
#endif

#if (HAVE_EZBOX_WAN_NIC == 1)
	/* WAN H/W parameters */
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
	/* WAN connection type [dhcp|static|pppoe|pptp|l2tp|disabled] */
	{ NVRAM_SERVICE_OPTION(WAN, TYPE), "dhcp" },
	/* WAN connection release [1|0] */
	{ NVRAM_SERVICE_OPTION(WAN, RELEASE), "0" },
	/* WAN hostname */
	{ NVRAM_SERVICE_OPTION(WAN, HOSTNAME), "" },
	/* WAN MTU setting mode [1|0] */
	{ NVRAM_SERVICE_OPTION(WAN, MTU_ENABLE), "0" },
	/* Negotiate MTU to the smaller of this value or the peer MRU */
	{ NVRAM_SERVICE_OPTION(WAN, MTU), "1500" },

	/* WAN static IP parameters */
	/* WAN interface */
	{ NVRAM_SERVICE_OPTION(WAN, STATIC_IFNAME), "eth1" },
	/* WAN IP address */
	{ NVRAM_SERVICE_OPTION(WAN, STATIC_IPADDR), "0.0.0.0" },
	/* WAN netmask */
	{ NVRAM_SERVICE_OPTION(WAN, STATIC_NETMASK), "0.0.0.0" },
	/* WAN gateway */
	{ NVRAM_SERVICE_OPTION(WAN, STATIC_GATEWAY), "0.0.0.0" },
	/* WAN DNS server IP address */
	{ NVRAM_SERVICE_OPTION(WAN, STATIC_DNS1), "" },
	{ NVRAM_SERVICE_OPTION(WAN, STATIC_DNS2), "" },
	{ NVRAM_SERVICE_OPTION(WAN, STATIC_DNS3), "" },

	/* WAN DHCP Client parameters */
	/* WAN interface */
	{ NVRAM_SERVICE_OPTION(WAN, DHCP_IFNAME), "eth1" },
	/* WAN DHCP gateway set enable [0|1] */
	{ NVRAM_SERVICE_OPTION(WAN, DHCP_GATEWAY_ENABLE), "0" },
	/* WAN DHCP gateway MAC address */
	{ NVRAM_SERVICE_OPTION(WAN, DHCP_GATEWAY_MAC), "FF:FF:FF:FF:FF:FF" },
	/* WAN WINS server IP address [x.x.x.x x.x.x.x ...] */
	{ NVRAM_SERVICE_OPTION(WAN, DHCP_WINS), "" },


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
	/* PPTP DNS server IP address */
	{ NVRAM_SERVICE_OPTION(WAN, PPTP_DNS1), "" },
	{ NVRAM_SERVICE_OPTION(WAN, PPTP_DNS2), "" },
	{ NVRAM_SERVICE_OPTION(WAN, PPTP_DNS3), "" },
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
#endif

	/* rc services controller */
#if (HAVE_EZBOX_SERVICE_SYSLOG == 1)
	/* syslog disabled */
	{ NVRAM_SERVICE_OPTION(RC, SYSLOG_ENABLE), "0" },
#endif

#if (HAVE_EZBOX_SERVICE_TELNETD == 1)
	/* telnetd enabled */
	{ NVRAM_SERVICE_OPTION(RC, TELNETD_ENABLE), "1" },
#if (HAVE_EZBOX_LAN_NIC == 1)
	{ NVRAM_SERVICE_OPTION(RC, TELNETD_BINDING), "lan" },
#elif (HAVE_EZBOX_WAN_NIC == 1)
	{ NVRAM_SERVICE_OPTION(RC, TELNETD_BINDING), "wan" },
#endif
#endif

#if (HAVE_EZBOX_SERVICE_DNSMASQ == 1)
	/* dnsmasq enabled */
	{ NVRAM_SERVICE_OPTION(RC, DNSMASQ_ENABLE), "1" },
#if (HAVE_EZBOX_LAN_NIC == 1)
	{ NVRAM_SERVICE_OPTION(RC, DNSMASQ_BINDING), "lan" },
#elif (HAVE_EZBOX_WAN_NIC == 1)
	{ NVRAM_SERVICE_OPTION(RC, DNSMASQ_BINDING), "wan" },
#endif
	/* dnsmasq DHCP server */
	/* Enable DHCP server [1|0] */
	{ NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_ENABLE), "1" },
	/* First assignable DHCP address */
	{ NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_START_IPADDR), 
	  EZCFG_DNSMASQ_DEFAULT_DHCPD_START_IPADDR },
	/* Last assignable DHCP address */
	{ NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_END_IPADDR),
	  EZCFG_DNSMASQ_DEFAULT_DHCPD_END_IPADDR },
	/* LAN lease time in minutes */
	{ NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_LEASE),
	  EZCFG_DNSMASQ_DEFAULT_DHCPD_LEASE },
	/* LAN DHCP gateway IP address */
	{ NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_GATEWAY), "" },
	/* LAN DHCP DNS server use WAN settings */
	{ NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_WAN_DNS_ENABLE), "1" },
	/* LAN DHCP DNS server IP address */
	{ NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_DNS1), "" },
	{ NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_DNS2), "" },
	{ NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_DNS3), "" },
#endif

#if (HAVE_EZBOX_SERVICE_IPTABLES == 1)
	/* iptables enabled */
	{ NVRAM_SERVICE_OPTION(RC, IPTABLES_ENABLE), "1" },
	{ NVRAM_SERVICE_OPTION(RC, IPTABLES_BINDING), "wan" },
#endif

#if (HAVE_EZBOX_SERVICE_NANO_X == 1)
	/* nano-X enabled */
	{ NVRAM_SERVICE_OPTION(RC, NANO_X_ENABLE), "1" },
	/* nano-X mouse port */
	{ NVRAM_SERVICE_OPTION(NANO_X, MOUSE_PORT), "/dev/input/mouse0" },
	/* nano-X console device */
	{ NVRAM_SERVICE_OPTION(NANO_X, CONSOLE), "/dev/console" },
#endif

};

char *default_nvram_savings[] = {
	NVRAM_SERVICE_OPTION(SYS, SERIAL_NUMBER),
	NVRAM_SERVICE_OPTION(SYS, LANGUAGE),
};

/* Public functions */
int ezcfg_nvram_get_num_default_nvram_settings(void)
{
	return ARRAY_SIZE(default_nvram_settings);
}

int ezcfg_nvram_get_num_default_nvram_savings(void)
{
	return ARRAY_SIZE(default_nvram_savings);
}
