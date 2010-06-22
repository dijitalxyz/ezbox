/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : main.c
 *
 * Description  : EZCD main program
 *
 * Copyright (C) 2010 by TANG HUI
 *
 * History      Rev       Description
 * 2010-06-13   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>

#include "ezcd.h"

int threads_max;

static int ezcd_exit;

static void signal_handler(int sig_num)
{
	if (sig_num == SIGCHLD) {
		do {
			/* nothing */
		} while (waitpid(-1, &sig_num, WNOHANG) > 0);
	} else {
		ezcd_exit = sig_num;
	}
}

static void show_usage(void)
{
	printf("Usage: ezcd [-d] [-c max_worker_threads]\n");
	printf("\n");
	printf("  -d\tdaemonize\n");
	printf("  -c\tmax worker threads\n");
	printf("  -h\thelp\n");
	printf("\n");
}

static void perror_msg(char *text)
{
	fprintf(stderr, "%s\n", text);
}

static int mem_size_mb(void)
{
	FILE *fp;
	char buf[4096];
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

int main(int argc, char **argv)
{
	int daemonize = 0;
	int c = 0;
	struct ezcd_context *ctx;

	threads_max = 0;
	for (;;) {
		c = getopt( argc, argv, "c:dh");
		if (c == EOF) break;
		switch (c) {
			case 'c':
				threads_max = atoi(optarg);
				break;
			case 'd':
				daemonize = 1;
				break;
			case 'h':
			default:
				show_usage();
				exit(EXIT_FAILURE);
		}
        }

	ezcd_exit = 0;
	signal(SIGCHLD, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);

	ctx = ezcd_start();
	if (ctx == NULL) {
		printf("%s\n", "Cannot initialize ezcd context");
		exit(EXIT_FAILURE);
	}

	if (threads_max <= 0) {
		int memsize = mem_size_mb();

		/* set value depending on the amount of RAM */
		if (memsize > 0)
			threads_max = 2 + (memsize / 8);
		else
			threads_max = 2;
	}

	if (daemonize)
	{
		printf("daemonizing\n");
		if (daemon(0, 1) < 0) {
			perror_msg("daemon");
			ezcd_stop(ctx);
			exit(EXIT_FAILURE);
		}
	}

	fflush(stdout);
	while (ezcd_exit == 0)
		sleep(1);

	printf("Exiting on signal %d, "
		"waiting for all threads to finish...", ezcd_exit);
	fflush(stdout);
	ezcd_stop(ctx);
	printf("%s", " done.\n");

	return (EXIT_SUCCESS);
}

