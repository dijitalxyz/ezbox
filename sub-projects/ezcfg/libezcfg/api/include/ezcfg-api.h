#ifndef _EZCFG_API_H_
#define _EZCFG_API_H_

#include "ezcfg-api-errno.h"

/* Master thread interface */
struct ezcfg_master;
int ezcfg_api_master_set_config_file(const char *path);
struct ezcfg_master *ezcfg_api_master_start(const char *name, int threads_max);
int ezcfg_api_master_stop(struct ezcfg_master *master);
int ezcfg_api_master_reload(struct ezcfg_master *master);

/* CTRL interface */
int ezcfg_api_ctrl_set_config_file(const char *path);
int ezcfg_api_ctrl_exec(char *const argv[], char *output, size_t len);

/* NVRAM interface */
int ezcfg_api_nvram_set_config_file(const char *path);
int ezcfg_api_nvram_get(const char *name, char *value, size_t len);
int ezcfg_api_nvram_set(const char *name, const char *value);
int ezcfg_api_nvram_unset(const char *name);
int ezcfg_api_nvram_set_multi(char *list, const int num);
int ezcfg_api_nvram_list(char *list, size_t len);
int ezcfg_api_nvram_info(char *list, size_t len);
int ezcfg_api_nvram_commit(void);
void ezcfg_api_nvram_set_debug(bool enable_debug);
int ezcfg_api_nvram_insert_socket(
	const char *domain,
	const char *type,
	const char *protocol,
	const char *address);
int ezcfg_api_nvram_remove_socket(
	const char *domain,
	const char *type,
	const char *protocol,
	const char *address);

/* rc interface */
bool ezcfg_api_rc_require_semaphore(void);
bool ezcfg_api_rc_release_semaphore(void);

/* UPnP interface */
int ezcfg_api_upnp_set_task_file(const char *path);
FILE *ezcfg_api_upnp_open_task_file(const char *mode);
bool ezcfg_api_upnp_close_task_file(FILE *fp);

/* u-boot-env interface */
int ezcfg_api_ubootenv_get(char *name, char *value, size_t len);
int ezcfg_api_ubootenv_set(char *name, char *value);
int ezcfg_api_ubootenv_list(char *list, size_t len);
int ezcfg_api_ubootenv_check(void);
int ezcfg_api_ubootenv_size(void);

/* firmware interface */
int ezcfg_api_firmware_upgrade(char *name, char *model);

#endif
