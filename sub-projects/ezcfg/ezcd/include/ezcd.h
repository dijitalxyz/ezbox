#ifndef _EZCD_H_
#define _EZCD_H_

#include "ezcfg.h"
#include "ezcfg-private.h"

#include "ezcfg-api.h"

#include "utils.h"

/* utils function declaration */
char *utils_get_kernel_version(void);
rc_func_t *utils_find_rc_func(char *name);
proc_stat_t *utils_find_pid_by_name(char *pidName);
uint32_t utils_crc32(unsigned char *data, int len);

/* command entry point */
int preinit_main(int argc, char **argv);
int ezcd_main(int argc, char **argv);
int ezcm_main(int argc, char **argv);
int nvram_main(int argc, char **argv);
int rc_main(int argc, char **argv);
int ubootenv_main(int argc, char **argv);

#endif
