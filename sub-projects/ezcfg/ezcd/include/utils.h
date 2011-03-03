#ifndef _UTILS_H_
#define _UTILS_H_

/* shell command */
#define CMD_CAT			"/bin/cat"
#define CMD_CHMOD		"/bin/chmod"
#define CMD_EZCD		"/sbin/ezcd"
#define CMD_HOTPLUG2		"/sbin/hotplug2"
#define CMD_IFCONFIG		"/sbin/ifconfig"
#define CMD_IFDOWN		"/sbin/ifdown"
#define CMD_IFUP		"/sbin/ifup"
#define CMD_KILLALL		"/usr/bin/killall"
#define CMD_MKDIR  		"/bin/mkdir"
#define CMD_MV  		"/bin/mv"
#define CMD_RM  		"/bin/rm"
#define CMD_ROUTE  		"/sbin/route"
#define CMD_UDEVTRIGGER		"/sbin/udevtrigger"

/* root HOME path */
#define ROOT_HOME_PATH		"/root"

/* udhcpc script path */
#define UDHCPC_SCRIPT_PATH	"/usr/share/udhcpc/default.script"

typedef struct rc_func_s {
	char *name;
	int flag;
	int (*func)(int flag);
} rc_func_t;

typedef struct proc_stat_s {
	int pid;
	char state;
} proc_stat_t;

/* rc state */
enum {
	RC_BOOT = 0,
	RC_START,
	RC_STOP,
	RC_RESTART,
	RC_RELOAD,
};

/* WAN type */
enum {
	WAN_TYPE_UNKNOWN = 0,
	WAN_TYPE_DHCP,
	WAN_TYPE_STATIC,
	WAN_TYPE_PPPOE,
};

#endif

