/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-types.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2010-12-02   0.1       Splite it from ezcfg.h
 * ============================================================================
 */

#ifndef _EZCFG_TYPES_H_
#define _EZCFG_TYPES_H_

#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * common/ezcfg.c
 * ezbox config context
 */
struct ezcfg;

/* thread/master.c */
struct ezcfg_master;

/* uevent/uevent.c */
struct ezcfg_uevent;

/* igrs/igrs.c */
struct ezcfg_igrs;

/* upnp/ssdp.c */
struct ezcfg_upnp_ssdp;

/* upnp/upnp.c */
struct ezcfg_upnp;

/* uuid/uuid.c */
struct ezcfg_uuid;

/* ctrl/ctrl.c - daemon runtime setup */
struct ezcfg_ctrl;

#endif /* _EZCFG_TYPES_H_ */
