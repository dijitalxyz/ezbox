#ifndef _RC_FUNC_H_
#define _RC_FUNC_H_

extern int rc_base_files(int flag);
extern int rc_ezcd(int flag);
extern int rc_nvram(int flag);
extern int rc_ezcm(int flag);

extern int rc_ezcfg_httpd(int flag);
#if (HAVE_EZBOX_LAN_NIC == 1)
extern int rc_lan_ezcfg_httpd(int flag);
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
extern int rc_wan_ezcfg_httpd(int flag);
#endif

extern int rc_hotplug2(int flag);
extern int rc_init(int flag);
extern int rc_load_modules(int flag);
extern int rc_login(int flag);
extern int rc_loopback(int flag);
extern int rc_netbase(int flag);
extern int rc_system(int flag);

#if (HAVE_EZBOX_SERVICE_SYSLOG == 1)
extern int rc_syslog(int flag);
#if (HAVE_EZBOX_LAN_NIC == 1)
extern int rc_lan_syslog(int flag);
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
extern int rc_wan_syslog(int flag);
#endif
#endif

#if (HAVE_EZBOX_SERVICE_TELNETD == 1)
extern int rc_telnetd(int flag);
#if (HAVE_EZBOX_LAN_NIC == 1)
extern int rc_lan_telnetd(int flag);
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
extern int rc_wan_telnetd(int flag);
#endif
#endif

#if (HAVE_EZBOX_SERVICE_DNSMASQ == 1)
extern int rc_dnsmasq(int flag);
#if (HAVE_EZBOX_LAN_NIC == 1)
extern int rc_lan_dnsmasq(int flag);
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
extern int rc_wan_dnsmasq(int flag);
#endif
#endif

#if (HAVE_EZBOX_SERVICE_IPTABLES == 1)
extern int rc_iptables(int flag);
#if (HAVE_EZBOX_LAN_NIC == 1)
extern int rc_lan_iptables(int flag);
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
extern int rc_wan_iptables(int flag);
#endif
#endif

#if (HAVE_EZBOX_SERVICE_NANO_X == 1)
extern int rc_nano_x(int flag);
#elif (HAVE_EZBOX_SERVICE_KDRIVE == 1)
extern int rc_kdrive(int flag);
#endif

#if (HAVE_EZBOX_SERVICE_DILLO == 1)
extern int rc_dillo(int flag);
#endif

#if (HAVE_EZBOX_LAN_NIC == 1)
extern int rc_lan(int flag);
extern int rc_lan_if(int flag);
extern int rc_lan_services(int flag);
#endif

#if (HAVE_EZBOX_WAN_NIC == 1)
extern int rc_wan(int flag);
extern int rc_wan_if(int flag);
extern int rc_wan_services(int flag);
#endif

#endif
