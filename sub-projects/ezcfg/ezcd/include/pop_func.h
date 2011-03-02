#ifndef _POP_FUNC_H_
#define _POP_FUNC_H_

extern int pop_etc_hostname(int flag);
extern int pop_etc_profile(int flag);
extern int pop_etc_banner(int flag);
extern int pop_etc_mtab(int flag);
extern int pop_etc_ezcfg_conf(int flag);
extern int pop_etc_hotplug2_rules(int flag);
extern int pop_etc_modules(int flag);
extern int pop_etc_inittab(int flag);
extern int pop_etc_passwd(int flag);
extern int pop_etc_group(int flag);
extern int pop_etc_network_interfaces(int flag);
extern int pop_etc_hosts(int flag);
extern int pop_etc_resolv_conf(int flag);
extern int pop_etc_protocols(int flag);
extern int pop_etc_dnsmasq_conf(int flag);

#endif
