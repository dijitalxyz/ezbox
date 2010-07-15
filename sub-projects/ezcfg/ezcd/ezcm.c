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

static void ezcm_show_usage(void)
{
	printf("Usage: ezcm [-m message]\n");
	printf("\n");
	printf("  -m\tmessage sent to ezcd\n");
	printf("  -h\thelp\n");
	printf("\n");
}

static int sock_read (int fd, void* buff, int count)
{
	void* pts = buff;
	int status = 0, n;

	if (count <= 0) return SOCKERR_OK;

	while (status != count) {
		n = read (fd, pts + status, count - status);

		if (n < 0) {
			if (errno == EINTR) {
				continue;
			}
			else
				return SOCKERR_IO;
		}

		if (n == 0)
			return SOCKERR_CLOSED;

		status += n;
	}

	return status;
}

static int sock_write (int fd, const void* buff, int count)
{
	const void* pts = buff;
	int status = 0, n;

	if (count < 0) return SOCKERR_OK;

	while (status != count) {
		n = write (fd, pts + status, count - status);
		if (n < 0) {
			if (errno == EPIPE)
				return SOCKERR_CLOSED;
			else if (errno == EINTR) {
				continue;
			}
			else
				return SOCKERR_IO;
		}
		status += n;
	}

	return status;
}

int ezcm_main(int argc, char **argv)
{
	int c = 0;
	int conn_fd = -1;
	int n = 0;
	char buf[32];
	char data[32];

	memset(buf, 0, sizeof(buf));
	for (;;) {
		c = getopt( argc, argv, "hm:");
		if (c == EOF) break;
		switch (c) {
			case 'm':
				snprintf(buf, sizeof(buf), "%s\r\n\r\n", optarg);
				break;
			case 'h':
			default:
				ezcm_show_usage();
				exit(EXIT_FAILURE);
		}
        }

	//conn_fd = ezcd_client_connection(EZCD_SOCKET_PATH, 'a');
	if (conn_fd < 0) {
		printf("error : %s\n", "ezcd_client_connection");
		exit(EXIT_FAILURE);
	}

        n = sock_write (conn_fd, buf, strlen(buf)+1);
        if (n == SOCKERR_IO) {
            printf ("write error on fd %d\n", conn_fd);
        }
        else if (n == SOCKERR_CLOSED) {
            printf ("fd %d has been closed.\n", conn_fd);
        }
        else
            printf ("Wrote %s to server. \n", buf);

	memset(data, 0, sizeof(data));
        n = sock_read (conn_fd, data, strlen(buf)+1);
        if (n == SOCKERR_IO) {
            printf ("read error on fd %d\n", conn_fd);
        }
        else if (n == SOCKERR_CLOSED) {
            printf ("fd %d has been closed.\n", conn_fd);
        }
        else
            printf ("Got that! data is %s\n", data);

	return (EXIT_SUCCESS);
}
