/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-nvram_defaults.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-16   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_NVRAM_DEFAULTS_H_
#define _EZCFG_NVRAM_DEFAULTS_H_

#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ezcfg-ezbox_distro.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ezcfg nvram rc control names */
#define EZCFG_NVRAM_RC_SYSLOG_ENABLE_NAME            "rc_syslog_enable"
#define EZCFG_NVRAM_RC_TELNETD_ENABLE_NAME           "rc_telnetd_enable"

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
