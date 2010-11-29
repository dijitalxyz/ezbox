/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-soap_http.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-29   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_SOAP_HTTP_H_
#define _EZCFG_SOAP_HTTP_H_

#include "ezcfg.h"

struct ezcfg_soap_http {
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_soap *soap;
};

/* soap/soap_http_nvram.c */
int ezcfg_soap_http_handle_nvram_request(struct ezcfg_soap_http *sh, struct ezcfg_nvram *nvram);

#endif
