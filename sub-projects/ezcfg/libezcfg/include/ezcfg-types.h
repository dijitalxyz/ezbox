/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-types.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2013 by ezbox-project
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

/* agent/agent_core.c */
struct ezcfg_agent;

/* thread/master.c */
struct ezcfg_master;

/* thread/worker.c */
struct ezcfg_worker;

/* http/http.c */
struct ezcfg_http;

/* uevent/uevent.c */
struct ezcfg_uevent;

/* igrs/igrs.c */
struct ezcfg_igrs;
struct ezcfg_igrs_msg_op;

/* upnp/upnp.c */
struct ezcfg_upnp;

/* upnp/upnp_ssdp.c */
struct ezcfg_upnp_ssdp;

/* upnp/upnp_gena.c */
struct ezcfg_upnp_gena;

/* uuid/uuid.c */
struct ezcfg_uuid;

/* ctrl/ctrl.c - daemon runtime setup */
struct ezcfg_ctrl;

/* xml/xml.c - daemon runtime setup */
struct ezcfg_xml_element;
struct ezcfg_xml;

/* socket/socket.c */
struct ezcfg_socket;

/* http/http.c */
struct ezcfg_http;

/* nvram/nvram.c */
struct ezcfg_nvram;

/* ezctp/ezctp.c */
struct ezcfg_ezctp;

/* ssl/ssl.c */
struct ezcfg_ssl;

/* websocket/websocket.c */
struct ezcfg_websocket;

/* websocket/http_websocket.c */
struct ezcfg_http_websocket;

#endif /* _EZCFG_TYPES_H_ */
