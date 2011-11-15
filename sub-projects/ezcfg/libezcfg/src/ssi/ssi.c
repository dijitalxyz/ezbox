/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ssi/ssi.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-15   0.1       Write it from scratch
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

#include <locale.h>
#include <libintl.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

struct ezcfg_ssi {
	struct ezcfg *ezcfg;
	struct ezcfg_nvram *nvram;
	char *document_root;
	char *path;
	FILE *fp;
};

/**
 * Private functions
 **/

/**
 * Public functions
 **/
void ezcfg_ssi_delete(struct ezcfg_ssi *ssi)
{
	ASSERT(ssi != NULL);

	if (ssi->fp != NULL) {
		fclose(ssi->fp);
	}
	if (ssi->path != NULL) {
		free(ssi->path);
	}
	if (ssi->document_root != NULL) {
		free(ssi->document_root);
	}

	free(ssi);
}

struct ezcfg_ssi *ezcfg_ssi_new(struct ezcfg *ezcfg)
{
	struct ezcfg_ssi *ssi;

	ASSERT(ezcfg != NULL);

	ssi = calloc(1, sizeof(struct ezcfg_ssi));
	if (ssi != NULL) {
		ssi->ezcfg = ezcfg;
		ssi->nvram = NULL;
		ssi->path = NULL;
		ssi->fp = NULL;
	}
	return ssi;
}

bool ezcfg_ssi_set_document_root(struct ezcfg_ssi *ssi, const char *root)
{
	char *p;

	ASSERT(ssi != NULL);
	ASSERT(root != NULL);

	p = strdup(root);
	if (p == NULL) {
		return false;
	}

	if (ssi->document_root != NULL) {
		free(ssi->document_root);
	}
	ssi->document_root = p;
	return true;
}

bool ezcfg_ssi_set_path(struct ezcfg_ssi *ssi, const char *path)
{
	char *p;

	ASSERT(ssi != NULL);
	ASSERT(path != NULL);

	p = strdup(path);
	if (p == NULL) {
		return false;
	}

	if (ssi->path != NULL) {
		free(ssi->path);
	}
	ssi->path = p;
	return true;
}

FILE *ezcfg_ssi_open_file(struct ezcfg_ssi *ssi, const char *mode)
{
	FILE *fp;
	char buf[256];

	ASSERT(ssi != NULL);

	if (ssi->document_root == NULL || ssi->path == NULL)
		return NULL;

	snprintf(buf, sizeof(buf), "%s%s", ssi->document_root, ssi->path);
	fp = fopen(buf, mode);
	if (fp == NULL)
		return NULL;

	if (ssi->fp != NULL) {
		fclose(ssi->fp);
	}
	ssi->fp = fp;
	return fp;
}

int ezcfg_ssi_file_get_line(struct ezcfg_ssi *ssi, char *buf, size_t size)
{
	if (fgets(buf, size, ssi->fp) != NULL) {
		return strlen(buf);
	}
	return -1;
}
