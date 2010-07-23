/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg-private.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-12   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _LIBEZCFG_PRIVATE_H_
#define _LIBEZCFG_PRIVATE_H_

#include <syslog.h>
#include <signal.h>
#include <stdint.h>
#include <stdbool.h>

#include "libezcfg.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

typedef void * (*ezcfg_thread_func_t)(void *);

static inline void __attribute__((always_inline, format(printf, 2, 3)))
ezcfg_log_null(struct ezcfg *ezcfg, const char *format, ...) {}

#define ezcfg_log_cond(ezcfg, prio, arg...) \
  do { \
    if (ezcfg_get_log_priority(ezcfg) >= prio) \
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

/* libezcfg.c */
void ezcfg_log(struct ezcfg *ezcfg,
               int priority, const char *file, int line, const char *fn,
               const char *format, ...)
               __attribute__((format(printf, 6, 7)));
const char *ezcfg_get_rules_path(struct ezcfg *ezcfg);
struct ezcfg_list_entry *ezcfg_add_property(struct ezcfg *ezcfg, const char *key, const char *value);
struct ezcfg_list_entry *ezcfg_get_properties_list_entry(struct ezcfg *ezcfg);


/* libezcfg-list.c */
struct ezcfg_list_node {
	struct ezcfg_list_node *next, *prev;
};

void ezcfg_list_init(struct ezcfg_list_node *list);
int ezcfg_list_is_empty(struct ezcfg_list_node *list);
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

struct ezcfg_list_entry *ezcfg_list_entry_add(struct ezcfg *ezcfg, struct ezcfg_list_node *list,
                                              const char *name, const char *value,
                                              int unique, int sort);
void ezcfg_list_entry_delete(struct ezcfg_list_entry *entry);

/* libezcfg-thread.c */
int ezcfg_thread_start(struct ezcfg *ezcfg, int stacksize, ezcfg_thread_func_t func, void *param);

/* libezcfg-socket.c */
struct ezcfg_socket;
void ezcfg_socket_delete(struct ezcfg_socket *sp);
struct ezcfg_socket *ezcfg_socket_new(struct ezcfg *ezcfg, int family, const char *socket_path);
struct ezcfg_socket *ezcfg_socket_calloc(struct ezcfg *ezcfg, int size);
struct ezcfg_socket *ezcfg_socket_new_accepted_socket(const struct ezcfg_socket *listener);
void ezcfg_socket_close_sock(struct ezcfg_socket *sp);
void ezcfg_socket_set_close_on_exec(struct ezcfg_socket *sp);
int ezcfg_socket_get_sock(const struct ezcfg_socket *sp);
struct ezcfg_socket *ezcfg_socket_get_next(const struct ezcfg_socket *sp);
int ezcfg_socket_get_local_socket_len(struct ezcfg_socket *sp);
int ezcfg_socket_get_local_socket_domain(struct ezcfg_socket *sp);
char *ezcfg_socket_get_local_socket_path(struct ezcfg_socket *sp);
int ezcfg_socket_get_remote_socket_len(struct ezcfg_socket *sp);
int ezcfg_socket_get_remote_socket_domain(struct ezcfg_socket *sp);
char *ezcfg_socket_get_remote_socket_path(struct ezcfg_socket *sp);
int ezcfg_socket_enable_receiving(struct ezcfg_socket *sp);
int ezcfg_socket_enable_listening(struct ezcfg_socket *sp, int backlog);
int ezcfg_socket_set_receive_buffer_size(struct ezcfg_socket *sp, int size);
void ezcfg_socket_list_delete(struct ezcfg_socket **list);
int ezcfg_socket_list_insert(struct ezcfg_socket **list, struct ezcfg_socket *sp);
struct ezcfg_socket * ezcfg_socket_list_next(struct ezcfg_socket **list);
int ezcfg_socket_queue_get_socket(const struct ezcfg_socket *queue, int position, struct ezcfg_socket *sp);
int ezcfg_socket_queue_set_socket(struct ezcfg_socket *queue, int position, const struct ezcfg_socket *sp);
int ezcfg_socket_set_remote(struct ezcfg_socket *sp, int domain, const char *socket_path);
int ezcfg_socket_connect_remote(struct ezcfg_socket *sp);
int ezcfg_socket_read(struct ezcfg_socket *sp, void *buf, int len, int flags);
int ezcfg_socket_write(struct ezcfg_socket *sp, const void *buf, int len, int flags);

/* libezcfg-http.c */
struct ezcfg_http;
void ezcfg_http_delete(struct ezcfg_http *http);
struct ezcfg_http *ezcfg_http_new(struct ezcfg *ezcfg);
void ezcfg_http_delete_remote_user(struct ezcfg_http *http);
void ezcfg_http_delete_post_data(struct ezcfg_http *http);
void ezcfg_http_reset_attributes(struct ezcfg_http *http);
bool ezcfg_http_parse_request(struct ezcfg_http *http, char *buf);
char *ezcfg_http_get_version(struct ezcfg_http *http);
void ezcfg_http_set_status_code(struct ezcfg_http *http, int status_code);
void ezcfg_http_set_post_data(struct ezcfg_http *http, char *data);
void ezcfg_http_set_post_data_len(struct ezcfg_http *http, int len);
char *ezcfg_http_get_header(struct ezcfg_http *http, char *name);
void ezcfg_http_dump(struct ezcfg_http *http);

/* libezcfg-master.c */
struct ezcfg_master;
int ezcfg_master_set_receive_buffer_size(struct ezcfg_master *master, int size);
struct ezcfg_master *ezcfg_master_start(struct ezcfg *ezcfg);
void ezcfg_master_stop(struct ezcfg_master *master);
void ezcfg_master_set_threads_max(struct ezcfg_master *master, int threads_max);
void ezcfg_master_thread(struct ezcfg_master *master);
struct ezcfg *ezcfg_master_get_ezcfg(struct ezcfg_master *master);
bool ezcfg_master_is_stop(struct ezcfg_master *master);
bool ezcfg_master_get_socket(struct ezcfg_master *master, struct ezcfg_socket *sp);
void ezcfg_master_stop_worker(struct ezcfg_master *master);


/* libezcfg-worker.c */
struct ezcfg_worker;
/* worker inherits master's resource */
struct ezcfg_worker *ezcfg_worker_new(struct ezcfg_master *master);
void ezcfg_worker_thread(struct ezcfg_worker *worker);


/* libezcfg-ctrl.c - daemon runtime setup */
struct ezcfg_ctrl;
void ezcfg_ctrl_delete(struct ezcfg_ctrl *ezctrl);
struct ezcfg_ctrl *ezcfg_ctrl_new_from_socket(struct ezcfg *ezcfg, int family, const char *socket_path);
int ezcfg_ctrl_connect(struct ezcfg_ctrl *ezctrl);
int ezcfg_ctrl_read(struct ezcfg_ctrl *ezctrl, void *buf, int len, int flags);
int ezcfg_ctrl_write(struct ezcfg_ctrl *ezctrl, const void *buf, int len, int flags);


/* libezcfg-util.c */
#define UTIL_PATH_SIZE				1024
#define UTIL_NAME_SIZE				512
#define UTIL_LINE_SIZE				16384
#define EZCFG_ALLOWED_CHARS_INPUT		"/ $%?,"

int util_log_priority(const char *priority);
void util_remove_trailing_chars(char *path, char c);
size_t util_strpcpy(char **dest, size_t size, const char *src);
size_t util_strscpy(char *dest, size_t size, const char *src);

#endif
