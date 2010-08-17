/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg-thread.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
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

#include "libezcfg.h"
#include "libezcfg-private.h"

/*
 * ezcfg-thread - ezbox config multi-threads model
 *
 */

int ezcfg_thread_start(struct ezcfg *ezcfg, int stacksize, ezcfg_thread_func_t func, void *param)
{
	pthread_t thread_id;
	pthread_attr_t attr;
	int retval;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (stacksize > 0)
		pthread_attr_setstacksize(&attr, stacksize);

	if ((retval = pthread_create(&thread_id, &attr, func, param)) != 0) {
		err(ezcfg, "%s: %s", __func__, strerror(retval));
	}
	return retval;
}

