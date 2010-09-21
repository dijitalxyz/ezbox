#ifndef _EZCD_H_
#define _EZCD_H_

#include <sys/types.h>
#include <sys/param.h>
#include <signal.h>

#include "libezcfg.h"
#include "libezcfg-private.h"

#include "ezcfg-api.h"

#define EZCD_VERSION(a,b,c,d) ((((a) << 24) + (((b) << 16) + ((c) <<8))) + (d))
#define EZCD_VERSION_MAJOR(x) (((x) >> 24) & 0xFF)
#define EZCD_VERSION_MINOR(x) (((x) >> 16) & 0xFF)
#define EZCD_VERSION_MICRO(x) (((x) >> 8) & 0xFF)
#define EZCD_VERSION_REV(x)   ((x) & 0xFF)

#define EZCD_HEADER_SIZE	16

#define EZCD_SPACE		0x20000 /* 128K Bytes */

#define EZCD_SOCKET_PATH	"/tmp/ezcd/ezcd.socket"

#define SOCKERR_IO          -1
#define SOCKERR_CLOSED      -2
#define SOCKERR_INVARG      -3
#define SOCKERR_TIMEOUT     -4
#define SOCKERR_OK          0


typedef struct ezcd_header_s {
	unsigned char 	magic[4];
	unsigned int 	length;
	unsigned int 	version;
	unsigned int 	checksum;
} ezcd_header_t;

struct ezcd_context;

struct ezcd_context *ezcd_start(void);
void ezcd_stop(struct ezcd_context *ctx);
void ezcd_set_threads_max(struct ezcd_context *ctx, int threads_max);
int ezcd_set_listening_socket(struct ezcd_context *ctx, char *sock_name);

int ezcd_client_connection (const char *name, char project);

/* command entry point */
int ezcd_main(int argc, char **argv);
int ezci_main(int argc, char **argv);
int ezcm_main(int argc, char **argv);
int init_main(int argc, char **argv);
int nvram_main(int argc, char **argv);

#endif
