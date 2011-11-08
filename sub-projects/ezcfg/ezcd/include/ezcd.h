#ifndef _EZCD_H_
#define _EZCD_H_

#include "ezcfg.h"
#include "ezcfg-igrs.h"
#include "ezcfg-api.h"

#include "utils.h"

#define RCSO_PATH_PREFIX "/lib/rcso"
#define RCSO_PATH_PREFIX2 "/usr/lib/rcso"
#define ACTION_PATH_PREFIX "/lib/rcso/action"
#define ACTION_PATH_PREFIX2 "/usr/lib/rcso/action"
#define EXEC_PATH_PREFIX "/lib/rcso/exec"
#define EXEC_PATH_PREFIX2 "/usr/lib/rcso/exec"

/* utils function declaration */

/* FIXME: these functions return pointer get from malloc, must free it */
char *utils_file_get_keyword(char *filename, char *keyword);
char *utils_file_get_keyword_by_index(char *filename, char *keyword, int idx);
char *utils_get_kernel_version(void);

int utils_udev_trigger(void);
int utils_udev_pop_nodes(void);

int utils_get_kernel_modules(char *buf, int buf_len);

int utils_install_kernel_module(char *name, char *args);
int utils_remove_kernel_module(char *name);

int utils_get_bootcfg_keyword(char *name, char *buf, int buf_len);

int utils_get_boot_device_path(char *buf, int buf_len);
int utils_get_boot_device_fs_type(char *buf, int buf_len);
int utils_get_data_device_path(char *buf, int buf_len);
int utils_get_data_device_fs_type(char *buf, int buf_len);

bool utils_file_get_line(FILE *fp, char *buf, size_t size, char *comment, char *remove);

int utils_mount_partition(char *dev, char *path, char *fs_type, char *args);
int utils_umount_partition(char *path);
int utils_remount_partition(char *dev, char *path, char *fs_type, char *args);
int utils_mount_boot_partition_readonly(void);
int utils_mount_boot_partition_writable(void);
int utils_remount_boot_partition_readonly(void);
int utils_remount_boot_partition_writable(void);
int utils_mount_data_partition_writable(void);

int utils_sync_nvram_with_cfg(char *path);
int utils_sync_cfg_with_nvram(char *path);

ssize_t utils_safe_read(int fd, void *buf, size_t count);
ssize_t utils_safe_write(int fd, const void *buf, size_t count);
ssize_t utils_full_write(int fd, const void *buf, size_t len);

int utils_parse_args(char *buf, size_t size, char **argv);
int utils_device_open(const char *device, int mode);

rc_func_t *utils_find_rc_func(char *name);
int utils_get_rc_funcs_number(void);
char *utils_get_rc_func_name(int i);

proc_stat_t *utils_find_pid_by_name(char *pidName);
bool utils_has_process_by_name(char *pidName);
uint32_t utils_crc32(unsigned char *data, int len);
int utils_get_rc_act_type(char *act);
#if (HAVE_EZBOX_WAN_NIC == 1)
int utils_get_wan_type(void);
#endif
bool utils_ezcd_is_alive(void);
bool utils_ezcd_is_ready(void);
bool utils_ezcd_wait_up(int s);
bool utils_ezcd_wait_down(int s);
#if (HAVE_EZBOX_LAN_NIC == 1)
bool utils_service_binding_lan(char *name);
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
bool utils_service_binding_wan(char *name);
bool utils_wan_interface_is_up(void);
bool utils_wan_interface_wait_up(int s);
bool utils_wan_interface_wait_down(int s);
#endif

/* nvram function */
int utils_nvram_cmp(const char *name, const char *value);

/* command entry point */
int preinit_main(int argc, char **argv);
int init_main(int argc, char **argv);
int ezcd_main(int argc, char **argv);
int ezcm_main(int argc, char **argv);
int nvram_main(int argc, char **argv);
int rc_main(int argc, char **argv);
int shelld_main(int argc, char **argv);
int ubootenv_main(int argc, char **argv);
int udhcpc_script_main(int argc, char **argv);
int upfw_main(int argc, char **argv);

#endif
