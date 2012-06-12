#ifndef _POP_FUNC_H_
#define _POP_FUNC_H_

extern int pop_etc_hostname(int flag);
extern int pop_etc_profile(int flag);
extern int pop_etc_banner(int flag);
extern int pop_etc_mtab(int flag);
extern int pop_etc_ezcd_conf(int flag);
extern int pop_etc_nvram_conf(int flag);
extern int pop_etc_ezcm_conf(int flag);
//extern int pop_etc_hotplug2_rules(int flag);
extern int pop_etc_mdev_conf(int flag);
extern int pop_etc_modules(int flag);
extern int pop_etc_inittab(int flag);
extern int pop_etc_passwd(int flag);
extern int pop_etc_group(int flag);
extern int pop_etc_network_interfaces(int flag);
extern int pop_etc_hosts(int flag);
extern int pop_etc_mactab(int flag);
extern int pop_etc_resolv_conf(int flag);
extern int pop_etc_protocols(int flag);
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
extern int pop_etc_ssl_openssl_cnf(int flag);
#endif
#if (HAVE_EZBOX_SERVICE_EZCFG_IGRSD == 1)
extern int pop_etc_ezcfg_igrsd(int flag);
#endif
#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD == 1)
extern int pop_etc_ezcfg_upnpd(int flag);
extern int pop_etc_ezcfg_upnpd_igd1(int flag);
#endif
#if (HAVE_EZBOX_SERVICE_DNSMASQ == 1)
extern int pop_etc_dnsmasq_conf(int flag);
#endif
#if (HAVE_EZBOX_SERVICE_DHID == 1)
extern int pop_etc_dhid_conf(int flag);
#endif
#if (HAVE_EZBOX_SERVICE_IPTABLES == 1)
extern int pop_etc_l7_protocols(int flag);
extern int pop_etc_iptables_modules(int flag);
extern int pop_etc_iptables_firewall(int flag);
#endif
#if (HAVE_EZBOX_SERVICE_FONTCONFIG == 1)
extern int pop_etc_fonts_fonts_dtd(int flag);
extern int pop_etc_fonts_fonts_conf(int flag);
#endif
#if (HAVE_EZBOX_SERVICE_DILLO == 1)
extern int pop_etc_dillo_dillorc(int flag);
#endif
#if (HAVE_EZBOX_SERVICE_WPA_SUPPLICANT == 1)
extern int pop_etc_wpa_supplicant_conf(int flag);
#endif
#if (HAVE_EZBOX_SERVICE_EMC2 == 1)
extern int pop_etc_emc2_rtapi_conf(int flag);
extern int pop_etc_emc2_configs(int flag);
#endif

#endif
