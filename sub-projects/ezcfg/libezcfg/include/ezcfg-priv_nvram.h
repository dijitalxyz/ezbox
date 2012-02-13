/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_nvram.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2012-02-10   0.1       Split it from ezcfg-private.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_NVRAM_H_
#define _EZCFG_PRIV_NVRAM_H_

#include "ezcfg-types.h"

/* nvram/nvram.c */
bool ezcfg_nvram_delete(struct ezcfg_nvram *nvram);
struct ezcfg_nvram *ezcfg_nvram_new(struct ezcfg *ezcfg);
bool ezcfg_nvram_set_backend_type(struct ezcfg_nvram *nvram, const int index, const int type);
bool ezcfg_nvram_set_coding_type(struct ezcfg_nvram *nvram, const int index, const int type);
bool ezcfg_nvram_set_storage_path(struct ezcfg_nvram *nvram, const int index, const char *path);
bool ezcfg_nvram_set_total_space(struct ezcfg_nvram *nvram, const int total_space);
int ezcfg_nvram_get_total_space(struct ezcfg_nvram *nvram);
int ezcfg_nvram_get_free_space(struct ezcfg_nvram *nvram);
int ezcfg_nvram_get_used_space(struct ezcfg_nvram *nvram);
bool ezcfg_nvram_get_version_string(struct ezcfg_nvram *nvram, char *buf, size_t len);
bool ezcfg_nvram_get_storage_backend_string(struct ezcfg_nvram *nvram, const int index, char *buf, size_t len);
bool ezcfg_nvram_get_storage_coding_string(struct ezcfg_nvram *nvram, const int index, char *buf, size_t len);
bool ezcfg_nvram_get_storage_path_string(struct ezcfg_nvram *nvram, const int index, char *buf, size_t len);
bool ezcfg_nvram_set_default_settings(struct ezcfg_nvram *nvram, ezcfg_nv_pair_t *settings, int num_settings);
bool ezcfg_nvram_set_entry(struct ezcfg_nvram *nvram, const char *name, const char *value);
bool ezcfg_nvram_get_entry_value(struct ezcfg_nvram *nvram, const char *name, char **value);
bool ezcfg_nvram_unset_entry(struct ezcfg_nvram *nvram, const char *name);
bool ezcfg_nvram_get_all_entries_list(struct ezcfg_nvram *nvram, struct ezcfg_list_node *list);
bool ezcfg_nvram_commit(struct ezcfg_nvram *nvram);
bool ezcfg_nvram_fill_storage_info(struct ezcfg_nvram *nvram, const char *conf_path);
bool ezcfg_nvram_initialize(struct ezcfg_nvram *nvram);
bool ezcfg_nvram_load(struct ezcfg_nvram *nvram, int flag);
bool ezcfg_nvram_reload(struct ezcfg_nvram *nvram);
bool ezcfg_nvram_match_entry(struct ezcfg_nvram *nvram, char *name1, char *name2);
bool ezcfg_nvram_match_entry_value(struct ezcfg_nvram *nvram, char *name, char *value);
bool ezcfg_nvram_is_valid_entry_value(struct ezcfg_nvram *nvram, char *name, char *value);
bool ezcfg_nvram_insert_socket(struct ezcfg_nvram *nvram, struct ezcfg_link_list *list);
bool ezcfg_nvram_remove_socket(struct ezcfg_nvram *nvram, struct ezcfg_link_list *list);
bool ezcfg_nvram_insert_ssl(struct ezcfg_nvram *nvram, struct ezcfg_link_list *list);
bool ezcfg_nvram_remove_ssl(struct ezcfg_nvram *nvram, struct ezcfg_link_list *list);
bool ezcfg_nvram_set_multi_entries(struct ezcfg_nvram *nvram, struct ezcfg_link_list *list);
bool ezcfg_nvram_unset_multi_entries(struct ezcfg_nvram *nvram, struct ezcfg_link_list *list);

#endif /* _EZCFG_PRIV_NVRAM_H_ */
