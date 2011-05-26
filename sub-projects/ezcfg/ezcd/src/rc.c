/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc.c
 *
 * Description  : ezbox rc program
 *
 * Copyright (C) 2010 by ezbox-project
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

#if 0
#include <linux/sem.h>
#endif

#include "ezcd.h"

#define RC_SEM_PATH	EZCFG_SEM_ROOT_PATH "/rc_sem"

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

union semun {
	int val;                        /* value for SETVAL */
	struct semid_ds *buf;   /* buffer for IPC_STAT & IPC_SET */
	unsigned short *array;  /* array for GETALL & SETALL */
	struct seminfo *__buf;  /* buffer for IPC_INFO */
	void *__pad;
};

int rc_main(int argc, char **argv)
{
	rc_func_t *f = NULL;
	int flag = RC_BOOT;
	int s = 0;
	int fd = -1;
	pid_t pid;
	int ret = EXIT_FAILURE;
	int key, semid;
	union semun arg;
	struct sembuf require_res, release_res;

	/* only accept two/three arguments */
	if ((argc != 3) && (argc != 4))
		return (EXIT_FAILURE);

	if (argv[1] != NULL) {
		f = utils_find_rc_func(argv[1]);
	}

	if (argv[2] != NULL) {
		if (strcmp(argv[2], "restart") == 0)
			flag = RC_RESTART;
		else if (strcmp(argv[2], "start") == 0)
			flag = RC_START;
		else if (strcmp(argv[2], "stop") == 0)
			flag = RC_STOP;
		else if (strcmp(argv[2], "reload") == 0)
			flag = RC_RELOAD;
	}

	if (argc == 4) {
		s = atoi(argv[3]);
		if ((s < 1) || (s > 90)) {
			return (EXIT_FAILURE);
		}
		//sleep(s);
	}

	if (getuid() != 0) {
		exit(EXIT_FAILURE);
	}

	/* set umask before creating any file/directory */
	ret = chdir("/");
	umask(0022);

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
		ret = EXIT_SUCCESS;
		break;

	case -1:
		/* error */
		return (EXIT_FAILURE);

	default:
		/* parant process */
		DBG("<6>rc: child pid = [%d]\n", pid);
		return (EXIT_SUCCESS);
	}

	/* child process main */

	/* prepare semaphore */
	mkdir(EZCFG_SEM_ROOT_PATH, 0777);
	key = ftok(RC_SEM_PATH, 'a');

	/* create a semaphore set that only includes one semaphore */
	semid = semget(key, 1, IPC_CREAT|IPC_EXCL|00666);
	while (semid < 0) {
		sleep(1);
		DBG("<6>rc: wait 1 second to create sem.\n");
		semid = semget(key, 1, IPC_CREAT|IPC_EXCL|00666);
	}
	DBG("<6>rc: create sem OK.\n");

	/* initialize semaphore */
	arg.val = 1;
	if (semctl(semid, 0, SETVAL, arg) == -1) {
		DBG("<6>rc: semctl setval error\n");
		goto rc_exit;
	}

	/* now require available resource */
	require_res.sem_num = 0;
	require_res.sem_op = -1;
	require_res.sem_flg = SEM_UNDO;

	if (semop(semid, &require_res, 1) == -1) {
		DBG("<6>rc: semop require_res error\n");
		goto rc_exit;
	}

	/* handle rc operations */
	if (flag != RC_BOOT && f != NULL) {
		/* wait s seconds */
		if (s > 0)
			sleep(s);

		ret = f->func(flag);
	}

	/* now release resource */
	release_res.sem_num = 0;
	release_res.sem_op = 1;
	release_res.sem_flg = SEM_UNDO;

	if (semop(semid, &release_res, 1) == -1) {
		DBG("<6>rc: semop release_res error\n");
		goto rc_exit;
	}

rc_exit:
	if (semctl(semid, 0, IPC_RMID) == -1) {
		DBG("<6>rc: semctl IPC_RMID error\n");
	}
	else {
		DBG("<6>rc: remove sem OK.\n");
	}

	/* special actions for rc_system stop/restart */
	if (strcmp("system", argv[1]) == 0) {
		if (flag == RC_STOP) {
			if (utils_ezcd_wait_down(0) == true) {
				DBG("<6>rc: system stop.\n");
				system(CMD_POWEROFF);
			}
		}
		else if (flag == RC_RESTART) {
			if (utils_ezcd_wait_down(0) == true) {
				DBG("<6>rc: system restart.\n");
				system(CMD_REBOOT);
			}
		}
	}

	return (ret);
}
