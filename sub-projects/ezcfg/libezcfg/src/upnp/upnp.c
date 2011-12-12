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

typedef struct upnp_if_s {
	/* monitoring root devices */
	char ifname[IFNAMSIZ];
	struct upnp_if_s *next;
} upnp_if_t;

struct ezcfg_upnp {
	struct ezcfg *ezcfg;

	int role; /* controlled device/control point */
	int type; /* standardizeddcps */
	upnp_if_t *ifs;

	/* UPnP info */
	int version_major; /* UPnP major version, must be 1 */
	int version_minor; /* UPnP minor version, should be 0 or 1 */

	char *URLBase; /* UPnP UDA 1.0 defined, UPnP UDA 1.1 deprecated */

	union {
		upnp_device_t dev;
		upnp_control_point_t cp;
	} u;

	struct ezcfg_upnp *next; /* link pointer */
};

/**
 * private functions
 **/
static void upnp_device_icon_delete(device_icon_t *ip)
{
	if (ip->mimetype != NULL)
		free(ip->mimetype);

	if (ip->url != NULL)
		free(ip->url);

	free(ip);
}

static void upnp_device_delete_icon_list(device_icon_t **list)
{
	device_icon_t *ip;
	ip = *list;
	while (ip != NULL) {
		*list = ip->next;
		upnp_device_icon_delete(ip);
		ip = *list;
	}
}

static void upnp_service_delete(upnp_service_t *sp)
{
	if (sp->serviceType != NULL)
		free(sp->serviceType);

	if (sp->serviceId != NULL)
		free(sp->serviceId);

	if (sp->SCPDURL != NULL)
		free(sp->SCPDURL);

	if (sp->controlURL != NULL)
		free(sp->controlURL);

	if (sp->eventSubURL != NULL)
		free(sp->eventSubURL);

	free(sp);
}

static void upnp_device_delete_service_list(upnp_service_t **list)
{
	upnp_service_t *sp;
	sp = *list;
	while (sp != NULL) {
		*list = sp->next;
		upnp_service_delete(sp);
		sp = *list;
	}
}

static void upnp_device_delete_list(upnp_device_t **list);

static void upnp_delete_device(upnp_device_t dev)
{
	if (dev.deviceType != NULL)
		free(dev.deviceType);

	if (dev.friendlyName != NULL)
		free(dev.friendlyName);

	if (dev.manufacturer != NULL)
		free(dev.manufacturer);

	if (dev.manufacturerURL != NULL)
		free(dev.manufacturerURL);

	if (dev.modelDescription != NULL)
		free(dev.modelDescription);

	if (dev.modelName != NULL)
		free(dev.modelName);

	if (dev.modelNumber != NULL)
		free(dev.modelNumber);

	if (dev.modelURL != NULL)
		free(dev.modelURL);

	if (dev.serialNumber != NULL)
		free(dev.serialNumber);

	if (dev.UDN != NULL)
		free(dev.UDN);

	if (dev.UPC != NULL)
		free(dev.UPC);

	if (dev.iconList != NULL)
		upnp_device_delete_icon_list(&(dev.iconList));

	if (dev.serviceList != NULL)
		upnp_device_delete_service_list(&(dev.serviceList));

	if (dev.deviceList != NULL)
		upnp_device_delete_list(&(dev.deviceList));

	if (dev.presentationURL != NULL)
		free(dev.presentationURL);
}

static void upnp_device_delete_list(upnp_device_t **list)
{
	upnp_device_t *dp;
	dp = *list;
	while (dp != NULL) {
		*list = dp->next;
		upnp_delete_device(*dp);
		free(dp);
		dp = *list;
	}
}

static void upnp_delete_control_point(upnp_control_point_t cp)
{
	return;
}

static void upnp_delete_if_list(upnp_if_t **list)
{
	upnp_if_t *ip;
	ip = *list;
	while (ip != NULL) {
		*list = ip->next;
		free(ip);
		ip = *list;
	}
}

static bool upnp_fill_info(struct ezcfg_upnp *upnp, struct ezcfg_xml *xml)
{
	int pi, si, ci;
	char *p;

	/* get <specVersion> */
	pi = 0; si = -1;
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_SPEC_VERSION_ELEMENT_NAME);
	if (ci < pi) {
		return false;
	}

	/* get <major> */
	pi = ci; si = -1;
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_MAJOR_ELEMENT_NAME);
	if (ci < pi) {
		return false;
	}
	p = ezcfg_xml_get_element_content_by_index(xml, ci);
	if (p == NULL) {
		return false;
	}
	upnp->version_major = atoi(p);
	if (upnp->version_major != 1) {
		return false;
	}

	/* get <minor> */
	si = ci;
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_MINOR_ELEMENT_NAME);
	if (ci < pi) {
		return false;
	}
	p = ezcfg_xml_get_element_content_by_index(xml, ci);
	if (p == NULL) {
		return false;
	}
	upnp->version_minor = atoi(p);
	if ((upnp->version_minor != 0) &&
	    (upnp->version_minor != 1)) {
		return false;
	}

	/* get <URLBase> FIXME: UDA-1.0 only!!!*/
	if ((upnp->version_major == 1) &&
	    (upnp->version_minor == 0)) {
		pi = 0; si = -1;
		ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_URL_BASE_ELEMENT_NAME);
		if (ci > pi) {
			p = ezcfg_xml_get_element_content_by_index(xml, ci);
			if (p != NULL) {
				upnp->URLBase = strdup(p);
				if (upnp->URLBase == NULL) {
					return false;
				}
			}
		}
	}

	/* get <device> */
	pi = 0; si = -1;
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_DEVICE_ELEMENT_NAME);
	if (ci < pi) {
		return false;
	}
	pi = ci; si = -1;
	/* get <deviceType>, REQUIRED */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_DEVICE_TYPE_ELEMENT_NAME);
	if (ci < pi) {
		return false;
	}
	p = ezcfg_xml_get_element_content_by_index(xml, ci);
	if (p == NULL) {
		return false;
	}
	(upnp->u).dev.deviceType = strdup(p);
	if ((upnp->u).dev.deviceType == NULL) {
		return false;
	}

	/* get <friendlyName>, REQUIRED */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_FRIENDLY_NAME_ELEMENT_NAME);
	if (ci < pi) {
		return false;
	}
	p = ezcfg_xml_get_element_content_by_index(xml, ci);
	if (p == NULL) {
		return false;
	}
	(upnp->u).dev.friendlyName = strdup(p);
	if ((upnp->u).dev.friendlyName == NULL) {
		return false;
	}

	/* get <manufacturer>, REQUIRED */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_MANUFACTURER_ELEMENT_NAME);
	if (ci < pi) {
		return false;
	}
	p = ezcfg_xml_get_element_content_by_index(xml, ci);
	if (p == NULL) {
		return false;
	}
	(upnp->u).dev.manufacturer = strdup(p);
	if ((upnp->u).dev.manufacturer == NULL) {
		return false;
	}

	/* get <manufacturerURL>, OPTIONAL */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_MANUFACTURER_URL_ELEMENT_NAME);
	if (ci > pi) {
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p != NULL) {
			(upnp->u).dev.manufacturerURL = strdup(p);
			if ((upnp->u).dev.manufacturerURL == NULL) {
				return false;
			}
		}
	}

	/* get <modelDescription>, RECOMMENDED */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_MODEL_DESCRIPTION_ELEMENT_NAME);
	if (ci > pi) {
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p != NULL) {
			(upnp->u).dev.modelDescription = strdup(p);
			if ((upnp->u).dev.modelDescription == NULL) {
				return false;
			}
		}
	}

	/* get <modelName>, REQUIRED */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_MODEL_NAME_ELEMENT_NAME);
	if (ci < pi) {
		return false;
	}
	p = ezcfg_xml_get_element_content_by_index(xml, ci);
	if (p == NULL) {
		return false;
	}
	(upnp->u).dev.modelName = strdup(p);
	if ((upnp->u).dev.modelName == NULL) {
		return false;
	}

	/* get <modelNumber>, RECOMMENDED */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_MODEL_NUMBER_ELEMENT_NAME);
	if (ci > pi) {
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p != NULL) {
			(upnp->u).dev.modelNumber = strdup(p);
			if ((upnp->u).dev.modelNumber == NULL) {
				return false;
			}
		}
	}

	/* get <modelURL>, OPTIONAL */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_MODEL_URL_ELEMENT_NAME);
	if (ci > pi) {
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p != NULL) {
			(upnp->u).dev.modelURL = strdup(p);
			if ((upnp->u).dev.modelURL == NULL) {
				return false;
			}
		}
	}

	/* get <serialNumber>, RECOMMENDED */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_SERIAL_NUMBER_ELEMENT_NAME);
	if (ci > pi) {
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p != NULL) {
			(upnp->u).dev.serialNumber = strdup(p);
			if ((upnp->u).dev.serialNumber == NULL) {
				return false;
			}
		}
	}

	/* get <UDN>, REQUIRED */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_UDN_ELEMENT_NAME);
	if (ci < pi) {
		return false;
	}
	p = ezcfg_xml_get_element_content_by_index(xml, ci);
	if (p == NULL) {
		return false;
	}
	(upnp->u).dev.UDN = strdup(p);
	if ((upnp->u).dev.UDN == NULL) {
		return false;
	}

	/* get <UPC>, OPTIONAL */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_UPC_ELEMENT_NAME);
	if (ci > pi) {
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p != NULL) {
			(upnp->u).dev.UPC = strdup(p);
			if ((upnp->u).dev.UPC == NULL) {
				return false;
			}
		}
	}

#if 0
	if (dev.iconList != NULL)
		upnp_device_delete_icon_list(&(dev.iconList));

	if (dev.serviceList != NULL)
		upnp_device_delete_service_list(&(dev.serviceList));

	if (dev.deviceList != NULL)
		upnp_device_delete_list(&(dev.deviceList));
#endif

	/* get <presentationURL>, RECOMMENDED */
	ci = ezcfg_xml_get_element_index(xml, pi, si, EZCFG_UPNP_DESC_PRESENTATION_URL_ELEMENT_NAME);
	if (ci > pi) {
		p = ezcfg_xml_get_element_content_by_index(xml, ci);
		if (p != NULL) {
			(upnp->u).dev.presentationURL = strdup(p);
			if ((upnp->u).dev.presentationURL == NULL) {
				return false;
			}
		}
	}

	return true;
}

/**
 * Public functions
 **/
void ezcfg_upnp_delete(struct ezcfg_upnp *upnp)
{
	struct ezcfg *ezcfg;

	ASSERT(upnp != NULL);

	ezcfg = upnp->ezcfg;

	if (upnp->role == EZCFG_UPNP_ROLE_DEVICE) {
		upnp_delete_device(upnp->u.dev);
	}
	else if (upnp->role == EZCFG_UPNP_ROLE_CONTROL_POINT) {
		upnp_delete_control_point(upnp->u.cp);
	}

	upnp_delete_if_list(&(upnp->ifs));

	if (upnp->URLBase != NULL)
		free(upnp->URLBase);

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
	upnp->role = EZCFG_UPNP_ROLE_UNKNOWN;
	upnp->ifs = NULL;
	upnp->URLBase = NULL;
	upnp->next = NULL;

	return upnp;
}

bool ezcfg_upnp_set_role(struct ezcfg_upnp *upnp, int role)
{
	ASSERT(upnp != NULL);

	upnp->role = role;

	return true;
}

bool ezcfg_upnp_set_type(struct ezcfg_upnp *upnp, int type)
{
	ASSERT(upnp != NULL);

	upnp->type = type;

	return true;
}

bool ezcfg_upnp_if_list_insert(struct ezcfg_upnp *upnp, char ifname[IFNAMSIZ])
{
	upnp_if_t *ifp;

	ASSERT(upnp != NULL);

	/* initialize upnp_if_t data structure */
	ifp = calloc(1, sizeof(upnp_if_t));
	if (ifp == NULL)
		return false;

	memset(ifp, 0, sizeof(upnp_if_t));
	strncpy(ifp->ifname, ifname, IFNAMSIZ);
	ifp->next = upnp->ifs;
	upnp->ifs = ifp;
	return true;
}

bool ezcfg_upnp_list_insert(struct ezcfg_upnp **list, struct ezcfg_upnp *upnp)
{
	ASSERT(list != NULL);
	ASSERT(upnp != NULL);

	upnp->next = *list;
	*list = upnp;
	return true;
}

void ezcfg_upnp_list_delete(struct ezcfg_upnp **list)
{
	struct ezcfg_upnp *up;

	ASSERT(list != NULL);

	up = *list;
	while (up != NULL) {
		*list = up->next;
		ezcfg_upnp_delete(up);
		up = *list;
	}
}

bool ezcfg_upnp_parse_description(struct ezcfg_upnp *upnp, const char *path)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml = NULL;
	struct stat stat_buf;
	char *buf = NULL;
	int buf_len;
	FILE *fp = NULL;
	bool ret = false;

	ASSERT(upnp != NULL);
	ASSERT(path != NULL);

	if (stat(path, &stat_buf) < 0)
		return false;

	buf_len = stat_buf.st_size + 1;
	buf = calloc(buf_len, 1);
	if (buf == NULL)
		return false;

	memset(buf, 0, buf_len);

	fp = fopen(path, "r");
	if (fp == NULL) {
		goto func_out;
	}

	if (fread(buf, 1, stat_buf.st_size, fp) != stat_buf.st_size) {
		goto func_out;
	}
	buf[buf_len -1] = '\0';

	ezcfg = upnp->ezcfg;

	xml = ezcfg_xml_new(ezcfg);
	if (xml == NULL) {
		goto func_out;
	}
	
	if (ezcfg_xml_parse(xml, buf, buf_len) == false) {
		goto func_out;
	}

	/* fill UPnP info */
	if (upnp_fill_info(upnp, xml) == false) {
		goto func_out;
	}

	ret = true;

func_out:
	if (fp != NULL)
		fclose(fp);

	if (buf != NULL)
		free(buf);

	if (xml != NULL)
		ezcfg_xml_delete(xml);

	return ret;
}
