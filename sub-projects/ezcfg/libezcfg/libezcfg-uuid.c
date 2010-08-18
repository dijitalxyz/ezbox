/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg-uuid.c
 *
 * Description  : implement UUID functions (RFC 4122)
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-08-06   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

#include "libezcfg.h"
#include "libezcfg-private.h"

/* UUID binary representation according to UUID standards */
struct uuid_binary {
	unsigned int time_low; /* bits 0-31 of time field */
	unsigned short time_mid; /* bits 32-47 of time field */
	unsigned short time_hi_and_version; /* bits 48-59 of time field plus 4 bit version */
	unsigned char clock_seq_hi_and_reserved; /* bits 8-13 of clock sequence field plus 2 bit variant */
	unsigned char clock_seq_low; /* bits 0-7 of clock sequence field */
	unsigned char node[6]; /* bits 0-47 of node MAC address */
};


struct ezcfg_uuid {
	struct ezcfg *ezcfg;
	int version;
	struct uuid_binary uuid_bin;
};

bool ezcfg_uuid_delete(struct ezcfg_uuid *uuid)
{
	struct ezcfg *ezcfg;

	ASSERT(uuid != NULL);

	ezcfg = uuid->ezcfg;

	free(uuid);
	return true;
}

struct ezcfg_uuid *ezcfg_uuid_new(struct ezcfg *ezcfg)
{
	struct ezcfg_uuid *uuid;

	ASSERT(ezcfg != NULL);

	uuid = calloc(1, sizeof(struct ezcfg_uuid));
	if (uuid == NULL) {
		err(ezcfg, "can not calloc uuid\n");
		return NULL;
	}

	memset(uuid, 0, sizeof(struct ezcfg_uuid));
	uuid->ezcfg = ezcfg;
	return uuid;
}

bool ezcfg_uuid_set_version(struct ezcfg_uuid *uuid, int version)
{
	struct ezcfg *ezcfg;

	ASSERT(uuid != NULL);

	ezcfg = uuid->ezcfg;

	uuid->version = version;
	return true;
}
