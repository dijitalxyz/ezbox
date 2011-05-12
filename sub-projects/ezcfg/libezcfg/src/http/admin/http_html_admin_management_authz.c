/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/admin/http_html_admin_management_authz.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-04   0.1       Write it from scratch
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
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/sysinfo.h>
#include <pthread.h>

#include <locale.h>
#include <libintl.h>

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-http.h"
#include "ezcfg-html.h"
#include "ezcfg-http_html_admin.h"

/**
 * Private functions
 **/
static int set_html_main_management_authz(
	struct ezcfg_http_html_admin *admin,
	struct ezcfg_locale *locale,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	struct ezcfg_nvram *nvram;
	struct ezcfg_html *html;
	int content_index, child_index;
	int p_index, select_index;
	int input_index;
	char buf[1024];
	char *p = NULL;
	int i;
	int ret = -1;

	ASSERT(admin != NULL);
	ASSERT(pi > 1);

	ezcfg = admin->ezcfg;
	nvram = admin->nvram;
	html = admin->html;

	/* <div id="main"> */
	si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_DIV_ELEMENT_NAME, NULL);
	if (si < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, si, EZCFG_HTML_ID_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_MAIN, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <div id="menu"> */
	child_index = ezcfg_http_html_admin_set_html_menu(admin, si, -1);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_menu.\n");
		goto func_exit;
	}

	/* <div id="content"> */
	content_index = ezcfg_html_add_body_child(html, si, child_index, EZCFG_HTML_DIV_ELEMENT_NAME, NULL);
	if (content_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, content_index, EZCFG_HTML_ID_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_CONTENT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	child_index = -1;
	/* <h3>Access Control</h3> */
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_H3_ELEMENT_NAME, ezcfg_locale_text(locale, "Access Control"));
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Administrator Password : </p> */
	snprintf(buf, sizeof(buf), "%s&nbsp;:&nbsp;",
		ezcfg_locale_text(locale, "Administrator Password"));
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Administrator Password : <input type="password" maxlength="63" name="ezcfg_auth.0.secret" value=""/></p> */
	/* save <p> index */
	p_index = child_index;
	child_index = -1;

	input_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
	if (input_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_PASSWORD, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_MAXLENGTH_ATTRIBUTE_NAME, "63", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_SECRET), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* restore <p> index */
	child_index = p_index;

	/* <p>Re-Enter to Confirm : </p> */
	snprintf(buf, sizeof(buf), "%s&nbsp;:&nbsp;",
		ezcfg_locale_text(locale, "Re-Enter to Confirm"));
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Re-Enter to Confirm : <input type="password" maxlength="63" name="ezcfg_auth.0.secret2" value=""/></p> */
	/* save <p> index */
	p_index = child_index;
	child_index = -1;

	input_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
	if (input_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_PASSWORD, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_MAXLENGTH_ATTRIBUTE_NAME, "63", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_SECRET2), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* restore <p> index */
	child_index = p_index;

	/* <h3>Web Access Setting</h3> */
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_H3_ELEMENT_NAME, ezcfg_locale_text(locale, "Web Access Setting"));
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Service Switch : </p> */
	snprintf(buf, sizeof(buf), "%s&nbsp;:&nbsp;",
		ezcfg_locale_text(locale, "Service Switch"));
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* save <p> index */
	p_index = child_index;
	child_index = -1;

	/* <p>Service Switch : <select name="ezcfg_httpd_enable"></select></p> */
	child_index = -1;
	select_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_SELECT_ELEMENT_NAME, NULL);
	if (select_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, select_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(EZCFG, HTTPD_ENABLE), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <p>Service Switch : <select name="ezcfg_httpd_enable"><option value="1" selected="selected">Enabled</option></select></p> */
	buf[0] = '\0';
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EZCFG, HTTPD_ENABLE), &p);
	if (p != NULL) {
		snprintf(buf, sizeof(buf), "%s", p);
		free(p);
	}
	child_index = -1;
	for (i = 0; i < 2; i++) {
		char tmp[2];
		snprintf(tmp, sizeof(tmp), "%d", i);
		child_index = ezcfg_html_add_body_child(html, select_index, child_index, EZCFG_HTML_OPTION_ELEMENT_NAME, ezcfg_locale_text(locale, ezcfg_util_text_get_service_switch(i == 1)));
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, tmp, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		if (strcmp(tmp, buf) == 0) {
			ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		}
	}

	/* restore <p> index */
	child_index = p_index;

	if (strcmp(buf, "1") == 0) {
		char tmp[2];

		tmp[0] = '\0';
		ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EZCFG, HTTPD_HTTP), &p);
		if (p != NULL) {
			snprintf(tmp, sizeof(tmp), "%s", p);
			free(p);
		}
		/* <p>Access via HTTP : </p> */
		snprintf(buf, sizeof(buf), "%s&nbsp;:&nbsp;",
			ezcfg_locale_text(locale, "Access via HTTP"));
		child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}

		/* <p>Access via HTTP : <input type="radio" name="ezcfg_httpd_http" value="1">Yes</input><input type="radio" name="ezcfg_httpd_http" value="0">No</input></p> */
		/* save <p> index */
		p_index = child_index;
		child_index = -1;

		snprintf(buf, sizeof(buf), "&nbsp;%s&nbsp;",
			ezcfg_locale_text(locale, "Yes"));
		input_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, buf);
		if (input_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_RADIO, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(EZCFG, HTTPD_HTTP), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, "1", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		if (strcmp(tmp, "1") == 0) {
			ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_CHECKED_ATTRIBUTE_NAME, EZCFG_HTML_CHECKED_ATTRIBUTE_NAME, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		}

		snprintf(buf, sizeof(buf), "&nbsp;%s&nbsp;",
			ezcfg_locale_text(locale, "No"));
		input_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, buf);
		if (input_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_RADIO, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(EZCFG, HTTPD_HTTP), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, "0", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		if (strcmp(tmp, "0") == 0) {
			ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_CHECKED_ATTRIBUTE_NAME, EZCFG_HTML_CHECKED_ATTRIBUTE_NAME, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		}

		/* restore <p> index */
		child_index = p_index;

		tmp[0] = '\0';
		ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(EZCFG, HTTPD_HTTPS), &p);
		if (p != NULL) {
			snprintf(tmp, sizeof(tmp), "%s", p);
			free(p);
		}
		/* <p>Access via HTTPS : </p> */
		snprintf(buf, sizeof(buf), "%s&nbsp;:&nbsp;",
			ezcfg_locale_text(locale, "Access via HTTPS"));
		child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}

		/* <p>Access via HTTPS : <input type="radio" name="ezcfg_httpd_https" value="1">Yes</input><input type="radio" name="ezcfg_httpd_https" value="0">No</input></p> */
		/* save <p> index */
		p_index = child_index;
		child_index = -1;

		snprintf(buf, sizeof(buf), "&nbsp;%s&nbsp;",
			ezcfg_locale_text(locale, "Yes"));
		input_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, buf);
		if (input_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_RADIO, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(EZCFG, HTTPD_HTTPS), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, "1", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		if (strcmp(tmp, "1") == 0) {
			ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_CHECKED_ATTRIBUTE_NAME, EZCFG_HTML_CHECKED_ATTRIBUTE_NAME, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		}

		snprintf(buf, sizeof(buf), "&nbsp;%s&nbsp;",
			ezcfg_locale_text(locale, "No"));
		input_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, buf);
		if (input_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_RADIO, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(EZCFG, HTTPD_HTTPS), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, "0", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		if (strcmp(tmp, "0") == 0) {
			ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_CHECKED_ATTRIBUTE_NAME, EZCFG_HTML_CHECKED_ATTRIBUTE_NAME, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		}

		/* restore <p> index */
		child_index = p_index;
	}

	/* <p>&nbsp;</p> */
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, "&nbsp;");
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <input> buttons part */
	child_index = ezcfg_http_html_admin_set_html_button(admin, content_index, child_index);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_button.\n");
		goto func_exit;
	}

	/* must return main index */
	ret = si;

func_exit:
	return ret;
}

static int build_admin_management_authz_response(struct ezcfg_http_html_admin *admin)
{
	struct ezcfg *ezcfg;
	struct ezcfg_html *html = NULL;
	struct ezcfg_locale *locale = NULL;
	int head_index, body_index, child_index;
	int container_index, form_index;
	int rc = 0;
	
	ASSERT(admin != NULL);

	ezcfg = admin->ezcfg;

	/* set locale info */
	locale = ezcfg_locale_new(ezcfg);
	if (locale != NULL) {
		ezcfg_locale_set_domain(locale, EZCFG_HTTP_HTML_ADMIN_MANAGEMENT_AUTHZ_DOMAIN);
		ezcfg_locale_set_dir(locale, EZCFG_HTTP_HTML_LANG_DIR);
	}

	html = ezcfg_html_new(ezcfg);

	if (html == NULL) {
		err(ezcfg, "can not alloc html.\n");
		rc = -1;
		goto func_exit;
	}

	/* set admin->html */
	admin->html = html;

	/* clean HTML structure info */
	ezcfg_html_reset_attributes(html);

	/* build HTML */
	ezcfg_html_set_version_major(html, 4);
	ezcfg_html_set_version_minor(html, 1);

	/* HTML root */
	rc = ezcfg_html_set_root(html, EZCFG_HTML_HTML_ELEMENT_NAME);
	if (rc < 0) {
		err(ezcfg, "ezcfg_html_set_root.\n");
		rc = -1;
		goto func_exit;
	}

	/* HTML Head */
	head_index = ezcfg_html_set_head(html, EZCFG_HTML_HEAD_ELEMENT_NAME);
	if (head_index < 0) {
		err(ezcfg, "ezcfg_html_set_head error.\n");
		rc = -1;
		goto func_exit;
	}

	/* set admin common Head */
	child_index = ezcfg_http_html_admin_set_html_common_head(admin, head_index, -1);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_common_head error.\n");
		rc = -1;
		goto func_exit;
	}

	/* HTML Title */
	child_index = ezcfg_html_add_head_child(html, head_index, child_index, EZCFG_HTML_TITLE_ELEMENT_NAME, ezcfg_locale_text(locale, "Authorization"));
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child error.\n");
		rc = -1;
		goto func_exit;
	}

	/* HTML Body */
	body_index = ezcfg_html_set_body(html, EZCFG_HTML_BODY_ELEMENT_NAME);
	if (body_index < 0) {
		err(ezcfg, "ezcfg_html_set_body error.\n");
		rc = -1;
		goto func_exit;
	}

	/* HTML div container */
	container_index = ezcfg_html_add_body_child(html, body_index, -1, EZCFG_HTML_DIV_ELEMENT_NAME, NULL);
	if (container_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		rc = -1;
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, container_index, EZCFG_HTML_ID_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_CONTAINER, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* HTML form */
	form_index = ezcfg_html_add_body_child(html, container_index, -1, EZCFG_HTML_FORM_ELEMENT_NAME, NULL);
	if (form_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		rc = -1;
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, "management_authz", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_METHOD_ATTRIBUTE_NAME, "post", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_ACTION_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI "management_authz", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* HTML div head */
	child_index = ezcfg_http_html_admin_set_html_head(admin, form_index, -1);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_head error.\n");
		rc = -1;
		goto func_exit;
	}

	/* HTML div main */
	child_index = set_html_main_management_authz(admin, locale, form_index, child_index);
	if (child_index < 0) {
		err(ezcfg, "set_html_main_management_authz error.\n");
		rc = -1;
		goto func_exit;
	}

	/* HTML div foot */
	child_index = ezcfg_http_html_admin_set_html_foot(admin, form_index, child_index);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_foot error.\n");
		rc = -1;
		goto func_exit;
	}

	/* set return value */
	rc = 0;
func_exit:
	if (locale != NULL)
		ezcfg_locale_delete(locale);

	return rc;
}

static bool do_admin_management_authz_action(struct ezcfg_http_html_admin *admin)
{
	struct ezcfg *ezcfg;
	struct ezcfg_link_list *list;
	char *passwd, *passwd2;
	bool ret = false;

	ezcfg = admin->ezcfg;
	list = admin->post_list;

	if (ezcfg_http_html_admin_get_action(admin) == HTTP_HTML_ADMIN_ACT_SAVE) {
		/* set administrator password */
		passwd = ezcfg_link_list_get_node_value_by_name(list, NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_SECRET));
		passwd2 = ezcfg_link_list_get_node_value_by_name(list, NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_SECRET2));
		if ((passwd != NULL) && (passwd2 != NULL)){
			if ((*passwd == '\0') ||
			    (strcmp(passwd, passwd2) != 0)){
				ret = ezcfg_link_list_remove(list, NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_SECRET));
			}
		}
		else {
			ret = ezcfg_link_list_remove(list, NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_SECRET));
		}
		/* remove temp variable */
		ret = ezcfg_link_list_remove(list, NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_SECRET2));

		ret = ezcfg_http_html_admin_save_settings(admin);
	}
	return ret;
}

static bool handle_admin_management_authz_post(struct ezcfg_http_html_admin *admin)
{
	struct ezcfg *ezcfg;
	bool ret = false;

	ezcfg = admin->ezcfg;

	if (ezcfg_http_html_admin_handle_post_data(admin) == true) {
		ret = do_admin_management_authz_action(admin);
	}
	return ret;
}

/**
 * Public functions
 **/
int ezcfg_http_html_admin_management_authz_handler(struct ezcfg_http_html_admin *admin)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	int ret = -1;

	ASSERT(admin != NULL);

	ezcfg = admin->ezcfg;
	http = admin->http;

	/* admin management_authz uri=[/admin/management_authz] */
	if (ezcfg_http_request_method_cmp(http, EZCFG_HTTP_METHOD_POST) == 0) {
		/* do post handling */
		info(ezcfg, "[%s]\n", ezcfg_http_get_message_body(http));
		handle_admin_management_authz_post(admin);
	}

	ret = build_admin_management_authz_response(admin);
	return ret;
}
