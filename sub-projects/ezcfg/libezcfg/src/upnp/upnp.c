/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : upnp/upnp.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-09   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-upnp.h"

/**
 * private functions
 **/

/**
 * Public functions
 **/
void ezcfg_upnp_delete(struct ezcfg_upnp *upnp)
{
	struct ezcfg *ezcfg;

	ASSERT(upnp != NULL);

	ezcfg = upnp->ezcfg;

	free(upnp);
}

/**
 * ezcfg_upnp_new:
 * Create ezcfg upnp protocol data structure
 * Returns: a new ezcfg upnp protocol data structure
 **/
struct ezcfg_upnp *ezcfg_upnp_new(struct ezcfg *ezcfg)
{
	struct ezcfg_upnp *upnp;

	ASSERT(ezcfg != NULL);

	/* initialize upnp protocol data structure */
	upnp = calloc(1, sizeof(struct ezcfg_upnp));
	if (upnp == NULL) {
		return NULL;
	}

	memset(upnp, 0, sizeof(struct ezcfg_upnp));
	upnp->ezcfg = ezcfg;

	return upnp;
}

