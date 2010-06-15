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

#ifdef DEBUG
#define DBG_PRINT(format, args...) fprintf(stderr, format, ##args)
#else
#define DBG_PRINT(format, args...)
#endif

void show_usage(void)
{
	printf("Usage: ezcd [-d]\n");
	printf("\n");
	printf("  -d          daemonize\n");
	printf("\n");
	exit(1);
}

void perror_msg_and_die(char *text)
{
	fprintf(stderr, "%s\n", text);
	exit(1);
}

void error_msg_and_die(char *text)
{
	perror_msg_and_die(text);
}

int main(int argc, char **argv)
{
	int daemonize = 0;
	int c = 0;

	for (;;) {
		c = getopt( argc, argv, "hd");
		if (c == EOF) break;
		switch (c) {
			case 'd':
				daemonize = 1;
				break;
			case 'h':
			default:
				show_usage();
				exit(1);
		}
        }

	if (daemonize)
	{
		DBG_PRINT("  daemonizing\n");
		if (daemon(0, 1) < 0)
			perror_msg_and_die("daemon");
	}

	do {
		sleep(1);
	} while(1);

	return 0;
}
