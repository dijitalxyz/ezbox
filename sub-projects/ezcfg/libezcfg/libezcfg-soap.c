/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg-soap.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-08-03   0.1       Write it from scratch
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

struct ezcfg_soap {
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;

	/* SOAP info */
	unsigned short version_major;
	unsigned short version_minor;

	/* SOAP Envelope */
	int env_index; /* SOAP Envelope element index in xml-root */
	char *env_ns; /* SOAP envelope namespace */
	char *env_enc; /* SOAP envelope encodingstyle */

	/* SOAP Body */
	int body_index; /* SOAP Body element index in xml->root */
};

/**
 * Public functions
 **/
void ezcfg_soap_delete(struct ezcfg_soap *soap)
{
	struct ezcfg *ezcfg;

	assert(soap != NULL);

	ezcfg = soap->ezcfg;

	if (soap->xml != NULL)
		ezcfg_xml_delete(soap->xml);

	free(soap);
}

/**
 * ezcfg_soap_new:
 * Create ezcfg soap info builder data structure
 * Returns: a new ezcfg soap info builder data structure
 **/
struct ezcfg_soap *ezcfg_soap_new(struct ezcfg *ezcfg)
{
	struct ezcfg_soap *soap;

	assert(ezcfg != NULL);

	/* initialize soap info builder data structure */
	soap = calloc(1, sizeof(struct ezcfg_soap));
	if (soap == NULL) {
		err(ezcfg, "initialize soap builder error.\n");
		return NULL;
	}

	memset(soap, 0, sizeof(struct ezcfg_soap));

	soap->xml = ezcfg_xml_new(ezcfg);
	if (soap->xml == NULL) {
		ezcfg_soap_delete(soap);
		return NULL;
	}

	soap->env_index = -1;
	soap->body_index = -1;
	soap->ezcfg = ezcfg;

	return soap;
}


unsigned short ezcfg_soap_get_version_major(struct ezcfg_soap *soap)
{
	struct ezcfg *ezcfg;

	assert(soap != NULL);

	ezcfg = soap->ezcfg;

	return soap->version_major;
}

unsigned short ezcfg_soap_get_version_minor(struct ezcfg_soap *soap)
{
	struct ezcfg *ezcfg;

	assert(soap != NULL);

	ezcfg = soap->ezcfg;

	return soap->version_minor;
}

bool ezcfg_soap_set_version_major(struct ezcfg_soap *soap, unsigned short major)
{
	struct ezcfg *ezcfg;

	assert(soap != NULL);

	ezcfg = soap->ezcfg;

	soap->version_major = major;

	return true;
}

bool ezcfg_soap_set_version_minor(struct ezcfg_soap *soap, unsigned short minor)
{
	struct ezcfg *ezcfg;

	assert(soap != NULL);

	ezcfg = soap->ezcfg;

	soap->version_minor = minor;

	return true;
}

int ezcfg_soap_set_envelope(struct ezcfg_soap *soap, const char *name)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;
	struct ezcfg_xml_element *elem;

	assert(soap != NULL);
	assert(soap->xml != NULL);
	assert(name != NULL);

	ezcfg = soap->ezcfg;
	xml = soap->xml;

	elem = ezcfg_xml_element_new(xml, name, NULL);

	if (elem == NULL) {
		err(ezcfg, "Cannot initialize soap envelope\n");
		return -1;
	}

	soap->env_index = ezcfg_xml_add_element(xml, NULL, NULL, elem);
	return soap->env_index;
}

bool ezcfg_soap_add_envelope_attribute(struct ezcfg_soap *soap, const char *name, const char *value, int pos) {
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;
	struct ezcfg_xml_element *elem;

	assert(soap != NULL);
	assert(name != NULL);
	assert(value != NULL);

	ezcfg = soap->ezcfg;
	xml = soap->xml;

	if (soap->env_index < 0) {
		err(ezcfg, "no soap envelope element!\n");
		return false;
	}

	elem = ezcfg_xml_get_element_by_index(xml, soap->env_index);
	if (elem == NULL) {
		err(ezcfg, "soap envelope element not set correct!\n");
		return false;
	}

	return ezcfg_xml_element_add_attribute(xml, elem, name, value, pos);
}

int ezcfg_soap_write(struct ezcfg_soap *soap, char *buf, int len)
{
	struct ezcfg *ezcfg;

	assert(soap != NULL);
	assert(buf != NULL);
	assert(len > 0);

	ezcfg = soap->ezcfg;

        return ezcfg_xml_write(soap->xml, buf, len);
}
