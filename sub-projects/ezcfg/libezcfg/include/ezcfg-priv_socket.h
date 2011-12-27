/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_socket.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-14   0.1       Split it from ezcfg-private.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_SOCKET_H_
#define _EZCFG_PRIV_SOCKET_H_

#include "ezcfg-types.h"

/* socket/socket.c */
void ezcfg_socket_delete(struct ezcfg_socket *sp);
struct ezcfg_socket *ezcfg_socket_new(struct ezcfg *ezcfg, const int family, const int type, const int proto, const char *socket_path);
struct ezcfg_socket *ezcfg_socket_fake_new(struct ezcfg *ezcfg, const int family, const int type, const int proto, const char *socket_path);
struct ezcfg_socket *ezcfg_socket_calloc(struct ezcfg *ezcfg, int size);
struct ezcfg_socket *ezcfg_socket_new_accepted_socket(const struct ezcfg_socket *listener);
void ezcfg_socket_close_sock(struct ezcfg_socket *sp);
void ezcfg_socket_set_close_on_exec(struct ezcfg_socket *sp);
struct ezcfg *ezcfg_socket_get_ezcfg(const struct ezcfg_socket *sp);
int ezcfg_socket_get_sock(const struct ezcfg_socket *sp);
bool ezcfg_socket_set_proto(struct ezcfg_socket *sp, const int proto);
int ezcfg_socket_get_proto(const struct ezcfg_socket *sp);
bool ezcfg_socket_set_buffer(struct ezcfg_socket *sp, char *buf, int buf_len);
char *ezcfg_socket_get_buffer(const struct ezcfg_socket *sp);
int ezcfg_socket_get_buffer_len(const struct ezcfg_socket *sp);
struct ezcfg_socket *ezcfg_socket_get_next(const struct ezcfg_socket *sp);
int ezcfg_socket_get_local_socket_len(struct ezcfg_socket *sp);
int ezcfg_socket_get_local_socket_domain(struct ezcfg_socket *sp);
char *ezcfg_socket_get_local_socket_path(struct ezcfg_socket *sp);
char *ezcfg_socket_get_local_socket_ip(struct ezcfg_socket *sp);
char *ezcfg_socket_get_group_interface_ip(struct ezcfg_socket *sp);
int ezcfg_socket_get_remote_socket_len(struct ezcfg_socket *sp);
int ezcfg_socket_get_remote_socket_domain(struct ezcfg_socket *sp);
char *ezcfg_socket_get_remote_socket_path(struct ezcfg_socket *sp);
int ezcfg_socket_binding(struct ezcfg_socket *sp);
int ezcfg_socket_enable_receiving(struct ezcfg_socket *sp);
int ezcfg_socket_enable_listening(struct ezcfg_socket *sp, int backlog);
int ezcfg_socket_enable_again(struct ezcfg_socket *sp);
int ezcfg_socket_enable_sending(struct ezcfg_socket *sp);
int ezcfg_socket_set_receive_buffer_size(struct ezcfg_socket *sp, int size);
bool ezcfg_socket_compare(struct ezcfg_socket *sp1, struct ezcfg_socket *sp2);
void ezcfg_socket_list_delete(struct ezcfg_socket **list);
bool ezcfg_socket_list_delete_socket(struct ezcfg_socket **list, struct ezcfg_socket *sp);
int ezcfg_socket_list_insert(struct ezcfg_socket **list, struct ezcfg_socket *sp);
bool ezcfg_socket_list_in(struct ezcfg_socket **list, struct ezcfg_socket *sp);
bool ezcfg_socket_list_set_need_delete(struct ezcfg_socket **list, struct ezcfg_socket *sp, bool need_delete);
struct ezcfg_socket * ezcfg_socket_list_next(struct ezcfg_socket **list);
int ezcfg_socket_queue_get_socket(const struct ezcfg_socket *queue, int pos, struct ezcfg_socket *sp);
int ezcfg_socket_queue_set_socket(struct ezcfg_socket *queue, int pos, const struct ezcfg_socket *sp);
void ezcfg_socket_set_need_unlink(struct ezcfg_socket *sp, bool need_unlink);
void ezcfg_socket_set_need_delete(struct ezcfg_socket *sp, bool need_delete);
bool ezcfg_socket_get_need_delete(struct ezcfg_socket *sp);
int ezcfg_socket_set_remote(struct ezcfg_socket *sp, int domain, const char *socket_path);
int ezcfg_socket_connect_remote(struct ezcfg_socket *sp);
int ezcfg_socket_read(struct ezcfg_socket *sp, void *buf, int len, int flags);
int ezcfg_socket_write(struct ezcfg_socket *sp, const void *buf, int len, int flags);

/* socket/socket_http.c */
int ezcfg_socket_read_http_header(struct ezcfg_socket *sp, struct ezcfg_http *http, char *buf, int bufsiz, int *nread);
char *ezcfg_socket_read_http_content(struct ezcfg_socket *sp, struct ezcfg_http *http, char *buf, int header_len, int *bufsiz, int *nread);

#endif /* _EZCFG_PRIV_SOCKET_H_ */
