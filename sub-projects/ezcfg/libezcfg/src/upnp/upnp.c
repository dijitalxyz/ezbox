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

typedef struct device_icon_s {
	/* icon elements */
	char *mimetype;
	int width;
	int height;
	int depth;
	char *url;
	/* link */
	struct device_icon_s *next;
} device_icon_t;

typedef struct upnp_service_s {
	/* service elements */
	char *serviceType;
	char *serviceId;
	char *SCPDURL;
	char *controlURL;
	char *eventSubURL;
	/* link */
	struct upnp_service_s *next;
} upnp_service_t;

typedef struct upnp_device_s {
	/* device elements */
	char *deviceType;
	char *friendlyName;
	char *manufacturer;
	char *manufacturerURL;
	char *modelDescription;
	char *modelName;
	char *modelNumber;
	char *modelURL;
	char *serialNumber;
	char *UDN;
	char *UPC;
	device_icon_t *iconList;
	upnp_service_t *serviceList;
	struct upnp_device_s *deviceList; /* first child node */
	char *presentationURL;
	/* link */
	struct upnp_device_s *next; /* sibling node */
	/* implement info */
	void *data;
} upnp_device_t;

typedef struct upnp_control_point_s {
	/* monitoring root devices */
	int root_dev_max_num;
	upnp_device_t *root_devs;
} upnp_control_point_t;

struct ezcfg_upnp {
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_xml *xml;

	/* UPnP info */
	unsigned short version_major; /* UPnP major version, must be 1 */
	unsigned short version_minor; /* UPnP minor version, should be 0 or 1 */

	char *URLBase; /* UPnP UDA 1.0 defined, UPnP UDA 1.1 deprecated */

	upnp_device_t *root_devs;
	upnp_control_point_t *cps;
};

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

