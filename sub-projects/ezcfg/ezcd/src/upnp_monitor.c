/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : upnp_monitor.c
 *
 * Description  : ezbox upnp monitor daemon program
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-19   0.1       Write it from scratch
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
#include <dlfcn.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/sysinfo.h>

#include "ezcd.h"

#if 1
#define DBG(format, args...) do {\
	FILE *fp = fopen("/dev/kmsg", "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

typedef struct task_node_s {
	char *cmd;
	long time;
	struct task_node_s *next;
} task_node_t;

static bool running = false;
static bool alarmed = false;

static void terminate_handler(int sig)
{
	DBG("<6>upnp_monitor: terminated\n");
	running = false;
}

static void alarm_handler(int sig)
{
	DBG("<6>upnp_monitor: alarmed\n");
	alarmed = true;
}

static void delete_task(task_node_t *task)
{
	if (task != NULL) {
		if (task->cmd != NULL)
			free(task->cmd);
		free(task);
	}
}

static task_node_t *insert_task(task_node_t *head, task_node_t *task)
{
	task_node_t *tp, *pre;

	if (task == NULL) {
		return head;
	}
	if (head == NULL) {
		task->next = NULL;
		return task;
	}

	pre = NULL;
	tp = head;
	while((tp != NULL) && (task->time >= tp->time)) {
		pre = tp;
		tp = tp->next;
	}

	if (pre == NULL) {
		/* task should be the first node in queue */
		task->next = head;
		return task;
	}
	else {
		pre->next = task;
		task->next = tp;
		return head;
	}
}

int upnp_monitor_main(int argc, char **argv)
{
	pid_t pid;
	proc_stat_t *pidList;
	task_node_t *task_queue = NULL, *task;
	struct sysinfo si;
	int i;

	/* first check if upnp_monitor has run */
	pid = getpid();
	pidList = utils_find_pid_by_name("upnp_monitor");
	if (pidList == NULL) {
		printf("find upnp_monitor pid error!\n");
		return (EXIT_FAILURE);
	}

	for (i=0; pidList[i].pid > 0; i++) {
		if (pidList[i].pid != pid) {
			free(pidList);
			printf("upnp_monitor is running!\n");
			return (EXIT_FAILURE);
		}
	}

	free(pidList);

	/* then fork to run as a daemon */
	pid = fork();
	if (pid < 0) {
		DBG("<6>upnp_monitor: can't fork");
		return (EXIT_FAILURE);
	}
	if (pid > 0) {
		return (EXIT_SUCCESS); /* parent */
	}

	/* child */
	/* unset umask */
	umask(0);

	signal(SIGTERM, terminate_handler);
	signal(SIGALRM, alarm_handler);

	running = true;
	alarmed = false;
	/* main loop */
	while (running == true) {
		/* read new task from task file */
		if (sysinfo(&si) == 0) {
			task = malloc(sizeof(task_node_t));
			if (task != NULL) {
				memset(task, 0, sizeof(task_node_t));
				task->cmd = strdup("ezcm upnp ssdp notify_alive");
				task->time = si.uptime+20;
				task->next = NULL;
				task_queue = insert_task(task_queue, task);
			}
		}

		/* execute task in queue */
		if (sysinfo(&si) < 0) {
			DBG("<6>upnp_monitor: sysinfo\n");
			continue;
		}
		task = task_queue;
		while((task != NULL) && (task->time <= si.uptime)) {
			if (task->cmd != NULL) {
				utils_system(task->cmd);
			}
			task_queue = task->next;
			delete_task(task);
			task = task_queue;
		}

		/* do next loop if alarmed */
		if (alarmed == true) {
			alarmed = false;
			continue;
		}

		/* setup sleep time */
		if (task == NULL) {
			/* wait for signal */
			pause();
		}
		else {
			if (sysinfo(&si) < 0) {
				DBG("<6>upnp_monitor: sysinfo\n");
				continue;
			}
			if (task->time > si.uptime) {
				sleep(task->time - si.uptime);
			}
		}
	}

	/* clean up */
	return (EXIT_SUCCESS);
}
