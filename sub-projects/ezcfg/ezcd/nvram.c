/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : nvram.c
 *
 * Description  : ezbox config interface
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-08-22   0.1       Write it from scratch
 * ============================================================================
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcd.h"

static bool debug = false;

static void log_fn(struct ezcfg *ezcfg, int priority,
                   const char *file, int line, const char *fn,
                   const char *format, va_list args)
{
	if (debug) {
		char buf[1024*8];
		struct timeval tv;
		struct timezone tz;

		vsnprintf(buf, sizeof(buf), format, args);
		gettimeofday(&tv, &tz);
		fprintf(stderr, "%llu.%06u [%u] %s(%d): %s",
		        (unsigned long long) tv.tv_sec, (unsigned int) tv.tv_usec,
		        (int) getpid(), fn, line, buf);
	} else {
		vsyslog(priority, format, args);
        }
}

static void nvram_show_usage(void)
{
	printf("Usage: nvram [commands]\n");
	printf("\n");
	printf("  [commands]--\n");
	printf("    get <name>          get nvram value by <name>\n");
	printf("    set <name> <value>  set nvram <name> with <value>\n");
	printf("    unset <name>        remove nvram <name> and its <value>\n");
	printf("    list                list all nvram\n");
	printf("    commit              save nvram to storage device\n");
	printf("\n");
}

int nvram_main(int argc, char **argv)
{
	int rc = 0;
	char buf[1024];
	char msg[4096];
	int msg_len, n;
	struct ezcfg *ezcfg = NULL;
	struct ezcfg_ctrl *ezctrl = NULL;
	struct ezcfg_soap_http *sh = NULL;
	struct ezcfg_soap *soap = NULL;
	struct ezcfg_http *http = NULL;

	memset(buf, 0, sizeof(buf));
	memset(msg, 0, sizeof(msg));

	debug = true;

	ezcfg = ezcfg_new();
	if (ezcfg == NULL) {
		printf("error : %s\n", "ezcfg_new");
		rc = 1;
		goto exit;
	}

	ezcfg_log_init("nvram");
	ezcfg_set_log_fn(ezcfg, log_fn);
	info(ezcfg, "version %s\n", VERSION);

	sh = ezcfg_soap_http_new(ezcfg);
	if (sh == NULL) {
		err(ezcfg, "%s\n", "Cannot initialize nvram SOAP/HTTP builder");
		rc = 2;
		goto exit;
	}

	soap = ezcfg_soap_http_get_soap(sh);
	http = ezcfg_soap_http_get_http(sh);

	if (argc < 2) {
		err(ezcfg, "need more arguments.\n");
		rc = 1;
		nvram_show_usage();
		goto exit;
	}

	if (strcmp(argv[1], "get") == 0) {
		if (argc != 3) {
			err(ezcfg, "number of arguments is incorrect.\n");
			rc = 1;
			nvram_show_usage();
			goto exit;
		}

		if (strlen(argv[2]) > 0) {
			/* build SOAP */
			ezcfg_soap_http_set_soap_version_major(sh, 1);
			ezcfg_soap_http_set_soap_version_minor(sh, 2);

			/* SOAP Envelope */
			ezcfg_soap_set_envelope(soap, EZCFG_IGRS_ENVELOPE_ELEMENT_NAME);
			buf[0] = '\0';
			n = ezcfg_soap_write(soap, buf, sizeof(buf));
			ezcfg_http_set_message_body(http, buf, n);

			/* build HTTP request line */
			ezcfg_http_set_request_method(http, EZCFG_SOAP_HTTP_METHOD_GET);
			snprintf(buf, sizeof(buf), "/ezcfg/nvram/getNvram/%s", argv[2]);
			ezcfg_http_set_request_uri(http, buf);
			ezcfg_http_set_version_major(http, 1);
			ezcfg_http_set_version_minor(http, 1);
		}
		else {
			err(ezcfg, "the nvram name is empty.\n");
			rc = 1;
			nvram_show_usage();
			goto exit;
		}
	}
	else if (strcmp(argv[1], "set") == 0) {
		if (argc != 4) {
			err(ezcfg, "number of arguments is incorrect.\n");
			rc = 1;
			nvram_show_usage();
			goto exit;
		}

		if (strlen(argv[2]) > 0) {
		}
		else {
			err(ezcfg, "the nvram name is empty.\n");
			rc = 1;
			nvram_show_usage();
			goto exit;
		}
	}
	else if (strcmp(argv[1], "unset") == 0) {
		if (argc != 3) {
			err(ezcfg, "number of arguments is incorrect.\n");
			rc = 1;
			nvram_show_usage();
			goto exit;
		}

		if (strlen(argv[2]) > 0) {
		}
		else {
			err(ezcfg, "the nvram name is empty.\n");
			rc = 1;
			nvram_show_usage();
			goto exit;
		}
	}
	else if (strcmp(argv[1], "list") == 0) {
		if (argc != 2) {
			err(ezcfg, "number of arguments is incorrect.\n");
			rc = 1;
			nvram_show_usage();
			goto exit;
		}

	}
	else if (strcmp(argv[1], "commit") == 0) {
		if (argc != 2) {
			err(ezcfg, "number of arguments is incorrect.\n");
			rc = 1;
			nvram_show_usage();
			goto exit;
		}
	}

	msg_len = ezcfg_soap_http_write_message(sh, msg, sizeof(msg));
	info(ezcfg, "debug\n");

	snprintf(buf, sizeof(buf), "%s-%d", EZCFG_NVRAM_SOCK_PATH, getpid());
	ezctrl = ezcfg_ctrl_new_from_socket(ezcfg, AF_LOCAL, EZCFG_PROTO_SOAP_HTTP, buf, EZCFG_NVRAM_SOCK_PATH);

	if (ezctrl == NULL) {
		err(ezcfg, "%s\n", "Cannot initialize nvram controller");
		rc = 2;
		goto exit;
	}

	if (ezcfg_ctrl_connect(ezctrl) < 0) {
		err(ezcfg, "controller connect fail: %m\n");
		rc = 3;
		goto exit;
	}

	if (ezcfg_ctrl_write(ezctrl, msg, msg_len, 0) < 0) {
		err(ezcfg, "controller write: %m\n");
		rc = 4;
		goto exit;
	}
	info(ezcfg, "sent message=[%s]\n\n\n", msg);

	if (ezcfg_ctrl_read(ezctrl, msg, sizeof(msg), 0) < 0) {
		err(ezcfg, "controller write: %m\n");
		rc = 5;
		goto exit;
	}
	info(ezcfg, "received message=[%s]\n", msg);

exit:
        if (sh != NULL)
                ezcfg_soap_http_delete(sh);

        if (ezctrl != NULL)
                ezcfg_ctrl_delete(ezctrl);

        if (ezcfg != NULL)
                ezcfg_delete(ezcfg);

        return rc;
}
