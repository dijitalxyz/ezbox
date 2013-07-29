/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_agent.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2013-07-29   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_AGENT_H_
#define _EZCFG_PRIV_AGENT_H_

/* bitmap for agent state */
#define AGENT_STATE_RUNNING	0
#define AGENT_STATE_STOPPED	1

/* agent/agent_core.c */
struct ezcfg_agent *ezcfg_agent_start(struct ezcfg *ezcfg);

#endif /* _EZCFG_PRIV_AGENT_H_ */
