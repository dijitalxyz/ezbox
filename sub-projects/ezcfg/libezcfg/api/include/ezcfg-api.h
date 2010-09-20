#ifndef _EZCFG_API_H_
#define _EZCFG_API_H_

#include "ezcfg-api-errno.h"

/* NVRAM interface */
int ezcfg_api_nvram_get(const char *name, char *value, size_t len);

#endif
