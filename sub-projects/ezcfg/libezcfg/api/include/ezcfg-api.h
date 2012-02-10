#ifndef _EZCFG_API_H_
#define _EZCFG_API_H_

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
struct arg_nvram_socket {
	char *domain;
	char *type;
	char *protocol;
	char *address;
};

struct arg_nvram_ssl {
	char *domain;
	char *type;
	char *protocol;
	char *address;
};

int ezcfg_api_nvram_set_config_file(const char *path);
int ezcfg_api_nvram_get(const char *name, char *value, size_t len);
int ezcfg_api_nvram_set(const char *name, const char *value);
int ezcfg_api_nvram_unset(const char *name);
int ezcfg_api_nvram_set_multi(char *list, const int num);
int ezcfg_api_nvram_list(char *list, size_t len);
int ezcfg_api_nvram_info(char *list, size_t len);
int ezcfg_api_nvram_commit(void);
void ezcfg_api_nvram_set_debug(bool enable_debug);
int ezcfg_api_nvram_insert_socket(struct arg_nvram_socket a);
int ezcfg_api_nvram_remove_socket(struct arg_nvram_socket a);
int ezcfg_api_nvram_insert_ssl(struct arg_nvram_ssl a);
int ezcfg_api_nvram_remove_ssl(struct arg_nvram_ssl a);

/* rc interface */
bool ezcfg_api_rc_require_semaphore(void);
bool ezcfg_api_rc_release_semaphore(void);

/* UUID interface */
int ezcfg_api_uuid_set_config_file(const char *path);
int ezcfg_api_uuid1_string(char *str, int len);
int ezcfg_api_uuid3_string(char *str, int len);
int ezcfg_api_uuid4_string(char *str, int len);
int ezcfg_api_uuid5_string(char *str, int len);

/* UPnP interface */
int ezcfg_api_upnp_set_task_file(const char *path);
int ezcfg_api_upnp_get_task_file(char *path, int len);
bool ezcfg_api_upnp_lock_task_file(void);
bool ezcfg_api_upnp_unlock_task_file(void);

/* u-boot-env interface */
int ezcfg_api_ubootenv_get(char *name, char *value, size_t len);
int ezcfg_api_ubootenv_set(char *name, char *value);
int ezcfg_api_ubootenv_list(char *list, size_t len);
int ezcfg_api_ubootenv_check(void);
size_t ezcfg_api_ubootenv_size(void);

/* firmware interface */
int ezcfg_api_firmware_upgrade(char *name, char *model);

#endif
