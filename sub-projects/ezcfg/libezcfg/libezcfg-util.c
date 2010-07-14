/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg-util.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-12   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "libezcfg.h"
#include "libezcfg-private.h"

extern void *mempcpy(void *dest, const void *src, size_t n);

int util_log_priority(const char *priority)
{
	char *endptr;
	int prio;

	prio = strtol(priority, &endptr, 10);
	if (endptr[0] == '\0' || isspace(endptr[0]))
		return prio;
	if (strncmp(priority, "err", 3) == 0)
		return LOG_ERR;
	if (strncmp(priority, "info", 4) == 0)
		return LOG_INFO;
	if (strncmp(priority, "debug", 5) == 0)
		return LOG_DEBUG;
	return 0;
}

void util_remove_trailing_chars(char *path, char c)
{
	size_t len;

	if (path == NULL)
		return;
	len = strlen(path);
	while (len > 0 && path[len-1] == c)
		path[--len] = '\0';
}

/*
 * Concatenates strings. In any case, terminates in _all_ cases with '\0'
 * and moves the @dest pointer forward to the added '\0'. Returns the
 * remaining size, and 0 if the string was truncated.
 */
size_t util_strpcpy(char **dest, size_t size, const char *src)
{
	size_t len;

	len = strlen(src);
	if (len >= size) {
		if (size > 1)
			*dest = mempcpy(*dest, src, size-1);
		size = 0;
		*dest[0] = '\0';
	} else {
		if (len > 0) {
			*dest = mempcpy(*dest, src, len);
			size -= len;
		}
		*dest[0] = '\0';
	}
	return size;
}

/* copies string */
size_t util_strscpy(char *dest, size_t size, const char *src)
{
	char *s;

	s = dest;
	return util_strpcpy(&s, size, src);
}
