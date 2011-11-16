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

struct link_node {
	char *attr;
	char *value;
	struct link_node *next;
};

struct ssi_directive_entry {
	char *elem;
	struct link_node *lists;
};

struct ezcfg_ssi {
	struct ezcfg *ezcfg;
	struct ezcfg_nvram *nvram;
	char *document_root;
	char *path;
	FILE *fp;
	char *remaining_data;
	struct ssi_directive_entry *directive_entry;
};

#define SSI_STARTING_SEQUENCE     "<!--#"
#define SSI_STARTING_SEQUENCE_LEN 5
#define SSI_ENDING_SEQUENCE       "-->"
#define SSI_ENDING_SEQUENCE_LEN   3

/**
 * Private functions
 **/
struct ssi_directive_entry *parse_directive_entry(char *buf)
{
	struct ssi_directive_entry *entry;

	entry = calloc(1, sizeof(struct ssi_directive_entry));
	if (entry != NULL) {
		entry->elem = NULL;
		entry->lists = NULL;
	}
	return entry;
}

int handle_directive_entry(
	struct ssi_directive_entry *entry,
	char *buf, size_t size)
{
	return 0;
}

void delete_directive_entry(struct ssi_directive_entry *entry)
{
	struct link_node *l;
	while(entry->lists != NULL) {
		l = entry->lists;
		entry->lists = l->next;
		if (l->attr != NULL)
			free(l->attr);
		if (l->value != NULL)
			free(l->value);
		free(l);
	} 
	if (entry->elem != NULL)
		free(entry->elem);
	free(entry);
}

/**
 * Public functions
 **/
void ezcfg_ssi_delete(struct ezcfg_ssi *ssi)
{
	ASSERT(ssi != NULL);

	if (ssi->directive_entry != NULL) {
		free(ssi->directive_entry);
	}
	if (ssi->remaining_data != NULL) {
		free(ssi->remaining_data);
	}
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
		ssi->remaining_data = NULL;
		ssi->directive_entry = NULL;
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
	char *s, *e, *p;
	int ret;
	if (ssi->directive_entry == NULL) {
		/* check if there is some remaining data should be handled */
		if (ssi->remaining_data == NULL) {
			/* OK, it should read new line from SSI file */
			if (fgets(buf, size, ssi->fp) == NULL) {
				return -1;
			}
		}
		else {
			strcpy(buf, ssi->remaining_data);
			free(ssi->remaining_data);
			ssi->remaining_data = NULL;
		}

		/* search SSI starting sequence */
		s = strstr(buf, SSI_STARTING_SEQUENCE);
		if (s == NULL) {
			return strlen(buf);
		}

		/* search SSI ending sequence */
		e = strstr(s+SSI_STARTING_SEQUENCE_LEN, SSI_ENDING_SEQUENCE);
		if (e == NULL) {
			/* no matching SSI ending sequence */
			return -1;
		}

		/* find an SSI diretive */
		/* first store the data after SSI directive entry */
		ssi->remaining_data = strdup(e+SSI_ENDING_SEQUENCE_LEN);
		if (ssi->remaining_data == NULL) {
			return -1;
		}

		/* then construct SSI directive entry */
		p = s + SSI_STARTING_SEQUENCE_LEN;
		*e = '\0';
		ssi->directive_entry = parse_directive_entry(p);
		if (ssi->directive_entry == NULL) {
			return -1;
		}

		/* cut off the buf and return first part */
		*s = '\0';
		ret = strlen(buf);
	}
	else {
		ret = handle_directive_entry(ssi->directive_entry, buf, size);
		if (ret == 0) {
			delete_directive_entry(ssi->directive_entry);
			ssi->directive_entry = NULL;
		}
	}
	return ret;
}
