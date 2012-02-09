/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_ssl.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2012-02-08   0.1       Split it from ezcfg-private.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_SSL_H_
#define _EZCFG_PRIV_SSL_H_

#include "ezcfg-types.h"

/* ssl/ssl.c */
struct ezcfg_ssl *ezcfg_ssl_new(struct ezcfg *ezcfg, const int role, const int method);
void ezcfg_ssl_delete(struct ezcfg_ssl *sslp);
struct ezcfg_socket *ezcfg_ssl_get_socket(struct ezcfg_ssl *sslp);
bool ezcfg_ssl_set_socket(struct ezcfg_ssl *sslp, struct ezcfg_socket *sp);
bool ezcfg_ssl_is_valid(struct ezcfg_ssl *sslp);
bool ezcfg_ssl_list_in(struct ezcfg_ssl **list, struct ezcfg_ssl *sslp);
bool ezcfg_ssl_list_insert(struct ezcfg_ssl **list, struct ezcfg_ssl *sslp);
void ezcfg_ssl_list_delete(struct ezcfg_ssl **list);

#endif /* _EZCFG_PRIV_SSL_H_ */
