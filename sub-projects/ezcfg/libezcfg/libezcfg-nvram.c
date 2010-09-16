/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg-nvram.c
 *
 * Description  : implement Non-Volatile RAM
 * Warning      : must exclusively use it, say lock NVRAM before using it.
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-08-20   0.1       Write it from scratch
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

#define NVRAM_VERSOIN_MAJOR 0x01 /* version[0] */
#define NVRAM_VERSOIN_MINOR 0x00 /* version[1] */
#define NVRAM_VERSOIN_MICRO 0x00 /* version[2] */
#define NVRAM_VERSOIN_REV   0x01 /* version[3] */ 

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

struct ezcfg_nvram_pair {
	char *name;
	char *value;
};

static struct ezcfg_nvram_pair default_nvram_settings[] = {
	{ "mytest", "ok" },
	{ "mytest2", "ok2" },
};

/* same data type as nvram_node->len!!!
 * free[0] -> 2^4=16 bytes;
 * free[1] -> free[0]*4 = 16*4 = 64 bytes;
 * free[2] -> free[1]*4 = 64*4 = 256 bytes;
 * free[3] -> free[2]*4 = 256*4 = 1024 bytes;
 * free[4] -> free[3]*2 = 1024*2 = 2k bytes;
 * free[5] -> free[4]*2 = 2k*2 = 4k bytes;
 * free[6] -> free[5]*2 = 4k*2 = 8k bytes;
 * free[7] -> free[6]*2 = 8k*2 = 16k bytes;
 */
static unsigned short default_block_sizes[8] = {
	16,
	64,
	256,
	1024,
	2  * 1024,
	4  * 1024,
	8  * 1024,
	16 * 1024,	
};

struct nvram_header {
	unsigned char magic[4];
	unsigned char version[4];
	unsigned long length;
	unsigned long checksum;
};

struct nvram_node {
	unsigned short len; /* max size is 64k */
	                    /* len = strlen(name) + strlen(value) + 2 */
	unsigned short flags;
	char *name;
	char *value;
	struct nvram_node *next;
};

struct nvram_pool {
	int count;
	struct nvram_node *head;
};

struct ezcfg_nvram {
	struct ezcfg *ezcfg;
	struct nvram_header header;

	/* storage backend type, mapping to nvram header->magic
	 * 0 -> NONE : in memory only
	 * 1 -> FILE : store on file-system a file
	 * 2 -> FLSH : store on flash chip
	 */
	int backend_type;

	/* nvram storage device/file path */
	char *store_path;

	/* default settings */
	int num_default_settings;
	struct ezcfg_nvram_pair *default_settings;

	/* total_space = sizeof(header) + free_space + used_space */
	int total_space; /* storage space for nvram */
	int free_space; /* unused space */
	int used_space; /* user used space */

	int num_block_size; /* block pool length */
	unsigned short *block_sizes;
	struct nvram_pool *free_nodes;
	struct nvram_pool *used_nodes;
};

/*
 * Private functions
 */
static bool nvram_init_by_defaults(struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;
	struct ezcfg_nvram_pair *nvp;
	int i;

	ezcfg = nvram->ezcfg;

	for (i=0; i<nvram->num_default_settings; i++) {
		nvp = &nvram->default_settings[i];
		info(ezcfg, "name=[%s] value=[%s]\n", nvp->name, nvp->value);
		if (ezcfg_nvram_set_node_value(nvram, nvp->name, nvp->value) == false) {
			err(ezcfg, "set nvram error.\n");
			return false;
		}
	}
	return true;
}

static bool nvram_init_from_file(struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;
	struct nvram_header *ph;
	struct ezcfg_nvram_pair *nvp;
	FILE *fp;
	char *buf, *p, *q;
	char *name, *value;
	int len, i;
	bool ret;

	ezcfg = nvram->ezcfg;
	ph = &nvram->header;
	ret = true;
	fp = NULL;
	buf = NULL;

	fp = fopen(nvram->store_path, "r");
	if (fp == NULL) {
		err(ezcfg, "can't open file for nvram init\n");
		return false;
	}

	if (fread(ph, sizeof(struct nvram_header), 1, fp) < 1) {
		err(ezcfg, "read nvram header error\n");
		ret = false;
		goto init_exit;
	}

	buf = calloc(ph->length, sizeof(char));
	if (buf == NULL) {
		err(ezcfg, "can't calloc for nvram_init_from_file\n");
		ret = false;
		goto init_exit;
	}
	
	if (fread(buf, sizeof(char), ph->length, fp) < ph->length) {
		err(ezcfg, "read nvram info from file error\n");
		ret = false;
		goto init_exit;
	}

	/* do checksum calculate */

	/* setup nvram node info */
	p = buf;
	while (p < buf+len) {
		len = strlen(p);
		q = strchr(p, '=');
		if (q == NULL) {
			err(ezcfg, "nvram pair file store format error\n");
			ret = false;
			goto init_exit;
		}
		*q = '\0';
		name = p;
		value = q+1;
		info(ezcfg, "name=[%s] value=[%s]\n", name, value);
		if (ezcfg_nvram_set_node_value(nvram, name, value) == false) {
			err(ezcfg, "set nvram error.\n");
			ret = false;
			goto init_exit;
		}
		p += (len+1);
	}

	/* fill missing critic nvram with default settings */
	for (i=0; i<nvram->num_default_settings; i++) {
		nvp = &nvram->default_settings[i];
		value = ezcfg_nvram_get_node_value(nvram, nvp->name);
		info(ezcfg, "name=[%s] value=[%s] default value=[%s]\n", nvp->name, value, nvp->value);
		if (value == NULL) {  
			if (ezcfg_nvram_set_node_value(nvram, nvp->name, nvp->value) == false) {
				err(ezcfg, "set nvram error.\n");
				ret = false;
				goto init_exit;
			}
		}
		else {
			free(value);
		}
	}

	ret = true;

init_exit:
	if (buf != NULL) {
		free(buf);
	}

	if (fp != NULL) {
		fclose(fp);
	}

	return ret;
}

static bool nvram_init_from_flash(struct ezcfg_nvram *nvram)
{
	return true;
}

static void build_nvram_buffer(struct ezcfg_nvram *nvram, char *buf, int len)
{
	struct ezcfg *ezcfg;
	struct nvram_pool *pl_used;
	struct nvram_node *node;
	char *p;
	int i, n;

	ASSERT(nvram != NULL);
	ASSERT(buf != NULL);

	ezcfg = nvram->ezcfg;

	p = buf;

	/* find all nvram nodes */
	for (i = 0; i < nvram->num_block_size; i++) {
		pl_used = &nvram->used_nodes[i];
		node = pl_used->head;
		while (node != NULL) {
			n = sprintf(p, "%s=%s", node->name, node->value);
			p += n+1; /* one more char for \0-terminated string */
			node = node->next;
		}
	}
}

static void generate_nvram_header(struct ezcfg_nvram *nvram, char *buf, int len)
{
	struct nvram_header *ph;
	int i;

	ph = &nvram->header;
	for (i=0; i<4; i++) {
		ph->magic[i] = default_magics[nvram->backend_type][i];
	}
	for (i=0; i<4; i++) {
		ph->version[i] = default_version[i];
	}
	ph->length = nvram->used_space;
	ph->checksum = 0;
}

static void nvram_header_copy(struct nvram_header *dest, const struct nvram_header *src)
{
	memcpy(dest, src, sizeof(struct nvram_header));
}

static bool nvram_commit_to_file(struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;
	struct nvram_header *ph;
	FILE *fp;
	char *buf, *p;
	size_t len;
	bool ret;

	ezcfg = nvram->ezcfg;
	ret = true;
	buf = NULL;
	fp = NULL;

	len = sizeof(struct nvram_header) + nvram->used_space;
	buf = calloc(len, sizeof(char));
	if (buf == NULL) {
		err(ezcfg, "can't calloc for nvram_commit_to_file\n");
		return false;
	}
	
	/* skip struct nvram_header */
	ph = (struct nvram_header *)buf;
	p = buf + sizeof(struct nvram_header);

	/* build nvram pair buffer represent */
	build_nvram_buffer(nvram, p, nvram->used_space);
	
	/* generate nvram header info */
	generate_nvram_header(nvram, p, nvram->used_space);

	/* sync nvram header info to buf */
	nvram_header_copy(ph, &nvram->header);

	fp = fopen(nvram->store_path, "w");
	if (fp == NULL) {
		err(ezcfg, "can't open file for nvram commit\n");
		ret = false;
		goto commit_exit;
	}

	if (fwrite(buf, sizeof(char), len, fp) < len) {
		err(ezcfg, "write nvram to file error\n");
		ret = false;
		goto commit_exit;
	}

	ret = true;

commit_exit:
	if (buf != NULL) {
		free(buf);
	}

	if (fp != NULL) {
		fclose(fp);
	}

	return ret;
}

static bool nvram_commit_to_flash(struct ezcfg_nvram *nvram)
{
	return true;
}

/*
 * Public functions
 */

bool ezcfg_nvram_delete(struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;
	struct nvram_pool *pl;
	struct nvram_node *node;
	int i;

	ASSERT(nvram != NULL);

	ezcfg = nvram->ezcfg;

	if (nvram->store_path != NULL) {
		free(nvram->store_path);
	}

	if (nvram->free_nodes != NULL) {
		for(i = 0; i < nvram->num_block_size; i++) {
			pl = &nvram->free_nodes[i];
			node = pl->head;
			while (node != NULL) {
				pl->head = node->next;
				free(node->name);
				free(node);
				node = pl->head;
			}
		}
		free(nvram->free_nodes);
	}

	if (nvram->used_nodes != NULL) {
		for(i = 0; i < nvram->num_block_size; i++) {
			pl = &nvram->used_nodes[i];
			node = pl->head;
			while (node != NULL) {
				pl->head = node->next;
				free(node->name);
				free(node);
				node = pl->head;
			}
		}
		free(nvram->used_nodes);
	}

	free(nvram);
	return true;
}

struct ezcfg_nvram *ezcfg_nvram_new(struct ezcfg *ezcfg)
{
	struct ezcfg_nvram *nvram;
	struct nvram_pool *free_nodes;
	struct nvram_pool *used_nodes;

	ASSERT(ezcfg != NULL);

	nvram = calloc(1, sizeof(struct ezcfg_nvram));
	if (nvram == NULL) {
		err(ezcfg, "can not calloc nvram\n");
		return NULL;
	}

	memset(nvram, 0, sizeof(struct ezcfg_nvram));
	nvram->num_block_size = ARRAY_SIZE(default_block_sizes);
	nvram->block_sizes = default_block_sizes;

	free_nodes = calloc(nvram->num_block_size, sizeof(struct nvram_pool));
	if (free_nodes == NULL) {
		err(ezcfg, "can not calloc nvram free_nodes pool\n");
		free(nvram);
		return NULL;
	}

	used_nodes = calloc(nvram->num_block_size, sizeof(struct nvram_pool));
	if (used_nodes == NULL) {
		err(ezcfg, "can not calloc nvram free_nodes pool\n");
		free(nvram);
		free(free_nodes);
		return NULL;
	}

	memset(free_nodes, 0, sizeof(struct nvram_pool) * nvram->num_block_size);
	memset(used_nodes, 0, sizeof(struct nvram_pool) * nvram->num_block_size);

	nvram->ezcfg = ezcfg;
	nvram->free_nodes = free_nodes;
	nvram->used_nodes = used_nodes;

	/* set default settings */
	nvram->num_default_settings = ARRAY_SIZE(default_nvram_settings);
	nvram->default_settings = default_nvram_settings;

	return nvram;
}

bool ezcfg_nvram_set_backend_type(struct ezcfg_nvram *nvram, const int type)
{
	struct ezcfg *ezcfg;

	ASSERT(nvram != NULL);
	ASSERT(type >= 0);

	ezcfg = nvram->ezcfg;

	nvram->backend_type = type;

	return true;
}

bool ezcfg_nvram_set_store_path(struct ezcfg_nvram *nvram, const char *path)
{
	struct ezcfg *ezcfg;
	char *p;

	ASSERT(nvram != NULL);
	ASSERT(path != NULL);

	ezcfg = nvram->ezcfg;

	p = strdup(path);
	if (p == NULL) {
		err(ezcfg, "can not calloc nvram store path\n");
		return false;
	}

	if (nvram->store_path) {
		free(nvram->store_path);
	}
	nvram->store_path = p;

	return true;
}

char *ezcfg_nvram_get_store_path(struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;

	ASSERT(nvram != NULL);

	ezcfg = nvram->ezcfg;

	return nvram->store_path;
}

bool ezcfg_nvram_set_total_space(struct ezcfg_nvram *nvram, const int total_space)
{
	struct ezcfg *ezcfg;

	ASSERT(nvram != NULL);

	ezcfg = nvram->ezcfg;

	if (total_space < nvram->used_space) {
		return false;
	}

	nvram->total_space = total_space;
	nvram->free_space = total_space - sizeof(struct nvram_header) - nvram->used_space;
	return true;
}

int ezcfg_nvram_get_total_space(struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;

	ASSERT(nvram != NULL);

	ezcfg = nvram->ezcfg;

	return nvram->total_space;
}

bool ezcfg_nvram_set_default_settings(struct ezcfg_nvram *nvram, struct ezcfg_nvram_pair *settings, int num_settings)
{
	nvram->default_settings = settings;
	nvram->num_default_settings = num_settings;
	return true;
}

bool ezcfg_nvram_set_node_value(struct ezcfg_nvram *nvram, const char *name, const char *value)
{
	struct ezcfg *ezcfg;
	struct nvram_pool *pl_used, *pl_free, *pl_used_new, *pl_free_new;
	struct nvram_node *pre_node=NULL, *cur_node=NULL, *new_node=NULL;
	int i, j;
	int name_len, value_len;

	ASSERT(nvram != NULL);
	ASSERT(name != NULL);
	ASSERT(value != NULL);

	ezcfg = nvram->ezcfg;

	name_len = strlen(name);
	value_len = strlen(value);

	if (name_len + value_len + 2 > nvram->block_sizes[nvram->num_block_size - 1]) {
		err(ezcfg, "too large name or value for nvram set\n");
		return false;
	}

	/* find nvram node */
	for (i = 0; i < nvram->num_block_size; i++) {
		pl_used = &nvram->used_nodes[i];
		pl_free = &nvram->free_nodes[i];
		pre_node=NULL;
		cur_node = pl_used->head;
		while (cur_node != NULL && strcmp(cur_node->name, name) != 0) {
			pre_node = cur_node;
			cur_node = pre_node->next;
		}
		/* we found nvram node */
		if (cur_node != NULL) {
			/* first check if the space is enough */
			if (nvram->free_space < (name_len + value_len + 2 - (int)cur_node->len)) {
				err(ezcfg, "no enough space for nvram set\n");
				return false;
			}

			/* first clean original info */
			nvram->used_space -= cur_node->len;
			nvram->free_space += cur_node->len;

			/* block has enough space for nvram node */
			if (name_len + value_len + 2 <= (int)nvram->block_sizes[i]) {
				sprintf(cur_node->value, "%s", value);
				cur_node->len = (name_len + value_len + 2);
				nvram->used_space += cur_node->len;
				nvram->free_space -= cur_node->len;
				return true;
			}
			/* block has not enough space for nvram node */
			/* find a free nvram node */
			j = i+1;
			while (name_len + value_len + 2 > (int)nvram->block_sizes[j]) {
				j++;
			}
			pl_free_new = &nvram->free_nodes[j];
			pl_used_new = &nvram->used_nodes[j];

			/* free pool has node */
			if (pl_free_new->count > 0) {
				/* put original nvram node to free pool */
				if (pre_node == NULL) {
					pl_used->head = cur_node->next;
				}
				else {
					pre_node->next = cur_node->next;
				}
				cur_node->next = pl_free->head;
				pl_free->head = cur_node;
				pl_free->count++;
				nvram->used_space -= cur_node->len;
				nvram->free_space += cur_node->len;

				/* set new nvram node */
				cur_node = pl_free_new->head;
				pl_free_new->head = cur_node->next;
				pl_free_new->count--;
				sprintf(cur_node->name, "%s", name);
				cur_node->value = cur_node->name + name_len + 1;
				sprintf(cur_node->value, "%s", value);
				cur_node->len = (name_len + value_len + 2);

				/* put nvram node to used pool */
				cur_node->next = pl_used_new->head;
				pl_used_new->head = cur_node;
				pl_used_new->count++;
				nvram->used_space += cur_node->len;
				nvram->free_space -= cur_node->len;
				return true;
			}
			/* free pool no node, create one and put it to used pool */
			else {
				new_node = calloc(1, sizeof(struct nvram_node));
				if (new_node == NULL) {
					err(ezcfg, "no enough memory for nvram set new_node.\n");
					return false;
				}
				new_node->name = calloc(nvram->block_sizes[j], sizeof(char));
				if (new_node->name == NULL) {
					err(ezcfg, "no enough memory for nvram set new_node name.\n");
					free(new_node);
					return false;
				}
				/* set new node info */
				sprintf(new_node->name, "%s", name);
				new_node->value = new_node->name + name_len + 1;
				sprintf(new_node->value, "%s", value);
				new_node->len = name_len + value_len + 2;

				/* put original nvram node to free pool */
				if (pre_node == NULL) {
					pl_used->head = cur_node->next;
				}
				else {
					pre_node->next = cur_node->next;
				}
				cur_node->next = pl_free->head;
				pl_free->head = cur_node;
				pl_free->count++;
				nvram->used_space -= cur_node->len;
				nvram->free_space += cur_node->len;

				/* put new node to used pool */
				new_node->next = pl_used_new->head;
				pl_used_new->head = new_node;
				pl_used_new->count++;
				nvram->used_space += new_node->len;
				nvram->free_space -= new_node->len;
				return true;
			}
		}
	}

	/* not find nvram node */
	/* find the fit block pool index */
	j = 0;
	while (name_len + value_len + 2 > (int)nvram->block_sizes[j]) {
		j++;
	}
	pl_free_new = &nvram->free_nodes[j];
	pl_used_new = &nvram->used_nodes[j];

	new_node = calloc(1, sizeof(struct nvram_node));
	if (new_node == NULL) {
		err(ezcfg, "no enough memory for nvram set new_node.\n");
		return false;
	}
	new_node->name = calloc(nvram->block_sizes[j], sizeof(char));
	if (new_node->name == NULL) {
		err(ezcfg, "no enough memory for nvram set new_node name.\n");
		free(new_node);
		return false;
	}
	/* set new node info */
	sprintf(new_node->name, "%s", name);
	new_node->value = new_node->name + name_len + 1;
	sprintf(new_node->value, "%s", value);
	new_node->len = name_len + value_len + 2;

	/* put it to used pool */
	new_node->next = pl_used_new->head;
	pl_used_new->head = new_node;
	pl_used_new->count++;
	nvram->used_space += new_node->len;
	nvram->free_space -= new_node->len;
	return true;
}

char *ezcfg_nvram_get_node_value(struct ezcfg_nvram *nvram, const char *name)
{
	struct ezcfg *ezcfg;
	struct nvram_pool *pl_used;
	struct nvram_node *node=NULL;
	char *value;
	int i, name_len;

	ASSERT(nvram != NULL);
	ASSERT(name != NULL);

	ezcfg = nvram->ezcfg;

	name_len = strlen(name);

	/* find nvram node */
	for (i = 0; i < nvram->num_block_size; i++) {
		pl_used = &nvram->used_nodes[i];
		node = pl_used->head;
		while (node != NULL && strcmp(node->name, name) != 0) {
			node = node->next;
		}
		/* we found nvram node */
		if (node != NULL) {
			value = calloc(node->len - name_len - 1, sizeof(char));
			if (value == NULL) {
				err(ezcfg, "not enough memory for get nvram node.\n");
				return NULL;
			}
			sprintf(value, "%s", node->value);
			return value;
		}
	}

	/* not found nvram node */
	return NULL;
}

bool ezcfg_nvram_get_all_nodes_list(const struct ezcfg_nvram *nvram, struct ezcfg_list_node *list)
{
	struct ezcfg *ezcfg;
	struct ezcfg_list_entry *entry;
	struct nvram_pool *pl_used;
	struct nvram_node *node;
	int i;

	ASSERT(nvram != NULL);
	ASSERT(list != NULL);

	ezcfg = nvram->ezcfg;

	/* find all nvram nodes */
	for (i = 0; i < nvram->num_block_size; i++) {
		pl_used = &nvram->used_nodes[i];
		node = pl_used->head;
		while (node != NULL) {
			entry = ezcfg_list_entry_add(ezcfg, list, node->name, node->value, 1, 0);
			if (entry == NULL) {
				return false;
			}
			node = node->next;
		}
	}
	return true;
}

bool ezcfg_nvram_unset_node_value(struct ezcfg_nvram *nvram, const char *name)
{
	struct ezcfg *ezcfg;
	struct nvram_pool *pl_used, *pl_free;
	struct nvram_node *pre_node=NULL, *cur_node=NULL;
	int i;

	ASSERT(nvram != NULL);
	ASSERT(name != NULL);

	ezcfg = nvram->ezcfg;

	/* find nvram node */
	for (i = 0; i < nvram->num_block_size; i++) {
		pl_used = &nvram->used_nodes[i];
		pre_node=NULL;
		cur_node = pl_used->head;
		while (cur_node != NULL && strcmp(cur_node->name, name) != 0) {
			pre_node = cur_node;
			cur_node = pre_node->next;
		}
		/* we found nvram node */
		if (cur_node != NULL) {
			pl_free = &nvram->free_nodes[i];
			/* put original nvram node to free pool */
			if (pre_node == NULL) {
				pl_used->head = cur_node->next;
			}
			else {
				pre_node->next = cur_node->next;
			}
			cur_node->next = pl_free->head;
			pl_free->head = cur_node;
			pl_free->count++;
			nvram->used_space -= cur_node->len;
			nvram->free_space += cur_node->len;
			return true;
		}
	}

	/* not find nvram node */
	return false;
}

bool ezcfg_nvram_commit(struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;
	bool ret;

	ASSERT(nvram != NULL);

	ezcfg = nvram->ezcfg;

	switch (nvram->backend_type) {
	case EZCFG_NVRAM_BACKEND_TYPE_NONE :
		info(ezcfg, "nvram in memory only, do nothing\n");
		ret = true;
		break;

	case EZCFG_NVRAM_BACKEND_TYPE_FILE :
		info(ezcfg, "nvram store on file-system file\n");
		ret = nvram_commit_to_file(nvram);
		break;

	case EZCFG_NVRAM_BACKEND_TYPE_FLASH :
		info(ezcfg, "nvram store on flash chip\n");
		ret = nvram_commit_to_flash(nvram);
		break;

	default:
		err(ezcfg, "unknown nvram type.\n");
		ret = false;
		break;
	}
	return ret;
}

bool ezcfg_nvram_initialize(struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;
	bool ret;

	ASSERT(nvram != NULL);

	ezcfg = nvram->ezcfg;

	if (nvram->backend_type != EZCFG_NVRAM_BACKEND_TYPE_NONE &&
	    nvram->store_path == NULL) {
		err(ezcfg, "not setting nvram store path.\n");
		return false;
	}

	switch (nvram->backend_type) {
	case EZCFG_NVRAM_BACKEND_TYPE_NONE :
		ret = nvram_init_by_defaults(nvram);
		break;

	case EZCFG_NVRAM_BACKEND_TYPE_FILE :
		ret = nvram_init_from_file(nvram);
		if (ret == false) {
			ret = nvram_init_by_defaults(nvram);
		}
		break;

	case EZCFG_NVRAM_BACKEND_TYPE_FLASH :
		ret = nvram_init_from_flash(nvram);
		if (ret == false) {
			ret = nvram_init_by_defaults(nvram);
		}
		break;

	default:
		err(ezcfg, "unknown nvram type.\n");
		ret = false;
		break;
	}
	return ret;
}

