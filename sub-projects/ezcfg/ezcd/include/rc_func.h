#ifndef _RC_FUNC_H_
#define _RC_FUNC_H_

extern int rc_base_files(int flag);
extern int rc_ezcd(int flag);
extern int rc_hotplug2(int flag);
extern int rc_init(int flag);
extern int rc_lan(int flag);
extern int rc_lan_if(int flag);
extern int rc_load_modules(int flag);
extern int rc_login(int flag);
extern int rc_loopback(int flag);
extern int rc_netbase(int flag);
extern int rc_syslog(int flag);
extern int rc_system(int flag);
extern int rc_telnetd(int flag);
extern int rc_wan(int flag);
extern int rc_wan_if(int flag);

#endif
