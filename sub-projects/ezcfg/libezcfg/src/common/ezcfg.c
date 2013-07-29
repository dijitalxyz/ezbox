/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2013 by ezbox-project
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
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

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
	char 		config_file[EZCFG_PATH_MAX];
	int		log_priority;
	char 		rules_path[EZCFG_PATH_MAX];
	char 		root_path[EZCFG_PATH_MAX];
	char 		sem_ezcfg_path[EZCFG_PATH_MAX];
	char 		shm_ezcfg_path[EZCFG_PATH_MAX];
	size_t 		shm_ezcfg_size;
	size_t 		shm_ezcfg_nvram_queue_length;
	size_t 		shm_ezcfg_rc_queue_length;
	char 		sock_agent_ctrl_path[EZCFG_PATH_MAX];
	char 		sock_ctrl_path[EZCFG_PATH_MAX];
	char 		sock_nvram_path[EZCFG_PATH_MAX];
	char 		sock_uevent_path[EZCFG_PATH_MAX];
	char 		sock_master_path[EZCFG_PATH_MAX];
	char 		web_document_root_path[EZCFG_PATH_MAX];
	char 		locale[EZCFG_LOCALE_MAX];
	pthread_mutex_t locale_mutex; /* Protects locale */
#if (HAVE_EZBOX_SERVICE_EZCTP == 1)
	char 		shm_ezctp_path[EZCFG_PATH_MAX];
	size_t 		shm_ezctp_size;
	size_t 		shm_ezctp_cq_unit_size;
	size_t 		shm_ezctp_cq_length;
#endif
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
 * ezcfg_common_set_log_fn:
 * @ezcfg: ezcfg library context
 * @log_fn: function to be called for logging messages
 *
 * The built-in logging writes to stderr. It can be
 * overridden by a custom function, to plug log messages
 * into the users' logging functionality.
 *
 **/
void ezcfg_common_set_log_fn(struct ezcfg *ezcfg,
                      void (*log_fn)(struct ezcfg *ezcfg,
                                    int priority, const char *file, int line, const char *fn,
                                    const char *format, va_list args))
{
	ezcfg->log_fn = log_fn;
}

/**
 * ezcfg_common_get_log_priority:
 * @ezcfg: ezcfg library context
 *
 * The initial logging priority is read from the ezcfg config file
 * at startup.
 *
 * Returns: the current logging priority
 **/
int ezcfg_common_get_log_priority(struct ezcfg *ezcfg)
{
	return ezcfg->log_priority;
}

/**
 * ezcfg_common_set_log_priority:
 * @ezcfg: ezcfg library context
 * @priority: the new logging priority
 *
 * Set the current logging priority. The value controls which messages
 * are logged.
 **/
void ezcfg_common_set_log_priority(struct ezcfg *ezcfg, int priority)
{
	char num[32];

	ezcfg->log_priority = priority;
	snprintf(num, sizeof(num), "%u", ezcfg->log_priority);
	ezcfg_common_add_property(ezcfg, "EZCFG_LOG", num);
}

char *ezcfg_common_get_config_file(struct ezcfg *ezcfg)
{
	return ezcfg->config_file;
}

void ezcfg_common_set_config_file(struct ezcfg *ezcfg, char *file)
{
	if (file == NULL)
		return;

	snprintf(ezcfg->config_file, EZCFG_PATH_MAX, "%s", file);
}

char *ezcfg_common_get_rules_path(struct ezcfg *ezcfg)
{
	return ezcfg->rules_path;
}

void ezcfg_common_set_rules_path(struct ezcfg *ezcfg, char *path)
{
	if (path == NULL)
		return;

	snprintf(ezcfg->rules_path, EZCFG_PATH_MAX, "%s", path);
}

char *ezcfg_common_get_root_path(struct ezcfg *ezcfg)
{
	return ezcfg->root_path;
}

void ezcfg_common_set_root_path(struct ezcfg *ezcfg, char *path)
{
	if (path == NULL)
		return;

	snprintf(ezcfg->root_path, EZCFG_PATH_MAX, "%s", path);
}

char *ezcfg_common_get_sem_ezcfg_path(struct ezcfg *ezcfg)
{
	return ezcfg->sem_ezcfg_path;
}

void ezcfg_common_set_sem_ezcfg_path(struct ezcfg *ezcfg, char *path)
{
	if (path == NULL)
		return;

	snprintf(ezcfg->sem_ezcfg_path, EZCFG_PATH_MAX, "%s", path);
}

char *ezcfg_common_get_shm_ezcfg_path(struct ezcfg *ezcfg)
{
	return ezcfg->shm_ezcfg_path;
}

void ezcfg_common_set_shm_ezcfg_path(struct ezcfg *ezcfg, char *path)
{
	if (path == NULL)
		return;

	snprintf(ezcfg->shm_ezcfg_path, EZCFG_PATH_MAX, "%s", path);
}

size_t ezcfg_common_get_shm_ezcfg_size(struct ezcfg *ezcfg)
{
	return ezcfg->shm_ezcfg_size;
}

size_t ezcfg_common_get_shm_ezcfg_nvram_queue_length(struct ezcfg *ezcfg)
{
	return ezcfg->shm_ezcfg_nvram_queue_length;
}

size_t ezcfg_common_get_shm_ezcfg_rc_queue_length(struct ezcfg *ezcfg)
{
	return ezcfg->shm_ezcfg_rc_queue_length;
}

#if (HAVE_EZBOX_SERVICE_EZCTP == 1)
char *ezcfg_common_get_shm_ezctp_path(struct ezcfg *ezcfg)
{
	return ezcfg->shm_ezctp_path;
}

void ezcfg_common_set_shm_ezctp_path(struct ezcfg *ezcfg, char *path)
{
	if (path == NULL)
		return;

	snprintf(ezcfg->shm_ezctp_path, EZCFG_PATH_MAX, "%s", path);
}

size_t ezcfg_common_get_shm_ezctp_size(struct ezcfg *ezcfg)
{
	return ezcfg->shm_ezctp_size;
}

size_t ezcfg_common_get_shm_ezctp_cq_unit_size(struct ezcfg *ezcfg)
{
	return ezcfg->shm_ezctp_cq_unit_size;
}

size_t ezcfg_common_get_shm_ezctp_cq_length(struct ezcfg *ezcfg)
{
	return ezcfg->shm_ezctp_cq_length;
}
#endif

char *ezcfg_common_get_sock_agent_ctrl_path(struct ezcfg *ezcfg)
{
	return ezcfg->sock_agent_ctrl_path;
}

void ezcfg_common_set_sock_agent_ctrl_path(struct ezcfg *ezcfg, char *path)
{
	if (path == NULL)
		return;

	snprintf(ezcfg->sock_agent_ctrl_path, EZCFG_PATH_MAX, "%s", path);
}

char *ezcfg_common_get_sock_ctrl_path(struct ezcfg *ezcfg)
{
	return ezcfg->sock_ctrl_path;
}

void ezcfg_common_set_sock_ctrl_path(struct ezcfg *ezcfg, char *path)
{
	if (path == NULL)
		return;

	snprintf(ezcfg->sock_ctrl_path, EZCFG_PATH_MAX, "%s", path);
}

char *ezcfg_common_get_sock_nvram_path(struct ezcfg *ezcfg)
{
	return ezcfg->sock_nvram_path;
}

void ezcfg_common_set_sock_nvram_path(struct ezcfg *ezcfg, char *path)
{
	if (path == NULL)
		return;

	snprintf(ezcfg->sock_nvram_path, EZCFG_PATH_MAX, "%s", path);
}

char *ezcfg_common_get_sock_uevent_path(struct ezcfg *ezcfg)
{
	return ezcfg->sock_uevent_path;
}

void ezcfg_common_set_sock_uevent_path(struct ezcfg *ezcfg, char *path)
{
	if (path == NULL)
		return;

	snprintf(ezcfg->sock_uevent_path, EZCFG_PATH_MAX, "%s", path);
}

char *ezcfg_common_get_sock_master_path(struct ezcfg *ezcfg)
{
	return ezcfg->sock_master_path;
}

void ezcfg_common_set_sock_master_path(struct ezcfg *ezcfg, char *path)
{
	if (path == NULL)
		return;

	snprintf(ezcfg->sock_master_path, EZCFG_PATH_MAX, "%s", path);
}

char *ezcfg_common_get_web_document_root_path(struct ezcfg *ezcfg)
{
	return ezcfg->web_document_root_path;
}

void ezcfg_common_set_web_document_root_path(struct ezcfg *ezcfg, char *path)
{
	if (path == NULL)
		return;

	snprintf(ezcfg->web_document_root_path, EZCFG_PATH_MAX, "%s", path);
}

char *ezcfg_common_get_locale(struct ezcfg *ezcfg)
{
	return ezcfg->locale;
}

void ezcfg_common_set_locale(struct ezcfg *ezcfg, char *locale)
{
	if (locale == NULL)
		return;

	snprintf(ezcfg->locale, EZCFG_LOCALE_MAX, "%s", locale);
}

int ezcfg_common_locale_mutex_lock(struct ezcfg *ezcfg)
{
	
	return pthread_mutex_lock(&ezcfg->locale_mutex);
}

int ezcfg_common_locale_mutex_unlock(struct ezcfg *ezcfg)
{
	
	return pthread_mutex_unlock(&ezcfg->locale_mutex);
}

struct ezcfg_list_entry *ezcfg_common_add_property(struct ezcfg *ezcfg, const char *key, const char *value)
{
	if (value == NULL) {
		struct ezcfg_list_entry *list_entry;

		list_entry = ezcfg_common_get_properties_list_entry(ezcfg);
		list_entry = ezcfg_list_entry_get_by_name(list_entry, key);
		if (list_entry != NULL)
			ezcfg_list_entry_delete(list_entry);
		return NULL;
	}
        return ezcfg_list_entry_add(ezcfg, &ezcfg->properties_list, key, value, 1, 0);
}

struct ezcfg_list_entry *ezcfg_common_get_properties_list_entry(struct ezcfg *ezcfg)
{
	return ezcfg_list_get_entry(&ezcfg->properties_list);
}

void ezcfg_common_load_conf(struct ezcfg *ezcfg)
{
	char *p;
	size_t size;

	/* find log_level keyword */
	p = ezcfg_util_get_conf_string(ezcfg->config_file, EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_LOG_LEVEL);
	if (p != NULL) {
		ezcfg_common_set_log_priority(ezcfg, ezcfg_util_log_priority(p));
		free(p);
	}

	/* find rules_path keyword */
	p = ezcfg_util_get_conf_string(ezcfg->config_file, EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_RULES_PATH);
	if (p != NULL) {
		ezcfg_util_remove_trailing_char(p, '/');
		snprintf(ezcfg->rules_path, EZCFG_PATH_MAX, "%s", p);
		free(p);
	}

	/* find root_path keyword */
	p = ezcfg_util_get_conf_string(ezcfg->config_file, EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_ROOT_PATH);
	if (p != NULL) {
		ezcfg_util_remove_trailing_char(p, '/');
		snprintf(ezcfg->root_path, EZCFG_PATH_MAX, "%s", p);
		free(p);
	}

	/* find sem_ezcfg_path keyword */
	p = ezcfg_util_get_conf_string(ezcfg->config_file, EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_SEM_EZCFG_PATH);
	if (p != NULL) {
		ezcfg_util_remove_trailing_char(p, '/');
		snprintf(ezcfg->sem_ezcfg_path, EZCFG_PATH_MAX, "%s", p);
		free(p);
	}

	/* find shm_ezcfg_path keyword */
	p = ezcfg_util_get_conf_string(ezcfg->config_file, EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_SHM_EZCFG_PATH);
	if (p != NULL) {
		ezcfg_util_remove_trailing_char(p, '/');
		snprintf(ezcfg->shm_ezcfg_path, EZCFG_PATH_MAX, "%s", p);
		free(p);
	}

	/* find shm_ezcfg_size keyword */
	p = ezcfg_util_get_conf_string(ezcfg->config_file, EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_SHM_EZCFG_SIZE);
	if (p != NULL) {
		size = strtoul(p, NULL, 10);
		if (size <= EZCFG_SHM_SIZE_MAX) {
			ezcfg->shm_ezcfg_size = size;
		}
		free(p);
	}

	/* find shm_ezcfg_nvram_queue_length keyword */
	ezcfg->shm_ezcfg_nvram_queue_length = EZCFG_SHM_EZCFG_NVRAM_QUEUE_MIN;
	p = ezcfg_util_get_conf_string(ezcfg->config_file, EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_SHM_EZCFG_NVRAM_QUEUE_LENGTH);
	if (p != NULL) {
		size = strtoul(p, NULL, 10);
		if (size > EZCFG_SHM_EZCFG_NVRAM_QUEUE_MIN) {
			ezcfg->shm_ezcfg_nvram_queue_length = size;
		}
		free(p);
	}

	/* find shm_ezcfg_rc_queue_length keyword */
	ezcfg->shm_ezcfg_rc_queue_length = EZCFG_SHM_EZCFG_RC_QUEUE_MIN;
	p = ezcfg_util_get_conf_string(ezcfg->config_file, EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_SHM_EZCFG_RC_QUEUE_LENGTH);
	if (p != NULL) {
		size = strtoul(p, NULL, 10);
		if (size > EZCFG_SHM_EZCFG_RC_QUEUE_MIN) {
			ezcfg->shm_ezcfg_rc_queue_length = size;
		}
		free(p);
	}

#if(HAVE_EZBOX_SERVICE_EZCTP == 1)
	/* find shm_ezctp_path keyword */
	p = ezcfg_util_get_conf_string(ezcfg->config_file, EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_SHM_EZCTP_PATH);
	if (p != NULL) {
		ezcfg_util_remove_trailing_char(p, '/');
		snprintf(ezcfg->shm_ezctp_path, EZCFG_PATH_MAX, "%s", p);
		free(p);
	}

	/* find shm_ezctp_size keyword */
	p = ezcfg_util_get_conf_string(ezcfg->config_file, EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_SHM_EZCTP_SIZE);
	if (p != NULL) {
		size = strtoul(p, NULL, 10);
		if (size <= EZCFG_SHM_SIZE_MAX) {
			ezcfg->shm_ezctp_size = size;
		}
		free(p);
	}

	/* find shm_ezctp_cq_unit_size keyword */
	p = ezcfg_util_get_conf_string(ezcfg->config_file, EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_SHM_EZCTP_CQ_UNIT_SIZE);
	if (p != NULL) {
		size = strtoul(p, NULL, 10);
		ezcfg->shm_ezctp_cq_unit_size = size;
		free(p);
	}

	/* find shm_ezctp_cq_length keyword */
	p = ezcfg_util_get_conf_string(ezcfg->config_file, EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_SHM_EZCTP_CQ_LENGTH);
	if (p != NULL) {
		size = strtoul(p, NULL, 10);
		ezcfg->shm_ezctp_cq_length = size;
		free(p);
	}
#endif

	/* find sock_ctrl_path keyword */
	p = ezcfg_util_get_conf_string(ezcfg->config_file, EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_SOCK_CTRL_PATH);
	if (p != NULL) {
		ezcfg_util_remove_trailing_char(p, '/');
		snprintf(ezcfg->sock_ctrl_path, EZCFG_PATH_MAX, "%s", p);
		free(p);
	}
	else {
		/* FIXME: must set sock_ctrl_path */
		snprintf(ezcfg->sock_ctrl_path, EZCFG_PATH_MAX, "%s", EZCFG_SOCK_CTRL_PATH);
	}

	/* find sock_nvram_path keyword */
	p = ezcfg_util_get_conf_string(ezcfg->config_file, EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_SOCK_NVRAM_PATH);
	if (p != NULL) {
		ezcfg_util_remove_trailing_char(p, '/');
		snprintf(ezcfg->sock_nvram_path, EZCFG_PATH_MAX, "%s", p);
		free(p);
	}

	/* find sock_uevent_path keyword */
	p = ezcfg_util_get_conf_string(ezcfg->config_file, EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_SOCK_UEVENT_PATH);
	if (p != NULL) {
		ezcfg_util_remove_trailing_char(p, '/');
		snprintf(ezcfg->sock_uevent_path, EZCFG_PATH_MAX, "%s", p);
		free(p);
	}

	/* find sock_master_path keyword */
	p = ezcfg_util_get_conf_string(ezcfg->config_file, EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_SOCK_MASTER_PATH);
	if (p != NULL) {
		ezcfg_util_remove_trailing_char(p, '/');
		snprintf(ezcfg->sock_master_path, EZCFG_PATH_MAX, "%s", p);
		free(p);
	}

	/* find web_document_root_path keyword */
	p = ezcfg_util_get_conf_string(ezcfg->config_file, EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_WEB_DOCUMENT_ROOT_PATH);
	if (p != NULL) {
		ezcfg_util_remove_trailing_char(p, '/');
		snprintf(ezcfg->web_document_root_path, EZCFG_PATH_MAX, "%s", p);
		free(p);
	}

	/* find locale keyword */
	p = ezcfg_util_get_conf_string(ezcfg->config_file, EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_LOCALE);
	if (p != NULL) {
		snprintf(ezcfg->locale, EZCFG_LOCALE_MAX, "%s", p);
		free(p);
	}
}

/**
 * ezcfg_new:
 *
 * Create ezcfg library context.
 *
 * Returns: a new ezcfg library context
 **/

struct ezcfg *ezcfg_new(char *path)
{
	struct ezcfg *ezcfg = NULL;

	if (path == NULL)
		return NULL;

	ezcfg = calloc(1, sizeof(struct ezcfg));
	if (ezcfg) {
		/* initialize ezcfg library context */
		memset(ezcfg, 0, sizeof(struct ezcfg));

		ezcfg->log_fn = log_stderr;
		ezcfg->log_priority = LOG_DEBUG;
		ezcfg_list_init(&ezcfg->properties_list);

		/* set config file path */
		ezcfg_common_set_config_file(ezcfg, path);

		ezcfg_common_load_conf(ezcfg);

		/* initialize locale mutex */
		pthread_mutex_init(&ezcfg->locale_mutex, NULL);

		/* new ezcfg OK! */
		return ezcfg;
	}

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
	pthread_mutex_destroy(&ezcfg->locale_mutex);
	free(ezcfg);
}
