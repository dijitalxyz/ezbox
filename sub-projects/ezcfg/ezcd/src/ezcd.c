/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : ezcd.c
 *
 * Description  : ezbox config daemon main program
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
#include <sys/poll.h>
#include <sys/signalfd.h>

#include "ezcd.h"

#define EZCD_PRIORITY                  -4
#define EZCI_PRIORITY                  -2

static int ezcd_exit;
static int ezcd_state;
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

static void signal_handler(int sig_num)
{
	if (sig_num == SIGCHLD) {
		do {
			/* nothing */
		} while (waitpid(-1, &sig_num, WNOHANG) > 0);
	}
	else if (sig_num == SIGHUP) {
		ezcd_state = RC_RELOAD;
	}
	else {
		ezcd_exit = 1;
	}
}

static void ezcd_show_usage(void)
{
	printf("Usage: ezcd [-d] [-D] [-c max_worker_threads]\n");
	printf("\n");
	printf("  -d\tdaemonize\n");
	printf("  -D\tdebug mode\n");
	printf("  -c\tmax worker threads\n");
	printf("  -h\thelp\n");
	printf("\n");
}

static int mem_size_mb(void)
{
	FILE *fp;
	char buf[1024];
	long int memsize = -1;

	fp = fopen("/proc/meminfo", "r");
	if (fp == NULL)
		return -1;

	while (fgets(buf, sizeof(buf), fp) != NULL) {
		long int value;

		if (sscanf(buf, "MemTotal: %ld kB", &value) == 1) {
			memsize = value / 1024;
			break;
		}
	}

	fclose(fp);
	return memsize;
}

int ezcd_main(int argc, char **argv)
{
	bool daemonize = false;
	int fd = -1;
	FILE *fp = NULL;
	int threads_max = 0;
	struct ezcfg *ezcfg = NULL;
	struct ezcfg_master *master = NULL;
	int rc = 1;
	int ret = -1;

	for (;;) {
		int c;
		c = getopt( argc, argv, "c:dDh");
		if (c == EOF) break;
		switch (c) {
			case 'c':
				threads_max = atoi(optarg);
				break;
			case 'd':
				daemonize = true;
				break;
			case 'D':
				debug = true;
				break;
			case 'h':
			default:
				ezcd_show_usage();
				goto exit;
		}
        }

	if (getuid() != 0) {
		fprintf(stderr, "root privileges required\n");
		exit(EXIT_FAILURE);
	}

	/* set umask before creating any file/directory */
	ret = chdir("/");
	umask(022);

	mkdir("/var/ezcfg", 0777);

	/* before opening new files, make sure std{in,out,err} fds are in a sane state */
	fd = open("/dev/null", O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "cannot open /dev/null\n");
	}
	if (write(STDOUT_FILENO, 0, 0) < 0)
		dup2(fd, STDOUT_FILENO);
	if (write(STDERR_FILENO, 0, 0) < 0)
		dup2(fd, STDERR_FILENO);

	ezcd_exit = 0;
	ezcd_state = RC_BOOT;
	signal(SIGCHLD, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGHUP, signal_handler);

	if (!debug) {
		dup2(fd, STDIN_FILENO);
		dup2(fd, STDOUT_FILENO);
		dup2(fd, STDERR_FILENO);
	}
	if (fd > STDERR_FILENO)
		close(fd);

	if (daemonize)
	{
		pid_t pid;

		pid = fork();
		switch (pid) {
		case 0:
			/* child process */
			break;

		case -1:
			/* error */
			rc = 4;
			goto exit;

		default:
			/* parant process */
			rc = 0;
			goto exit;
		}
	}

	/* main process */
	ezcfg = ezcfg_new();
	if (ezcfg == NULL)
		goto exit;

	ezcfg_log_init("ezcd");
	ezcfg_set_log_fn(ezcfg, log_fn);
	info(ezcfg, "version %s\n", VERSION);

	master = ezcfg_master_start(ezcfg);
	if (master == NULL) {
		err(ezcfg, "%s\n", "Cannot initialize ezcd master");
		rc = 3;
		goto exit;
	}
	ezcd_state = RC_START;

	if (threads_max <= 0) {
		int memsize = mem_size_mb();

		/* set value depending on the amount of RAM */
		if (memsize > 0)
			threads_max = 2 + (memsize / 8);
		else
			threads_max = 2;
	}
	ezcfg_master_set_threads_max(master, threads_max);

        /* set scheduling priority for the main daemon process */
	setpriority(PRIO_PROCESS, 0, EZCD_PRIORITY);

	setsid();

	fp = fopen("/dev/kmsg", "w");
	if (fp != NULL) {
		fprintf(fp, "<6>ezcd: starting version " VERSION "\n");
		fclose(fp);
        }

	while (ezcd_exit == 0) {
		sleep(1);
		if (ezcd_state == RC_RELOAD) {
			ezcfg_master_reload(master);
			ezcd_state = RC_START;
		}
	}
	rc = 0;

exit:
	ezcfg_master_stop(master);
	ezcfg_delete(ezcfg);
	ezcfg_log_close();
	return rc;
}
