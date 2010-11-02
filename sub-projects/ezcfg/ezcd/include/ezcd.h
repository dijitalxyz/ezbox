#ifndef _EZCD_H_
#define _EZCD_H_

#include "libezcfg.h"
#include "libezcfg-private.h"

#include "ezcfg-api.h"

/* rc state */
enum {
	RC_BOOT,
	RC_RESTART,
	RC_STOP,
	RC_START,
};

/* utils function declaration */
char * utils_get_kernel_version(void);

/* command entry point */
int preinit_main(int argc, char **argv);
int ezcd_main(int argc, char **argv);
int ezcm_main(int argc, char **argv);
int nvram_main(int argc, char **argv);
int ubootenv_main(int argc, char **argv);

#endif
