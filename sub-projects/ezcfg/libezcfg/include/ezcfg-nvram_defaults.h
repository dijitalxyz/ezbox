/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-nvram_defaults.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-16   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_NVRAM_DEFAULTS_H_
#define _EZCFG_NVRAM_DEFAULTS_H_

#include "ezcfg-sys.h"
#include "ezcfg-ezcfg.h"
#include "ezcfg-loopback.h"
#include "ezcfg-lan.h"
#include "ezcfg-wan.h"
#include "ezcfg-wlan.h"
#include "ezcfg-rc.h"
#include "ezcfg-ui.h"
#include "ezcfg-dnsmasq.h"
#include "ezcfg-iptables.h"
#include "ezcfg-wpa_supplicant.h"
#include "ezcfg-dhid.h"
#include "ezcfg-dmcrypt.h"
#include "ezcfg-nano_x.h"
#include "ezcfg-kdrive.h"
#include "ezcfg-dillo.h"
#include "ezcfg-emc2.h"
#include "ezcfg-upnp.h"
#include "ezcfg-ssl.h"
#include "ezcfg-openssl.h"

#define GLUE2(a, b)      	a ## b
#define GLUE3(a, b, c)   	a ## b ## c
#define GLUE4(a, b, c, d)	a ## b ## c ## d

#define NVRAM_PREFIX(service)      	GLUE3(EZCFG_, service, _NVRAM_PREFIX)
#define SERVICE_OPTION(service, name)	GLUE4(EZCFG_, service, _, name)

#define NVRAM_SERVICE_OPTION(service, name) \
	NVRAM_PREFIX(service) SERVICE_OPTION(service, name)

#endif
