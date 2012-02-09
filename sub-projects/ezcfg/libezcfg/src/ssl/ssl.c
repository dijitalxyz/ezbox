/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ssl/ssl.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-12   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <linux/netlink.h>

#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/opensslv.h>
#endif

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-ssl.h"

#if 1
#define DBG(format, args...) do { \
	char path[256]; \
	FILE *dbg_fp; \
	snprintf(path, 256, "/tmp/%d-debug.txt", getpid()); \
	dbg_fp = fopen(path, "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, "tid=[%d] ", (int)gettid()); \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

/*
 * SSL depends on stream socket
 */
struct ezcfg_ssl {
	struct ezcfg *ezcfg;
	struct ezcfg_socket *sp;        /* stream socket pointer */
	int role;                       /* combine, server, client */
	int method;                     /* SSL2, SSL3, TLS1, peer determed */
	SSL_CTX *ctx;                   /* SSL context */
	SSL *ssl;

	struct ezcfg_ssl *next; /* link pointer */
};


/**
 * private functions
 */
static bool ssl_is_same(struct ezcfg_ssl *s1, struct ezcfg_ssl *s2)
{
	return false;
}


/**
 * public functions
 */

/**
 * ezcfg_ssl_delete:
 * Delete ezcfg SSL socket.
 * Returns:
 **/
void ezcfg_ssl_delete(struct ezcfg_ssl *sslp)
{
	struct ezcfg *ezcfg;
	ASSERT(sslp != NULL);

	ezcfg = sslp->ezcfg;

	if (sslp->ctx != NULL)
		SSL_CTX_free(sslp->ctx);

	free(sslp);
}

/**
 * ezcfg_socket_new:
 * Create ezcfg SSL socket.
 * Returns: a new ezcfg SSL socket
 **/
struct ezcfg_ssl *ezcfg_ssl_new(struct ezcfg *ezcfg, const int role, const int method)
{
	struct ezcfg_ssl *sslp;
#if (OPENSSL_VERSION_NUMBER >= 0x10000000L)
	const SSL_METHOD *meth;
#else
	SSL_METHOD *meth;
#endif

	sslp = malloc(sizeof(struct ezcfg_ssl));
	if (sslp == NULL)
		return NULL;
	memset(sslp, 0, sizeof(struct ezcfg_ssl));

	SSL_library_init(); /* load encryption & hash algorithms for SSL */
	SSL_load_error_strings(); /* load the error strings for good error reporting */

	switch (role) {
	case EZCFG_SSL_ROLE_UNKNOWN :
		if (method == EZCFG_SSL_METHOD_SSLV2) {
			meth = SSLv2_method();
		}
		else if (method == EZCFG_SSL_METHOD_SSLV3) {
			meth = SSLv3_method();
		}
		else if (method == EZCFG_SSL_METHOD_TLSV1) {
			meth = TLSv1_method();
		}
		else if (method == EZCFG_SSL_METHOD_SSLV23) {
			meth = SSLv23_method();
		}
		else {
			goto fail_exit;
		}
		break;

	case EZCFG_SSL_ROLE_SERVER :
		if (method == EZCFG_SSL_METHOD_SSLV2) {
			meth = SSLv2_server_method();
		}
		else if (method == EZCFG_SSL_METHOD_SSLV3) {
			meth = SSLv3_server_method();
		}
		else if (method == EZCFG_SSL_METHOD_TLSV1) {
			meth = TLSv1_server_method();
		}
		else if (method == EZCFG_SSL_METHOD_SSLV23) {
			meth = SSLv23_server_method();
		}
		else {
			goto fail_exit;
		}
		break;

	case EZCFG_SSL_ROLE_CLIENT :
		if (method == EZCFG_SSL_METHOD_SSLV2) {
			meth = SSLv2_client_method();
		}
		else if (method == EZCFG_SSL_METHOD_SSLV3) {
			meth = SSLv3_client_method();
		}
		else if (method == EZCFG_SSL_METHOD_TLSV1) {
			meth = TLSv1_client_method();
		}
		else if (method == EZCFG_SSL_METHOD_SSLV23) {
			meth = SSLv23_client_method();
		}
		else {
			goto fail_exit;
		}
		break;

	default :
		goto fail_exit;
	}

	sslp->ctx = SSL_CTX_new(meth);

	return sslp;

fail_exit:
	ezcfg_ssl_delete(sslp);
	return NULL;
}

struct ezcfg_socket *ezcfg_ssl_get_socket(struct ezcfg_ssl *sslp)
{
	struct ezcfg *ezcfg;

	ASSERT(sslp != NULL);

	ezcfg = sslp->ezcfg;

	return sslp->sp;
}

bool ezcfg_ssl_set_socket(struct ezcfg_ssl *sslp, struct ezcfg_socket *sp)
{
	struct ezcfg *ezcfg;

	ASSERT(sslp != NULL);

	ezcfg = sslp->ezcfg;
	if (sslp->sp != NULL) {
		ezcfg_socket_delete(sp);
	}
	sslp->sp = sp;

	return true;
}

bool ezcfg_ssl_is_valid(struct ezcfg_ssl *sslp)
{
	struct ezcfg *ezcfg;

	ASSERT(sslp != NULL);

	ezcfg = sslp->ezcfg;

	return true;
}

bool ezcfg_ssl_list_in(struct ezcfg_ssl **list, struct ezcfg_ssl *sslp)
{
	struct ezcfg *ezcfg;
	struct ezcfg_ssl *sp;

	ASSERT(list != NULL);
	ASSERT(sslp != NULL);

	ezcfg = sslp->ezcfg;

	sp = *list;
	while (sp != NULL) {
		if (ssl_is_same(sp, sslp) == true) {
			return true;
		}
		sp = sp->next;
	}
	return false;
}

bool ezcfg_ssl_list_insert(struct ezcfg_ssl **list, struct ezcfg_ssl *sslp)
{
	ASSERT(list != NULL);
	ASSERT(sslp != NULL);

	sslp->next = *list;
	*list = sslp;
	return true;
}

void ezcfg_ssl_list_delete(struct ezcfg_ssl **list)
{
	struct ezcfg_ssl *sslp;

	ASSERT(list != NULL);

	sslp = *list;
	while (sslp != NULL) {
		*list = sslp->next;
		ezcfg_ssl_delete(sslp);
		sslp = *list;
	}
}
