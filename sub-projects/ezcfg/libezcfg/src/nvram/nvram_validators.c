/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : nvram/nvram_validators.c
 *
 * Description  : implement validating Non-Volatile RAM settings
 *
 * Copyright (C) 2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-25   0.1       Write it from scratch
 * ============================================================================
 */

#include <string.h>
#include "ezcfg-private.h"
#include "ezcfg.h"

static bool sys_language_validator(const char *value)
{
	return true;
}

static bool ui_tz_area_validator(const char *value)
{
	return true;
}

static bool ui_tz_location_validator(const char *value)
{
	return true;
}

ezcfg_nv_validator_t default_nvram_validators[] = {
	/* System configuration */
	{ NVRAM_SERVICE_OPTION(SYS, LANGUAGE), sys_language_validator },

	/* UI configuration */
	{ NVRAM_SERVICE_OPTION(UI, TZ_AREA), ui_tz_area_validator },
	{ NVRAM_SERVICE_OPTION(UI, TZ_LOCATION), ui_tz_location_validator },
};


bool ezcfg_nvram_validate_value(struct ezcfg *ezcfg, char *name, char *value)
{
	int i;
	bool ret = false;
	ezcfg_nv_validator_t v;

	ASSERT(ezcfg != NULL);
	ASSERT(name != NULL);
	ASSERT(value != NULL);

	for(i = 0; i < ARRAY_SIZE(default_nvram_validators); i++) {
		v = default_nvram_validators[i];
		if (strcmp(v.name, name) == 0) {
			ret = v.handler(value);
			break;
		}
	}

	return ret;
}

/* Public functions */
int ezcfg_nvram_get_num_default_nvram_validators(void)
{
	return ARRAY_SIZE(default_nvram_validators);
}

