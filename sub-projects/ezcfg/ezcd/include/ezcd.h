#ifndef _EZCD_H_
#define _EZCD_H_

#include "ezcfg.h"
#include "ezcfg-igrs.h"
#include "ezcfg-api.h"

#include "utils.h"

/* utils function declaration */
char *utils_get_kernel_modules(void);
char *utils_get_kernel_version(void);
rc_func_t *utils_find_rc_func(char *name);
proc_stat_t *utils_find_pid_by_name(char *pidName);
uint32_t utils_crc32(unsigned char *data, int len);
int utils_get_wan_type(void);
bool utils_ezcd_is_alive(void);
bool utils_ezcd_is_up(void);
bool utils_ezcd_wait_up(int s);
bool utils_service_binding_lan(char *name);
bool utils_service_binding_wan(char *name);
bool utils_wan_interface_is_up(void);

/* nvram function */
int nvram_match(const char *name, const char *value);

/* command entry point */
int preinit_main(int argc, char **argv);
int ezcd_main(int argc, char **argv);
int ezcm_main(int argc, char **argv);
int nvram_main(int argc, char **argv);
int rc_main(int argc, char **argv);
int ubootenv_main(int argc, char **argv);
int udhcpc_script_main(int argc, char **argv);
int upfw_main(int argc, char **argv);

#endif
