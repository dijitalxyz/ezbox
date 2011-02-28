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
#define CMD_RM  		"/bin/rm"
#define CMD_UDEVTRIGGER		"/sbin/udevtrigger"

/* root HOME path */
#define ROOT_HOME_PATH		"/root"

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
	RC_BOOT,
	RC_RESTART,
	RC_STOP,
	RC_START,
};

#endif

