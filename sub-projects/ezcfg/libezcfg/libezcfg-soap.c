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
	int envelope_index; /* SOAP Envelope element index in xml-root */
	char *env_ns; /* SOAP envelope namespace */
	char *env_enc; /* SOAP envelope encodingstyle */

	/* SOAP header */
	int header_index; /* SOAP Header element index in xml-root */

	/* SOAP Body */
	int body_index; /* SOAP Body element index in xml->root */
};

/**
 * Public functions
 **/
void ezcfg_soap_delete(struct ezcfg_soap *soap)
{
	struct ezcfg *ezcfg;

	ASSERT(soap != NULL);

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

	ASSERT(ezcfg != NULL);

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

	soap->envelope_index = -1;
	soap->body_index = -1;
	soap->ezcfg = ezcfg;

	return soap;
}

unsigned short ezcfg_soap_get_version_major(struct ezcfg_soap *soap)
{
	struct ezcfg *ezcfg;

	ASSERT(soap != NULL);

	ezcfg = soap->ezcfg;

	return soap->version_major;
}

unsigned short ezcfg_soap_get_version_minor(struct ezcfg_soap *soap)
{
	struct ezcfg *ezcfg;

	ASSERT(soap != NULL);

	ezcfg = soap->ezcfg;

	return soap->version_minor;
}

bool ezcfg_soap_set_version_major(struct ezcfg_soap *soap, unsigned short major)
{
	struct ezcfg *ezcfg;

	ASSERT(soap != NULL);

	ezcfg = soap->ezcfg;

	soap->version_major = major;

	return true;
}

bool ezcfg_soap_set_version_minor(struct ezcfg_soap *soap, unsigned short minor)
{
	struct ezcfg *ezcfg;

	ASSERT(soap != NULL);

	ezcfg = soap->ezcfg;

	soap->version_minor = minor;

	return true;
}

int ezcfg_soap_set_envelope(struct ezcfg_soap *soap, const char *name)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;
	struct ezcfg_xml_element *elem;

	ASSERT(soap != NULL);
	ASSERT(soap->xml != NULL);
	ASSERT(name != NULL);

	ezcfg = soap->ezcfg;
	xml = soap->xml;

	elem = ezcfg_xml_element_new(xml, name, NULL);

	if (elem == NULL) {
		err(ezcfg, "Cannot initialize soap envelope\n");
		return -1;
	}

	soap->envelope_index = ezcfg_xml_add_element(xml, NULL, NULL, elem);
	return soap->envelope_index;
}

bool ezcfg_soap_add_envelope_attribute(struct ezcfg_soap *soap, const char *name, const char *value, int pos) {
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;
	struct ezcfg_xml_element *elem;

	ASSERT(soap != NULL);
	ASSERT(name != NULL);
	ASSERT(value != NULL);

	ezcfg = soap->ezcfg;
	xml = soap->xml;

	if (soap->envelope_index < 0) {
		err(ezcfg, "no soap envelope element!\n");
		return false;
	}

	elem = ezcfg_xml_get_element_by_index(xml, soap->envelope_index);
	if (elem == NULL) {
		err(ezcfg, "soap envelope element not set correct!\n");
		return false;
	}

	return ezcfg_xml_element_add_attribute(xml, elem, name, value, pos);
}

int ezcfg_soap_set_body(struct ezcfg_soap *soap, const char *name)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;
	struct ezcfg_xml_element *envelope, *elem;

	ASSERT(soap != NULL);
	ASSERT(soap->xml != NULL);
	ASSERT(name != NULL);

	ezcfg = soap->ezcfg;
	xml = soap->xml;

	elem = ezcfg_xml_element_new(xml, name, NULL);

	if (elem == NULL) {
		err(ezcfg, "Cannot initialize soap body\n");
		return -1;
	}

	if (soap->envelope_index < 0) {
		err(ezcfg, "no envelope for soap body\n");
		return -1;
	}

	envelope = ezcfg_xml_get_element_by_index(xml, soap->envelope_index);
	if (envelope == NULL) {
		err(ezcfg, "soap envelope is invalid\n");
		return -1;
	}

	soap->body_index = ezcfg_xml_add_element(xml, envelope, NULL, elem);
	return soap->body_index;
}

int ezcfg_soap_get_body_index(struct ezcfg_soap *soap)
{
	struct ezcfg *ezcfg;

	ASSERT(soap != NULL);

	ezcfg = soap->ezcfg;
	return soap->body_index;
}

int ezcfg_soap_add_body_child(struct ezcfg_soap *soap, const int pi, const int si, const char *name, const char *content)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;
	struct ezcfg_xml_element *parent, *sibling, *elem;

	ASSERT(soap != NULL);
	ASSERT(soap->xml != NULL);
	ASSERT(name != NULL);
	ASSERT(pi > 0);

	ezcfg = soap->ezcfg;
	xml = soap->xml;

	elem = ezcfg_xml_element_new(xml, name, content);

	if (elem == NULL) {
		err(ezcfg, "Cannot initialize soap body\n");
		return -1;
	}

	if (soap->body_index < 0) {
		err(ezcfg, "no body for soap child\n");
		return -1;
	}

	parent = ezcfg_xml_get_element_by_index(xml, pi);
	if (parent == NULL) {
		err(ezcfg, "soap parent index is invalid\n");
		return -1;
	}

	sibling = NULL;
	if (si > 0) {
		sibling = ezcfg_xml_get_element_by_index(xml, si);
		if (sibling == NULL) {
			err(ezcfg, "soap sibling index is invalid\n");
			return -1;
		}
	}

	return ezcfg_xml_add_element(xml, parent, sibling, elem);
}

bool ezcfg_soap_add_body_child_attribute(struct ezcfg_soap *soap, int ei, const char *name, const char *value, int pos) {
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;
	struct ezcfg_xml_element *elem;

	ASSERT(soap != NULL);
	ASSERT(ei > 0);
	ASSERT(name != NULL);
	ASSERT(value != NULL);

	ezcfg = soap->ezcfg;
	xml = soap->xml;

	if (soap->envelope_index < 0) {
		err(ezcfg, "no soap envelope element!\n");
		return false;
	}

	elem = ezcfg_xml_get_element_by_index(xml, ei);
	if (elem == NULL) {
		err(ezcfg, "soap element index is invalid!\n");
		return false;
	}

	return ezcfg_xml_element_add_attribute(xml, elem, name, value, pos);
}

int ezcfg_soap_get_element_index(struct ezcfg_soap *soap, const int pi, const char *name)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;

	ASSERT(soap != NULL);
	ASSERT(soap->xml != NULL);
	ASSERT(name != NULL);
	ASSERT(pi > 0);

	ezcfg = soap->ezcfg;
	xml = soap->xml;

	if (soap->body_index < 0) {
		err(ezcfg, "no body for soap child\n");
		return -1;
	}

	return ezcfg_xml_get_element_index(xml, pi, name);
}

char *ezcfg_soap_get_element_content_by_index(struct ezcfg_soap *soap, const int i)
{
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;

	ASSERT(soap != NULL);
	ASSERT(soap->xml != NULL);
	ASSERT(i > 0);

	ezcfg = soap->ezcfg;
	xml = soap->xml;

	return ezcfg_xml_get_element_content_by_index(xml, i);
}

bool ezcfg_soap_parse_request(struct ezcfg_soap *soap, char *buf, int len)
{
	/* buf must be \0-terminated */
	struct ezcfg *ezcfg;
	struct ezcfg_xml *xml;

	ASSERT(soap != NULL);
	ASSERT(soap->xml != NULL);
	ASSERT(buf != NULL);
	ASSERT(len > 0);

	ezcfg = soap->ezcfg;
	xml = soap->xml;

	if (ezcfg_xml_parse(xml, buf, len) == false) {
		return false;
	}

	return true;
}

int ezcfg_soap_write(struct ezcfg_soap *soap, char *buf, int len)
{
	struct ezcfg *ezcfg;

	ASSERT(soap != NULL);
	ASSERT(buf != NULL);
	ASSERT(len > 0);

	ezcfg = soap->ezcfg;

        return ezcfg_xml_write(soap->xml, buf, len);
}
