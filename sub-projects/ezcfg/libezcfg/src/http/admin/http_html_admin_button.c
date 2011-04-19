/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/admin/http_html_admin_button.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-19   0.1       Write it from scratch
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

#include <locale.h>
#include <libintl.h>

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-http.h"
#include "ezcfg-html.h"

/**
 * Private functions
 **/

/**
 * Public functions
 **/

int ezcfg_http_html_admin_set_html_button(
	struct ezcfg_http *http,
	struct ezcfg_nvram *nvram,
	struct ezcfg_html *html,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	struct ezcfg_locale *locale = NULL;
	int button_index;
	int input_index;
	int ret = -1;

	ASSERT(http != NULL);
	ASSERT(nvram != NULL);
	ASSERT(html != NULL);
	ASSERT(pi > 1);

	ezcfg = html->ezcfg;

        /* set locale info */
	locale = ezcfg_locale_new(ezcfg);
	if (locale != NULL) {
		ezcfg_locale_set_domain(locale, EZCFG_HTTP_HTML_ADMIN_BUTTON_DOMAIN);
		ezcfg_locale_set_dir(locale, EZCFG_HTTP_HTML_LANG_DIR);
	}

	/* <div id="button"> */
	button_index = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_DIV_ELEMENT_NAME, NULL);
	if (button_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, button_index, EZCFG_HTML_ID_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_DIV_ID_BUTTON, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	input_index = -1;

	/* <input type="submit" name="act_refresh" value="refresh"> */
	input_index = ezcfg_html_add_body_child(html, button_index, input_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
	if (input_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child err.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_SUBMIT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, "act_refresh", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, ezcfg_locale_text(locale, "Refresh"), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <input type="submit" name="act_cancel" value="cancel"> */
	input_index = ezcfg_html_add_body_child(html, button_index, input_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
	if (input_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child err.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_SUBMIT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, "act_cancel", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, ezcfg_locale_text(locale, "Cancel"), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <input type="submit" name="act_apply" value="apply"> */
	input_index = ezcfg_html_add_body_child(html, button_index, input_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
	if (input_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child err.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_SUBMIT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, "act_apply", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, ezcfg_locale_text(locale, "Apply"), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* must return menu index */
	ret = button_index;
func_exit:
	if (locale != NULL)
		ezcfg_locale_delete(locale);

	return ret;
}

