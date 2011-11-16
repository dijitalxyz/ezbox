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

struct directive_format {
	char *elem;
	int argc;
	char **argv;
};

struct ssi_directive_entry {
	struct directive_format *format;
	char **values;
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

#define SSI_ARGC_MAX              4

static char *include_argv[] = { "virtual" };
static char *exec_argv[] = { "cmd" };
static char *echo_argv[] = { "var", "ns" };
static struct directive_format supported_directives[] = {
	{ "include", 1, include_argv, },
	{ "exec", 1, exec_argv, },
	{ "echo", 2, echo_argv, },
	{ NULL, 0, NULL }
};

/**
 * Private functions
 **/
void remove_escape_backslash(char *s)
{
	char *p = s;
	while(*s != '\0') {
		if (*s == '\\' && *(s+1) == '"') s++;
		*p++ = *s++;
	}
	*p = '\0';
}

void delete_directive_entry(struct ssi_directive_entry *entry)
{
	int i, argc = 0;
	char *p;

	if (entry->format != NULL) {
		argc = (entry->format)->argc;
	}
	for (i = 0; i < argc; i++) {
		p = (entry->values)[i];
		if (p != NULL) {
			free(p);
		}
	}
	free(entry);
}

bool ssi_parse_directive_entry(struct ezcfg_ssi *ssi, char *buf)
{
	struct ssi_directive_entry *entry;
	struct directive_format *dfp;
	bool ret = false;
	char *s, *p;
	int i, len;
	char *values_s[SSI_ARGC_MAX];
	char *values_e[SSI_ARGC_MAX];

	entry = calloc(1, sizeof(struct ssi_directive_entry));
	if (entry == NULL) {
		return false;
	}

	entry->format = NULL;
	entry->values = NULL;

	/* find element */
	for (dfp = supported_directives; dfp->elem != NULL; dfp++) {
		s = dfp->elem;
		len = strlen(s);
		if (strncmp(s, buf, len) == 0 && buf[len] == ' ') {
			/* find match element */
			p = buf+len+1;
			for (i = 0; i < dfp->argc; i++) {
				while (*p == ' ') p++;
				s = (dfp->argv)[i];
				len = strlen(s);
				if (strncmp(s, p, len) != 0)
					break;
				if (p[len] != '=')
					break;
				if (p[len+1] != '"')
					break;
				s = p+len+2;
				/* try to find ending '"' */
				p = strchr(s, '"');
				while(p != NULL && *(p-1) == '\\')
					p = strchr(p+1, '"');
				if (p == NULL)
					break;
				values_s[i] = s; /* s point to charactor after '"' */
				values_e[i] = p; /* p point to '"' */
				p++;
			}
			if (i == dfp->argc) {
				/* find match directive format */
				entry->values = calloc(dfp->argc, sizeof(char *));
				if (entry->values == NULL) {
					goto func_exit;
				}
				/* copy values */
				for (i = 0; i < dfp->argc; i++) {
					s = values_s[i];
					p = values_e[i];
					len = p -s;
					p = calloc(len+1, sizeof(char));
					if(p == NULL) {
						goto func_exit;
					}
					strncpy(p, s, len);
					*(p+len) = '\0';
					remove_escape_backslash(p);
					(entry->values)[i] = p;
				}
			}
		}
	}

	if (ssi->directive_entry != NULL) {
		delete_directive_entry(ssi->directive_entry);
	}
	ssi->directive_entry = entry;
	/* make entry undeleted */
	entry = NULL;
	ret = true;
func_exit:
	if (entry != NULL)
		delete_directive_entry(entry);
	return ret;
}

int ssi_handle_directive_entry(
	struct ezcfg_ssi *ssi,
	char *buf, size_t size)
{
	return 0;
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
		if (ssi_parse_directive_entry(ssi, p) == false) {
			return -1;
		}

		/* cut off the buf and return first part */
		*s = '\0';
		ret = strlen(buf);
	}
	else {
		ret = ssi_handle_directive_entry(ssi, buf, size);
		if (ret == 0) {
			delete_directive_entry(ssi->directive_entry);
			ssi->directive_entry = NULL;
		}
	}
	return ret;
}
