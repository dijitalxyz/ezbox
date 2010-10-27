/* ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : ubootenv.c
 *
 * Description  : ezcfg API for u-boot environment parameters manipulate
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-10-27   0.1       Write it from scratch
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

#include "libezcfg.h"
#include "libezcfg-private.h"

#include "ezcfg-api.h"

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


/* Check whether full response is buffered. Return:
 *   -1  if response is malformed
 *    0  if response is not yet fully buffered
 *   >0  actual response length, including last \r\n\r\n
 */
static int get_response_len(const char *buf, size_t buflen)
{
	const char *s, *e;
	int len = 0;

	for (s = buf, e = s + buflen - 1; len <= 0 && s < e; s++) {
		/* Control characters are not allowed but >=128 is. */
		if (!isprint(* (unsigned char *) s) && *s != '\r' &&
		    *s != '\n' && * (unsigned char *) s < 128) {
			len = -1;
		} else if (s[0] == '\n' && s[1] == '\n') {
			len = (int) (s - buf) + 2;
		} else if (s[0] == '\n' && &s[1] < e &&
		           s[1] == '\r' && s[2] == '\n') {
			len = (int) (s - buf) + 3;
		}
	}

	return len;
}


/**
 * Keep reading the input into buffer buf, until \r\n\r\n appears in the
 * buffer (which marks the end of HTTP request). Buffer buf may already
 * have some data. The length of the data is stored in nread.
 * Upon every read operation, increase nread by the number of bytes read.
 **/
static int read_response(struct ezcfg_ctrl *ezctrl, char *buf, int bufsiz, int *nread)
{
	int n, response_len;

	ASSERT(ezctrl != NULL);

        response_len = 0;

	while (*nread < bufsiz && response_len == 0) {
		n = ezcfg_socket_read(ezcfg_ctrl_get_socket(ezctrl), buf + *nread, bufsiz - *nread, 0);
		if (n <= 0) {
			break;
		} else {
			*nread += n;
			response_len = get_response_len(buf, (size_t) *nread);
                }
        }

        return response_len;
}



/**
 * ezcfg_api_ubootenv_get:
 * @name: u-boot env parameter name
 * @value: buffer to store u-boot env parameter value
 * @len: buffer size
 *
 **/
int ezcfg_api_ubootenv_get(const char *name, char *value, size_t len)
{
	int rc = 0;

	return rc;
}

/**
 * ezcfg_api_ubootenv_set:
 * @name: u-boot env parameter name
 * @value: buffer stored u-boot env parameter value
 *
 **/
int ezcfg_api_ubootenv_set(const char *name, const char *value)
{
	int rc = 0;

	return rc;
}

/**
 * ezcfg_api_ubootenv_list:
 * @list: buffer to store u-boot env parameter pairs
 * @len: buffer size
 *
 **/
int ezcfg_api_ubootenv_list(char *list, size_t len)
{
	int rc = 0;

	return rc;
}

