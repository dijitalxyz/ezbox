/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : ezcm.c
 *
 * Description  : ezbox config interface
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-06-13   0.1       Write it from scratch
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
		char buf[1024];
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

static void ezcm_show_usage(void)
{
	printf("Usage: ezcm [-m message]\n");
	printf("\n");
	printf("  -m\tmessage sent to ezcd\n");
	printf("  -D\tdebug mode\n");
	printf("  -h\thelp\n");
	printf("\n");
}

int ezcm_main(int argc, char **argv)
{
	int c = 0;
	int rc = 0;
	char buf[32];
	char msg[4096];
	int msg_len;
	struct ezcfg *ezcfg = NULL;
	struct ezcfg_igrs *igrs = NULL;
	struct ezcfg_ctrl *ezctrl = NULL;

	memset(buf, 0, sizeof(buf));
	memset(msg, 0, sizeof(msg));
	for (;;) {
		c = getopt( argc, argv, "Dhm:");
		if (c == EOF) break;
		switch (c) {
			case 'm':
				break;
			case 'D':
				debug = true;
				break;
			case 'h':
			default:
				ezcm_show_usage();
				return 0;
		}
        }

	//conn_fd = ezcd_client_connection(EZCD_SOCKET_PATH, 'a');
	ezcfg = ezcfg_new();
	if (ezcfg == NULL) {
		printf("error : %s\n", "ezcfg_new");
		rc = 1;
		goto exit;
	}

	ezcfg_log_init("ezcm");
	ezcfg_set_log_fn(ezcfg, log_fn);
	info(ezcfg, "version %s\n", VERSION);

	dbg(ezcfg, "debug\n");
	igrs = ezcfg_igrs_new(ezcfg);
	dbg(ezcfg, "debug\n");
	if (igrs == NULL) {
		err(ezcfg, "%s\n", "Cannot initialize ezcm igrs builder");
		rc = 2;
		goto exit;
	}

	dbg(ezcfg, "debug\n");
	ezcfg_igrs_build_message(igrs, "CreateSessionRequest");
	dbg(ezcfg, "debug\n");
	msg_len = ezcfg_igrs_write_message(igrs, msg, sizeof(msg));

#if 0
	xml = ezcfg_xml_new(ezcfg);
	if (xml == NULL) {
		err(ezcfg, "%s\n", "Cannot initialize ezcm xml parser");
		rc = 2;
		goto exit;
	}

	elem = ezcfg_xml_element_new(xml, EZCFG_SOAP_ENV_ELEMENT_NAME, NULL);
	if (elem == NULL) {
		err(ezcfg, "%s\n", "Cannot initialize ezcm xml element");
		rc = 2;
		goto exit;
	}
	ezcfg_xml_element_add_attribute(xml, elem, EZCFG_SOAP_ENV_NS_NAME, EZCFG_SOAP_ENV_NS_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_xml_element_add_attribute(xml, elem, EZCFG_SOAP_ENV_ENC_NAME, EZCFG_SOAP_ENV_ENC_VALUE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	ezcfg_xml_add_element(xml, NULL, NULL, elem);

	elem2 = ezcfg_xml_element_new(xml, EZCFG_SOAP_BODY_ELEMENT_NAME, NULL);
	if (elem2 == NULL) {
		err(ezcfg, "%s\n", "Cannot initialize ezcm xml element");
		rc = 2;
		goto exit;
	}
	ezcfg_xml_add_element(xml, elem, NULL, elem2);

	ezcfg_xml_write(xml, soap_buf, sizeof(soap_buf));


	snprintf(msg, sizeof(msg),
"M-POST /IGRS HTTP/1.1\r\n"
"HOST: 192.168.1.1:3880\r\n"
"MAN: \"http://www.igrs.org/session\"; ns=01\r\n"
"01-IGRSVersion: IGRS/1.0\r\n"
"01-IGRSMessageType:CreateSessionRequest\r\n"
"01-TargetDeviceId: \r\n"
"01-SourceDeviceId: \r\n"
"01-SequenceId: 1\r\n"
"Content-type: text/xml; charset=utf-8\r\n"
"Content-length: %d\r\n"
"MAN: \"http://www.w3.org/2002/12/soap-envelope\"; ns=02\r\n"
"02-SoapAction: \"IGRS-CreateSession-Request\"\r\n"
"\r\n"
"%s",strlen(soap_buf), soap_buf);
#endif
	snprintf(buf, sizeof(buf), "%s-%d", EZCFG_CTRL_SOCK_PATH, getpid());

	ezctrl = ezcfg_ctrl_new_from_socket(ezcfg, AF_LOCAL, EZCFG_PROTO_IGRS, buf);

	if (ezctrl == NULL) {
		err(ezcfg, "%s\n", "Cannot initialize ezcm controller");
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
	info(ezcfg, "sent message=[%s]\n", msg);

#if 1
	if (ezcfg_ctrl_read(ezctrl, msg, sizeof(msg), 0) < 0) {
		err(ezcfg, "controller write: %m\n");
		rc = 5;
		goto exit;
	}
	info(ezcfg, "received message=[%s]\n", msg);
#endif
exit:
	if (igrs != NULL)
		ezcfg_igrs_delete(igrs);

	if (ezctrl != NULL)
		ezcfg_ctrl_delete(ezctrl);

	if (ezcfg != NULL)
		ezcfg_delete(ezcfg);

	return rc;
}
