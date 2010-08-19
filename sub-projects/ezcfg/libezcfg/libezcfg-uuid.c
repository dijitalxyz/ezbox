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
#include <sys/time.h>
#include <pthread.h>

#include "libezcfg.h"
#include "libezcfg-private.h"

/* set the following to the number of 100ns ticks of the actual
   resolution of your system's clock, right now we use 1usec = 10*100ns */
#define UUIDS_PER_TICK 10

/* time offset between UUID and Unix Epoch time according to standards.
   (UUID UTC base time is October 15, 1582
    Unix UTC base time is January  1, 1970) */
#define UUID_TIMEOFFSET 0x01B21DD213814000LL


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
	union {
		unsigned char *mac; /* UUID version 1: time, clock and node based */
		void *md5; /* UUID version 3: name based with MD5 */
		void *prng; /* UUID version 4: random number based */
		void *sha1; /* UUID version 5: name based with SHA-1 */
        } u;
	struct timeval time_last;
	unsigned long  time_seq;
};

/*
 * Private functions
 */
static bool gen_uuid_v1(struct ezcfg_uuid *uuid)
{
	struct ezcfg *ezcfg;
	struct timeval time_now;
	unsigned long long t;

	ezcfg = uuid->ezcfg;

	/* get time */
	while (1) {
		/* get current system time */
		if (gettimeofday(&time_now, NULL) == -1) {
			err(ezcfg, "can not gettimeofday.\n");
			return false;
		}

		/* check whether system time changed */
		if (!(time_now.tv_sec == uuid->time_last.tv_sec &&
		      time_now.tv_usec == uuid->time_last.tv_usec)) {
			/* reset time sequence counter and continue */
			uuid->time_seq = 0;
			break;
		}

		if (uuid->time_seq < UUIDS_PER_TICK) {
			uuid->time_seq++;
			break;
		}
		usleep(1);
	}

	/* convert to 64-bit
	 * Offset between UUID formatted times and Unix formatted times.
	 * UUID UTC base time is October 15, 1582.
	 * Unix base time is January 1, 1970.
	 */
	t = ( time_now.tv_sec * 10000000 ) + ( time_now.tv_usec * 10 ) + UUID_TIMEOFFSET;

	if (uuid->time_seq > 0) {
		t += uuid->time_seq;
	}

	/* generate node */
	memcpy(uuid->uuid_bin.node, uuid->u.mac, 6);

	return true;
}

/*
 * Public functions
 */

bool ezcfg_uuid_delete(struct ezcfg_uuid *uuid)
{
	struct ezcfg *ezcfg;

	ASSERT(uuid != NULL);

	ezcfg = uuid->ezcfg;

	switch(uuid->version) {
	case 1:
		if (uuid->u.mac)
			free(uuid->u.mac);
		break;
	case 3:
		if (uuid->u.md5)
			free(uuid->u.md5);
		break;
	case 4:
		if (uuid->u.prng)
			free(uuid->u.prng);
		break;
	case 5:
		if (uuid->u.sha1)
			free(uuid->u.sha1);
		break;
	}

	free(uuid);
	return true;
}

struct ezcfg_uuid *ezcfg_uuid_new(struct ezcfg *ezcfg, int version)
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
	uuid->version = version;

	switch(uuid->version) {
	case 1:
		uuid->u.mac = calloc(6, sizeof(unsigned char));
		if (uuid->u.mac == NULL) {
			err(ezcfg, "no enough memory for uuid mac.\n");
			free(uuid);
			return NULL;
		}
		break;
	case 3:
		uuid->u.md5 = calloc(6, sizeof(unsigned char));
		if (uuid->u.md5 == NULL) {
			err(ezcfg, "no enough memory for uuid md5.\n");
			free(uuid);
			return NULL;
		}
		break;
	case 4:
		uuid->u.prng = calloc(6, sizeof(unsigned char));
		if (uuid->u.prng == NULL) {
			err(ezcfg, "no enough memory for uuid prng.\n");
			free(uuid);
			return NULL;
		}
		break;
	case 5:
		uuid->u.sha1 = calloc(6, sizeof(unsigned char));
		if (uuid->u.sha1 == NULL) {
			err(ezcfg, "no enough memory for uuid sha1.\n");
			free(uuid);
			return NULL;
		}
		break;
	default:
		err(ezcfg, "unknown uuid version.\n");
		free(uuid);
		return NULL;
	}
	return uuid;
}

int ezcfg_uuid_get_version(struct ezcfg_uuid *uuid)
{
	struct ezcfg *ezcfg;

	ASSERT(uuid != NULL);

	ezcfg = uuid->ezcfg;

	return uuid->version;
}

bool ezcfg_uuid_generate(struct ezcfg_uuid *uuid)
{
	struct ezcfg *ezcfg;

	ASSERT(uuid != NULL);

	ezcfg = uuid->ezcfg;

	switch(uuid->version) {
	case 1:
		return gen_uuid_v1(uuid);
		break;

	case 2:
		break;

	case 3:
		break;

	case 4:
		break;

	case 5:
		break;

	default:
		err(ezcfg, "uuid version is incorrect.\n");
		return false;
	}

	return true;
}

bool ezcfg_uuid_export_str(struct ezcfg_uuid *uuid, char *buf, int len)
{
	struct ezcfg *ezcfg;

	ASSERT(uuid != NULL);
	ASSERT(buf != NULL);
	ASSERT(len > EZCFG_UUID_STRING_LEN);

	ezcfg = uuid->ezcfg;

	if (snprintf(buf, EZCFG_UUID_STRING_LEN+1,
	         "%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
	         (unsigned long)uuid->uuid_bin.time_low,
	         (unsigned int)uuid->uuid_bin.time_mid,
	         (unsigned int)uuid->uuid_bin.time_hi_and_version,
	         (unsigned int)uuid->uuid_bin.clock_seq_hi_and_reserved,
	         (unsigned int)uuid->uuid_bin.clock_seq_low,
	         (unsigned int)uuid->uuid_bin.node[0],
	         (unsigned int)uuid->uuid_bin.node[1],
	         (unsigned int)uuid->uuid_bin.node[2],
	         (unsigned int)uuid->uuid_bin.node[3],
	         (unsigned int)uuid->uuid_bin.node[4],
	         (unsigned int)uuid->uuid_bin.node[5]) != EZCFG_UUID_STRING_LEN) {
		err(ezcfg, "export str error.\n");
		return false;
	}
	return true;
}

bool ezcfg_uuid_v1_set_mac(struct ezcfg_uuid *uuid, unsigned char *mac, int len)
{
	struct ezcfg *ezcfg;

	ASSERT(uuid != NULL);
	ASSERT(mac != NULL);
	ASSERT(len >= 6);

	ezcfg = uuid->ezcfg;

	if (uuid->version != 1) {
		err(ezcfg, "uuid version should be 1 for set_mac.\n");
		return false;
	}

	memcpy(uuid->u.mac, mac, 6);

	return true;
}

bool ezcfg_uuid_v1_enforce_multicast_mac(struct ezcfg_uuid *uuid)
{
	struct ezcfg *ezcfg;
	int i;
	time_t t;

	ASSERT(uuid != NULL);

	ezcfg = uuid->ezcfg;

	if (uuid->version != 1) {
		err(ezcfg, "uuid version should be 1 for enforce_multicast_mac.\n");
		return false;
	}

	for(i=0; i<6; i++) {
		/* set random seed */
		srand((unsigned)time(&t));
		uuid->u.mac[i] = rand() & 0x00ff;
	}

	/* set to local multicast MAC address */
	uuid->u.mac[0] |= 0x03;
	uuid->u.mac[0] |= 0x80;
	
	return true;
}
