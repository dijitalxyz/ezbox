/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-http_html_admin.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-25   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_HTTP_HTML_ADMIN_H_
#define _EZCFG_HTTP_HTML_ADMIN_H_

enum {
	HTTP_HTML_ADMIN_ACT_CANCEL = 0,
	HTTP_HTML_ADMIN_ACT_SAVE,
	HTTP_HTML_ADMIN_ACT_REFRESH
};


struct ezcfg_http_html_admin {
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	struct ezcfg_nvram *nvram;
	struct ezcfg_link_list *post_list;
	struct ezcfg_html *html;
};


/* ezcfg HTTP html admin request/response mode */
#define EZCFG_HTTP_HTML_ADMIN_STATUS_SYSTEM_DOMAIN     "admin_status_system"
#define EZCFG_HTTP_HTML_ADMIN_SETUP_SYSTEM_DOMAIN      "admin_setup_system"

#define EZCFG_HTTP_HTML_ADMIN_HEAD_DOMAIN              "admin_head"
#define EZCFG_HTTP_HTML_ADMIN_MAIN_DOMAIN              "admin_main"
#define EZCFG_HTTP_HTML_ADMIN_FOOT_DOMAIN              "admin_foot"
#define EZCFG_HTTP_HTML_ADMIN_MENU_DOMAIN              "admin_menu"
#define EZCFG_HTTP_HTML_ADMIN_CONTENT_DOMAIN           "admin_content"
#define EZCFG_HTTP_HTML_ADMIN_BUTTON_DOMAIN            "admin_button"


#define EZCFG_HTTP_HTML_ADMIN_DIV_ID_CONTAINER     "container"
#define EZCFG_HTTP_HTML_ADMIN_DIV_ID_HEAD          "head"
#define EZCFG_HTTP_HTML_ADMIN_DIV_ID_MAIN          "main"
#define EZCFG_HTTP_HTML_ADMIN_DIV_ID_FOOT          "foot"
#define EZCFG_HTTP_HTML_ADMIN_DIV_ID_MENU          "menu"
#define EZCFG_HTTP_HTML_ADMIN_DIV_ID_CONTENT       "content"
#define EZCFG_HTTP_HTML_ADMIN_DIV_ID_BUTTON        "button"

#define EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_SUBMIT        "submit"
#define EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_FILE          "file"
#define EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_TEXT          "text"

#define EZCFG_HTTP_HTML_ADMIN_INPUT_NAME_ACT_SAVE      "act_save"
#define EZCFG_HTTP_HTML_ADMIN_INPUT_NAME_ACT_CANCEL    "act_cancel"
#define EZCFG_HTTP_HTML_ADMIN_INPUT_NAME_ACT_REFRESH   "act_refresh"
#define EZCFG_HTTP_HTML_ADMIN_INPUT_NAME_ACT_RELEASE   "act_release"
#define EZCFG_HTTP_HTML_ADMIN_INPUT_NAME_ACT_RENEW     "act_renew"
#define EZCFG_HTTP_HTML_ADMIN_INPUT_NAME_WAN_HOSTNAME  NVRAM_SERVICE_OPTION(WAN, HOSTNAME)
#define EZCFG_HTTP_HTML_ADMIN_INPUT_NAME_WAN_DOMAIN    NVRAM_SERVICE_OPTION(WAN, DOMAIN)
#define EZCFG_HTTP_HTML_ADMIN_INPUT_NAME_WAN_MTU       NVRAM_SERVICE_OPTION(WAN, MTU)


#define EZCFG_HTTP_HTML_ADMIN_SELECT_NAME_LANGUAGE     NVRAM_SERVICE_OPTION(SYS, LANGUAGE)
#define EZCFG_HTTP_HTML_ADMIN_SELECT_NAME_TZ_AREA      NVRAM_SERVICE_OPTION(UI, TZ_AREA)
#define EZCFG_HTTP_HTML_ADMIN_SELECT_NAME_TZ_LOCATION  NVRAM_SERVICE_OPTION(UI, TZ_LOCATION)
#define EZCFG_HTTP_HTML_ADMIN_SELECT_NAME_WAN_TYPE     NVRAM_SERVICE_OPTION(WAN, TYPE)
#define EZCFG_HTTP_HTML_ADMIN_SELECT_NAME_WAN_MTU_ENABLE \
	NVRAM_SERVICE_OPTION(WAN, MTU_ENABLE)


#define EZCFG_HTTP_HTML_ADMIN_OPTION_VALUE_EN_HK       "en_HK"
#define EZCFG_HTTP_HTML_ADMIN_OPTION_VALUE_ZH_CN       "zh_CN"

#define EZCFG_HTTP_HTML_ADMIN_P_CLASS_WARNING          "warning"

#endif /* _EZCFG_HTTP_HTML_ADMIN_H_ */
