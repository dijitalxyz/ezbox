/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-upnp_gena.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-29   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_UPNP_GENA_H_
#define _EZCFG_UPNP_GENA_H_

#include "ezcfg.h"
#include "ezcfg-http.h"
#include "ezcfg-upnp.h"

struct ezcfg_upnp_gena {
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_upnp *upnp;
};

#endif /* _EZCFG_UPNP_GENA_H_ */
