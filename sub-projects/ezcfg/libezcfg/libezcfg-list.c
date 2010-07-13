/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg-list.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-12   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdlib.h>

#include "libezcfg.h"
#include "libezcfg-private.h"

/**
 * SECTION:libezcfg-list
 * @short_description: list operation
 *
 * Libezcfg list operations.
 */

/**
 * ezcfg_list_entry:
 *
 * Opaque object representing one entry in a list. An entry contains
 * contains a name, and optionally a value.
 */
struct ezcfg_list_entry {
	struct ezcfg_list_node node;
	struct ezcfg *ezcfg;
	struct ezcfg_list_node *list;
	char *name;
	char *value;
	unsigned int flags;
};

static void ezcfg_list_node_insert_between(struct ezcfg_list_node *new,
                                           struct ezcfg_list_node *prev,
                                           struct ezcfg_list_node *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

void ezcfg_list_node_append(struct ezcfg_list_node *new, struct ezcfg_list_node *list)
{
	ezcfg_list_node_insert_between(new, list->prev, list);
}

void ezcfg_list_node_remove(struct ezcfg_list_node *entry)
{
	struct ezcfg_list_node *prev = entry->prev;
	struct ezcfg_list_node *next = entry->next;

	next->prev = prev;
	prev->next = next;

	entry->prev = NULL;
	entry->next = NULL;
}


void ezcfg_list_entry_delete(struct ezcfg_list_entry *entry)
{
	ezcfg_list_node_remove(&entry->node);
	free(entry->name);
	free(entry->value);
	free(entry);
}

