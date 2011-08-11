#ifndef _UTILS_H_
#define _UTILS_H_

/* shell command */
#define CMD_CAT			"/bin/cat"
#define CMD_CHMOD		"/bin/chmod"
#define CMD_CHPASSWD		"/sbin/chpasswd"
#define CMD_EZCD		"/sbin/ezcd"
//#define CMD_HOTPLUG2		"/sbin/hotplug2"
#define CMD_IFCONFIG		"/sbin/ifconfig"
#define CMD_IFDOWN		"/sbin/ifdown"
#define CMD_IFUP		"/sbin/ifup"
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
#define CMD_SYSLOGD  		"/sbin/syslogd"
#define CMD_TELNETD  		"/sbin/telnetd"
#define CMD_UDEVTRIGGER		"/sbin/udevtrigger"
#define CMD_UMOUNT		"/bin/umount"

/* boot config file path */
#define BOOT_CONFIG_FILE_PATH   "/boot/ezbox_boot.cfg"

/* upgrade config file path */
#define UPGRADE_CONFIG_FILE_PATH   "/boot/ezbox_upgrade.cfg"

/* root HOME path */
#define ROOT_HOME_PATH		"/root"

/* ezcd command config file path */
#define EZCD_CONFIG_FILE_PATH	"/etc/ezcd.conf"

/* ezcm command config file path */
#define EZCM_CONFIG_FILE_PATH	"/etc/ezcm.conf"

/* nvram command config file path */
#define NVRAM_CONFIG_FILE_PATH	"/etc/nvram.conf"

/* udhcpc script path */
#define UDHCPC_SCRIPT_PATH	"/usr/share/udhcpc/default.script"

/* Linux kernel version */
#define KERNEL_VERSION(a,b,c)	(((a) << 16) + ((b) << 8) + (c))

/* Linux kernel char __initdata boot_command_line[COMMAND_LINE_SIZE]; */
#define KERNEL_COMMAND_LINE_SIZE	512

#define RC_RUN_LEVEL(a,b,c)	(((a) << 16) + ((b) << 8) + (c))

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

/* rc state */
enum {
	RC_BOOT = 0,
	RC_START,
	RC_STOP,
	RC_RESTART,
	RC_RELOAD,
};

/* rc debug state */
enum {
	RC_DEBUG_UNKNOWN = 0,
	RC_DEBUG_DUMP,
};

/* WAN type */
enum {
	WAN_TYPE_UNKNOWN = 0,
	WAN_TYPE_DHCP,
	WAN_TYPE_STATIC,
	WAN_TYPE_PPPOE,
};

#endif

