#ifndef _EZCFG_API_H_
#define _EZCFG_API_H_

#include "ezcfg-api-errno.h"

/* NVRAM interface */
int ezcfg_api_nvram_get(const char *name, char *value, size_t len);
int ezcfg_api_nvram_set(const char *name, const char *value);
int ezcfg_api_nvram_unset(const char *name);
int ezcfg_api_nvram_list(char *buf, size_t len);
int ezcfg_api_nvram_commit(void);
void ezcfg_api_nvram_set_debug(bool enable_debug);

#endif
