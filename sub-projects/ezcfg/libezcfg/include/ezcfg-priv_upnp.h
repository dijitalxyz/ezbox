/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_upnp.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-09   0.1       Split it from ezcfg-private.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_UPNP_H_
#define _EZCFG_PRIV_UPNP_H_

#include "ezcfg-types.h"
#include <net/if.h>

/* upnp/upnp.c */
void ezcfg_upnp_delete(struct ezcfg_upnp *upnp);
struct ezcfg_upnp *ezcfg_upnp_new(struct ezcfg *ezcfg);
bool ezcfg_upnp_set_role(struct ezcfg_upnp *upnp, int role);
bool ezcfg_upnp_set_type(struct ezcfg_upnp *upnp, int type);
bool ezcfg_upnp_if_list_insert(struct ezcfg_upnp *upnp, char ifname[IFNAMSIZ]);
bool ezcfg_upnp_parse_description(struct ezcfg_upnp *upnp, const char *path);

#endif /* _EZCFG_PRIV_UPNP_H_ */
