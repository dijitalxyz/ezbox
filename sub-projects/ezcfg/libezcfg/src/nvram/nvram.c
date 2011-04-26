/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : nvram/nvram.c
 *
 * Description  : implement Non-Volatile RAM
 * Warning      : must exclusively use it, say lock NVRAM before using it.
 *
 * Copyright (C) 2010 by ezbox-project
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

	/* default unsets */
	int num_default_unsets;
	char **default_unsets;

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

static bool nvram_match_entry_value(struct ezcfg_nvram *nvram, const char *name1, char *name2)
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

static bool nvram_init_by_defaults(struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;
	ezcfg_nv_pair_t *nvp;
	int i;

	ezcfg = nvram->ezcfg;

	/* clean up runtime generated nvram values */
	for (i=0; i<nvram->num_default_unsets; i++) {
		nvram_unset_entry(nvram, nvram->default_unsets[i]);
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

static bool nvram_init_from_file(struct ezcfg_nvram *nvram, const int index)
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

	fp = fopen(nvram->storage[index].path, "r");
	if (fp == NULL) {
		err(ezcfg, "can't open file for nvram init\n");
		return false;
	}

	if (fread(nvram->buffer, nvram->total_space, 1, fp) < 1) {
		err(ezcfg, "read nvram buffer error\n");
		goto init_exit;
	}

	header = (struct nvram_header *)nvram->buffer;
	if ((nvram->total_space - header->size) != 0) {
		err(ezcfg, "nvram size is not matched\n");
		goto init_exit;
	}

	data = nvram->buffer + sizeof(struct nvram_header);

	/* do CRC-32 calculation */
	crc = ezcfg_util_crc32((unsigned char *)data, nvram->total_space - sizeof(struct nvram_header));
	if (header->crc != crc) {
		err(ezcfg, "nvram crc is error.\n");
		goto init_exit;
	}

	/* fill nvram space info */
	nvram->used_space = header->used;
	nvram->free_space = nvram->total_space - sizeof(struct nvram_header) - nvram->used_space;

	/* clean up runtime generated nvram values */
	for (i=0; i<nvram->num_default_unsets; i++) {
		nvram_unset_entry(nvram, nvram->default_unsets[i]);
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
					goto init_exit;
				}
			}
		}
		else {
			free(value);
		}
	}

	ret = true;

init_exit:
	if (fp != NULL) {
		fclose(fp);
	}

	return ret;
}

static bool nvram_init_from_flash(struct ezcfg_nvram *nvram, const int index)
{
	return true;
}

static void generate_nvram_header(struct ezcfg_nvram *nvram, const int index)
{
	struct nvram_header *header;
	char *data;
	int i;

	if (index >= EZCFG_NVRAM_STORAGE_NUM)
		return;

	header = (struct nvram_header *)nvram->buffer;
	data = nvram->buffer + sizeof(struct nvram_header);

	for (i=0; i<4; i++) {
		header->magic[i] = default_magics[nvram->storage[index].backend][i];
	}
	for (i=0; i<4; i++) {
		header->coding[i] = default_codings[nvram->storage[index].coding][i];
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

static bool nvram_commit_to_file(struct ezcfg_nvram *nvram, const int index)
{
	struct ezcfg *ezcfg;
	FILE *fp = NULL;
	bool ret = false;

	ezcfg = nvram->ezcfg;

	/* generate nvram header info */
	generate_nvram_header(nvram, index);

	fp = fopen(nvram->storage[index].path, "w");
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

static bool nvram_commit_to_flash(struct ezcfg_nvram *nvram, const int index)
{
	return true;
}

static void sync_ui_settings(struct ezcfg_nvram *nvram)
{
	char *p;

	/* ui_tz_area */
	if (nvram_match_entry_value(nvram, NVRAM_SERVICE_OPTION(UI, TZ_AREA), NVRAM_SERVICE_OPTION(SYS, TZ_AREA)) == false) {
		nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(SYS, TZ_AREA), &p);
		if (p != NULL) {
			nvram_set_entry(nvram, NVRAM_SERVICE_OPTION(UI, TZ_AREA), p);
			free(p);
		}
	}

	/* ui_tz_location */
	if (nvram_match_entry_value(nvram, NVRAM_SERVICE_OPTION(UI, TZ_LOCATION), NVRAM_SERVICE_OPTION(SYS, TZ_LOCATION)) == false) {
		nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(SYS, TZ_LOCATION), &p);
		if (p != NULL) {
			nvram_set_entry(nvram, NVRAM_SERVICE_OPTION(UI, TZ_LOCATION), p);
			free(p);
		}
	}
}

static void nvram_sync_entries(struct ezcfg_nvram *nvram)
{
	/* sync UI settings */
	sync_ui_settings(nvram);
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

	nvram = (struct ezcfg_nvram *)calloc(1, sizeof(struct ezcfg_nvram));
	if (nvram == NULL) {
		err(ezcfg, "can not calloc nvram\n");
		return NULL;
	}

	memset(nvram, 0, sizeof(struct ezcfg_nvram));

	nvram->ezcfg = ezcfg;

	/* initialize nvram mutex */
	pthread_mutex_init(&nvram->mutex, NULL);

	/* set default settings */
	nvram->num_default_settings = ezcfg_nvram_get_num_default_nvram_settings();
	nvram->default_settings = default_nvram_settings;

	/* set default unsets */
	nvram->num_default_unsets = ezcfg_nvram_get_num_default_nvram_unsets();
	nvram->default_unsets = default_nvram_unsets;

	/* set default validators */
	nvram->num_default_validators = ezcfg_nvram_get_num_default_nvram_validators();
	nvram->default_validators = default_nvram_validators;

	return nvram;
}

bool ezcfg_nvram_set_backend_type(struct ezcfg_nvram *nvram, const int index, const int type)
{
	struct ezcfg *ezcfg;

	ASSERT(nvram != NULL);
	ASSERT(index < EZCFG_NVRAM_STORAGE_NUM);
	ASSERT(type >= 0);

	ezcfg = nvram->ezcfg;

	nvram->storage[index].backend = type;

	return true;
}

bool ezcfg_nvram_set_coding_type(struct ezcfg_nvram *nvram, const int index, const int type)
{
	struct ezcfg *ezcfg;

	ASSERT(nvram != NULL);
	ASSERT(index < EZCFG_NVRAM_STORAGE_NUM);
	ASSERT(type >= 0);

	ezcfg = nvram->ezcfg;

	nvram->storage[index].coding = type;

	return true;
}

bool ezcfg_nvram_set_storage_path(struct ezcfg_nvram *nvram, const int index, const char *path)
{
	struct ezcfg *ezcfg;
	char *p;

	ASSERT(nvram != NULL);
	ASSERT(index < EZCFG_NVRAM_STORAGE_NUM);
	ASSERT(path != NULL);

	ezcfg = nvram->ezcfg;

	p = strdup(path);
	if (p == NULL) {
		err(ezcfg, "can not calloc nvram storage path\n");
		return false;
	}

	if (nvram->storage[index].path) {
		free(nvram->storage[index].path);
	}
	nvram->storage[index].path = p;

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

	if (total_space < sizeof(struct nvram_header) + nvram->used_space) {
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
	if (total_space > sizeof(struct nvram_header)+nvram->used_space) {
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

bool ezcfg_nvram_get_storage_backend_string(struct ezcfg_nvram *nvram, const int index, char *buf, size_t len)
{
	struct ezcfg *ezcfg;
	int i;

	ASSERT(nvram != NULL);
	ASSERT(index >= 0);
	ASSERT(index < EZCFG_NVRAM_STORAGE_NUM);
	ASSERT(buf != NULL);
	ASSERT(len > 0);

	ezcfg = nvram->ezcfg;

	i = nvram->storage[index].backend;

	snprintf(buf, len, "%c%c%c%c", 
	         default_magics[i][0],
	         default_magics[i][1],
	         default_magics[i][2],
	         default_magics[i][3]);

	return true;
}

bool ezcfg_nvram_get_storage_coding_string(struct ezcfg_nvram *nvram, const int index, char *buf, size_t len)
{
	struct ezcfg *ezcfg;
	int i;

	ASSERT(nvram != NULL);
	ASSERT(index >= 0);
	ASSERT(index < EZCFG_NVRAM_STORAGE_NUM);
	ASSERT(buf != NULL);
	ASSERT(len > 0);

	ezcfg = nvram->ezcfg;

	i = nvram->storage[index].coding;

	snprintf(buf, len, "%c%c%c%c", 
	         default_codings[i][0],
	         default_codings[i][1],
	         default_codings[i][2],
	         default_codings[i][3]);

	return true;
}

bool ezcfg_nvram_get_storage_path_string(struct ezcfg_nvram *nvram, const int index, char *buf, size_t len)
{
	struct ezcfg *ezcfg;

	ASSERT(nvram != NULL);
	ASSERT(index >= 0);
	ASSERT(index < EZCFG_NVRAM_STORAGE_NUM);
	ASSERT(buf != NULL);
	ASSERT(len > 0);

	ezcfg = nvram->ezcfg;

	buf[0] = '\0';
	if (nvram->storage[index].path != NULL) {
		snprintf(buf, len, "%s", nvram->storage[index].path);
	}

	return true;
}

bool ezcfg_nvram_set_default_settings(struct ezcfg_nvram *nvram, ezcfg_nv_pair_t *settings, int num_settings)
{
	nvram->default_settings = settings;
	nvram->num_default_settings = num_settings;
	return true;
}

bool ezcfg_nvram_set_default_validators(struct ezcfg_nvram *nvram, ezcfg_nv_validator_t *validators, int num_validators)
{
	nvram->default_validators = validators;
	nvram->num_default_validators = num_validators;
	return true;
}

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

bool ezcfg_nvram_initialize(struct ezcfg_nvram *nvram)
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

	/* break the loop when init is OK */
	for (i = 0; (ret == false) && (i < EZCFG_NVRAM_STORAGE_NUM); i++) {
		switch (nvram->storage[i].backend) {
		case EZCFG_NVRAM_BACKEND_NONE :
			ret = nvram_init_by_defaults(nvram);
			break;

		case EZCFG_NVRAM_BACKEND_FILE :
			ret = nvram_init_from_file(nvram, i);
			if (ret == false) {
				err(ezcfg, "nvram_init_from_file fail, use default settings.\n");
				ret = nvram_init_by_defaults(nvram);
			}
			break;

		case EZCFG_NVRAM_BACKEND_FLASH :
			ret = nvram_init_from_flash(nvram, i);
			if (ret == false) {
				err(ezcfg, "nvram_init_from_flash fail, use default settings.\n");
				ret = nvram_init_by_defaults(nvram);
			}
			break;

		default:
			err(ezcfg, "unknown nvram type.\n");
			ret = false;
			break;
		}
	}

	/* sync nvram entries */
	nvram_sync_entries(nvram);

	/* unlock nvram access */
	pthread_mutex_unlock(&nvram->mutex);

	return ret;
}

bool ezcfg_nvram_match_entry_value(struct ezcfg_nvram *nvram, char *name1, char *name2)
{
	struct ezcfg *ezcfg;
	bool ret = false;

	ASSERT(nvram != NULL);
	ASSERT(name1 != NULL);
	ASSERT(name2 != NULL);

	ezcfg = nvram->ezcfg;

	/* lock nvram access */
	pthread_mutex_lock(&nvram->mutex);

	ret = nvram_match_entry_value(nvram, name1, name2);

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

