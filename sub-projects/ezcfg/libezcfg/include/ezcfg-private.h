/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-private.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-12   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_PRIVATE_H_
#define _EZCFG_PRIVATE_H_

#include <syslog.h>
#include <signal.h>
#include <stdint.h>
#include <stdbool.h>

#include "ezcfg.h"

typedef void * (*ezcfg_thread_func_t)(void *);
/* name-value pair struct */
typedef struct ezcfg_nv_pair_s {
	char *name;
	char *value;
} ezcfg_nv_pair_t;


/* common/ezcfg.c */
const char *ezcfg_get_rules_path(struct ezcfg *ezcfg);
struct ezcfg_list_entry *ezcfg_add_property(struct ezcfg *ezcfg, const char *key, const char *value);
struct ezcfg_list_entry *ezcfg_get_properties_list_entry(struct ezcfg *ezcfg);


/* common/list.c */
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


/* thread/thread.c */
int ezcfg_thread_start(struct ezcfg *ezcfg, int stacksize, ezcfg_thread_func_t func, void *param);


/* nvram/nvram-defaults.c */
extern ezcfg_nv_pair_t default_nvram_settings[];
extern char *default_nvram_unsets[];
int ezcfg_nvram_get_num_default_nvram_settings(void);
int ezcfg_nvram_get_num_default_nvram_unsets(void);

/* nvram/nvram.c */
struct ezcfg_nvram;
bool ezcfg_nvram_delete(struct ezcfg_nvram *nvram);
struct ezcfg_nvram *ezcfg_nvram_new(struct ezcfg *ezcfg);
bool ezcfg_nvram_set_backend_type(struct ezcfg_nvram *nvram, const int index, const int type);
bool ezcfg_nvram_set_coding_type(struct ezcfg_nvram *nvram, const int index, const int type);
bool ezcfg_nvram_set_storage_path(struct ezcfg_nvram *nvram, const int index, const char *path);
char *ezcfg_nvram_get_storage_path(struct ezcfg_nvram *nvram, const int index);
bool ezcfg_nvram_set_total_space(struct ezcfg_nvram *nvram, const int total_space);
int ezcfg_nvram_get_total_space(struct ezcfg_nvram *nvram);
bool ezcfg_nvram_set_default_settings(struct ezcfg_nvram *nvram, ezcfg_nv_pair_t *settings, int num_settings);
bool ezcfg_nvram_set_entry(struct ezcfg_nvram *nvram, const char *name, const char *value);
bool ezcfg_nvram_get_entry_value(struct ezcfg_nvram *nvram, const char *name, char **value);
bool ezcfg_nvram_unset_entry(struct ezcfg_nvram *nvram, const char *name);
bool ezcfg_nvram_get_all_entries_list(struct ezcfg_nvram *nvram, struct ezcfg_list_node *list);
bool ezcfg_nvram_commit(struct ezcfg_nvram *nvram);
bool ezcfg_nvram_fill_storage_info(struct ezcfg_nvram *nvram, const char *conf_path);
bool ezcfg_nvram_initialize(struct ezcfg_nvram *nvram);


/* uuid/uuid.c */
int ezcfg_uuid_get_version(struct ezcfg_uuid *uuid);
bool ezcfg_uuid_set_store_name(struct ezcfg_uuid *uuid, const char *store_name);
char *ezcfg_uuid_get_store_name(struct ezcfg_uuid *uuid);
bool ezcfg_uuid_v1_set_mac(struct ezcfg_uuid *uuid, unsigned char *mac, int len);
bool ezcfg_uuid_v1_enforce_multicast_mac(struct ezcfg_uuid *uuid);


/* socket/socket.c */
struct ezcfg_socket;
void ezcfg_socket_delete(struct ezcfg_socket *sp);
struct ezcfg_socket *ezcfg_socket_new(struct ezcfg *ezcfg, const int family, const unsigned char proto, const char *socket_path);
struct ezcfg_socket *ezcfg_socket_calloc(struct ezcfg *ezcfg, int size);
struct ezcfg_socket *ezcfg_socket_new_accepted_socket(const struct ezcfg_socket *listener);
void ezcfg_socket_close_sock(struct ezcfg_socket *sp);
void ezcfg_socket_set_close_on_exec(struct ezcfg_socket *sp);
int ezcfg_socket_get_sock(const struct ezcfg_socket *sp);
unsigned char ezcfg_socket_get_proto(const struct ezcfg_socket *sp);
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
int ezcfg_socket_queue_get_socket(const struct ezcfg_socket *queue, int pos, struct ezcfg_socket *sp);
int ezcfg_socket_queue_set_socket(struct ezcfg_socket *queue, int pos, const struct ezcfg_socket *sp);
void ezcfg_socket_set_need_unlink(struct ezcfg_socket *sp, bool need_unlink);
int ezcfg_socket_set_remote(struct ezcfg_socket *sp, int domain, const char *socket_path);
int ezcfg_socket_connect_remote(struct ezcfg_socket *sp);
int ezcfg_socket_read(struct ezcfg_socket *sp, void *buf, int len, int flags);
int ezcfg_socket_write(struct ezcfg_socket *sp, const void *buf, int len, int flags);


/* http/http.c */
struct ezcfg_http;
void ezcfg_http_delete(struct ezcfg_http *http);
struct ezcfg_http *ezcfg_http_new(struct ezcfg *ezcfg);
void ezcfg_http_set_state_request(struct ezcfg_http *http);
void ezcfg_http_set_state_response(struct ezcfg_http *http);
bool ezcfg_http_is_state_request(struct ezcfg_http *http);
bool ezcfg_http_is_state_response(struct ezcfg_http *http);
void ezcfg_http_delete_remote_user(struct ezcfg_http *http);
void ezcfg_http_reset_attributes(struct ezcfg_http *http);
bool ezcfg_http_parse_header(struct ezcfg_http *http, char *buf, int len);
unsigned short ezcfg_http_get_version_major(struct ezcfg_http *http);
unsigned short ezcfg_http_get_version_minor(struct ezcfg_http *http);
bool ezcfg_http_set_version_major(struct ezcfg_http *http, unsigned short major);
bool ezcfg_http_set_version_minor(struct ezcfg_http *http, unsigned short minor);
bool ezcfg_http_set_method_strings(struct ezcfg_http *http, const char **method_strings, unsigned char num_methods);
bool ezcfg_http_set_known_header_strings(struct ezcfg_http *http, const char **header_strings, unsigned char num_headers);
bool ezcfg_http_set_status_code_maps(struct ezcfg_http *http, const void *maps, unsigned short num_status_codes);
unsigned char ezcfg_http_set_request_method(struct ezcfg_http *http, const char *method);
char *ezcfg_http_get_request_uri(struct ezcfg_http *http);
bool ezcfg_http_set_request_uri(struct ezcfg_http *http, const char *uri);
unsigned short ezcfg_http_set_status_code(struct ezcfg_http *http, unsigned short status_code);
char *ezcfg_http_set_message_body(struct ezcfg_http *http, const char *body, int len);
char *ezcfg_http_get_message_body(struct ezcfg_http *http);
int ezcfg_http_get_message_body_len(struct ezcfg_http *http);
char *ezcfg_http_get_header_value(struct ezcfg_http *http, char *name);
void ezcfg_http_dump(struct ezcfg_http *http);
int ezcfg_http_get_request_line_length(struct ezcfg_http *http);
int ezcfg_http_write_request_line(struct ezcfg_http *http, char *buf, int len);
int ezcfg_http_get_status_line_length(struct ezcfg_http *http);
int ezcfg_http_write_status_line(struct ezcfg_http *http, char *buf, int len);
int ezcfg_http_get_start_line_length(struct ezcfg_http *http);
int ezcfg_http_write_start_line(struct ezcfg_http *http, char *buf, int len);
int ezcfg_http_get_crlf_length(struct ezcfg_http *http);
int ezcfg_http_write_crlf(struct ezcfg_http *http, char *buf, int len);
int ezcfg_http_get_headers_length(struct ezcfg_http *http);
int ezcfg_http_write_headers(struct ezcfg_http *http, char *buf, int len);
bool ezcfg_http_add_header(struct ezcfg_http *http, char *name, char *value);
int ezcfg_http_write_message_body(struct ezcfg_http *http, char *buf, int len);


/* socket/socket_http.c */
int ezcfg_socket_read_http_header(struct ezcfg_socket *sp, struct ezcfg_http *http, char *buf, int bufsiz, int *nread);
char *ezcfg_socket_read_http_content(struct ezcfg_socket *sp, struct ezcfg_http *http, char *buf, int header_len, int *bufsiz, int *nread);


/* xml/xml.c */
struct ezcfg_xml_element;
struct ezcfg_xml;
void ezcfg_xml_delete(struct ezcfg_xml *xml);
struct ezcfg_xml *ezcfg_xml_new(struct ezcfg *ezcfg);
void ezcfg_xml_reset_attributes(struct ezcfg_xml *xml);
int ezcfg_xml_get_max_elements(struct ezcfg_xml *xml);
bool ezcfg_xml_set_max_elements(struct ezcfg_xml *xml, const int max_elements);
int ezcfg_xml_get_num_elements(struct ezcfg_xml *xml);
int ezcfg_xml_normalize_document(struct ezcfg_xml *xml, char *buf, int len);
bool ezcfg_xml_parse(struct ezcfg_xml *xml, char *buf, int len);
int ezcfg_xml_get_message_length(struct ezcfg_xml *xml);
int ezcfg_xml_write_message(struct ezcfg_xml *xml, char *buf, int len);
void ezcfg_xml_element_delete(struct ezcfg_xml_element *elem);
struct ezcfg_xml_element *ezcfg_xml_element_new(
        struct ezcfg_xml *xml,
        const char *name, const char *content);
int ezcfg_xml_add_element(
        struct ezcfg_xml *xml,
        const int pi,
        const int si,
        struct ezcfg_xml_element *elem);
int ezcfg_xml_get_element_index(struct ezcfg_xml *xml, const int pi, const int si, char *name);
bool ezcfg_xml_element_add_attribute(
        struct ezcfg_xml *xml,
        struct ezcfg_xml_element *elem,
        const char *name, const char *value, int pos);
struct ezcfg_xml_element *ezcfg_xml_get_element_by_index(struct ezcfg_xml *xml, const int index);
bool ezcfg_xml_set_element_content_by_index(struct ezcfg_xml *xml, const int index, const char *content);
char *ezcfg_xml_get_element_content_by_index(struct ezcfg_xml *xml, const int index);

/* soap/soap.c */
struct ezcfg_soap;
void ezcfg_soap_delete(struct ezcfg_soap *soap);
struct ezcfg_soap *ezcfg_soap_new(struct ezcfg *ezcfg);
void ezcfg_soap_reset_attributes(struct ezcfg_soap *soap);
unsigned short ezcfg_soap_get_version_major(struct ezcfg_soap *soap);
unsigned short ezcfg_soap_get_version_minor(struct ezcfg_soap *soap);
bool ezcfg_soap_set_version_major(struct ezcfg_soap *soap, unsigned short major);
bool ezcfg_soap_set_version_minor(struct ezcfg_soap *soap, unsigned short minor);
int ezcfg_soap_get_max_nodes(struct ezcfg_soap *soap);
bool ezcfg_soap_set_max_nodes(struct ezcfg_soap *soap, const int max_nodes);
int ezcfg_soap_set_envelope(struct ezcfg_soap *soap, const char *name);
bool ezcfg_soap_add_envelope_attribute(struct ezcfg_soap *soap, const char *name, const char *value, int pos);
int ezcfg_soap_set_body(struct ezcfg_soap *soap, const char *name);
int ezcfg_soap_get_body_index(struct ezcfg_soap *soap);
int ezcfg_soap_add_body_child(struct ezcfg_soap *soap, int pi, int si, const char *name, const char *content);
bool ezcfg_soap_add_body_child_attribute(struct ezcfg_soap *soap, int ei, const char *name, const char *value, int pos);
int ezcfg_soap_get_element_index(struct ezcfg_soap *soap, const int pi, const int si, char *name);
char *ezcfg_soap_get_element_content_by_index(struct ezcfg_soap *soap, const int index);
bool ezcfg_soap_parse(struct ezcfg_soap *soap, char *buf, int len);
int ezcfg_soap_get_message_length(struct ezcfg_soap *soap);
int ezcfg_soap_write_message(struct ezcfg_soap *soap, char *buf, int len);

/* soap/soap_http.c */
struct ezcfg_soap_http;
void ezcfg_soap_http_delete(struct ezcfg_soap_http *sh);
struct ezcfg_soap_http *ezcfg_soap_http_new(struct ezcfg *ezcfg);
struct ezcfg_soap *ezcfg_soap_http_get_soap(struct ezcfg_soap_http *sh);
struct ezcfg_http *ezcfg_soap_http_get_http(struct ezcfg_soap_http *sh);
unsigned short ezcfg_soap_http_get_soap_version_major(struct ezcfg_soap_http *sh);
unsigned short ezcfg_soap_http_get_soap_version_minor(struct ezcfg_soap_http *sh);
bool ezcfg_soap_http_set_soap_version_major(struct ezcfg_soap_http *sh, unsigned short major);
bool ezcfg_soap_http_set_soap_version_minor(struct ezcfg_soap_http *sh, unsigned short minor);
unsigned short ezcfg_soap_http_get_http_version_major(struct ezcfg_soap_http *sh);
unsigned short ezcfg_soap_http_get_http_version_minor(struct ezcfg_soap_http *sh);
bool ezcfg_soap_http_set_http_version_major(struct ezcfg_soap_http *sh, unsigned short major);
bool ezcfg_soap_http_set_http_version_minor(struct ezcfg_soap_http *sh, unsigned short minor);
char *ezcfg_soap_http_get_http_header_value(struct ezcfg_soap_http *sh, char *name);
void ezcfg_soap_http_reset_attributes(struct ezcfg_soap_http *sh);
bool ezcfg_soap_http_parse_header(struct ezcfg_soap_http *sh, char *buf, int len);
bool ezcfg_soap_http_parse_message_body(struct ezcfg_soap_http *sh);
char *ezcfg_soap_http_set_message_body(struct ezcfg_soap_http *sh, const char *body, int len);
void ezcfg_soap_http_dump(struct ezcfg_soap_http *sh);
int ezcfg_soap_http_get_message_length(struct ezcfg_soap_http *sh);
int ezcfg_soap_http_write_message(struct ezcfg_soap_http *sh, char *buf, int len);

/* igrs/igrs.c */
struct ezcfg_igrs_msg_op;
void ezcfg_igrs_dump(struct ezcfg_igrs *igrs);
struct ezcfg_soap *ezcfg_igrs_get_soap(struct ezcfg_igrs *igrs);
struct ezcfg_http *ezcfg_igrs_get_http(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_message_type_ops(struct ezcfg_igrs *igrs, const struct ezcfg_igrs_msg_op *message_type_ops, unsigned short num_message_types);
unsigned short ezcfg_igrs_get_version_major(struct ezcfg_igrs *igrs);
unsigned short ezcfg_igrs_get_version_minor(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_target_client_id(struct ezcfg_igrs *igrs, unsigned int client_id);
unsigned int ezcfg_igrs_get_target_client_id(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_acknowledge_id(struct ezcfg_igrs *igrs, unsigned int ack_id);
unsigned int ezcfg_igrs_get_acknowledge_id(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_set_return_code(struct ezcfg_igrs *igrs, unsigned short code);
unsigned short ezcfg_igrs_get_return_code(struct ezcfg_igrs *igrs);
int ezcfg_igrs_handle_message(struct ezcfg_igrs *igrs);
char *ezcfg_igrs_get_http_header_value(struct ezcfg_igrs *igrs, char *name);
void ezcfg_igrs_reset_attributes(struct ezcfg_igrs *igrs);
bool ezcfg_igrs_parse_header(struct ezcfg_igrs *igrs, char *buf, int len);
bool ezcfg_igrs_parse_message_body(struct ezcfg_igrs *igrs);
char *ezcfg_igrs_set_message_body(struct ezcfg_igrs *igrs, const char *body, int len);
int ezcfg_igrs_http_get_message_length(struct ezcfg_igrs *igrs);
int ezcfg_igrs_http_write_message(struct ezcfg_igrs *igrs, char *buf, int len);

/* igrs/isdp.c */
struct ezcfg_isdp;
void ezcfg_isdp_delete(struct ezcfg_isdp *isdp);
struct ezcfg_isdp *ezcfg_isdp_new(struct ezcfg *ezcfg);
void ezcfg_isdp_dump(struct ezcfg_isdp *isdp);

/* thread/master.c */
int ezcfg_master_set_receive_buffer_size(struct ezcfg_master *master, int size);
void ezcfg_master_thread(struct ezcfg_master *master);
struct ezcfg *ezcfg_master_get_ezcfg(struct ezcfg_master *master);
bool ezcfg_master_is_stop(struct ezcfg_master *master);
bool ezcfg_master_get_socket(struct ezcfg_master *master, struct ezcfg_socket *sp);
struct ezcfg_nvram *ezcfg_master_get_nvram(struct ezcfg_master *master);

/* thread/worker.c */
struct ezcfg_worker;
/* worker inherits master's resource */
void ezcfg_worker_delete(struct ezcfg_worker *worker);
struct ezcfg_worker *ezcfg_worker_new(struct ezcfg_master *master);
void ezcfg_worker_thread(struct ezcfg_worker *worker);
/* worker call this to notify master */
void ezcfg_master_stop_worker(struct ezcfg_master *master, struct ezcfg_worker *worker);


/* ctrl/ctrl.c - daemon runtime setup */
struct ezcfg_socket *ezcfg_ctrl_get_socket(struct ezcfg_ctrl *ezctrl);


/* util/util.c */
#define UTIL_PATH_SIZE				1024
#define UTIL_NAME_SIZE				512
#define UTIL_LINE_SIZE				16384
#define EZCFG_ALLOWED_CHARS_INPUT		"/ $%?,"

int ezcfg_util_log_priority(const char *priority);
void ezcfg_util_remove_trailing_char(char *s, char c);
void ezcfg_util_remove_trailing_charlist(char *s, char *l);
char *ezcfg_util_skip_leading_char(char *s, char c);
char *ezcfg_util_skip_leading_charlist(char *s, char *l);

/* util/util_crc32.c */
uint32_t ezcfg_util_crc32(unsigned char *data, size_t len);

/* util/util_adler32.c */
uint32_t ezcfg_util_adler32(unsigned char *data, size_t len);

/* util/util_conf.c */
char *ezcfg_util_get_conf_string(const char *path, const char *keyword);

#endif
