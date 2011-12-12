/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : upnp/upnp_ssdp.c
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

#if 1
#define DBG(format, args...) do { \
	char path[256]; \
	FILE *fp; \
	snprintf(path, 256, "/tmp/%d-debug.txt", getpid()); \
	fp = fopen(path, "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

struct ezcfg_upnp_ssdp {
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_upnp *upnp;
};

/**
 * private functions
 **/

/**
 * Public functions
 **/
void ezcfg_upnp_ssdp_delete(struct ezcfg_upnp_ssdp *ssdp)
{
	struct ezcfg *ezcfg;

	ASSERT(ssdp != NULL);

	ezcfg = ssdp->ezcfg;

	if (ssdp->http != NULL) {
		ezcfg_http_delete(ssdp->http);
	}

	free(ssdp);
}

/**
 * ezcfg_upnp_ssdp_new:
 * Create ezcfg ssdp protocol data structure
 * Returns: a new ezcfg ssdp protocol data structure
 **/
struct ezcfg_upnp_ssdp *ezcfg_upnp_ssdp_new(struct ezcfg *ezcfg)
{
	struct ezcfg_upnp_ssdp *ssdp;

	ASSERT(ezcfg != NULL);

	/* initialize ssdp protocol data structure */
	ssdp = calloc(1, sizeof(struct ezcfg_upnp_ssdp));
	if (ssdp == NULL) {
		return NULL;
	}

	memset(ssdp, 0, sizeof(struct ezcfg_upnp_ssdp));
	ssdp->ezcfg = ezcfg;

	ssdp->http = ezcfg_http_new(ezcfg);
	if (ssdp->http == NULL) {
		goto fail_exit;
	}
	//ssdp->upnp = upnp;

	return ssdp;

fail_exit:
	ezcfg_upnp_ssdp_delete(ssdp);
	return NULL;
}

/**
 * ezcfg_upnp_ssdp_set_upnp:
 **/
bool ezcfg_upnp_ssdp_set_upnp(struct ezcfg_upnp_ssdp *ssdp, struct ezcfg_upnp *upnp)
{
	ASSERT(ssdp != NULL);

	ssdp->upnp = upnp;

	return true;
}
