/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/admin/http_html_admin_setup_system.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-21   0.1       Write it from scratch
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

struct lc_pair {
	char *lc_name;
	char *lc_desc;
};

static struct lc_pair support_langs[] = {
	{ "en_HK", "English (Hong Kong)" },
	{ "zh_CN", "Chinese (China)" },
};

static struct lc_pair support_areas[] = {
	{ "Africa", "Africa" },
	{ "America", "America" },
	{ "Antarctica", "Antarctica" },
	{ "Arctic", "Arctic Ocean" },
	{ "Asia", "Asia" },
	{ "Atlantic", "Atlantic Ocean" },
	{ "Australia", "Australia" },
	{ "Europe", "Europe" },
	{ "Indian", "Indian Ocean" },
	{ "Pacific", "Pacific Ocean" },
	{ "none", "Posix TZ format" },
};

static struct lc_pair africa_locations[] = {
	{ "Algeria", "Algeria" },
};

static struct lc_pair america_locations[] = {
	{ "Anguilla", "Anguilla" },
};

static struct lc_pair antarctica_locations[] = {
	{ "McMurdo", "McMurdo Station, Ross Island" },
};

static struct lc_pair arctica_locations[] = {
	{ "Longyearbyen", "Svalbard & Jan Mayen" },
};

static struct lc_pair asia_locations[] = {
	{ "Shanghai", "east China - Beijing, Guangdong, Shanghai, etc." },
	{ "Hong_Kong", "Hong Kong" },
};

static struct lc_pair atlantic_locations[] = {
	{ "Bermuda", "Bermuda" },
};

static struct lc_pair australia_locations[] = {
	{ "Lord_Howe", "Lord Howe Island" },
};

static struct lc_pair europe_locations[] = {
	{ "Mariehamn", "Aaland Islands" },
};

static struct lc_pair indian_locations[] = {
	{ "Chagos", "British Indian Ocean Territory" },
};

static struct lc_pair pacific_locations[] = {
	{ "Chatham", "Chatham Islands" },
};

static struct lc_pair none_locations[] = {
	{ "GST-10", "GST-10" },
};

/**
 * Private functions
 **/
static int set_html_main_setup_system(
	struct ezcfg_http *http,
	struct ezcfg_nvram *nvram,
	struct ezcfg_locale *locale,
	struct ezcfg_html *html,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	int main_index;
	int content_index, child_index;
	int p_index, select_index;
	char buf[1024];
	char tz_area[32];
	char tz_location[64];
	char *p = NULL;
	int i, location_length;
	struct lc_pair *lcp;
	int ret = -1;

	ASSERT(http != NULL);
	ASSERT(nvram != NULL);
	ASSERT(html != NULL);
	ASSERT(pi > 1);

	ezcfg = html->ezcfg;

	/* <div id="main"> */
	main_index = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_DIV_ELEMENT_NAME, NULL);
	if (main_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, main_index, EZCFG_HTML_ID_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_MAIN, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <div id="menu"> */
	child_index = ezcfg_http_html_admin_set_html_menu(http, nvram, html, main_index, -1);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_menu.\n");
		goto func_exit;
	}

	/* <div id="content"> */
	content_index = ezcfg_html_add_body_child(html, main_index, child_index, EZCFG_HTML_DIV_ELEMENT_NAME, NULL);
	if (content_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, content_index, EZCFG_HTML_ID_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_CONTENT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	child_index = -1;
	/* <h3>Localization</h3> */
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_H3_ELEMENT_NAME, ezcfg_locale_text(locale, "Localization"));
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Language : </p> */
	snprintf(buf, sizeof(buf), "%s&nbsp;:&nbsp;",
		ezcfg_locale_text(locale, "Language"));
	p_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (p_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Language : <select></select> </p> */
	child_index = -1;
	select_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_SELECT_ELEMENT_NAME, NULL);
	if (select_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, select_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_SELECT_NAME_LANGUAGE, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <p>Language : <select><option></option></select> </p> */
	buf[0] = '\0';
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(SYS, LANGUAGE), &p);
	if (p != NULL) {
		snprintf(buf, sizeof(buf), "%s", p);
		free(p);
	}
	child_index = -1;
	for (i = 0; i < ARRAY_SIZE(support_langs); i++) {
		lcp = &(support_langs[i]);
		child_index = ezcfg_html_add_body_child(html, select_index, child_index, EZCFG_HTML_OPTION_ELEMENT_NAME, ezcfg_locale_text(locale, lcp->lc_desc));
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, lcp->lc_name, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		if (strcmp(buf, lcp->lc_name) == 0) {
			ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		}
	}

#if 0
	/* <p><input type="submit" name="act_change_language" value="Change Language"></input></p> */
	p_index = ezcfg_html_add_body_child(html, content_index, p_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, p_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_SUBMIT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, p_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, "act_change_language", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, p_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, ezcfg_locale_text(locale, "Change Language"), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
#endif

	/* <p>Time Zone : </p> */
	snprintf(buf, sizeof(buf), "%s&nbsp;:&nbsp;",
		ezcfg_locale_text(locale, "Time Zone (Area)"));
	p_index = ezcfg_html_add_body_child(html, content_index, p_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (p_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>  (Area) : </p> */
	snprintf(buf, sizeof(buf), "&nbsp;&nbsp;(%s)&nbsp;:&nbsp;",
		ezcfg_locale_text(locale, "Time Zone (Area)"));
	p_index = ezcfg_html_add_body_child(html, content_index, p_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (p_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>  (Area) : <select></select> </p> */
	child_index = -1;
	select_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_SELECT_ELEMENT_NAME, NULL);
	if (select_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, select_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_SELECT_NAME_TZ_AREA, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <p>  (Area) : <select><option></option></select> </p> */
	tz_area[0] = '\0';
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(SYS, TZ_AREA), &p);
	if (p != NULL) {
		snprintf(tz_area, sizeof(tz_area), "%s", p);
		free(p);
	}

	child_index = -1;
	for (i = 0; i < ARRAY_SIZE(support_areas); i++) {
		lcp = &(support_areas[i]);
		child_index = ezcfg_html_add_body_child(html, select_index, child_index, EZCFG_HTML_OPTION_ELEMENT_NAME, ezcfg_locale_text(locale, lcp->lc_desc));
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, lcp->lc_name, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		if (strcmp(tz_area, lcp->lc_name) == 0) {
			ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		}
	}

#if 0
	/* <p><input type="submit" name="act_change_area" value="Change Area"></input></p> */
	p_index = ezcfg_html_add_body_child(html, content_index, p_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, p_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_SUBMIT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, p_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, "act_change_area", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, p_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, ezcfg_locale_text(locale, "Change Area"), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
#endif

	/* <p>  (Location) : </p> */
	snprintf(buf, sizeof(buf), "&nbsp;&nbsp;(%s)&nbsp;:&nbsp;",
		ezcfg_locale_text(locale, "Time Zone (Location)"));
	p_index = ezcfg_html_add_body_child(html, content_index, p_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (p_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>  (Location) : <select></select> </p> */
	child_index = -1;
	select_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_SELECT_ELEMENT_NAME, NULL);
	if (select_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, select_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_SELECT_NAME_TZ_LOCATION, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <p>  (Location) : <select><option></option></select> </p> */
	tz_location[0] = '\0';
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(SYS, TZ_LOCATION), &p);
	if (p != NULL) {
		snprintf(tz_location, sizeof(tz_location), "%s", p);
		free(p);
	}

	if (strcmp(tz_area, "Africa") == 0) {
		lcp = africa_locations;
		location_length = ARRAY_SIZE(africa_locations);
	}
	else if (strcmp(tz_area, "America") == 0) {
		lcp = america_locations;
		location_length = ARRAY_SIZE(america_locations);
	}
	else if (strcmp(tz_area, "Antarctica") == 0) {
		lcp = antarctica_locations;
		location_length = ARRAY_SIZE(antarctica_locations);
	}
	else if (strcmp(tz_area, "Arctica") == 0) {
		lcp = arctica_locations;
		location_length = ARRAY_SIZE(arctica_locations);
	}
	else if (strcmp(tz_area, "Asia") == 0) {
		lcp = asia_locations;
		location_length = ARRAY_SIZE(asia_locations);
	}
	else if (strcmp(tz_area, "Atlantic") == 0) {
		lcp = atlantic_locations;
		location_length = ARRAY_SIZE(atlantic_locations);
	}
	else if (strcmp(tz_area, "Australia") == 0) {
		lcp = australia_locations;
		location_length = ARRAY_SIZE(australia_locations);
	}
	else if (strcmp(tz_area, "Europe") == 0) {
		lcp = europe_locations;
		location_length = ARRAY_SIZE(europe_locations);
	}
	else if (strcmp(tz_area, "Indian") == 0) {
		lcp = indian_locations;
		location_length = ARRAY_SIZE(indian_locations);
	}
	else if (strcmp(tz_area, "Pacific") == 0) {
		lcp = pacific_locations;
		location_length = ARRAY_SIZE(pacific_locations);
	}
	else {
		lcp = none_locations;
		location_length = ARRAY_SIZE(none_locations);
	}
	child_index = -1;
	for (i = 0; i < location_length; i++, lcp++) {
		child_index = ezcfg_html_add_body_child(html, select_index, child_index, EZCFG_HTML_OPTION_ELEMENT_NAME, ezcfg_locale_text(locale, lcp->lc_desc));
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, lcp->lc_name, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		if (strcmp(tz_location, lcp->lc_name) == 0) {
			ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		}
	}

	/* restore index pointer */
	child_index = p_index;

	/* <input> buttons part */
	child_index = ezcfg_http_html_admin_set_html_button(http, nvram, html, content_index, child_index);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_button.\n");
		goto func_exit;
	}

	/* must return main index */
	ret = main_index;

func_exit:
	return ret;
}

static int build_admin_setup_system_response(struct ezcfg_http *http, struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;
	struct ezcfg_html *html = NULL;
	struct ezcfg_locale *locale = NULL;
	int head_index, body_index, child_index;
	int container_index, form_index;
	char *msg = NULL;
	int msg_len;
	int n;
	int rc = 0;
	bool ret;
	
	ASSERT(http != NULL);
	ASSERT(nvram != NULL);

	ezcfg = http->ezcfg;

	/* set locale info */
	locale = ezcfg_locale_new(ezcfg);
	if (locale != NULL) {
		ezcfg_locale_set_domain(locale, EZCFG_HTTP_HTML_ADMIN_SETUP_SYSTEM_DOMAIN);
		ezcfg_locale_set_dir(locale, EZCFG_HTTP_HTML_LANG_DIR);
	}

	html = ezcfg_html_new(ezcfg);

	if (html == NULL) {
		err(ezcfg, "can not alloc html.\n");
		rc = -1;
		goto func_exit;
	}

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
	child_index = ezcfg_http_html_admin_set_html_common_head(html, head_index, -1);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_common_head error.\n");
		rc = -1;
		goto func_exit;
	}

	/* HTML Title */
	child_index = ezcfg_html_add_head_child(html, head_index, child_index, EZCFG_HTML_TITLE_ELEMENT_NAME, ezcfg_locale_text(locale, "Setup System"));
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
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, "setup_system", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_METHOD_ATTRIBUTE_NAME, "post", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_ACTION_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI "setup_system", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* HTML div head */
	child_index = ezcfg_http_html_admin_set_html_head(http, nvram, html, form_index, -1);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_head error.\n");
		rc = -1;
		goto func_exit;
	}

	/* HTML div main */
	child_index = set_html_main_setup_system(http, nvram, locale, html, form_index, child_index);
	if (child_index < 0) {
		err(ezcfg, "set_html_main_setup_system error.\n");
		rc = -1;
		goto func_exit;
	}

	/* HTML div foot */
	child_index = ezcfg_http_html_admin_set_html_foot(http, nvram, html, form_index, child_index);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_foot error.\n");
		rc = -1;
		goto func_exit;
	}

	msg_len = ezcfg_html_get_message_length(html);
	if (msg_len < 0) {
		err(ezcfg, "ezcfg_html_get_message_length\n");
		rc = -1;
		goto func_exit;
	}
	msg_len++; /* one more for '\0' */
	msg = (char *)malloc(msg_len);
	if (msg == NULL) {
		err(ezcfg, "malloc error.\n");
		rc = -1;
		goto func_exit;
	}

	memset(msg, 0, msg_len);
	n = ezcfg_html_write_message(html, msg, msg_len);
	if (n < 0) {
		err(ezcfg, "ezcfg_html_write_message\n");
		rc = -1;
		goto func_exit;
	}

	/* FIXME: name point to http->request_uri !!!
         * never reset http before using name */
	/* clean http structure info */
	ezcfg_http_reset_attributes(http);
	ezcfg_http_set_status_code(http, 200);
	ezcfg_http_set_state_response(http);

	if (ezcfg_http_set_message_body(http, msg, n) == NULL) {
		err(ezcfg, "ezcfg_http_set_message_body\n");
		rc = -1;
		goto func_exit;
	}

	ret = ezcfg_http_html_admin_set_http_html_common_header(http);
	if (ret == false) {
		err(ezcfg, "ezcfg_http_html_admin_set_http_html_common_header error.\n");
		rc = -1;
		goto func_exit;
	}

	/* set return value */
	rc = 0;
func_exit:
	if (locale != NULL)
		ezcfg_locale_delete(locale);

	if (html != NULL)
		ezcfg_html_delete(html);

	if (msg != NULL)
		free(msg);

	return rc;
}

/**
 * Public functions
 **/
int ezcfg_http_html_admin_setup_system_handler(struct ezcfg_http *http, struct ezcfg_nvram *nvram)
{
	struct ezcfg *ezcfg;
	int ret = -1;

	ASSERT(http != NULL);
	ASSERT(nvram != NULL);

	ezcfg = http->ezcfg;

	/* admin setup_system uri=[/admin/setup_system] */
	if (ezcfg_http_request_method_cmp(http, EZCFG_HTTP_METHOD_POST) == 0) {
		/* do post handling */
	}

	ret = build_admin_setup_system_response(http, nvram);
	return ret;
}
