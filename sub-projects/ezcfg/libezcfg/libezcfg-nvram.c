/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg-nvram.c
 *
 * Description  : implement Non-Volatile RAM
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

#if 0
static unsigned char default_magics[][4] = {
	{ 'N', 'O', 'N', 'E' },
	{ 'F', 'I', 'L', 'E' },
	{ 'F', 'L', 'S', 'H' },
};
#endif

struct nvram_pair {
	char *name;
	char *value;
};

static struct nvram_pair default_nvram_settings[] = {
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

	/* storage type, mapping to nvram header->magic
	 * 0 -> NONE
	 * 1 -> FILE
	 * 2 -> FLSH
	 */
	int type;

	/* nvram storage device/file path */
	char *store_path;

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

	return nvram;
}

bool ezcfg_nvram_set_type(struct ezcfg_nvram *nvram, const int type)
{
	struct ezcfg *ezcfg;

	ASSERT(nvram != NULL);
	ASSERT(type >= 0);

	ezcfg = nvram->ezcfg;

	nvram->type = type;

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

bool ezcfg_nvram_set_node_value(struct ezcfg_nvram *nvram, const char *name, const char *value)
{
	struct ezcfg *ezcfg;
	struct nvram_pool *pl_used, *pl_free;
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
			/* block has enough space for nvram node */
			if (name_len + value_len + 2 <= (int)nvram->block_sizes[i]) {
				sprintf(cur_node->value, "%s", value);
				nvram->used_space += (name_len + value_len + 2 - cur_node->len);
				nvram->free_space -= (name_len + value_len + 2 - cur_node->len);
				cur_node->len = (name_len + value_len + 2);
				return true;
			}
			/* block has not enough space for nvram node */
			/* find a free nvram node */
			j = i+1;
			while (name_len + value_len + 2 > (int)nvram->block_sizes[j]) {
				j++;
			}
			pl_free = &nvram->free_nodes[j];
			pl_used = &nvram->used_nodes[j];

			/* free pool has node */
			if (pl_free->count > 0) {
				/* put original nvram node to free pool */
				if (pre_node == NULL) {
					pl_used->head = cur_node->next;
				}
				else {
					pre_node->next = cur_node->next;
				}
				cur_node->next = nvram->free_nodes[i].head;
				nvram->free_nodes[i].head = cur_node;
				nvram->free_nodes[i].count++;
				nvram->used_space -= cur_node->len;
				nvram->free_space += cur_node->len;

				/* set new nvram node */
				cur_node = pl_free->head;
				pl_free->head = cur_node->next;
				pl_free->count--;
				sprintf(cur_node->name, "%s", name);
				cur_node->value = cur_node->name + name_len + 1;
				sprintf(cur_node->value, "%s", value);
				cur_node->len = (name_len + value_len + 2);

				/* put nvram node to used pool */
				cur_node->next = pl_used->head;
				pl_used->head = cur_node;
				pl_used->count++;
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

				/* put it to used pool */
				new_node->next = pl_used->head;
				pl_used->head = new_node;
				pl_used->count++;
				nvram->used_space += new_node->len;
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
	pl_used = &nvram->used_nodes[j];

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
	new_node->next = pl_used->head;
	pl_used->head = new_node;
	pl_used->count++;
	nvram->used_space += new_node->len;
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

char *ezcfg_nvram_get_all_nodes(struct ezcfg_nvram *nvram, int *buf_len)
{
	struct ezcfg *ezcfg;
	struct nvram_pool *pl_used;
	struct nvram_node *node=NULL;
	char *buf, *p;
	int i, len;

	ASSERT(nvram != NULL);

	ezcfg = nvram->ezcfg;

	buf = calloc(nvram->used_space, sizeof(char));
	if (buf == NULL) {
		err(ezcfg, "not enough memory for get_all_nodes\n");
		return NULL;
	}

	p = buf;

	/* find all nvram nodes */
	for (i = 0; i < nvram->num_block_size; i++) {
		pl_used = &nvram->used_nodes[i];
		node = pl_used->head;
		while (node != NULL) {
			len = sprintf(p, "%s=%s", node->name, node->value);
			if (len +1 != node->len) {
				free(buf);
				return NULL;
			}
			p += node->len;
			node = node->next;
		}
	}

	*buf_len = nvram->used_space;
	return buf;
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
	struct nvram_pool *pl_used;
	struct nvram_node *node=NULL;
	char *buf, *p;
	int i, len;

	ASSERT(nvram != NULL);

	ezcfg = nvram->ezcfg;

	buf = calloc(nvram->used_space, sizeof(char));
	if (buf == NULL) {
		err(ezcfg, "not enough memory for nvram commit\n");
		return NULL;
	}

	p = buf;

	/* find all nvram nodes */
	for (i = 0; i < nvram->num_block_size; i++) {
		pl_used = &nvram->used_nodes[i];
		node = pl_used->head;
		while (node != NULL) {
			len = sprintf(p, "%s=%s", node->name, node->value);
			if (len +1 != node->len) {
				free(buf);
				return NULL;
			}
			p += node->len;
			node = node->next;
		}
	}

	return true;
}

bool ezcfg_nvram_initialize(struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;
	int i;
	struct nvram_pair *nvp;

	ASSERT(nvram != NULL);

	ezcfg = nvram->ezcfg;

	if (nvram->store_path == NULL) {
		err(ezcfg, "not setting nvram store path.\n");
		return false;
	}

	switch (nvram->type) {
	case 1:
		for (i=0; i<(int)ARRAY_SIZE(default_nvram_settings); i++) {
			nvp = &default_nvram_settings[i];
			info(ezcfg, "name=[%s] value=[%s]\n", nvp->name, nvp->value);
			if (ezcfg_nvram_set_node_value(nvram, nvp->name, nvp->value) == false) {
				err(ezcfg, "set nvram error.\n");
				return false;
			}
		}
		break;
	default:
		err(ezcfg, "unknown nvram type.\n");
		return false;
	}
	return true;
}

