/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg-nvram_defaults.c
 *
 * Description  : implement Non-Volatile RAM default settings
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-08-20   0.1       Write it from scratch
 * ============================================================================
 */

#include "libezcfg-private.h"
#include "libezcfg.h"

struct ezcfg_nvram_pair default_nvram_settings[] = {
	{ "mytest", "ok" },
	{ "mytest2", "ok2" },
};

/* Public functions */
int ezcfg_nvram_get_num_default_nvram_settings(void)
{
	return ARRAY_SIZE(default_nvram_settings);
}
