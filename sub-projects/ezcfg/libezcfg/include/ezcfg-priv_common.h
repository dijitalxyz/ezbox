/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_common.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-02   0.1       Split it from ezcfg.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_COMMON_H_
#define _EZCFG_PRIV_COMMON_H_

#include "ezcfg-types.h"

/*
 * common/ezcfg.c
 * ezbox config context
 */
void ezcfg_log(struct ezcfg *ezcfg,
               int priority, const char *file, int line, const char *fn,
               const char *format, ...)
               __attribute__((format(printf, 6, 7)));

static inline void __attribute__((always_inline, format(printf, 2, 3)))
ezcfg_log_null(struct ezcfg *ezcfg, const char *format, ...) {}

#define ezcfg_log_cond(ezcfg, prio, arg...) \
  do { \
    if (ezcfg_common_get_log_priority(ezcfg) >= prio) \
      ezcfg_log(ezcfg, prio, __FILE__, __LINE__, __FUNCTION__, ## arg); \
  } while (0)

/* FIXME: remove it later */
#define ENABLE_LOGGING	1
#define ENABLE_DEBUG	1

#ifdef ENABLE_LOGGING
#  ifdef ENABLE_DEBUG
#    define dbg(ezcfg, arg...) ezcfg_log_cond(ezcfg, LOG_DEBUG, ## arg)
#  else
#    define dbg(ezcfg, arg...) ezcfg_log_null(ezcfg, ## arg)
#  endif
#  define info(ezcfg, arg...) ezcfg_log_cond(ezcfg, LOG_INFO, ## arg)
#  define err(ezcfg, arg...) ezcfg_log_cond(ezcfg, LOG_ERR, ## arg)
#else
#  define dbg(ezcfg, arg...) ezcfg_log_null(ezcfg, ## arg)
#  define info(ezcfg, arg...) ezcfg_log_null(ezcfg, ## arg)
#  define err(ezcfg, arg...) ezcfg_log_null(ezcfg, ## arg)
#endif


static inline void ezcfg_log_init(const char *program_name)
{
	openlog(program_name, LOG_PID | LOG_CONS, LOG_DAEMON);
}

static inline void ezcfg_log_close(void)
{
	closelog();
}

void ezcfg_common_set_log_fn(struct ezcfg *ezcfg,
                      void (*log_fn)(struct ezcfg *ezcfg,
                                    int priority, const char *file, int line, const char *fn,
                                    const char *format, va_list args));
int ezcfg_common_get_log_priority(struct ezcfg *ezcfg);
void ezcfg_common_set_log_priority(struct ezcfg *ezcfg, int priority);

struct ezcfg *ezcfg_new(char *path);
void ezcfg_delete(struct ezcfg *ezcfg);

char *ezcfg_common_get_config_file(struct ezcfg *ezcfg);
void ezcfg_common_set_config_file(struct ezcfg *ezcfg, char *file);
char *ezcfg_common_get_rules_path(struct ezcfg *ezcfg);
void ezcfg_common_set_rules_path(struct ezcfg *ezcfg, char *path);
char *ezcfg_common_get_root_path(struct ezcfg *ezcfg);
void ezcfg_common_set_root_path(struct ezcfg *ezcfg, char *path);
char *ezcfg_common_get_sem_ezcfg_path(struct ezcfg *ezcfg);
void ezcfg_common_set_sem_ezcfg_path(struct ezcfg *ezcfg, char *path);
char *ezcfg_common_get_shm_ezcfg_path(struct ezcfg *ezcfg);
void ezcfg_common_set_shm_ezcfg_path(struct ezcfg *ezcfg, char *path);
size_t ezcfg_common_get_shm_ezcfg_size(struct ezcfg *ezcfg);
char *ezcfg_common_get_shm_ezctp_path(struct ezcfg *ezcfg);
void ezcfg_common_set_shm_ezctp_path(struct ezcfg *ezcfg, char *path);
size_t ezcfg_common_get_shm_ezctp_size(struct ezcfg *ezcfg);
char *ezcfg_common_get_sock_ctrl_path(struct ezcfg *ezcfg);
void ezcfg_common_set_sock_ctrl_path(struct ezcfg *ezcfg, char *path);
char *ezcfg_common_get_sock_nvram_path(struct ezcfg *ezcfg);
void ezcfg_common_set_sock_nvram_path(struct ezcfg *ezcfg, char *path);
char *ezcfg_common_get_sock_uevent_path(struct ezcfg *ezcfg);
void ezcfg_common_set_sock_uevent_path(struct ezcfg *ezcfg, char *path);
char *ezcfg_common_get_sock_master_path(struct ezcfg *ezcfg);
void ezcfg_common_set_sock_master_path(struct ezcfg *ezcfg, char *path);
char *ezcfg_common_get_web_document_root_path(struct ezcfg *ezcfg);
void ezcfg_common_set_web_document_root_path(struct ezcfg *ezcfg, char *path);
char *ezcfg_common_get_locale(struct ezcfg *ezcfg);
void ezcfg_common_set_locale(struct ezcfg *ezcfg, char *locale);
int ezcfg_common_locale_mutex_lock(struct ezcfg *ezcfg);
int ezcfg_common_locale_mutex_unlock(struct ezcfg *ezcfg);
struct ezcfg_list_entry *ezcfg_common_add_property(struct ezcfg *ezcfg, const char *key, const char *value);
struct ezcfg_list_entry *ezcfg_common_get_properties_list_entry(struct ezcfg *ezcfg);
void ezcfg_common_load_conf(struct ezcfg *ezcfg);


/*
 * commom/list.c
 * ezcfg_list
 *
 * access to ezcfg generated lists
 */
struct ezcfg_list_node {
	struct ezcfg_list_node *next, *prev;
};

void ezcfg_list_init(struct ezcfg_list_node *list);
bool ezcfg_list_is_empty(struct ezcfg_list_node *list);
int ezcfg_list_get_num_nodes(struct ezcfg_list_node *list);
void ezcfg_list_node_append(struct ezcfg_list_node *new, struct ezcfg_list_node *list);
void ezcfg_list_node_remove(struct ezcfg_list_node *entry);
#define ezcfg_list_node_foreach(node, list) \
	for (node = (list)->next; \
	     node != list; \
	     node = (node)->next)
#define ezcfg_list_node_foreach_safe(node, tmp, list) \
	for (node = (list)->next, tmp = (node)->next; \
	     node != list; \
	     node = tmp, tmp = (tmp)->next)
struct ezcfg_list_entry *ezcfg_list_entry_add(struct ezcfg *ezcfg, struct ezcfg_list_node *list,
                                              const char *name, const char *value,
                                              int unique, int sort);
void ezcfg_list_entry_delete(struct ezcfg_list_entry *entry);
void ezcfg_list_entry_remove(struct ezcfg_list_entry *entry);
void ezcfg_list_entry_insert_before(struct ezcfg_list_entry *new, struct ezcfg_list_entry *entry);
void ezcfg_list_entry_append(struct ezcfg_list_entry *new, struct ezcfg_list_node *list);
void ezcfg_list_cleanup_entries(struct ezcfg *ezcfg, struct ezcfg_list_node *name_list);
struct ezcfg_list_entry *ezcfg_list_get_entry(struct ezcfg_list_node *list);
struct ezcfg_list_entry *ezcfg_list_entry_get_next(struct ezcfg_list_entry *list_entry);
struct ezcfg_list_entry *ezcfg_list_entry_get_by_name(struct ezcfg_list_entry *list_entry, const char *name);
const char *ezcfg_list_entry_get_name(struct ezcfg_list_entry *list_entry);
const char *ezcfg_list_entry_get_value(struct ezcfg_list_entry *list_entry);

/**
 * ezcfg_list_entry_foreach:
 * @list_entry: entry to store the current position
 * @first_entry: first entry to start with
 *
 * Helper to iterate over all entries of a list.
 */
#define ezcfg_list_entry_foreach(list_entry, first_entry) \
	for (list_entry = first_entry; \
	     list_entry != NULL; \
	     list_entry = ezcfg_list_entry_get_next(list_entry))


/* common/link_list.c */
struct ezcfg_link_list;
struct ezcfg_link_list *ezcfg_link_list_new(struct ezcfg *ezcfg);
void ezcfg_link_list_delete(struct ezcfg_link_list *list);
bool ezcfg_link_list_insert(struct ezcfg_link_list *list, char *name, char *value);
bool ezcfg_link_list_append(struct ezcfg_link_list *list, char *name, char *value);
bool ezcfg_link_list_remove(struct ezcfg_link_list *list, char *name);
bool ezcfg_link_list_in(struct ezcfg_link_list *list, char *name);
int ezcfg_link_list_get_length(struct ezcfg_link_list *list);
char *ezcfg_link_list_get_node_name_by_index(struct ezcfg_link_list *list, int i);
char *ezcfg_link_list_get_node_value_by_index(struct ezcfg_link_list *list, int i);
char *ezcfg_link_list_get_node_value_by_name(struct ezcfg_link_list *list, char *name);

/* common/link_list.c */
struct ezcfg_shm;
int ezcfg_shm_get_ezcfg_sem_id(const struct ezcfg_shm *shm);
bool ezcfg_shm_insert_ezctp_market_data(struct ezcfg_shm *shm, const void *data, size_t n, size_t size);

#endif /* _EZCFG_PRIV_COMMON_H_ */
