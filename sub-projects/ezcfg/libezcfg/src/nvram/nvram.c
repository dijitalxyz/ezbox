/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : nvram/nvram.c
 *
 * Description  : implement Non-Volatile RAM
 * Warning      : must exclusively use it, say lock NVRAM before using it.
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2010-08-20   0.1       Write it from scratch
 * 2010-11-06   0.2       Rewrite it using sorted string storing approach
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

#include "ezcfg.h"
#include "ezcfg-private.h"

#define NVRAM_VERSOIN_MAJOR 0x00 /* version[0] */
#define NVRAM_VERSOIN_MINOR 0x01 /* version[1] */
#define NVRAM_VERSOIN_MICRO 0x00 /* version[2] */
#define NVRAM_VERSOIN_REV   0x03 /* version[3] */ 

#if 0
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

enum {
	NV_INIT = 0,
	NV_RELOAD,
};

static unsigned char default_magics[][4] = {
	{ 'N', 'O', 'N', 'E' },
	{ 'F', 'I', 'L', 'E' },
	{ 'F', 'L', 'S', 'H' },
};

static unsigned char default_version[4] = {
	NVRAM_VERSOIN_MAJOR ,
	NVRAM_VERSOIN_MINOR ,
	NVRAM_VERSOIN_MICRO ,
	NVRAM_VERSOIN_REV ,
};

static unsigned char default_codings[][4] = {
	{ 'N', 'O', 'N', 'E' },
	{ 'G', 'Z', 'I', 'P' },
};

struct nvram_header {
	unsigned char magic[4];
	unsigned char version[4];
	unsigned char coding[4];
	int32_t size;
	int32_t used;
	uint32_t crc;
};

struct nvram_storage {
	/* storage backend type, mapping to nvram header->magic
	 * 0 -> NONE : in memory only
	 * 1 -> FILE : store on file-system a file
	 * 2 -> FLSH : store on flash chip
	 */
	int backend;

	/* nvram content coding type, mapping to nvram header->coding
	 * 0 -> NONE : plain text
	 * 1 -> GZIP : store with gzip compress
	 */
	int coding;

	/* nvram storage device/file path */
	char *path;
};

/**************************************************************************
 *
 * NVRAM storage format
 *
 * The NVRAM is stored as a list of '\0' terminated "name=value"
 * strings. The end of the list is marked by a double '\0'.
 * New entries are added at "name" sorted position. Deleting an entry
 * shifts the remaining entries to the front. Replacing an entry is a
 * combination of deleting the old value and adding the new one.
 *
 * The NVRAM is preceeded by a header which contained info for
 * NVRAM storage medium, version, coding style and a 32 bit CRC over
 * the data part.
 *
 **************************************************************************
 */

struct ezcfg_nvram {
	struct ezcfg *ezcfg;

	pthread_mutex_t mutex; /* Protects nvram operations */
	char *buffer; /* NVRAM in memory buffer */

	/* storage info */
	struct nvram_storage storage[EZCFG_NVRAM_STORAGE_NUM];

	/* default settings */
	int num_default_settings;
	ezcfg_nv_pair_t *default_settings;

	/* default savings */
	int num_default_savings;
	char **default_savings;

	/* default validators */
	int num_default_validators;
	ezcfg_nv_validator_t *default_validators;

	/* total_space = sizeof(header) + free_space + used_space */
	int total_space; /* storage space for nvram */
	int free_space; /* unused space */
	int used_space; /* user used space */
};

/*
 * Private functions
 */
static char *find_nvram_entry_position(char *data, const char *name)
{
	int name_len, entry_len, cmp_len;
	char *entry;

	entry = data;
	name_len = strlen(name);

	while (*entry != '\0') {
		entry_len = strlen(entry) + 1;
		cmp_len = (entry_len > name_len) ? name_len : entry_len ;
		if ((strncmp(name, entry, name_len) == 0) &&
		     (*(entry + cmp_len) == '=')) {
			break;
		}
		else {
			entry += entry_len;
		}
	}
	return entry;
}

static bool nvram_set_entry(struct ezcfg_nvram *nvram, const char *name, const char *value)
{
	struct ezcfg *ezcfg;
	int name_len, entry_len, new_entry_len;
	struct nvram_header *header;
	char *data, *p;

	ezcfg = nvram->ezcfg;

	name_len = strlen(name);
	new_entry_len = name_len + strlen(value) + 2;

	header = (struct nvram_header *)nvram->buffer;
	data = nvram->buffer + sizeof(struct nvram_header);

	/* first entry */
	if (nvram->used_space == 0) {
		if (new_entry_len + 1 > nvram->free_space) {
			err(ezcfg, "no enough space for nvram entry set\n");
			return false;
		}
		sprintf(data, "%s=%s", name, value);
		*(data + new_entry_len + 1) = '\0';
		nvram->used_space += (new_entry_len + 1);
		nvram->free_space -= (new_entry_len + 1);
		return true;
	}

	p = find_nvram_entry_position(data, name);
	if (*p != '\0') {
		/* find nvram entry */
		entry_len = strlen(p) + 1;
		if (new_entry_len > (nvram->free_space + entry_len)) {
			err(ezcfg, "no enough space for nvram entry set\n");
			return false;
		}

		if (entry_len == new_entry_len) {
			/* replace directory */
			sprintf(p, "%s=%s", name, value);
			return true;
		}
		else {
			/* original entry is smaller/larger, move backward/forward */
			memmove(p + new_entry_len, p + entry_len, nvram->used_space - (p - data) - entry_len);
			sprintf(p, "%s=%s", name, value);
			nvram->used_space += (new_entry_len - entry_len);
			nvram->free_space -= (new_entry_len - entry_len);
			return true;
		}
	}
	else {
		/* not find nvram entry */
		if (new_entry_len > nvram->free_space) {
			err(ezcfg, "no enough space for nvram entry set\n");
			return false;
		}
		/* insert nvram entry */
		memmove(p + new_entry_len, p, nvram->used_space - (p - data));
		sprintf(p, "%s=%s", name, value);
		nvram->used_space += new_entry_len;
		nvram->free_space -= new_entry_len;
		return true;
	}
}

static bool nvram_unset_entry(struct ezcfg_nvram *nvram, const char *name)
{
	struct ezcfg *ezcfg;
	char *data, *p;
	int name_len, entry_len;
	bool ret = false;

	ezcfg = nvram->ezcfg;

	name_len = strlen(name);

	data = nvram->buffer + sizeof(struct nvram_header);

	p = find_nvram_entry_position(data, name);

	if (*p != '\0') {
		/* find nvram entry */
		entry_len = strlen(p) + 1;
		memmove(p, p + entry_len, nvram->used_space - (p - data) - entry_len);
		nvram->used_space -= entry_len;
		nvram->free_space += entry_len;
		ret = true;
	}
	else {
		/* not find nvram entry */
		ret = false;
	}

	return ret;
}

/* It's user's duty to free the returns string */
static bool nvram_get_entry_value(struct ezcfg_nvram *nvram, const char *name, char **value)
{
	struct ezcfg *ezcfg;
	char *p, *data;
	int name_len;

	ezcfg = nvram->ezcfg;

	data = nvram->buffer + sizeof(struct nvram_header);
	name_len = strlen(name);
	*value = NULL;

	/* find nvram entry position */
	p = find_nvram_entry_position(data, name);

	if (*p != '\0') {
		/* find nvram entry */
		*value = strdup(p + name_len + 1);
		if (*value == NULL) {
			err(ezcfg, "not enough memory for get nvram node.\n");
			return false;
		}
	}
	return true;
}

static bool nvram_match_entry(struct ezcfg_nvram *nvram, const char *name1, char *name2)
{
	struct ezcfg *ezcfg;
	char *data;
	char *p1, *p2;
	int name_len;

	ezcfg = nvram->ezcfg;

	data = nvram->buffer + sizeof(struct nvram_header);

	/* find nvram entry position */
	name_len = strlen(name1);
	p1 = find_nvram_entry_position(data, name1);

	if (*p1 == '\0') {
		return false;
	}
	/* find nvram entry */
	p1 += (name_len + 1);

	/* find nvram entry position */
	name_len = strlen(name2);
	p2 = find_nvram_entry_position(data, name2);

	if (*p2 == '\0') {
		return false;
	}
	/* find nvram entry */
	p2 += (name_len + 1);

	return (strcmp(p1, p2) == 0);
}

static bool nvram_match_entry_value(struct ezcfg_nvram *nvram, const char *name, char *value)
{
	struct ezcfg *ezcfg;
	char *data;
	char *p;
	int name_len;

	ezcfg = nvram->ezcfg;

	data = nvram->buffer + sizeof(struct nvram_header);

	/* find nvram entry position */
	name_len = strlen(name);
	p = find_nvram_entry_position(data, name);

	if (*p == '\0') {
		return false;
	}
	/* find nvram entry */
	p += (name_len + 1);

	return (strcmp(p, value) == 0);
}

static int nvram_get_socket_index(struct ezcfg_nvram *nvram,
	char *domain, char *type, char *protocol, char *address)
{
	char buf[32];
	int i;
	char *value;
	char *domain2, *type2, *protocol2, *address2;
	bool ret;

	i = 0;
	ret = nvram_get_entry_value(nvram,
		NVRAM_SERVICE_OPTION(EZCFG, COMMON_SOCKET_NUMBER),
		&value);
	if (value != NULL) {
		i = atoi(value);
		free(value);
	}
	for( ; i >= 0; i--) {
		/* get nvram socket domain */
		snprintf(buf, sizeof(buf), "%s%s.%d.%s",
			NVRAM_PREFIX(EZCFG),
			EZCFG_EZCFG_SECTION_SOCKET, i-1,
			EZCFG_EZCFG_KEYWORD_DOMAIN);
		ret = nvram_get_entry_value(nvram, buf, &domain2);

		/* get nvram socket type */
		snprintf(buf, sizeof(buf), "%s%s.%d.%s",
			NVRAM_PREFIX(EZCFG),
			EZCFG_EZCFG_SECTION_SOCKET, i-1,
			EZCFG_EZCFG_KEYWORD_TYPE);
		ret = nvram_get_entry_value(nvram, buf, &type2);

		/* get nvram socket protocol */
		snprintf(buf, sizeof(buf), "%s%s.%d.%s",
			NVRAM_PREFIX(EZCFG),
			EZCFG_EZCFG_SECTION_SOCKET, i-1,
			EZCFG_EZCFG_KEYWORD_PROTOCOL);
		ret = nvram_get_entry_value(nvram, buf, &protocol2);

		/* get nvram socket address */
		snprintf(buf, sizeof(buf), "%s%s.%d.%s",
			NVRAM_PREFIX(EZCFG),
			EZCFG_EZCFG_SECTION_SOCKET, i-1,
			EZCFG_EZCFG_KEYWORD_ADDRESS);
		ret = nvram_get_entry_value(nvram, buf, &address2);

		if (domain2 != NULL && type2 != NULL &&
		    protocol2 != NULL && address2 != NULL) {
			if ((strcmp(domain, domain2) == 0) &&
			    (strcmp(type, type2) == 0) &&
			    (strcmp(protocol, protocol2) == 0) &&
			    (strcmp(address, address2) == 0)) {
				free(domain2);
				free(type2);
				free(protocol2);
				free(address2);
				return(i);
			}
		}

		if (domain2 != NULL)
			free(domain2);
		if (type2 != NULL)
			free(type2);
		if (protocol2 != NULL)
			free(protocol2);
		if (address2 != NULL)
			free(address2);
	}
	return(0);
}

static bool nvram_remove_socket_by_index(struct ezcfg_nvram *nvram, int idx)
{
	char buf[32];
	int i;
	char *value;
	bool ret;

	i = 0;
	ret = nvram_get_entry_value(nvram,
		NVRAM_SERVICE_OPTION(EZCFG, COMMON_SOCKET_NUMBER),
		&value);
	if (value != NULL) {
		i = atoi(value);
		free(value);
	}

	if (i < idx) {
		return false;
	}

	/* put the last socket to index-th socket place */
	if (idx < i) {
		/* get nvram socket domain */
		snprintf(buf, sizeof(buf), "%s%s.%d.%s",
			NVRAM_PREFIX(EZCFG),
			EZCFG_EZCFG_SECTION_SOCKET, i-1,
			EZCFG_EZCFG_KEYWORD_DOMAIN);
		ret = nvram_get_entry_value(nvram, buf, &value);
		if (value == NULL) {
			ret = false;
			goto func_out;
		}

		/* set nvram socket domain */
		snprintf(buf, sizeof(buf), "%s%s.%d.%s",
			NVRAM_PREFIX(EZCFG),
			EZCFG_EZCFG_SECTION_SOCKET, idx-1,
			EZCFG_EZCFG_KEYWORD_DOMAIN);
		ret = nvram_set_entry(nvram, buf, value);
		free(value);
		if (ret == false) {
			goto func_out;
		}

		/* get nvram socket type */
		snprintf(buf, sizeof(buf), "%s%s.%d.%s",
			NVRAM_PREFIX(EZCFG),
			EZCFG_EZCFG_SECTION_SOCKET, i-1,
			EZCFG_EZCFG_KEYWORD_TYPE);
		ret = nvram_get_entry_value(nvram, buf, &value);
		if (value == NULL) {
			ret = false;
			goto func_out;
		}

		/* set nvram socket type */
		snprintf(buf, sizeof(buf), "%s%s.%d.%s",
			NVRAM_PREFIX(EZCFG),
			EZCFG_EZCFG_SECTION_SOCKET, idx-1,
			EZCFG_EZCFG_KEYWORD_TYPE);
		ret = nvram_set_entry(nvram, buf, value);
		free(value);
		if (ret == false) {
			goto func_out;
		}

		/* get nvram socket protocol */
		snprintf(buf, sizeof(buf), "%s%s.%d.%s",
			NVRAM_PREFIX(EZCFG),
			EZCFG_EZCFG_SECTION_SOCKET, i-1,
			EZCFG_EZCFG_KEYWORD_PROTOCOL);
		ret = nvram_get_entry_value(nvram, buf, &value);
		if (value == NULL) {
			ret = false;
			goto func_out;
		}

		/* set nvram socket protocol */
		snprintf(buf, sizeof(buf), "%s%s.%d.%s",
			NVRAM_PREFIX(EZCFG),
			EZCFG_EZCFG_SECTION_SOCKET, idx-1,
			EZCFG_EZCFG_KEYWORD_PROTOCOL);
		ret = nvram_set_entry(nvram, buf, value);
		free(value);
		if (ret == false) {
			goto func_out;
		}

		/* get nvram socket address */
		snprintf(buf, sizeof(buf), "%s%s.%d.%s",
			NVRAM_PREFIX(EZCFG),
			EZCFG_EZCFG_SECTION_SOCKET, i-1,
			EZCFG_EZCFG_KEYWORD_ADDRESS);
		ret = nvram_get_entry_value(nvram, buf, &value);
		if (value == NULL) {
			ret = false;
			goto func_out;
		}

		/* set nvram socket address */
		snprintf(buf, sizeof(buf), "%s%s.%d.%s",
			NVRAM_PREFIX(EZCFG),
			EZCFG_EZCFG_SECTION_SOCKET, idx-1,
			EZCFG_EZCFG_KEYWORD_ADDRESS);
		ret = nvram_set_entry(nvram, buf, value);
		free(value);
		if (ret == false) {
			goto func_out;
		}
	}

	/* update socket number */
	i--;
	snprintf(buf, sizeof(buf), "%d", i);
	ret = nvram_set_entry(nvram,
		NVRAM_SERVICE_OPTION(EZCFG, COMMON_SOCKET_NUMBER),
		buf);

func_out:
	return ret;
}

static bool nvram_cleanup_runtime_entries(struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;
	ezcfg_nv_pair_t *nvp;
	int i;
	char *data, *entry, *name, *p;
	bool ret;

	ezcfg = nvram->ezcfg;
	data = nvram->buffer + sizeof(struct nvram_header);
	entry = data;
	while(*entry != '\0') {
		p = strchr(entry, '=');
		if (p == NULL) {
			return false;
		}

		/* split name and value */
		*p = '\0';
		for (i=0; i<nvram->num_default_settings; i++) {
			nvp = &nvram->default_settings[i];
			if (strcmp(entry, nvp->name) == 0) {
				break;
			}
		}

		if (i == nvram->num_default_settings) {
			/* not in default_settings, it's a runtime nvram */
			name = strdup(entry);
			if (name == NULL) {
				*p = '=';
				ret = false;
				goto func_out;
			}
			/* restore '=' */
			*p = '=';
			ret = nvram_unset_entry(nvram, name);
			free(name);
			if (ret == false)
				goto func_out;

			/* try it again from the beginning */
			/* FIXME: no we just do it at the same place again */
			//entry = data;
		}
		else {
			/* restore '=' */
			*p = '=';
			/* move to next entry */
			entry += (strlen(entry) + 1);
		}
	}

	ret = true;
func_out:
	return ret;
}

static bool nvram_load_by_defaults(struct ezcfg_nvram *nvram, const int flag)
{
	struct ezcfg *ezcfg;
	ezcfg_nv_pair_t *nvp;
	int i;

	ezcfg = nvram->ezcfg;

	/* clean up runtime generated nvram values */
	if (flag == NV_INIT) {
		if (nvram_cleanup_runtime_entries(nvram) == false)
			return false;
	}

	/* fill missing critic nvram with default settings */
	for (i=0; i<nvram->num_default_settings; i++) {
		nvp = &nvram->default_settings[i];
		if (nvram_set_entry(nvram, nvp->name, nvp->value) == false) {
			err(ezcfg, "set nvram entry error.\n");
			return false;
		}
	}
	return true;
}

static bool nvram_load_from_file(struct ezcfg_nvram *nvram, const int idx, const int flag)
{
	struct ezcfg *ezcfg;
	struct nvram_header *header;
	char *data;
	FILE *fp;
	uint32_t crc;
	ezcfg_nv_pair_t *nvp;
	char *value = NULL;
	int i;
	bool ret = false;

	ezcfg = nvram->ezcfg;

	fp = fopen(nvram->storage[idx].path, "r");
	if (fp == NULL) {
		err(ezcfg, "can't open file for nvram load\n");
		return false;
	}

	if (fread(nvram->buffer, nvram->total_space, 1, fp) < 1) {
		err(ezcfg, "read nvram buffer error\n");
		goto load_exit;
	}

	header = (struct nvram_header *)nvram->buffer;
	if ((nvram->total_space - header->size) != 0) {
		err(ezcfg, "nvram size is not matched\n");
		goto load_exit;
	}

	data = nvram->buffer + sizeof(struct nvram_header);

	/* do CRC-32 calculation */
	crc = ezcfg_util_crc32((unsigned char *)data, nvram->total_space - sizeof(struct nvram_header));
	if (header->crc != crc) {
		err(ezcfg, "nvram crc is error.\n");
		goto load_exit;
	}

	/* fill nvram space info */
	nvram->used_space = header->used;
	nvram->free_space = nvram->total_space - sizeof(struct nvram_header) - nvram->used_space;

	/* clean up runtime generated nvram values */
	if (flag == NV_INIT) {
		if (nvram_cleanup_runtime_entries(nvram) == false)
			goto load_exit;
	}

	/* fill missing critic nvram with default settings */
	for (i=0; i<nvram->num_default_settings; i++) {
		nvp = &nvram->default_settings[i];
		ret = nvram_get_entry_value(nvram, nvp->name, &value);
		if (value == NULL) {
			if (ret == true) {
				if (nvram_set_entry(nvram, nvp->name, nvp->value) == false) {
					err(ezcfg, "set nvram error.\n");
					ret = false;
					goto load_exit;
				}
			}
		}
		else {
			free(value);
		}
	}

	ret = true;

load_exit:
	if (fp != NULL) {
		fclose(fp);
	}

	return ret;
}

static bool nvram_load_from_flash(struct ezcfg_nvram *nvram, const int idx, const int flag)
{
	return true;
}

static void generate_nvram_header(struct ezcfg_nvram *nvram, const int idx)
{
	struct nvram_header *header;
	char *data;
	int i;

	if (idx >= EZCFG_NVRAM_STORAGE_NUM)
		return;

	header = (struct nvram_header *)nvram->buffer;
	data = nvram->buffer + sizeof(struct nvram_header);

	for (i=0; i<4; i++) {
		header->magic[i] = default_magics[nvram->storage[idx].backend][i];
	}
	for (i=0; i<4; i++) {
		header->coding[i] = default_codings[nvram->storage[idx].coding][i];
	}
	for (i=0; i<4; i++) {
		header->version[i] = default_version[i];
	}
	header->size = nvram->total_space;
	header->used = nvram->used_space;

	memset(data + nvram->used_space, '\0', nvram->free_space);
	header->crc = ezcfg_util_crc32((unsigned char *)data, nvram->used_space + nvram->free_space);
}

#if 0
static void nvram_header_copy(struct nvram_header *dest, const struct nvram_header *src)
{
	memcpy(dest, src, sizeof(struct nvram_header));
}
#endif

static bool nvram_commit_to_file(struct ezcfg_nvram *nvram, const int idx)
{
	struct ezcfg *ezcfg;
	FILE *fp = NULL;
	bool ret = false;

	ezcfg = nvram->ezcfg;

	/* generate nvram header info */
	generate_nvram_header(nvram, idx);

	fp = fopen(nvram->storage[idx].path, "w");
	if (fp == NULL) {
		err(ezcfg, "can't open file for nvram commit\n");
		ret = false;
		goto commit_exit;
	}

	if (fwrite(nvram->buffer, nvram->total_space, 1, fp) < 1) {
		err(ezcfg, "write nvram to file error\n");
		ret = false;
		goto commit_exit;
	}

	ret = true;

commit_exit:
	if (fp != NULL) {
		fclose(fp);
	}

	return ret;
}

static bool nvram_commit_to_flash(struct ezcfg_nvram *nvram, const int idx)
{
	return true;
}

static void sync_ezcfg_settings(struct ezcfg_nvram *nvram)
{
	char *p;
	int number, i;
	char buf[64];

	/* ezcfg_common.socket_number */
	nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EZCFG, COMMON_SOCKET_NUMBER), &p);
	if (p != NULL) {
		number = atoi(p);
		free(p);
		for(i = 0; i < number; i++) {
			snprintf(buf, sizeof(buf), "%s%s.%d.%s",
				EZCFG_EZCFG_NVRAM_PREFIX,
				EZCFG_EZCFG_SECTION_SOCKET, i,
				EZCFG_EZCFG_KEYWORD_DOMAIN);
			nvram_get_entry_value(nvram, buf, &p);
			if (p != NULL) {
				/* It's OK for this socket setting */
				free(p);
			}
			else {
				snprintf(buf, sizeof(buf), "%d", i);
				nvram_set_entry(nvram, NVRAM_SERVICE_OPTION(EZCFG, COMMON_SOCKET_NUMBER), buf);
				break;
			}
		}
	}

	/* ezcfg_common.auth_number */
	nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EZCFG, COMMON_AUTH_NUMBER), &p);
	if (p != NULL) {
		number = atoi(p);
		free(p);
		for(i = 0; i < number; i++) {
			snprintf(buf, sizeof(buf), "%s%s.%d.%s",
				EZCFG_EZCFG_NVRAM_PREFIX,
				EZCFG_EZCFG_SECTION_AUTH, i,
				EZCFG_EZCFG_KEYWORD_TYPE);
			nvram_get_entry_value(nvram, buf, &p);
			if (p != NULL) {
				/* It's OK for this auth setting */
				free(p);
			}
			else {
				snprintf(buf, sizeof(buf), "%d", i);
				nvram_set_entry(nvram, NVRAM_SERVICE_OPTION(EZCFG, COMMON_AUTH_NUMBER), buf);
				break;
			}
		}
	}

	/* ezcfg_auth.0.user must be "root" */
	nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_USER), &p);
	if ((p == NULL) ||
	    ((p != NULL) && (strcmp(p, EZCFG_AUTH_USER_ADMIN_STRING) != 0))) {
		/* first release p */
		if (p != NULL)
			free(p);

		/* not valid admin user, fix it! */
		nvram_set_entry(nvram, NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_TYPE), EZCFG_AUTH_TYPE_HTTP_BASIC_STRING);
		nvram_set_entry(nvram, NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_USER), EZCFG_AUTH_USER_ADMIN_STRING);
		nvram_set_entry(nvram, NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_REALM), EZCFG_AUTH_REALM_ADMIN_STRING);
		nvram_set_entry(nvram, NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_DOMAIN), EZCFG_AUTH_DOMAIN_ADMIN_STRING);
		nvram_set_entry(nvram, NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_SECRET), EZCFG_AUTH_SECRET_ADMIN_STRING);
	}
}

static void sync_ui_settings(struct ezcfg_nvram *nvram)
{
	char *p;
	char tz_area[32];
	char tz_location[64];

	tz_area[0] = '\0';
	nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(UI, TZ_AREA), &p);
	if (p != NULL) {
		snprintf(tz_area, sizeof(tz_area), "%s", p);
		free(p);
	}

	tz_location[0] = '\0';
	nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(UI, TZ_LOCATION), &p);
	if (p != NULL) {
		snprintf(tz_location, sizeof(tz_location), "%s", p);
		free(p);
	}

	/* UI setting has been finished */
	if (ezcfg_util_tzdata_check_area_location(tz_area, tz_location) == true) {
		/* ui_tz_area */
		if (nvram_match_entry(nvram, NVRAM_SERVICE_OPTION(UI, TZ_AREA), NVRAM_SERVICE_OPTION(SYS, TZ_AREA)) == false) {
			nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(UI, TZ_AREA), &p);
			if (p != NULL) {
				nvram_set_entry(nvram, NVRAM_SERVICE_OPTION(SYS, TZ_AREA), p);
				free(p);
			}
		}

		/* ui_tz_location */
		if (nvram_match_entry(nvram, NVRAM_SERVICE_OPTION(UI, TZ_LOCATION), NVRAM_SERVICE_OPTION(SYS, TZ_LOCATION)) == false) {
			nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(UI, TZ_LOCATION), &p);
			if (p != NULL) {
				nvram_set_entry(nvram, NVRAM_SERVICE_OPTION(SYS, TZ_LOCATION), p);
				free(p);
			}
		}
	}
}

#if (HAVE_EZBOX_SERVICE_DNSMASQ == 1)
static void sync_dnsmasq_settings(struct ezcfg_nvram *nvram)
{
	char *p;
	int i, ret;
	int ip[4], mask[4], ip2[4];
	//bool ip_ok = false, mask_ok = false;
	char buf[64];

	ret = EZCFG_SERVICE_BINDING_UNKNOWN ;
	nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(RC, DNSMASQ_BINDING), &p);
	if (p != NULL) {
		ret = ezcfg_util_service_binding(p);
	}

	/* get ip/netmask */
	if (ret == EZCFG_SERVICE_BINDING_LAN) {
		nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(LAN, IPADDR), &p);
	}
	else if (ret == EZCFG_SERVICE_BINDING_WAN) {
		nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, IPADDR), &p);
	}
	else {
		return ;
	}

	if (p == NULL) {
		return ;
	}
	ret = sscanf(p, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
	free(p);
	if (ret != 4) {
		return ;
	}

	if (ret == EZCFG_SERVICE_BINDING_LAN) {
		nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(LAN, NETMASK), &p);
	}
	else if (ret == EZCFG_SERVICE_BINDING_WAN) {
		nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, NETMASK), &p);
	}
	else {
		return ;
	}
	if (p == NULL) {
		return ;
	}
	ret = sscanf(p, "%d.%d.%d.%d", &mask[0], &mask[1], &mask[2], &mask[3]);
	free(p);
	if (ret != 4) {
		return ;
	}

	/* set dnsmasq_dhcpd_start_ipaddr */
	nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_START_IPADDR), &p);
	if (p == NULL) {
		return ;
	}
	ret = sscanf(p, "%d.%d.%d.%d", &ip2[0], &ip2[1], &ip2[2], &ip2[3]);
	free(p);
	if (ret == 4) {
		for(i = 0; i < 4; i++) {
			ip2[i] &= (255 - mask[i]);
			ip2[i] |= (ip[i] & mask[i]);
		}
		snprintf(buf, sizeof(buf), "%d.%d.%d.%d", ip2[0], ip2[1], ip2[2], ip2[3]);
		nvram_set_entry(nvram, NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_START_IPADDR), buf);
	}

	/* set dnsmasq_dhcpd_end_ipaddr */
	nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_END_IPADDR), &p);
	if (p == NULL) {
		return ;
	}
	ret = sscanf(p, "%d.%d.%d.%d", &ip2[0], &ip2[1], &ip2[2], &ip2[3]);
	free(p);
	if (ret == 4) {
		for(i = 0; i < 4; i++) {
			ip2[i] &= (255 - mask[i]);
			ip2[i] |= (ip[i] & mask[i]);
		}
		snprintf(buf, sizeof(buf), "%d.%d.%d.%d", ip2[0], ip2[1], ip2[2], ip2[3]);
		nvram_set_entry(nvram, NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_END_IPADDR), buf);
	}

	/* set dnsmasq_dhcpd_enable */
	nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(RC, DNSMASQ_ENABLE), &p);
	if (p == NULL) {
		return ;
	}
	if (strcmp(p, "0") == 0) {
		nvram_set_entry(nvram, NVRAM_SERVICE_OPTION(DNSMASQ, DHCPD_ENABLE), p);
	}
	free(p);
}
#endif

static void nvram_sync_entries(struct ezcfg_nvram *nvram)
{
	/* sync ezcfg settings */
	sync_ezcfg_settings(nvram);

	/* sync UI settings */
	sync_ui_settings(nvram);

	/* sync LAN settings */
#if (HAVE_EZBOX_SERVICE_DNSMASQ == 1)
	sync_dnsmasq_settings(nvram);
#endif
}

static void check_sys_settings(struct ezcfg_nvram *nvram)
{
}

static void nvram_check_entries(struct ezcfg_nvram *nvram)
{
	/* sync UI settings */
	check_sys_settings(nvram);
}

/*
 * Public functions
 */

bool ezcfg_nvram_delete(struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;
	int i;

	ASSERT(nvram != NULL);

	ezcfg = nvram->ezcfg;

	/* lock nvram access */
	pthread_mutex_lock(&nvram->mutex);

	for (i = 0; i < EZCFG_NVRAM_STORAGE_NUM; i++) {
		if (nvram->storage[i].path != NULL) {
			free(nvram->storage[i].path);
		}
	}

	if (nvram->buffer != NULL) {
		free(nvram->buffer);
	}

	/* unlock nvram access */
	pthread_mutex_unlock(&nvram->mutex);

	pthread_mutex_destroy(&nvram->mutex);
	free(nvram);
	return true;
}

struct ezcfg_nvram *ezcfg_nvram_new(struct ezcfg *ezcfg)
{
	struct ezcfg_nvram *nvram;

	ASSERT(ezcfg != NULL);

	nvram = malloc(sizeof(struct ezcfg_nvram));
	if (nvram == NULL) {
		err(ezcfg, "can not malloc nvram\n");
		return NULL;
	}

	memset(nvram, 0, sizeof(struct ezcfg_nvram));

	nvram->ezcfg = ezcfg;

	/* initialize nvram mutex */
	pthread_mutex_init(&nvram->mutex, NULL);

	/* set default settings */
	nvram->num_default_settings = ezcfg_nvram_get_num_default_nvram_settings();
	nvram->default_settings = default_nvram_settings;

	/* set default savings */
	nvram->num_default_savings = ezcfg_nvram_get_num_default_nvram_savings();
	nvram->default_savings = default_nvram_savings;

	/* set default validators */
	nvram->num_default_validators = ezcfg_nvram_get_num_default_nvram_validators();
	nvram->default_validators = default_nvram_validators;

	return nvram;
}

bool ezcfg_nvram_set_backend_type(struct ezcfg_nvram *nvram, const int idx, const int type)
{
	struct ezcfg *ezcfg;

	ASSERT(nvram != NULL);
	ASSERT(idx < EZCFG_NVRAM_STORAGE_NUM);
	ASSERT(type >= 0);

	ezcfg = nvram->ezcfg;

	nvram->storage[idx].backend = type;

	return true;
}

bool ezcfg_nvram_set_coding_type(struct ezcfg_nvram *nvram, const int idx, const int type)
{
	struct ezcfg *ezcfg;

	ASSERT(nvram != NULL);
	ASSERT(idx < EZCFG_NVRAM_STORAGE_NUM);
	ASSERT(type >= 0);

	ezcfg = nvram->ezcfg;

	nvram->storage[idx].coding = type;

	return true;
}

bool ezcfg_nvram_set_storage_path(struct ezcfg_nvram *nvram, const int idx, const char *path)
{
	struct ezcfg *ezcfg;
	char *p;

	ASSERT(nvram != NULL);
	ASSERT(idx < EZCFG_NVRAM_STORAGE_NUM);
	ASSERT(path != NULL);

	ezcfg = nvram->ezcfg;

	p = strdup(path);
	if (p == NULL) {
		err(ezcfg, "can not alloc nvram storage path\n");
		return false;
	}

	if (nvram->storage[idx].path) {
		free(nvram->storage[idx].path);
	}
	nvram->storage[idx].path = p;

	return true;
}

bool ezcfg_nvram_set_total_space(struct ezcfg_nvram *nvram, const int total_space)
{
	struct ezcfg *ezcfg;
	char *buf;
	bool ret = false;

	ASSERT(nvram != NULL);

	ezcfg = nvram->ezcfg;

	/* lock nvram access */
	pthread_mutex_lock(&nvram->mutex);

	if (total_space < (int)sizeof(struct nvram_header) + nvram->used_space) {
		ret = false;
		goto func_exit;
	}

	buf = (char *)realloc(nvram->buffer, total_space);
	if (buf == NULL) {
		err(ezcfg, "can not realloc nvram buffer\n");
		ret = false;
		goto func_exit;
	}

	nvram->free_space = total_space-sizeof(struct nvram_header)-nvram->used_space;
	if (total_space > (int)sizeof(struct nvram_header)+nvram->used_space) {
		memset(buf+sizeof(struct nvram_header)+nvram->used_space, '\0', nvram->free_space);
	}
	nvram->buffer = buf;
	nvram->total_space = total_space;

func_exit:
	/* unlock nvram access */
	pthread_mutex_unlock(&nvram->mutex);

	return ret;
}

int ezcfg_nvram_get_total_space(struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;

	ASSERT(nvram != NULL);

	ezcfg = nvram->ezcfg;

	return nvram->total_space;
}

int ezcfg_nvram_get_free_space(struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;

	ASSERT(nvram != NULL);

	ezcfg = nvram->ezcfg;

	return nvram->free_space;
}

int ezcfg_nvram_get_used_space(struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;

	ASSERT(nvram != NULL);

	ezcfg = nvram->ezcfg;

	return nvram->used_space;
}

bool ezcfg_nvram_get_version_string(struct ezcfg_nvram *nvram, char *buf, size_t len)
{
	struct ezcfg *ezcfg;

	ASSERT(nvram != NULL);
	ASSERT(buf != NULL);
	ASSERT(len > 0);

	ezcfg = nvram->ezcfg;

	snprintf(buf, len, "%d.%d.%d.%d", 
	         default_version[0],
	         default_version[1],
	         default_version[2],
	         default_version[3]);

	return true;
}

bool ezcfg_nvram_get_storage_backend_string(struct ezcfg_nvram *nvram, const int idx, char *buf, size_t len)
{
	struct ezcfg *ezcfg;
	int i;

	ASSERT(nvram != NULL);
	ASSERT(idx >= 0);
	ASSERT(idx < EZCFG_NVRAM_STORAGE_NUM);
	ASSERT(buf != NULL);
	ASSERT(len > 0);

	ezcfg = nvram->ezcfg;

	i = nvram->storage[idx].backend;

	snprintf(buf, len, "%c%c%c%c", 
	         default_magics[i][0],
	         default_magics[i][1],
	         default_magics[i][2],
	         default_magics[i][3]);

	return true;
}

bool ezcfg_nvram_get_storage_coding_string(struct ezcfg_nvram *nvram, const int idx, char *buf, size_t len)
{
	struct ezcfg *ezcfg;
	int i;

	ASSERT(nvram != NULL);
	ASSERT(idx >= 0);
	ASSERT(idx < EZCFG_NVRAM_STORAGE_NUM);
	ASSERT(buf != NULL);
	ASSERT(len > 0);

	ezcfg = nvram->ezcfg;

	i = nvram->storage[idx].coding;

	snprintf(buf, len, "%c%c%c%c", 
	         default_codings[i][0],
	         default_codings[i][1],
	         default_codings[i][2],
	         default_codings[i][3]);

	return true;
}

bool ezcfg_nvram_get_storage_path_string(struct ezcfg_nvram *nvram, const int idx, char *buf, size_t len)
{
	struct ezcfg *ezcfg;

	ASSERT(nvram != NULL);
	ASSERT(idx >= 0);
	ASSERT(idx < EZCFG_NVRAM_STORAGE_NUM);
	ASSERT(buf != NULL);
	ASSERT(len > 0);

	ezcfg = nvram->ezcfg;

	buf[0] = '\0';
	if (nvram->storage[idx].path != NULL) {
		snprintf(buf, len, "%s", nvram->storage[idx].path);
	}

	return true;
}

bool ezcfg_nvram_set_default_settings(struct ezcfg_nvram *nvram, ezcfg_nv_pair_t *settings, int num_settings)
{
	nvram->default_settings = settings;
	nvram->num_default_settings = num_settings;
	return true;
}

#if 0
bool ezcfg_nvram_set_default_validators(struct ezcfg_nvram *nvram, ezcfg_nv_validator_t *validators, int num_validators)
{
	nvram->default_validators = validators;
	nvram->num_default_validators = num_validators;
	return true;
}
#endif

bool ezcfg_nvram_set_entry(struct ezcfg_nvram *nvram, const char *name, const char *value)
{
	struct ezcfg *ezcfg;
	bool ret = false;

	ASSERT(nvram != NULL);
	ASSERT(name != NULL);
	ASSERT(value != NULL);

	ezcfg = nvram->ezcfg;

	/* lock nvram access */
	pthread_mutex_lock(&nvram->mutex);

	ret = nvram_set_entry(nvram, name, value);

	/* unlock nvram access */
	pthread_mutex_unlock(&nvram->mutex);

	return ret;
}

/* It's user's duty to free the returns string */
bool ezcfg_nvram_get_entry_value(struct ezcfg_nvram *nvram, const char *name, char **value)
{
	struct ezcfg *ezcfg;
	bool ret = false;

	ASSERT(nvram != NULL);
	ASSERT(name != NULL);
	ASSERT(value != NULL);

	ezcfg = nvram->ezcfg;

	/* lock nvram access */
	pthread_mutex_lock(&nvram->mutex);

	ret = nvram_get_entry_value(nvram, name, value);

	/* unlock nvram access */
	pthread_mutex_unlock(&nvram->mutex);

	return ret;
}

bool ezcfg_nvram_get_all_entries_list(struct ezcfg_nvram *nvram, struct ezcfg_list_node *list)
{
	struct ezcfg *ezcfg;
	char *entry, *p;
	int entry_len;
	bool ret = false;

	ASSERT(nvram != NULL);
	ASSERT(list != NULL);

	ezcfg = nvram->ezcfg;

	/* lock nvram access */
	pthread_mutex_lock(&nvram->mutex);

	entry = nvram->buffer + sizeof(struct nvram_header);

	/* find all nvram entries */
	while (*entry != '\0') {
		entry_len = strlen(entry) + 1;
		p = strchr(entry, '=');
		if (p == NULL) {
			return false;
		}
		*p = '\0';
		if (ezcfg_list_entry_add(ezcfg, list, entry, p+1, 1, 0) == NULL) {
			*p = '=';
			ret = false;
			goto func_exit;
		}
		*p = '=';
		entry += entry_len;
	}
	ret = true;

func_exit:
	/* unlock nvram access */
	pthread_mutex_unlock(&nvram->mutex);

	return ret;
}

bool ezcfg_nvram_unset_entry(struct ezcfg_nvram *nvram, const char *name)
{
	bool ret = false;

	ASSERT(nvram != NULL);
	ASSERT(name != NULL);

	/* lock nvram access */
	pthread_mutex_lock(&nvram->mutex);

	ret = nvram_unset_entry(nvram, name);

	/* unlock nvram access */
	pthread_mutex_unlock(&nvram->mutex);

	return ret;
}

bool ezcfg_nvram_commit(struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;
	bool ret;
	int i;

	ASSERT(nvram != NULL);

	ezcfg = nvram->ezcfg;

	/* lock nvram access */
	pthread_mutex_lock(&nvram->mutex);

	/* check nvram entries */
	nvram_check_entries(nvram);

	/* sync nvram entries */
	nvram_sync_entries(nvram);

	for (i = 0; i < EZCFG_NVRAM_STORAGE_NUM; i++) {
		switch (nvram->storage[i].backend) {
		case EZCFG_NVRAM_BACKEND_NONE :
			info(ezcfg, "nvram in memory only, do nothing\n");
			ret = true;
			break;

		case EZCFG_NVRAM_BACKEND_FILE :
			info(ezcfg, "nvram store on file-system file\n");
			ret = nvram_commit_to_file(nvram, i);
			break;

		case EZCFG_NVRAM_BACKEND_FLASH :
			info(ezcfg, "nvram store on flash chip\n");
			ret = nvram_commit_to_flash(nvram, i);
			break;

		default:
			err(ezcfg, "unknown nvram type.\n");
			ret = false;
			break;
		}
	}

	/* unlock nvram access */
	pthread_mutex_unlock(&nvram->mutex);

	return ret;
}

bool ezcfg_nvram_fill_storage_info(struct ezcfg_nvram *nvram, const char *conf_path)
{
	char *p;
	int nvram_buffer_size;

	ASSERT(nvram != NULL);

	if (conf_path == NULL) {
		/* set default settings */
		ezcfg_nvram_set_backend_type(nvram, 0, EZCFG_NVRAM_BACKEND_FILE);
		ezcfg_nvram_set_coding_type(nvram, 0, EZCFG_NVRAM_CODING_NONE);
		ezcfg_nvram_set_storage_path(nvram, 0, EZCFG_NVRAM_STORAGE_PATH);
		ezcfg_nvram_set_total_space(nvram, EZCFG_NVRAM_BUFFER_SIZE);
		return true;
	}
	p = ezcfg_util_get_conf_string(conf_path, EZCFG_EZCFG_SECTION_NVRAM, 0, EZCFG_EZCFG_KEYWORD_BACKEND_TYPE);
	if (p == NULL) {
		ezcfg_nvram_set_backend_type(nvram, 0, EZCFG_NVRAM_BACKEND_FILE);
	}
	else {
		ezcfg_nvram_set_backend_type(nvram, 0, atoi(p));
		free(p);
	}

	p = ezcfg_util_get_conf_string(conf_path, EZCFG_EZCFG_SECTION_NVRAM, 0, EZCFG_EZCFG_KEYWORD_CODING_TYPE);
	if (p == NULL) {
		ezcfg_nvram_set_coding_type(nvram, 0, EZCFG_NVRAM_CODING_NONE);
	}
	else {
		ezcfg_nvram_set_coding_type(nvram, 0, atoi(p));
		free(p);
	}

	p = ezcfg_util_get_conf_string(conf_path, EZCFG_EZCFG_SECTION_NVRAM, 0, EZCFG_EZCFG_KEYWORD_STORAGE_PATH);
	if (p == NULL) {
		ezcfg_nvram_set_storage_path(nvram, 0, EZCFG_NVRAM_STORAGE_PATH);
	}
	else {
		ezcfg_nvram_set_storage_path(nvram, 0, p);
		free(p);
	}

	p = ezcfg_util_get_conf_string(conf_path, EZCFG_EZCFG_SECTION_NVRAM, 0, EZCFG_EZCFG_KEYWORD_BUFFER_SIZE);
	if (p == NULL) {
        	ezcfg_nvram_set_total_space(nvram, EZCFG_NVRAM_BUFFER_SIZE);
		nvram_buffer_size = EZCFG_NVRAM_BUFFER_SIZE;
	}
	else {
		nvram_buffer_size = atoi(p);
		ezcfg_nvram_set_total_space(nvram, nvram_buffer_size);
		free(p);
	}

	/* setup backup nvram storage */
#if (1 < EZCFG_NVRAM_STORAGE_NUM)
	p = ezcfg_util_get_conf_string(conf_path, EZCFG_EZCFG_SECTION_NVRAM, 1, EZCFG_EZCFG_KEYWORD_BACKEND_TYPE);
	if (p == NULL) {
		ezcfg_nvram_set_backend_type(nvram, 1, EZCFG_NVRAM_BACKEND_FILE);
	}
	else {
		ezcfg_nvram_set_backend_type(nvram, 1, atoi(p));
		free(p);
	}

	p = ezcfg_util_get_conf_string(conf_path, EZCFG_EZCFG_SECTION_NVRAM, 1, EZCFG_EZCFG_KEYWORD_CODING_TYPE);
	if (p == NULL) {
		ezcfg_nvram_set_coding_type(nvram, 1, EZCFG_NVRAM_CODING_NONE);
	}
	else {
		ezcfg_nvram_set_coding_type(nvram, 1, atoi(p));
		free(p);
	}

	p = ezcfg_util_get_conf_string(conf_path, EZCFG_EZCFG_SECTION_NVRAM, 1, EZCFG_EZCFG_KEYWORD_STORAGE_PATH);
	if (p == NULL) {
		ezcfg_nvram_set_storage_path(nvram, 1, EZCFG_NVRAM_BACKUP_STORAGE_PATH);
	}
	else {
		ezcfg_nvram_set_storage_path(nvram, 1, p);
		free(p);
	}

	p = ezcfg_util_get_conf_string(conf_path, EZCFG_EZCFG_SECTION_NVRAM, 1, EZCFG_EZCFG_KEYWORD_BUFFER_SIZE);
	if (p == NULL) {
		if (nvram_buffer_size < EZCFG_NVRAM_BUFFER_SIZE) {
			nvram_buffer_size = EZCFG_NVRAM_BUFFER_SIZE;
		}
        	ezcfg_nvram_set_total_space(nvram, nvram_buffer_size);
	}
	else {
		if (nvram_buffer_size < atoi(p)) {
			nvram_buffer_size = atoi(p);
		}
		ezcfg_nvram_set_total_space(nvram, nvram_buffer_size);
		free(p);
	}
#endif

	return true;
}

bool ezcfg_nvram_load(struct ezcfg_nvram *nvram, int flag)
{
	struct ezcfg *ezcfg;
	bool ret = false;
	int i;

	ASSERT(nvram != NULL);

	ezcfg = nvram->ezcfg;

	/* check whether the storage info set correctly */
	for (i = 0; i < EZCFG_NVRAM_STORAGE_NUM; i++) {
		if (nvram->storage[i].backend != EZCFG_NVRAM_BACKEND_NONE &&
		    nvram->storage[i].path == NULL) {
			err(ezcfg, "not setting nvram storage path.\n");
			return false;
		}
	}

	/* lock nvram access */
	pthread_mutex_lock(&nvram->mutex);

	/* break the loop when load is OK */
	for (i = 0; (ret == false) && (i < EZCFG_NVRAM_STORAGE_NUM); i++) {
		switch (nvram->storage[i].backend) {
		case EZCFG_NVRAM_BACKEND_NONE :
			ret = nvram_load_by_defaults(nvram, flag);
			break;

		case EZCFG_NVRAM_BACKEND_FILE :
			ret = nvram_load_from_file(nvram, i, flag);
			if (ret == false) {
				err(ezcfg, "nvram_load_from_file fail, use default settings.\n");
				ret = nvram_load_by_defaults(nvram, flag);
			}
			break;

		case EZCFG_NVRAM_BACKEND_FLASH :
			ret = nvram_load_from_flash(nvram, i, flag);
			if (ret == false) {
				err(ezcfg, "nvram_load_from_flash fail, use default settings.\n");
				ret = nvram_load_by_defaults(nvram, flag);
			}
			break;

		default:
			err(ezcfg, "unknown nvram type.\n");
			ret = false;
			break;
		}
	}

	/* check if restore_defaults is set */
	if ((flag == NV_INIT) && (nvram_match_entry_value(nvram,
		NVRAM_SERVICE_OPTION(SYS, RESTORE_DEFAULTS), "1") == true)) {
		struct ezcfg_link_list *list=NULL;
		char *name, *value;

		info(ezcfg, "restore system defaults.\n");

		list = ezcfg_link_list_new(ezcfg);
		if (list == NULL) {
			err(ezcfg, "restore system defaults fail.\n");
			ret = false;
			goto restore_out;
		}

		/* save the savings */
		for (i=0; i<nvram->num_default_savings; i++) {
			name = nvram->default_savings[i];
			ret = nvram_get_entry_value(nvram, name, &value);
			if (value != NULL) {
				ret = ezcfg_link_list_insert(list, name, value);
				free(value);
				if (ret == false) {
					err(ezcfg, "restore system defaults insert fail.\n");
					goto restore_out;
				}
			}
		}

		/* clean up nvram */
		nvram->used_space = 0;
		nvram->free_space = nvram->total_space-sizeof(struct nvram_header)-nvram->used_space;
		memset(nvram->buffer+sizeof(struct nvram_header)+nvram->used_space, '\0', nvram->free_space);

		/* set default settings */
		ret = nvram_load_by_defaults(nvram, flag);

		/* restore the savings */
		for(i=1; i<=ezcfg_link_list_get_length(list); i++) {
			name = ezcfg_link_list_get_node_name_by_index(list, i);
			value = ezcfg_link_list_get_node_value_by_index(list, i);
			ret = nvram_set_entry(nvram, name, value);
			if (ret == false) {
				err(ezcfg, "restore system defaults set fail.\n");
				goto restore_out;
			}
		}

restore_out:
		/* delete link list */
		if (list != NULL)
			ezcfg_link_list_delete(list);
	}

	/* check nvram entries */
	nvram_check_entries(nvram);

	/* sync nvram entries */
	nvram_sync_entries(nvram);

	/* unlock nvram access */
	pthread_mutex_unlock(&nvram->mutex);

	return ret;
}

bool ezcfg_nvram_initialize(struct ezcfg_nvram *nvram)
{
	return ezcfg_nvram_load(nvram, NV_INIT);
}

bool ezcfg_nvram_reload(struct ezcfg_nvram *nvram)
{
	return ezcfg_nvram_load(nvram, NV_RELOAD);
}

bool ezcfg_nvram_match_entry(struct ezcfg_nvram *nvram, char *name1, char *name2)
{
	struct ezcfg *ezcfg;
	bool ret = false;

	ASSERT(nvram != NULL);
	ASSERT(name1 != NULL);
	ASSERT(name2 != NULL);

	ezcfg = nvram->ezcfg;

	/* lock nvram access */
	pthread_mutex_lock(&nvram->mutex);

	ret = nvram_match_entry(nvram, name1, name2);

	/* unlock nvram access */
	pthread_mutex_unlock(&nvram->mutex);

	return ret;
}

bool ezcfg_nvram_match_entry_value(struct ezcfg_nvram *nvram, char *name, char *value)
{
	struct ezcfg *ezcfg;
	bool ret = false;

	ASSERT(nvram != NULL);
	ASSERT(name != NULL);
	ASSERT(value != NULL);

	ezcfg = nvram->ezcfg;

	/* lock nvram access */
	pthread_mutex_lock(&nvram->mutex);

	ret = nvram_match_entry_value(nvram, name, value);

	/* unlock nvram access */
	pthread_mutex_unlock(&nvram->mutex);

	return ret;
}

bool ezcfg_nvram_is_valid_entry_value(struct ezcfg_nvram *nvram, char *name, char *value)
{
	struct ezcfg *ezcfg;
	bool ret = false;

	ASSERT(nvram != NULL);
	ASSERT(name != NULL);
	ASSERT(value != NULL);

	ezcfg = nvram->ezcfg;

	/* lock nvram access */
	pthread_mutex_lock(&nvram->mutex);

	ret = ezcfg_nvram_validate_value(ezcfg, name, value);

	/* unlock nvram access */
	pthread_mutex_unlock(&nvram->mutex);

	return ret;
}

bool ezcfg_nvram_insert_socket(struct ezcfg_nvram *nvram, struct ezcfg_link_list *list)
{
	struct ezcfg *ezcfg;
	bool ret = false;
	char *name, *value;
	char *domain, *type, *protocol, *address;
	char buf[64];
	int i, list_length;

	ASSERT(nvram != NULL);
	ASSERT(list != NULL);

	ezcfg = nvram->ezcfg;
	domain = NULL;
	type = NULL;
	protocol = NULL;
	address = NULL;

	/* parse settings */
	list_length = ezcfg_link_list_get_length(list);
	for (i = 1; i < list_length+1; i++) {
		name = ezcfg_link_list_get_node_name_by_index(list, i);
		if (strcmp(name, "domain") == 0) {
			domain = ezcfg_link_list_get_node_value_by_index(list, i);
		}
		else if (strcmp(name, "type") == 0) {
			type = ezcfg_link_list_get_node_value_by_index(list, i);
		}
		else if (strcmp(name, "protocol") == 0) {
			protocol = ezcfg_link_list_get_node_value_by_index(list, i);
		}
		else if (strcmp(name, "address") == 0) {
			address = ezcfg_link_list_get_node_value_by_index(list, i);
		}
	}

	/* validate settings */
	if ((domain == NULL) || (type == NULL) ||
	    (protocol == NULL) || (address == NULL)) {
		return(false);
	}

	/* lock nvram access */
	pthread_mutex_lock(&nvram->mutex);

	/* get socket index in nvram */
	i = nvram_get_socket_index(nvram, domain, type, protocol, address);
	/* socket exist! */
	if (i > 0) {
		ret = true;
		goto func_out;
	}

	/* add the new socket */
	i = 0;
	ret = nvram_get_entry_value(nvram,
		NVRAM_SERVICE_OPTION(EZCFG, COMMON_SOCKET_NUMBER),
		&value);
	if (value != NULL) {
		i = atoi(value);
		free(value);
	}

	/* set nvram socket domain */
	snprintf(buf, sizeof(buf), "%s%s.%d.%s",
		NVRAM_PREFIX(EZCFG),
		EZCFG_EZCFG_SECTION_SOCKET, i,
		EZCFG_EZCFG_KEYWORD_DOMAIN);
	ret = nvram_set_entry(nvram, buf, domain);
	if (ret == false) {
		goto func_out;
	}

	/* set nvram socket type */
	snprintf(buf, sizeof(buf), "%s%s.%d.%s",
		NVRAM_PREFIX(EZCFG),
		EZCFG_EZCFG_SECTION_SOCKET, i,
		EZCFG_EZCFG_KEYWORD_TYPE);
	ret = nvram_set_entry(nvram, buf, type);
	if (ret == false) {
		goto func_out;
	}

	/* set nvram socket protocol */
	snprintf(buf, sizeof(buf), "%s%s.%d.%s",
		NVRAM_PREFIX(EZCFG),
		EZCFG_EZCFG_SECTION_SOCKET, i,
		EZCFG_EZCFG_KEYWORD_PROTOCOL);
	ret = nvram_set_entry(nvram, buf, protocol);
	if (ret == false) {
		goto func_out;
	}

	/* set nvram socket address */
	snprintf(buf, sizeof(buf), "%s%s.%d.%s",
		NVRAM_PREFIX(EZCFG),
		EZCFG_EZCFG_SECTION_SOCKET, i,
		EZCFG_EZCFG_KEYWORD_ADDRESS);
	ret = nvram_set_entry(nvram, buf, address);
	if (ret == false) {
		goto func_out;
	}

	/* update socket number */
	i++;
	snprintf(buf, sizeof(buf), "%d", i);
	ret = nvram_set_entry(nvram,
		NVRAM_SERVICE_OPTION(EZCFG, COMMON_SOCKET_NUMBER),
		buf);

func_out:
	/* unlock nvram access */
	pthread_mutex_unlock(&nvram->mutex);

	return ret;
}

bool ezcfg_nvram_remove_socket(struct ezcfg_nvram *nvram, struct ezcfg_link_list *list)
{
	struct ezcfg *ezcfg;
	bool ret = false;
	char *name;
	char *domain, *type, *protocol, *address;
	int i, list_length;

	ASSERT(nvram != NULL);
	ASSERT(list != NULL);

	ezcfg = nvram->ezcfg;
	domain = NULL;
	type = NULL;
	protocol = NULL;
	address = NULL;

	/* parse settings */
	list_length = ezcfg_link_list_get_length(list);
	for (i = 1; i < list_length+1; i++) {
		name = ezcfg_link_list_get_node_name_by_index(list, i);
		if (strcmp(name, "domain") == 0) {
			domain = ezcfg_link_list_get_node_value_by_index(list, i);
		}
		else if (strcmp(name, "type") == 0) {
			type = ezcfg_link_list_get_node_value_by_index(list, i);
		}
		else if (strcmp(name, "protocol") == 0) {
			protocol = ezcfg_link_list_get_node_value_by_index(list, i);
		}
		else if (strcmp(name, "address") == 0) {
			address = ezcfg_link_list_get_node_value_by_index(list, i);
		}
	}

	/* validate settings */
	if ((domain == NULL) || (type == NULL) ||
	    (protocol == NULL) || (address == NULL)) {
		return(false);
	}

	/* lock nvram access */
	pthread_mutex_lock(&nvram->mutex);

	/* get socket index in nvram */
	i = nvram_get_socket_index(nvram, domain, type, protocol, address);
	/* socket does not exist! */
	if (i < 1) {
		ret = true;
		goto func_out;
	}

	/* remove the i-th socket */
	ret = nvram_remove_socket_by_index(nvram, i);

func_out:
	/* unlock nvram access */
	pthread_mutex_unlock(&nvram->mutex);

	return ret;
}

bool ezcfg_nvram_set_multi_entries(struct ezcfg_nvram *nvram, struct ezcfg_link_list *list)
{
	struct ezcfg *ezcfg;
	bool ret = false;
	char *name, *value;
	int i, list_length;

	ASSERT(nvram != NULL);
	ASSERT(list != NULL);

	ezcfg = nvram->ezcfg;

	/* lock nvram access */
	pthread_mutex_lock(&nvram->mutex);

	/* parse settings */
	list_length = ezcfg_link_list_get_length(list);
	for (i = 1; i < list_length+1; i++) {
		name = ezcfg_link_list_get_node_name_by_index(list, i);
		value = ezcfg_link_list_get_node_value_by_index(list, i);
		if ((name == NULL) || (value == NULL)) {
			ret = false;
			goto func_out;
		}
		ret = nvram_set_entry(nvram, name, value);
		if (ret == false) {
			goto func_out;
		}
	}

func_out:
	/* unlock nvram access */
	pthread_mutex_unlock(&nvram->mutex);

	return ret;
}

bool ezcfg_nvram_unset_multi_entries(struct ezcfg_nvram *nvram, struct ezcfg_link_list *list)
{
	struct ezcfg *ezcfg;
	bool ret = false;
	char *name;
	int i, list_length;

	ASSERT(nvram != NULL);
	ASSERT(list != NULL);

	ezcfg = nvram->ezcfg;

	/* lock nvram access */
	pthread_mutex_lock(&nvram->mutex);

	/* parse settings */
	list_length = ezcfg_link_list_get_length(list);
	for (i = 1; i < list_length+1; i++) {
		name = ezcfg_link_list_get_node_name_by_index(list, i);
		if (name == NULL) {
			ret = false;
			goto func_out;
		}
		ret = nvram_unset_entry(nvram, name);
		if (ret == false) {
			goto func_out;
		}
	}

func_out:
	/* unlock nvram access */
	pthread_mutex_unlock(&nvram->mutex);

	return ret;
}
