#ifndef _EZCFG_API_H_
#define _EZCFG_API_H_

#include "ezcfg-api-errno.h"

/* NVRAM interface */
int ezcfg_api_nvram_get(const char *name, char *value, size_t len);
int ezcfg_api_nvram_set(const char *name, const char *value);
int ezcfg_api_nvram_unset(const char *name);
int ezcfg_api_nvram_list(char *list, size_t len);
int ezcfg_api_nvram_commit(void);
void ezcfg_api_nvram_set_debug(bool enable_debug);

/* u-boot-env interface */
int ezcfg_api_ubootenv_get(char *name, char *value, size_t len);
int ezcfg_api_ubootenv_set(char *name, char *value);
int ezcfg_api_ubootenv_list(char *list, size_t len);
int ezcfg_api_ubootenv_check(void);
int ezcfg_api_ubootenv_size(void);

/* firmware interface */
int ezcfg_api_firmware_upgrade(char *name, char *device, char *pattern);

#endif
