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
 *
 * Create ezcfg soap info builder data structure
 *
 * Returns: a new ezcfg soap info builder data structure
 **/
struct ezcfg_soap *ezcfg_soap_new(struct ezcfg *ezcfg, struct ezcfg_xml *xml)
{
	struct ezcfg_soap *soap;

	assert(ezcfg != NULL);
	assert(xml != NULL);

	/* initialize soap info builder data structure */
	soap = calloc(1, sizeof(struct ezcfg_soap));
	if (soap == NULL) {
		err(ezcfg, "initialize soap builder error.\n");
		return NULL;
	}

	memset(soap, 0, sizeof(struct ezcfg_soap));

	soap->env_index = ezcfg_xml_get_element_index(xml, EZCFG_SOAP_ENV_ELEMENT_NAME);
	if (soap->env_index != 0) {
		err(ezcfg, "invalid soap envelope element!\n");
		free(soap);
		return NULL;
	}

	soap->body_index = ezcfg_xml_get_element_index(xml, EZCFG_SOAP_BODY_ELEMENT_NAME);
	if (soap->body_index < 1) {
		err(ezcfg, "invalid soap body element!\n");
		free(soap);
		return NULL;
	}

	soap->ezcfg = ezcfg;
	soap->xml = xml;

	return soap;
}

