/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_ezctp.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2012-03-13   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_EZCTP_H_
#define _EZCFG_PRIV_EZCTP_H_

#include "ezcfg-types.h"

/* ezctp/ezctp.c */
bool ezcfg_ezctp_delete(struct ezcfg_ezctp *ezctp);
struct ezcfg_ezctp *ezcfg_ezctp_new(struct ezcfg *ezcfg);

#endif /* _EZCFG_PRIV_EZCTP_H_ */
