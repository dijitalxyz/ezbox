/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-sys.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-17   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_SYS_H_
#define _EZCFG_SYS_H_

/* ezcfg nvram name prefix */
#define EZCFG_SYS_NVRAM_PREFIX        "sys_"

/* ezcfg system names */
#define EZCFG_SYS_DEVICE_NAME             "device_name"
#define EZCFG_SYS_SERIAL_NUMBER           "serial_number"
#define EZCFG_SYS_HARDWARE_VERSION        "hardware_version"
#define EZCFG_SYS_SOFTWARE_VERSION        "software_version"

#define EZCFG_SYS_LANGUAGE                "language"
#define EZCFG_SYS_TZ_AREA                 "tz_area"
#define EZCFG_SYS_TZ_LOCATION             "tz_location"

#define EZCFG_SYS_RESTORE_DEFAULTS        "restore_defaults"
#define EZCFG_SYS_UPGRADE_IMAGE           "upgrade_image"
#define EZCFG_SYS_LAN_NIC                 "lan_nic"
#define EZCFG_SYS_WAN_NIC                 "wan_nic"

#endif
