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
	/* kernel modules */
	{ NVRAM_SERVICE_OPTION(SYS, MODULES), "" },

#if 0
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
#elif (HAVE_EZBOX_SERVICE_KDRIVE == 1)
	/* TinyX/kdrive enabled */
	{ NVRAM_SERVICE_OPTION(RC, KDRIVE_ENABLE), "1" },
	/* TinyX/kdrive mouse driver */
	{ NVRAM_SERVICE_OPTION(KDRIVE, MOUSE_DRIVER), "mouse" },
	/* TinyX/kdrive mouse device */
	{ NVRAM_SERVICE_OPTION(KDRIVE, MOUSE_DEVICE), "/dev/input/mouse0" },
	/* TinyX/kdrive mouse protocol */
	{ NVRAM_SERVICE_OPTION(KDRIVE, MOUSE_PROTOCOL), "ps/2" },
	/* TinyX/kdrive keybd driver */
	{ NVRAM_SERVICE_OPTION(KDRIVE, KEYBD_DRIVER), "keyboard" },
#endif

#if (HAVE_EZBOX_SERVICE_DILLO == 1)
	/* dillo enabled */
	{ NVRAM_SERVICE_OPTION(RC, DILLO_ENABLE), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, GEOMETRY), "780x580" },
	{ NVRAM_SERVICE_OPTION(DILLO, LOAD_IMAGES), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, LOAD_STYLESHEETS), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, PARSE_EMBEDDED_CSS), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, FILTER_AUTO_REQUESTS), "same_domain" },
	{ NVRAM_SERVICE_OPTION(DILLO, BUFFERED_DRAWING), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, SAVE_DIR), "/tmp" },
	{ NVRAM_SERVICE_OPTION(DILLO, FONT_SERIF), "DejaVu Serif" },
	{ NVRAM_SERVICE_OPTION(DILLO, FONT_SANS_SERIF), "DejaVu Sans" },
	{ NVRAM_SERVICE_OPTION(DILLO, FONT_CURSIVE), "URW Chancery L" },
	{ NVRAM_SERVICE_OPTION(DILLO, FONT_FANTASY), "DejaVu Sans" },
	{ NVRAM_SERVICE_OPTION(DILLO, FONT_MONOSPACE), "DejaVu Sans Mono" },
	{ NVRAM_SERVICE_OPTION(DILLO, FONT_FACTOR), "1.0" },
	{ NVRAM_SERVICE_OPTION(DILLO, FONT_MAX_SIZE), "100" },
	{ NVRAM_SERVICE_OPTION(DILLO, FONT_MIN_SIZE), "12" },
	{ NVRAM_SERVICE_OPTION(DILLO, SHOW_TOOLTIP), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, W3C_PLUS_HEURISTICS), "0" },
	{ NVRAM_SERVICE_OPTION(DILLO, START_PAGE),
		"http://" EZCFG_LOOPBACK_DEFAULT_IPADDR "/" },
	{ NVRAM_SERVICE_OPTION(DILLO, HOME),
		"http://" EZCFG_LOOPBACK_DEFAULT_IPADDR "/" },
#if 0
	{ NVRAM_SERVICE_OPTION(DILLO, SEARCH_URL),
		"http://www.google.com/search?ie=UTF-8&oe=UTF-8&q=%s" },
	{ NVRAM_SERVICE_OPTION(DILLO, HTTP_LANGUAGE), "en-US" },
	{ NVRAM_SERVICE_OPTION(DILLO, HTTP_MAX_CONNS), "6" },
	{ NVRAM_SERVICE_OPTION(DILLO, HTTP_PROXY), "http://localhost:8080/" },
	{ NVRAM_SERVICE_OPTION(DILLO, HTTP_PROXYUSER), "joe" },
	{ NVRAM_SERVICE_OPTION(DILLO, NO_PROXY), "localhost 127.0.0.1" },
#endif
	{ NVRAM_SERVICE_OPTION(DILLO, HTTP_REFERER), "host" },
#if 0
	{ NVRAM_SERVICE_OPTION(DILLO, HTTP_USER_AGENT), "Dillo/2.2" },
#endif
	{ NVRAM_SERVICE_OPTION(DILLO, BG_COLOR), "0xdcd1ba" },
	{ NVRAM_SERVICE_OPTION(DILLO, ALLOW_WHITE_BG), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, CONTRAST_VISITED_COLOR), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, PANEL_SIZE), "small" },
	{ NVRAM_SERVICE_OPTION(DILLO, SMALL_ICONS), "0" },
	{ NVRAM_SERVICE_OPTION(DILLO, SHOW_BACK), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, SHOW_FORW), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, SHOW_HOME), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, SHOW_RELOAD), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, SHOW_SAVE), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, SHOW_STOP), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, SHOW_BOOKMARKS), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, SHOW_TOOLS), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, SHOW_FILEMENU), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, SHOW_CLEAR_URL), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, SHOW_URL), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, SHOW_SEARCH), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, SHOW_HELP), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, SHOW_PROGRESS_BOX), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, FULLWINDOW_START), "0" },
	{ NVRAM_SERVICE_OPTION(DILLO, ENTERPRESS_FORCES_SUBMIT), "0" },
	{ NVRAM_SERVICE_OPTION(DILLO, MIDDLE_CLICK_OPENS_NEW_TAB), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, MIDDLE_CLICK_DRAGS_PAGE), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, FOCUS_NEW_TAB), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, SHOW_MSG), "1" },
	{ NVRAM_SERVICE_OPTION(DILLO, SHOW_EXTRA_WARNINGS), "1" },
#endif

#if (HAVE_EZBOX_SERVICE_EMC2 == 1)
	/* EMC2 enabled */
	{ NVRAM_SERVICE_OPTION(RC, EMC2_ENABLE), "1" },
	{ NVRAM_SERVICE_OPTION(EMC2, MODULES),
		"rtai_hal,rtai_sched,rtai_fifos,rtai_shm,rtai_sem,rtai_math,emc2/rtapi,emc2/hal_lib" },
	{ NVRAM_SERVICE_OPTION(EMC2, MODPATH), "/usr/modules" },
	{ NVRAM_SERVICE_OPTION(EMC2, RTLIB_DIR), "/usr/modules/emc2" },
	{ NVRAM_SERVICE_OPTION(EMC2, RTAPI_DEBUG), "5" },
	{ NVRAM_SERVICE_OPTION(EMC2, INIFILE), "/root/emc2/configs/ezcnc.ini" },
	/* [EMC] section */
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_EMC_VERSION), "1.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_EMC_MACHINE), "EZCNC" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_EMC_DEBUG), "1" },
	/* [DISPLAY] section */
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_DISPLAY_DISPLAY), "xemc" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_DISPLAY_CYCLE_TIME), "0.200" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_DISPLAY_HELP_FILE), "xemc.txt" },
	/* RELATIVE or MACHINE */
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_DISPLAY_POSITION_OFFSET), "RELATIVE" },
	/* COMMANDED or ACTUAL */
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_DISPLAY_POSITION_FEEDBACK), "ACTUAL" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_DISPLAY_MAX_FEED_OVERRIDE), "1.2" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_DISPLAY_PROGRAM_PREFIX), "/var/emc2/nc_files" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_DISPLAY_INTRO_GRAPHIC), "emc2.gif" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_DISPLAY_INTRO_TIME), "5" },
	/* [TASK] section */
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_TASK_TASK), "milltask" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_TASK_CYCLE_TIME), "0.010" },
	/* [RS274NGC] section */
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_RS274NGC_PARAMETER_FILE), "ezcnc_rs274ngc.var" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_RS274NGC_LOG_FILE), "ezcnc_rs274ngc.log" },
	/* [EMCMOT] section */
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_EMCMOT_EMCMOT), "motmod" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_EMCMOT_COMM_TIMEOUT), "1.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_EMCMOT_COMM_WAIT), "0.010" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_EMCMOT_BASE_PERIOD), "100000" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_EMCMOT_SERVO_PERIOD), "1000000" },
	/* [HAL] section */
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE_NUM), "4" },
	/* first hal file */
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_0", "ezcnc_stepper_1.hal" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_0" ".1",
		"loadrt trivkins" },
	/* second hal file */
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_1", "ezcnc_stepper_2.hal" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_1" ".1",
		"loadrt [EMCMOT]EMCMOT base_period_nsec=[EMCMOT]BASE_PERIOD servo_period_nsec=[EMCMOT]SERVO_PERIOD num_joints=[TRAJ]AXES" },
	/* third hal file */
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2", "ezcnc_stepper_3.hal" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".1",
		"loadrt stepgen step_type=0,0,0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".2",
		"addf stepgen.make-pulses base-thread" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".3",
		"addf stepgen.capture-position servo-thread" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".4",
		"addf motion-command-handler servo-thread" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".5",
		"addf motion-controller servo-thread" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".6",
		"addf stepgen.update-freq servo-thread" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".7",
		"net Xpos-cmd axis.0.motor-pos-cmd => stepgen.0.position-cmd" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".8",
		"net Ypos-cmd axis.1.motor-pos-cmd => stepgen.1.position-cmd" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".9",
		"net Zpos-cmd axis.2.motor-pos-cmd => stepgen.2.position-cmd" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".10",
		"net Xpos-fb stepgen.0.position-fb => axis.0.motor-pos-fb" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".11",
		"net Ypos-fb stepgen.1.position-fb => axis.1.motor-pos-fb" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".12",
		"net Zpos-fb stepgen.2.position-fb => axis.2.motor-pos-fb" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".13",
		"net Xen axis.0.amp-enable-out => stepgen.0.enable" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".14",
		"net Yen axis.1.amp-enable-out => stepgen.1.enable" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".15",
		"net Zen axis.2.amp-enable-out => stepgen.2.enable" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".16",
		"net Xstep <= stepgen.0.step" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".17",
		"net Xdir  <= stepgen.0.dir" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".18",
		"net Ystep <= stepgen.1.step" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".19",
		"net Ydir  <= stepgen.1.dir" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".20",
		"net Zstep <= stepgen.2.step" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".21",
		"net Zdir  <= stepgen.2.dir" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".22",
		"setp stepgen.0.position-scale [AXIS_0]SCALE" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".23",
		"setp stepgen.1.position-scale [AXIS_1]SCALE" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".24",
		"setp stepgen.2.position-scale [AXIS_2]SCALE" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".25",
		"setp stepgen.0.maxaccel [AXIS_0]STEPGEN_MAXACCEL" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".26",
		"setp stepgen.1.maxaccel [AXIS_1]STEPGEN_MAXACCEL" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_2" ".27",
		"setp stepgen.2.maxaccel [AXIS_2]STEPGEN_MAXACCEL" },
	/* fourth hal file */
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_3", "ezcnc_pinout.hal" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_3" ".1",
		"loadrt hal_parport cfg=\"0x0378\"" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_3" ".2",
		"addf parport.0.read base-thread 1" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_3" ".3",
		"addf parport.0.write base-thread -1" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_3" ".4",
		"net Xstep => parport.0.pin-03-out" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_3" ".5",
		"net Xdir  => parport.0.pin-02-out" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_3" ".6",
		"net Ystep => parport.0.pin-05-out" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_3" ".7",
		"net Ydir  => parport.0.pin-04-out" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_3" ".8",
		"net Zstep => parport.0.pin-07-out" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_3" ".9",
		"net Zdir  => parport.0.pin-06-out" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_3" ".10",
		"net estop-loop iocontrol.0.user-enable-out iocontrol.0.emc-enable-in" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_3" ".11",
		"net tool-prep-loop iocontrol.0.tool-prepare iocontrol.0.tool-prepared" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_3" ".12",
		"net tool-change-loop iocontrol.0.tool-change iocontrol.0.tool-changed" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALFILE) "_3" ".13",
		"net spindle-on motion.spindle-on => parport.0.pin-09-out" },
	/* hal command number */
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_HAL_HALCMD_NUM), "0" },
	/* [TRAJ] section */
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_TRAJ_AXES), "3" },
	/* COORDINATES = X Y Z A B C */
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_TRAJ_COORDINATES), "X Y Z" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_TRAJ_HOME), "0 0 0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_TRAJ_LINEAR_UNITS), "mm" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_TRAJ_ANGULAR_UNITS), "degree" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_TRAJ_CYCLE_TIME), "0.010" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_TRAJ_DEFAULT_VELOCITY), "0.424" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_TRAJ_MAX_VELOCITY), "30.48" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_TRAJ_DEFAULT_ACCELERATION), "300.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_TRAJ_MAX_ACCELERATION), "500.0" },
	/* [EMCIO] section */
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_EMCIO_EMCIO), "io" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_EMCIO_CYCLE_TIME), "0.100" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_EMCIO_TOOL_TABLE), "ezcnc.tbl" },
	/* [AXIS_0] section */
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_0_TYPE), "LINEAR" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_0_HOME), "0.000" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_0_MAX_VELOCITY), "30.48" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_0_MAX_ACCELERATION), "500.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_0_STEPGEN_MAXACCEL), "520.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_0_BACKLASH), "0.000" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_0_SCALE), "200" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_0_OUTPUT_SCALE), "1.000" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_0_MIN_LIMIT), "-1000.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_0_MAX_LIMIT), "1000.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_0_FERROR), "1.270" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_0_MIN_FERROR), "0.254" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_0_HOME_OFFSET), "0.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_0_HOME_SEARCH_VEL), "0.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_0_HOME_LATCH_VEL), "0.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_0_HOME_USE_INDEX), "NO" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_0_HOME_IGNORE_LIMITS), "NO" },
	/* [AXIS_1] section */
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_1_TYPE), "LINEAR" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_1_HOME), "0.000" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_1_MAX_VELOCITY), "30.48" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_1_MAX_ACCELERATION), "500.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_1_STEPGEN_MAXACCEL), "520.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_1_BACKLASH), "0.000" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_1_SCALE), "200" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_1_OUTPUT_SCALE), "1.000" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_1_MIN_LIMIT), "-1000.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_1_MAX_LIMIT), "1000.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_1_FERROR), "1.270" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_1_MIN_FERROR), "0.254" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_1_HOME_OFFSET), "0.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_1_HOME_SEARCH_VEL), "0.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_1_HOME_LATCH_VEL), "0.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_1_HOME_USE_INDEX), "NO" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_1_HOME_IGNORE_LIMITS), "NO" },
	/* [AXIS_2] section */
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_2_TYPE), "LINEAR" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_2_HOME), "0.000" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_2_MAX_VELOCITY), "30.48" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_2_MAX_ACCELERATION), "500.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_2_STEPGEN_MAXACCEL), "520.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_2_BACKLASH), "0.000" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_2_SCALE), "200" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_2_OUTPUT_SCALE), "1.000" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_2_MIN_LIMIT), "-1000.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_2_MAX_LIMIT), "1000.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_2_FERROR), "1.270" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_2_MIN_FERROR), "0.254" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_2_HOME_OFFSET), "0.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_2_HOME_SEARCH_VEL), "0.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_2_HOME_LATCH_VEL), "0.0" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_2_HOME_USE_INDEX), "NO" },
	{ NVRAM_SERVICE_OPTION(EMC2, CONF_AXIS_2_HOME_IGNORE_LIMITS), "NO" },

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
