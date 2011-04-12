/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/admin/http_html_admin_menu.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-12   0.1       Write it from scratch
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

int ezcfg_http_html_admin_set_html_menu(struct ezcfg_html *html, int pi, int si)
{
	struct ezcfg *ezcfg;
	struct ezcfg_locale *locale = NULL;
	int ul_index, li_index, a_index;
	int ul2_index, li2_index, a2_index;
	int child_index;
	int ret = -1;

	ASSERT(html != NULL);
	ASSERT(pi > 1);

	ezcfg = html->ezcfg;

        /* set locale info */
	locale = ezcfg_locale_new(ezcfg);
	if (locale != NULL) {
		ezcfg_locale_set_domain(locale, EZCFG_HTTP_HTML_ADMIN_MENU_DOMAIN);
		ezcfg_locale_set_dir(locale, EZCFG_HTTP_HTML_LANG_DIR);
	}

	/* HTML menu ul element */
	ul_index = ezcfg_html_add_head_child(html, pi, si, EZCFG_HTML_UL_ELEMENT_NAME, NULL);
	if (ul_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child err.\n");
		goto func_exit;
	}
	/* HTML menu li */
	li_index = ezcfg_html_add_head_child(html, ul_index, -1, EZCFG_HTML_LI_ELEMENT_NAME, NULL);
	if (li_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child err.\n");
		goto func_exit;
	}

	/* HTML menu li <a> */
	a_index = ezcfg_html_add_head_child(html, li_index, -1, EZCFG_HTML_A_ELEMENT_NAME, ezcfg_locale_text(locale, "Status"));
	if (a_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child err.\n");
		goto func_exit;
	}

	/* HTML menu li <a> <span /> </a> */
	child_index = ezcfg_html_add_head_child(html, a_index, -1, EZCFG_HTML_SPAN_ELEMENT_NAME, ezcfg_locale_text(locale, "The status of the device -- You can check running info and so on."));
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child err.\n");
		goto func_exit;
	}

	/* HTML submenu ul */
	ul2_index = ezcfg_html_add_head_child(html, li_index, a_index, EZCFG_HTML_UL_ELEMENT_NAME, NULL);
	if (ul2_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child err.\n");
		goto func_exit;
	}

	/* status_system */
	/* HTML submenu li */
	li2_index = ezcfg_html_add_head_child(html, ul2_index, -1, EZCFG_HTML_LI_ELEMENT_NAME, NULL);
	if (li2_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child err.\n");
		goto func_exit;
	}

	/* HTML submenu li <a> */
	a2_index = ezcfg_html_add_head_child(html, li2_index, -1, EZCFG_HTML_A_ELEMENT_NAME, ezcfg_locale_text(locale, "System"));
	if (a2_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child err.\n");
		goto func_exit;
	}

	/* HTML submenu li <a> <span /> </a> */
	child_index = ezcfg_html_add_head_child(html, a2_index, -1, EZCFG_HTML_SPAN_ELEMENT_NAME, ezcfg_locale_text(locale, "The status of the system -- You can check system info and so on."));
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child err.\n");
		goto func_exit;
	}

	/* status_lan */
	/* HTML submenu li */
	li2_index = ezcfg_html_add_head_child(html, ul2_index, -1, EZCFG_HTML_LI_ELEMENT_NAME, NULL);
	if (li2_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child err.\n");
		goto func_exit;
	}

	/* HTML submenu li <a> */
	a2_index = ezcfg_html_add_head_child(html, li2_index, -1, EZCFG_HTML_A_ELEMENT_NAME, ezcfg_locale_text(locale, "Local Network"));
	if (a2_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child err.\n");
		goto func_exit;
	}

	/* HTML submenu li <a> <span /> </a> */
	child_index = ezcfg_html_add_head_child(html, a2_index, -1, EZCFG_HTML_SPAN_ELEMENT_NAME, ezcfg_locale_text(locale, "The status of local network -- You can check local area networks info and so on."));
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_head_child err.\n");
		goto func_exit;
	}

	/* must return menu index */
	ret = ul_index;
func_exit:
	if (locale != NULL)
		ezcfg_locale_delete(locale);

	return ret;
}

