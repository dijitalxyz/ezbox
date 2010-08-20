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

static unsigned char default_magics[][4] = {
	{ 'N', 'O', 'N', 'E' },
	{ 'F', 'I', 'L', 'E' },
	{ 'F', 'L', 'S', 'H' },
};

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
static unsigned short default_block_size[8] = {
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

	/* free[0] -> 2^4=16 bytes;
	 * free[1] -> free[0]*4 = 16*4 = 64 bytes;
	 * free[2] -> free[1]*4 = 64*4 = 256 bytes;
	 * free[3] -> free[2]*4 = 256*4 = 1024 bytes;
	 * free[4] -> free[3]*4 = 1024*2 = 2k bytes;
	 * free[5] -> free[4]*4 = 2k*2 = 4k bytes;
	 * free[6] -> free[5]*2 = 4k*2 = 8k bytes;
	 * free[7] -> free[6]*2 = 8k*2 = 16k bytes;
	 */
	struct nvram_pool free_nodes[8];
	struct nvram_pool used_nodes[8];
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

	ASSERT(nvram != NULL);

	ezcfg = nvram->ezcfg;

	if (nvram->store_path != NULL) {
		free(nvram->store_path);
	}

	free(nvram);
	return true;
}

struct ezcfg_nvram *ezcfg_nvram_new(struct ezcfg *ezcfg)
{
	struct ezcfg_nvram *nvram;

	ASSERT(ezcfg != NULL);

	nvram = calloc(1, sizeof(struct ezcfg_nvram));
	if (nvram == NULL) {
		err(ezcfg, "can not calloc nvram\n");
		return NULL;
	}

	memset(nvram, 0, sizeof(struct ezcfg_nvram));
	nvram->ezcfg = ezcfg;

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

	nvram->total_space = total_space;
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
	struct nvram_node *pre_node=NULL, *cur_node=NULL;
	int i;

	ASSERT(nvram != NULL);
	ASSERT(name != NULL);
	ASSERT(value != NULL);

	ezcfg = nvram->ezcfg;

	if (strlen(name) + strlen(value) + 2 > default_block_size[(int)ARRAY_SIZE(default_block_size) - 1]) {
		err(ezcfg, "too large name or value for nvram set\n");
		return false;
	}

	/* find nvram node */
	for(i = 0; i < (int)ARRAY_SIZE(nvram->used_nodes) && cur_node == NULL; i++) {
		pl_used = &nvram->used_nodes[i];
		pre_node=NULL;
		cur_node=NULL;
		if (pl_used->count == 0) {
			continue;
		}
		cur_node = pl_used->head;
		while(cur_node != NULL) {
			if (strcmp(cur_node->name, name) == 0) {
				break;
			}
			pre_node = cur_node;
			cur_node = pre_node->next;
		}
	}

	/* we found nvram node */
	if (cur_node != NULL) {
		if (strlen(name) + strlen(value) + 2 <= (int)default_block_size[i]) {
			sprintf(cur_node->value, "%s", value);
			return true;
		}
		else {
			/* find a free nvram node */
			pl_free = &nvram->free_nodes[i+1];
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
				nvram->used_space -= strlen(cur_node->name);
				nvram->used_space -= strlen(cur_node->value);
				nvram->used_space -= 2;

				/* set new nvram node */
				cur_node = pl_free->head;
				pl_free->head = cur_node->next;
				pl_free->count--;
				sprintf(cur_node->name, "%s", name);
				cur_node->value = cur_node->name + strlen(name) + 1;
				sprintf(cur_node->value, "%s", value);

				/* put nvram node to used pool */
				pl_used = &nvram->used_nodes[i+1];
				cur_node->next = pl_used->head;
				pl_used->head = cur_node;
				pl_used->count++;
				nvram->used_space += strlen(name);
				nvram->used_space += strlen(value);
				nvram->used_space += 2;
			}
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
			if (ezcfg_nvram_set_node_value(nvram, nvp->name, nvp->value) == false) {
				err(ezcfg, "set nvram error.\n");
				return false;
			}
		}
		break;
	default:
		err(ezcfg, "uknown nvram type.\n");
		return false;
	}
	return true;
}

