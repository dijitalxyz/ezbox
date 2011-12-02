/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/worker_ctrl.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-01   0.1       Write it from scratch
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
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-soap_http.h"

#if 1
#define DBG(format, args...) do { \
	pid_t pid; \
	char path[256]; \
	FILE *fp; \
	pid = getpid(); \
	snprintf(path, 256, "/tmp/%d-debug.txt", pid); \
	fp = fopen(path, "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

#if 0
static bool ctrl_error_handler(struct ezcfg_worker *worker)
{
	return false;
}

static void send_ctrl_error(struct ezcfg_worker *worker, int status,
                            const char *reason, const char *fmt, ...)
{
	char buf[EZCFG_BUFFER_SIZE];
	va_list ap;
	int len;
	bool handled;

	handled = ctrl_error_handler(worker);

	if (handled == false) {
		buf[0] = '\0';
		len = 0;

		/* Errors 1xx, 204 and 304 MUST NOT send a body */
		if (status > 199 && status != 204 && status != 304) {
			len = snprintf(buf, sizeof(buf),
			               "Error %d: %s\n", status, reason);
			va_start(ap, fmt);
			len += vsnprintf(buf + len, sizeof(buf) - len, fmt, ap);
			va_end(ap);
			ezcfg_worker_set_num_bytes_sent(worker, len);
		}
		ezcfg_worker_printf(worker,
		              "HTTP/1.1 %d %s\r\n"
		              "Content-Type: text/plain\r\n"
		              "Content-Length: %d\r\n"
		              "Connection: close\r\n"
		              "\r\n%s", status, reason, len, buf);
	}
}
#endif

static void handle_ctrl_request(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;
	struct ezcfg_ctrl *ctrl;
	char *msg = NULL;
	//int msg_len;

	ASSERT(worker != NULL);

	ctrl = (struct ezcfg_ctrl *)ezcfg_worker_get_proto_data(worker);
	ASSERT(ctrl != NULL);

	ezcfg = ezcfg_worker_get_ezcfg(worker);

	if (ezcfg_ctrl_handle_message(ctrl) < 0) {
		//ezcfg_worker_write(worker, msg, msg_len);
	}
	else {
		//ezcfg_worker_write(worker, msg, msg_len);
	}
	if (msg != NULL)
		free(msg);
}

void ezcfg_worker_process_ctrl_new_connection(struct ezcfg_worker *worker)
{
	struct ezcfg *ezcfg;
	struct ezcfg_ctrl *ctrl;

	ASSERT(worker != NULL);

	ctrl = (struct ezcfg_ctrl *)ezcfg_worker_get_proto_data(worker);
	ASSERT(ctrl != NULL);

	ezcfg = ezcfg_worker_get_ezcfg(worker);
	handle_ctrl_request(worker);
}
