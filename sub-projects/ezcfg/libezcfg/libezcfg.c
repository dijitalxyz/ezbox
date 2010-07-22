/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : libezcfg.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2010 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-12   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>

#include "libezcfg.h"
#include "libezcfg-private.h"

/*
 * ezcfg - library context
 *
 * load/save the ezbox config and system environment
 * allows custom logging
 */

/*
 * ezbox config context
 */
struct ezcfg {
	void (*log_fn)(struct ezcfg *ezcfg,
                       int priority, const char *file, int line, const char *fn,
                       const char *format, va_list args);
	struct ezcfg_list_node properties_list;
	char 		*rules_path;
	int		log_priority;
};

void ezcfg_log(struct ezcfg *ezcfg,
               int priority, const char *file, int line, const char *fn,
               const char *format, ...)
{
	va_list args;

	va_start(args, format);
	ezcfg->log_fn(ezcfg, priority, file, line, fn, format, args);
	va_end(args);
}

static void log_stderr(struct ezcfg *ezcfg,
                       int priority, const char *file, int line, const char *fn,
                       const char *format, va_list args)
{
	fprintf(stderr, "libezcfg: %s: ", fn);
	vfprintf(stderr, format, args);
}

/**
 * ezcfg_set_log_fn:
 * @ezcfg: ezcfg library context
 * @log_fn: function to be called for logging messages
 *
 * The built-in logging writes to stderr. It can be
 * overridden by a custom function, to plug log messages
 * into the users' logging functionality.
 *
 **/
void ezcfg_set_log_fn(struct ezcfg *ezcfg,
                      void (*log_fn)(struct ezcfg *ezcfg,
                                    int priority, const char *file, int line, const char *fn,
                                    const char *format, va_list args))
{
	ezcfg->log_fn = log_fn;
	info(ezcfg, "custom logging function %p registered\n", ezcfg);
}

/**
 * ezcfg_get_log_priority:
 * @ezcfg: ezcfg library context
 *
 * The initial logging priority is read from the ezcfg config file
 * at startup.
 *
 * Returns: the current logging priority
 **/
int ezcfg_get_log_priority(struct ezcfg *ezcfg)
{
	return ezcfg->log_priority;
}

/**
 * ezcfg_set_log_priority:
 * @ezcfg: ezcfg library context
 * @priority: the new logging priority
 *
 * Set the current logging priority. The value controls which messages
 * are logged.
 **/
void ezcfg_set_log_priority(struct ezcfg *ezcfg, int priority)
{
	char num[32];

	ezcfg->log_priority = priority;
	snprintf(num, sizeof(num), "%u", ezcfg->log_priority);
	ezcfg_add_property(ezcfg, "EZCFG_LOG", num);
}

struct ezcfg_list_entry *ezcfg_add_property(struct ezcfg *ezcfg, const char *key, const char *value)
{
	if (value == NULL) {
		struct ezcfg_list_entry *list_entry;

		list_entry = ezcfg_get_properties_list_entry(ezcfg);
		list_entry = ezcfg_list_entry_get_by_name(list_entry, key);
		if (list_entry != NULL)
			ezcfg_list_entry_delete(list_entry);
		return NULL;
	}
        return ezcfg_list_entry_add(ezcfg, &ezcfg->properties_list, key, value, 1, 0);
}

struct ezcfg_list_entry *ezcfg_get_properties_list_entry(struct ezcfg *ezcfg)
{
	return ezcfg_list_get_entry(&ezcfg->properties_list);
}

/**
 * ezcfg_new:
 *
 * Create ezcfg library context.
 *
 * Returns: a new ezcfg library context
 **/

struct ezcfg *ezcfg_new(void)
{
	struct ezcfg *ezcfg = NULL;
	char *config_file = NULL;
	const char *env;
	FILE *fp;

	ezcfg = calloc(1, sizeof(struct ezcfg));
	if (ezcfg) {
		/* initialize ezcfg library context */
		memset(ezcfg, 0, sizeof(struct ezcfg));

		ezcfg->log_fn = log_stderr;
		ezcfg->log_priority = LOG_DEBUG;
		ezcfg_list_init(&ezcfg->properties_list);
		config_file = strdup(SYSCONFDIR "/ezcfg/ezcfg.conf");
		if (config_file == NULL) {
			goto fail_exit;
		}
		env = getenv("EZCFG_CONFIG_FILE");
		if (env != NULL) {
			free(config_file);
			config_file = strdup(env);
			util_remove_trailing_chars(config_file, '/');
		}
		if (config_file == NULL) {
			goto fail_exit;
		}
		fp = fopen(config_file, "re");
		if (fp != NULL) {
			char line[UTIL_LINE_SIZE];
			int line_nr = 0;

			while (fgets(line, sizeof(line), fp)) {
				size_t len;
				char *key;
				char *val;

				line_nr++;

				/* find key */
				key = line;
				while (isspace(key[0]))
					key++;

				/* comment or empty line */
				if (key[0] == '#' || key[0] == '\0')
					continue;

				/* split key/value */
				val = strchr(key, '=');
				if (val == NULL) {
					err(ezcfg, "missing <key>=<value> in '%s'[%i], skip line\n", config_file, line_nr);
					continue;
				}
				val[0] = '\0';
				val++;

				/* find value */
				while (isspace(val[0]))
					val++;

				/* terminate key */
				len = strlen(key);
				if (len == 0)
					continue;
				while (isspace(key[len-1]))
					len--;
				key[len] = '\0';

				/* terminate value */
				len = strlen(val);
				if (len == 0)
					continue;
				while (isspace(val[len-1]))
					len--;
				val[len] = '\0';

				if (len == 0)
					continue;

				/* unquote */
				if (val[0] == '"' || val[0] == '\'') {
					if (val[len-1] != val[0]) {
						err(ezcfg, "inconsistent quoting in '%s'[%i], skip line\n", config_file, line_nr);
						continue;
					}
					val[len-1] = '\0';
					val++;
				}

				if (strcmp(key, "ezcfg_log") == 0) {
					ezcfg_set_log_priority(ezcfg, util_log_priority(val));
					continue;
				}
				if (strcmp(key, "ezcfg_rules") == 0) {
					free(ezcfg->rules_path);
					ezcfg->rules_path = strdup(val);
					util_remove_trailing_chars(ezcfg->rules_path, '/');
					continue;
				}
			}
			fclose(fp);
		}

		env = getenv("EZCFG_LOG");
		if (env != NULL)
			ezcfg_set_log_priority(ezcfg, util_log_priority(env));

		dbg(ezcfg, "context %p created\n", ezcfg);
		dbg(ezcfg, "log_priority=%d\n", ezcfg->log_priority);
		dbg(ezcfg, "config_file='%s'\n", config_file);
		if (ezcfg->rules_path != NULL)
			dbg(ezcfg, "rules_path='%s'\n", ezcfg->rules_path);

		if (config_file != NULL) {
			free(config_file);
		}
		return ezcfg;
	}
fail_exit:
	if (config_file != NULL) {
		free(config_file);
	}
	ezcfg_delete(ezcfg);
	return NULL;
}

/**
 * ezcfg_delete:
 * @ezcfg: ezcfg library context
 *
 * Release the ezcfg library context.
 *
 **/
void ezcfg_delete(struct ezcfg *ezcfg)
{
	if (ezcfg == NULL)
		return;
	if (ezcfg->rules_path != NULL)
		free(ezcfg->rules_path);
	dbg(ezcfg, "context %p released\n", ezcfg);
	free(ezcfg);
}
