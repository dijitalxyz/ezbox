#ifndef _EZCD_H_
#define _EZCD_H_

#include "libezcfg.h"
#include "libezcfg-private.h"

#include "ezcfg-api.h"

#define STRUCT_LEN(name)	sizeof(name)/sizeof(name[0])

/* rc state */
enum {
	RC_BOOT,
	RC_RESTART,
	RC_STOP,
	RC_START,
};

struct rc_func {
	char *name;
	int flag;
	int (*func)(int flag);
};

/* utils function declaration */
char * utils_get_kernel_version(void);
struct rc_func * utils_find_rc_func(char *name);

/* command entry point */
int preinit_main(int argc, char **argv);
int ezcd_main(int argc, char **argv);
int ezcm_main(int argc, char **argv);
int nvram_main(int argc, char **argv);
int rc_main(int argc, char **argv);
int ubootenv_main(int argc, char **argv);

#endif
