/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc.c
 *
 * Description  : ezbox rc program
 *
 * Copyright (C) 2008-2012 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-02   0.1       Write it from scratch
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
#include <sys/mount.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
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

#include "ezcd.h"

#if 0
#define DBG(format, args...) do {\
	FILE *dbg_fp = fopen("/tmp/rc.log", "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

/* use example
 * 1. call action directly, rc return immediately,
 *    action runs in background
 *    rc dnsmasq start
 *
 * 2. sleep before call action
 *    rc 0.1 dnsmasq start
 *    sleep 0.1 second then call "dnsmasq start"
 *
 * 3. call action directly, and wait it to be finished
 *    rc -1 dnsmasq start
 *    call "dnsmasq start" and wait
 */
int rc_main(int argc, char **argv)
{
	bool b_sleep = false;
	bool b_fork = true;
	struct timespec req;
	int fd = -1;
	pid_t pid;
	int ret = EXIT_FAILURE;
	int i;
	char *p;
	char path[64];
	char name[32];
	void *handle;
	union {
		rc_function_t func;
		void * obj;
	} alias;
	struct ezcfg *ezcfg = NULL;
	char *sem_path = NULL;

	/* argv[0] : "rc" */
	/* argv[1]/argv[2] : action name */
	/* argv[2]/argv[3]... : action arguments */
	if (argc < 3)
		return (EXIT_FAILURE);

	if (getuid() != 0) {
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < argc; i++) {
		DBG("<6>rc: argv[%d]=[%s]\n", i, argv[i]);
	}

	i = 1;
	req.tv_sec = 0;
	req.tv_nsec = 0;
	if (isdigit(*argv[i])) {
		req.tv_sec = strtol(argv[i], &p, 10);
		if ((p != NULL) && (*p == '.')) {
			long base = 100000000;
			p++;
			while(isdigit(*p) && base > 0) {
				req.tv_nsec += (*p - '0')*base;
				base /= 10;
				p++;
			}
#if 0
			req.tv_nsec = strtol(p, (char **) NULL, 10);
			if (req.tv_nsec > 999999999)
				req.tv_nsec = 999999999;
#endif
		}
		DBG("<6>rc: req.tv_sec=[%ld]\n", req.tv_sec);
		DBG("<6>rc: req.tv_nsec=[%ld]\n", req.tv_nsec);
		if ((req.tv_sec > 0) || ((req.tv_sec == 0) && (req.tv_nsec > 0))) {
			b_sleep = true;
		}
		i++;
	}
	else if (strcmp(argv[i], "-1") == 0) {
		b_fork = false;
		i++;
	}

	/* set umask before creating any file/directory */
	ret = chdir("/");
	umask(0022);

	if (b_fork == true) {
		/* before opening new files, make sure std{in,out,err} fds are in a same state */
		fd = open("/dev/null", O_RDWR);
		if (fd < 0) {
			exit(EXIT_FAILURE);
		}
		dup2(fd, STDIN_FILENO);
		dup2(fd, STDOUT_FILENO);
		dup2(fd, STDERR_FILENO);
		if (fd > STDERR_FILENO)
			close(fd);

		/* daemonize */
		pid = fork();
		switch (pid) {
		case 0:
			/* child process */
			DBG("<6>rc: fork() to child process\n");
			ret = EXIT_SUCCESS;
			break;

		case -1:
			/* error */
			DBG("<6>rc: fork() error\n");
			return (EXIT_FAILURE);

		default:
			/* parant process */
			DBG("<6>rc: child pid = [%d]\n", pid);
			return (EXIT_SUCCESS);
		}
	}

	/* child process main */
	snprintf(name, sizeof(name), "rc_%s", argv[i]);
	snprintf(path, sizeof(path), "%s/%s.so", RCSO_PATH_PREFIX, name);
	handle = dlopen(path, RTLD_NOW);
	if (handle == NULL) {
		DBG("<6>rc: dlopen(%s) error %s\n", path, dlerror());
		snprintf(path, sizeof(path), "%s/%s.so", RCSO_PATH_PREFIX2, name);
		handle = dlopen(path, RTLD_NOW);
		if (handle == NULL) {
			DBG("<6>rc: dlopen(%s) error %s\n", path, dlerror());
			return (EXIT_FAILURE);
		}
	}

	/* clear any existing error */
	dlerror();

	alias.obj = dlsym(handle, name);

	if ((p = dlerror()) != NULL)  {
		DBG("<6>rc: dlsym error %s\n", p);
		goto rc_exit;
	}

	ezcfg = ezcfg_api_common_new(EZCD_CONFIG_FILE_PATH);
	if (ezcfg == NULL) {
		DBG("<6>rc: %s format error\n", EZCD_CONFIG_FILE_PATH);
		goto rc_exit;
	}
	sem_path = ezcfg_api_common_get_sem_ezcfg_path(ezcfg);
	if ((sem_path == NULL) || (*sem_path == '\0')) {
		DBG("<6>rc: sem_ezcfg_path error\n");
		goto rc_exit;
	}

	/* prepare semaphore */
	if (ezcfg_api_rc_require_semaphore(sem_path) == false) {
		DBG("<6>rc:pid=[%d] require semaphore error.\n", getpid());
		goto rc_exit;
	}

	/* handle rc operations */
	/* wait s seconds */
	if (b_sleep == true) {
		struct timespec rem;
		DBG("<6>rc: sleep!\n");
		if (nanosleep(&req, &rem) == -1) {
			DBG("<6>rc: nanosleep error!\n");
		}
		else {
			DBG("<6>rc: sleep finished!\n");
		}
	}

	ret = alias.func(argc - i, &(argv[i]));

	/* now release resource */
	if (ezcfg_api_rc_release_semaphore(sem_path) == false) {
		DBG("<6>rc:pid=[%d] release semaphore error.\n", getpid());
		goto rc_exit;
	}

rc_exit:
	if (ezcfg != NULL) {
		ezcfg_api_common_delete(ezcfg);
	}

	/* close loader handle */
	dlclose(handle);

	/* special actions for rc_system stop/restart */
#if 0
	if (strcmp("system", argv[1]) == 0) {
		if (strcmp("stop", argv[2]) == 0) {
			if (utils_ezcd_wait_down(0) == true) {
				DBG("<6>rc: system stop.\n");
				/* { SIGUSR1, SIGUSR2, SIGTERM } for ezbox init/halt, poweroff, reboot */
				ret = kill(1, SIGUSR2);
			}
		}
		else if (strcmp("restart", argv[2]) == 0) {
			if (utils_ezcd_wait_down(0) == true) {
				DBG("<6>rc: system restart.\n");
				/* { SIGUSR1, SIGUSR2, SIGTERM } for ezbox init/halt, poweroff, reboot */
				ret = kill(1, SIGTERM);
			}
		}
	}
#endif

	return (ret);
}
