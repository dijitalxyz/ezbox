#ifndef _UTILS_H_
#define _UTILS_H_

/* shell command */
#define CMD_CAT			"/bin/cat"
#define CMD_CHMOD		"/bin/chmod"
#define CMD_CHPASSWD		"/sbin/chpasswd"
#define CMD_EZCD		"/sbin/ezcd"
#define CMD_EZCM		"/sbin/ezcm"
//#define CMD_HOTPLUG2		"/sbin/hotplug2"
#define CMD_IFCONFIG		"/sbin/ifconfig"
#define CMD_IFDOWN		"/sbin/ifdown"
#define CMD_IFUP		"/sbin/ifup"
#define CMD_IPTABLES		"/usr/sbin/iptables"
#define CMD_IPTABLES_RESTORE	"/usr/sbin/iptables-restore"
#define CMD_INSMOD		"/sbin/insmod"
#define CMD_KILLALL		"/bin/killall"
#define CMD_KLOGD  		"/sbin/klogd"
#define CMD_LDCONFIG  		"/sbin/ldconfig"
#define CMD_LOGIN  		"/bin/login"
#define CMD_MDEV  		"/sbin/mdev"
#define CMD_MKDIR  		"/bin/mkdir"
#define CMD_MOUNT  		"/bin/mount"
#define CMD_MV  		"/bin/mv"
#define CMD_RC  		"/sbin/rc"
#define CMD_RM  		"/bin/rm"
#define CMD_RMMOD  		"/sbin/rmmod"
#define CMD_ROUTE  		"/sbin/route"
#define CMD_SH  		"/bin/sh"
#define CMD_SHELLD  		"/sbin/shelld"
#define CMD_SYSLOGD  		"/sbin/syslogd"
#define CMD_TELNETD  		"/sbin/telnetd"
#define CMD_UDEVTRIGGER		"/sbin/udevtrigger"
#define CMD_UMOUNT		"/bin/umount"
#define CMD_UPNP_MONITOR	"/sbin/upnp_monitor"

/* boot config file path */
#define BOOT_CONFIG_FILE_PATH   "/boot/ezbox_boot.cfg"

/* upgrade config file path */
#define UPGRADE_CONFIG_FILE_PATH   "/boot/ezbox_upgrade.cfg"

/* root HOME path */
#define ROOT_HOME_PATH		"/root"

/* ezcd command config file path */
#define EZCD_CONFIG_FILE_PATH	"/etc/ezcd.conf"
#define EZCD_CONFIG_FILE_PATH_NEW	"/etc/ezcd.conf.new"

/* ezcm command config file path */
//#define EZCM_CONFIG_FILE_PATH	"/etc/ezcm.conf"

/* nvram command config file path */
//#define NVRAM_CONFIG_FILE_PATH	"/etc/nvram.conf"

/* upnp_monitor task file path */
#define UPNP_MONITOR_TASK_FILE_PATH	"/etc/upnp_monitor/tasks"

/* udhcpc script path */
#define UDHCPC_SCRIPT_PATH	"/usr/share/udhcpc/default.script"

/* Linux kernel version */
#define KERNEL_VERSION(a,b,c)	(((a) << 16) + ((b) << 8) + (c))

/* Linux kernel char __initdata boot_command_line[COMMAND_LINE_SIZE]; */
#define KERNEL_COMMAND_LINE_SIZE	512

#define RC_RUN_LEVEL(a,b,c)	(((a) << 16) + ((b) << 8) + (c))

/* removed line tails */
#define LINE_TAIL_STRING	" \t\r\n"

#define RC_MAX_ARGS	16
#define RC_COMMAND_LINE_SIZE	256
#define IS_BLANK(c)	((c) == ' ' || (c) == '\t')
#define IS_COLON(c)	((c) == ':')

typedef int (*rc_function_t)(int, char **);

typedef struct rc_func_s {
	char *name;
	int flag;
	int (*func)(int flag);
	int start;
	int stop;
	char *deps;
} rc_func_t;

typedef struct proc_stat_s {
	int pid;
	char state;
} proc_stat_t;

/* rc action state */
enum {
	RC_ACT_UNKNOWN = 0,
	RC_ACT_BOOT,
	RC_ACT_START,
	RC_ACT_STOP,
	RC_ACT_RESTART,
	RC_ACT_RELOAD,
	RC_ACT_COMBINED,
};

/* rc debug states */
enum {
	RC_DEBUG_UNKNOWN = 0,
	RC_DEBUG_DUMP,
};

/* init states */
enum {
	INIT_IDLE,
};

/* WAN type */
enum {
	WAN_TYPE_UNKNOWN = 0,
	WAN_TYPE_DHCP,
	WAN_TYPE_STATIC,
	WAN_TYPE_PPPOE,
};

#endif

