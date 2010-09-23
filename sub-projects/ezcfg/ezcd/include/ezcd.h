#ifndef _EZCD_H_
#define _EZCD_H_

#include "libezcfg.h"
#include "libezcfg-private.h"

#include "ezcfg-api.h"

/* command entry point */
int preinit_main(int argc, char **argv);
int ezcd_main(int argc, char **argv);
int ezcm_main(int argc, char **argv);
int init_main(int argc, char **argv);
int nvram_main(int argc, char **argv);

#endif
