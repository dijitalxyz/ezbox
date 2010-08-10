/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg-xml.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-08-01   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <assert.h>
#include <pthread.h>

#include "libezcfg.h"
#include "libezcfg-private.h"

struct elem_attribute {
	char *name;
	char *value;
	struct elem_attribute *next;
};

struct ezcfg_xml_element {
	char *name;
	struct elem_attribute *attr_head;
	struct elem_attribute *attr_tail;
	char *content;
	int etag_index; /* etag index in root stack */
};

struct ezcfg_xml {
	struct ezcfg *ezcfg;
	/* prolog part of the xml document */
	unsigned short version_major;
	unsigned short version_minor;
	unsigned short encoding;
	/* element part of the xml document */
	int max_elements;
	int num_elements;
	struct ezcfg_xml_element **root; /* stack for representing xml doc tree */
};

const char *character_encoding_name[] = {
	/* bad encoding string */
	NULL,
	"UTF-8",
};

static void delete_elements(struct ezcfg_xml *xml)
{
	int i;
	struct ezcfg_xml_element *p;
	for (i=0; i < xml->num_elements; i++) {
		p = xml->root[i];
		if (p == NULL)
			continue;

		/* clean etag pointer */
		if (p->etag_index > 0) {
			xml->root[p->etag_index] = NULL;
		}

		ezcfg_xml_element_delete(p);
	}
}

void ezcfg_xml_delete(struct ezcfg_xml *xml)
{
	struct ezcfg *ezcfg;

	assert(xml != NULL);

	ezcfg = xml->ezcfg;

	if (xml->root != NULL)
		delete_elements(xml);

	free(xml);
}

/**
 * ezcfg_xml_new:
 *
 * Create ezcfg xml parser data structure
 *
 * Returns: a new ezcfg xml parser data structure
 **/
struct ezcfg_xml *ezcfg_xml_new(struct ezcfg *ezcfg)
{
	struct ezcfg_xml *xml;

	assert(ezcfg != NULL);

	/* initialize xml parser data structure */
	xml = calloc(1, sizeof(struct ezcfg_xml));
	if (xml == NULL) {
		err(ezcfg, "initialize xml parser error.\n");
		return NULL;
	}

	memset(xml, 0, sizeof(struct ezcfg_xml));

	xml->max_elements = EZCFG_XML_MAX_ELEMENTS * 2; /* must be times of 2 */
	xml->root=calloc(xml->max_elements, sizeof(struct ezcfg_xml_element *));
	if (xml->root == NULL) {
		err(ezcfg, "initialize xml element stack error.\n");
		free(xml);
		return NULL;
	}

	memset(xml->root, 0, sizeof(struct ezcfg_xml_element *) * xml->max_elements);

	xml->ezcfg = ezcfg;
	return xml;
}

void ezcfg_xml_element_delete(struct ezcfg_xml_element *elem)
{
	struct elem_attribute *a;
	assert(elem != NULL);
	if (elem->name)
		free(elem->name);
	while (elem->attr_head != NULL) {
		a = elem->attr_head;
		elem->attr_head = a->next;
		if (a->name != NULL)
			free(a->name);
		if (a->value != NULL)
			free(a->value);
	}
	if (elem->content)
		free(elem->content);
	free(elem);
}

struct ezcfg_xml_element *ezcfg_xml_element_new(
	struct ezcfg_xml *xml,
	const char *name, const char *content) {

	struct ezcfg *ezcfg;
	struct ezcfg_xml_element *elem;

	assert(xml != NULL);
	assert(name != NULL);

	ezcfg = xml->ezcfg;

	elem = calloc(1, sizeof(struct ezcfg_xml_element));
	if (elem == NULL) {
		err(ezcfg, "calloc xml element error.\n");
		return NULL;
	}

	memset(elem, 0, sizeof(struct ezcfg_xml_element));

	elem->name = strdup(name);
	if (elem->name == NULL) {
		err(ezcfg, "initialize xml element name error.\n");
		ezcfg_xml_element_delete(elem);
		return NULL;
	}
	if (content != NULL) {
		elem->content = strdup(content);
		if (elem->content == NULL) {
			err(ezcfg, "initialize xml element content error.\n");
			ezcfg_xml_element_delete(elem);
			return NULL;
		}
	}
	return elem;
}

bool ezcfg_xml_element_add_attribute(
	struct ezcfg_xml *xml,
	struct ezcfg_xml_element *elem,
	const char *name, const char *value, int pos) {

	struct ezcfg *ezcfg;
	struct elem_attribute *a;

	assert(xml != NULL);
	assert(elem != NULL);
	assert(name != NULL);
	assert(value != NULL);

	ezcfg = xml->ezcfg;

	a = calloc(1, sizeof(struct elem_attribute));
	if (a == NULL) {
		err(ezcfg, "no memory for element attribute\n");
		return false;
	}

	memset(a, 0, sizeof(struct elem_attribute));

	a->name = strdup(name);
	if (a->name == NULL) {
		err(ezcfg, "no memory for element attribute name\n");
		free(a);
		return false;
	}

	a->value = strdup(value);
	if (a->value == NULL) {
		err(ezcfg, "no memory for element attribute value\n");
		free(a->name);
		free(a);
		return false;
	}

	if (pos == EZCFG_XML_ELEMENT_ATTRIBUTE_HEAD) {
		a->next = elem->attr_head;
		elem->attr_head = a;
		if (elem->attr_tail == NULL) {
			elem->attr_tail = a;
		}
	} else if (pos == EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL) {
		if (elem->attr_tail == NULL) {
			elem->attr_head = a;
			elem->attr_tail = a;
		} else {
			a->next = elem->attr_tail->next;
			elem->attr_tail->next = a;
			elem->attr_tail = a;
		}
	} else {
		err(ezcfg, "not support element attribute position\n");
		free(a->name);
		free(a->value);
		free(a);
		return false;
	}
	return true;
}

int ezcfg_xml_add_element(
	struct ezcfg_xml *xml,
	struct ezcfg_xml_element *parent,
	struct ezcfg_xml_element *sibling,
	struct ezcfg_xml_element *elem) {

	struct ezcfg *ezcfg;
	struct ezcfg_xml_element **root;
	int i;

	assert(xml != NULL);
	assert(xml->num_elements+2 <= xml->max_elements);

	ezcfg = xml->ezcfg;
	root = xml->root;
	i = 0;

	if (parent == NULL) {
		/* root element */
		if(xml->num_elements != 0) {
			err(ezcfg, "xml root element must be only one, xml->num_elements=[%d]\n", xml->num_elements);
			return -1;
		}
		i = 0;
		root[i] = elem;
		xml->num_elements++;
		elem->etag_index = i+1;
		root[i+1] = elem;
		xml->num_elements++;
	} else if (sibling == NULL) {
		/* parent element's first child */
		for (i = xml->num_elements-1; i > 0; i--) {
			/* move 2 steps */
			root[i+2] = root[i];

			/* change etag_index for the start element */
			if (root[i]->etag_index == i) {
				root[i]->etag_index = i+2;
			}
			/* find parent element end tag */
			if (root[i] == parent) {
				break;
			}
		}
		if(i == 0) {
			err(ezcfg, "parent element not found\n");
			return -1;
		}
		root[i] = elem;
		xml->num_elements++;
		elem->etag_index = i+1;
		root[i+1] = elem;
		xml->num_elements++;
	} else {
		/* sibling element's next */
		for (i = xml->num_elements-1; i > 0; i--) {
			/* find sibling element end tag */
			if (root[i] == sibling) {
				break;
			}
			/* move 2 steps */
			root[i+2] = root[i];

			/* change etag_index for the start element */
			if (root[i]->etag_index == i) {
				root[i]->etag_index = i+2;
			}
		}
		if(i == 0) {
			err(ezcfg, "sibling element not found\n");
			return -1;
		}
		i++; /* move to next element slot */
		root[i] = elem;
		xml->num_elements++;
		elem->etag_index = i+1;
		root[i+1] = elem;
		xml->num_elements++;
	}
	return i;
}

int ezcfg_xml_write(struct ezcfg_xml *xml, char *buf, int len)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml_element **root;
	struct elem_attribute *a;
	int i;

	assert(xml != NULL);

	ezcfg = xml->ezcfg;
	root = xml->root;
	buf[0] = '\0';

	for (i = 0; i < xml->num_elements; i++) {
		if (root[i]->etag_index != i) {
			/* start tag */
			snprintf(buf+strlen(buf), len-strlen(buf), "<%s", root[i]->name);
			a = root[i]->attr_head;
			while(a != NULL) {
				snprintf(buf+strlen(buf), len-strlen(buf), " %s", a->name);
				snprintf(buf+strlen(buf), len-strlen(buf), "=\"%s\"", a->value);
				a = a->next;
			}
			snprintf(buf+strlen(buf), len-strlen(buf), ">");
			if (root[i]->content != NULL) {
				snprintf(buf+strlen(buf), len-strlen(buf), "%s", root[i]->content);
			} else {
				snprintf(buf+strlen(buf), len-strlen(buf), "\r\n");
			}
		}
		else {
			/* end tag */
			snprintf(buf+strlen(buf), len-strlen(buf), "</%s>\r\n", root[i]->name);
		}
	}
	return strlen(buf);
}

int ezcfg_xml_get_element_index(struct ezcfg_xml *xml, const char *name)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml_element *elem;
	int i;

	assert(xml != NULL);
	assert(xml->root != NULL);

	ezcfg = xml->ezcfg;
	for (i = 0; i < xml->num_elements; i++) {
		elem = xml->root[i];
		if (strcmp(elem->name, name) == 0)
			return i;
	}
	return -1;
}

struct ezcfg_xml_element *ezcfg_xml_get_element_by_index(struct ezcfg_xml *xml, int i)
{
	struct ezcfg *ezcfg;

	assert(xml != NULL);
	assert(xml->root != NULL);

	ezcfg = xml->ezcfg;

	return xml->root[i];
}
