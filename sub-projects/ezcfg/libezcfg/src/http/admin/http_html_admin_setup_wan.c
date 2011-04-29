/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : http/admin/http_html_admin_setup_wan.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-27   0.1       Write it from scratch
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

static int set_setup_wan_static(
	struct ezcfg_http_html_admin *admin,
	struct ezcfg_locale *locale,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	struct ezcfg_nvram *nvram;
	struct ezcfg_html *html;
	int child_index;
	int input_index;
	char name[32];
	char buf[1024];
	char *p = NULL;
	int i;

	ASSERT(admin != NULL);
	ASSERT(pi > 1);

	ezcfg = admin->ezcfg;
	nvram = admin->nvram;
	html = admin->html;

	/* <p>IP Address : </p> */
	snprintf(buf, sizeof(buf), "%s&nbsp;:&nbsp;",
		ezcfg_locale_text(locale, "IP Address"));
	si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (si < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>IP Address : <input type="text" maxlength="15" name="wan_static_ipaddr" value=""/></p> */
	child_index = -1;
	input_index = ezcfg_html_add_body_child(html, si, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
	if (input_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_TEXT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_MAXLENGTH_ATTRIBUTE_NAME, "15", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(WAN, STATIC_IPADDR), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, STATIC_IPADDR), &p);
	if (p != NULL) {
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, p, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		free(p);
	}

	/* <p>Subnet Mask : </p> */
	snprintf(buf, sizeof(buf), "%s&nbsp;:&nbsp;",
		ezcfg_locale_text(locale, "Subnet Mask"));
	si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (si < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Subnet Mask : <input type="text" maxlength="15" name="wan_static_netmask" value=""/></p> */
	child_index = -1;
	input_index = ezcfg_html_add_body_child(html, si, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
	if (input_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_TEXT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_MAXLENGTH_ATTRIBUTE_NAME, "15", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(WAN, STATIC_NETMASK), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, STATIC_NETMASK), &p);
	if (p != NULL) {
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, p, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		free(p);
	}

	/* <p>Default Gateway : </p> */
	snprintf(buf, sizeof(buf), "%s&nbsp;:&nbsp;",
		ezcfg_locale_text(locale, "Default Gateway"));
	si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (si < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Default Gateway : <input type="text" maxlength="15" name="wan_static_gateway" value=""/></p> */
	child_index = -1;
	input_index = ezcfg_html_add_body_child(html, si, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
	if (input_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_TEXT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_MAXLENGTH_ATTRIBUTE_NAME, "15", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(WAN, STATIC_GATEWAY), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, STATIC_GATEWAY), &p);
	if (p != NULL) {
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, p, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		free(p);
	}

	for (i = 1; i <= 3; i++) {
		if (i == 1) {
			snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WAN, STATIC_DNS1));
		}
		else if (i == 2) {
			snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WAN, STATIC_DNS2));
		}
		else if (i == 3) {
			snprintf(name, sizeof(name), "%s", NVRAM_SERVICE_OPTION(WAN, STATIC_DNS3));
		}
		else {
			err(ezcfg, "dns index is too large.\n");
			goto func_exit;
		}

		/* <p>DNS 1 : </p> */
		snprintf(buf, sizeof(buf), "%s %d&nbsp;:&nbsp;",
			ezcfg_locale_text(locale, "DNS"), i);
		si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_P_ELEMENT_NAME, buf);
		if (si < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}

		/* <p>DNS 1 : <input type="text" maxlength="15" name="wan_static_dns1" value=""/></p> */
		child_index = -1;
		input_index = ezcfg_html_add_body_child(html, si, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
		if (input_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_TEXT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_MAXLENGTH_ATTRIBUTE_NAME, "15", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, name, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_nvram_get_entry_value(nvram, name, &p);
		if (p != NULL) {
			ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, p, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
			free(p);
		}
	}

func_exit:
	return si;
}

static int set_setup_wan_pppoe(
	struct ezcfg_http_html_admin *admin,
	struct ezcfg_locale *locale,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	struct ezcfg_nvram *nvram;
	struct ezcfg_html *html;
	int child_index;
	int input_index, select_index;
	char buf[1024];
	char *p = NULL;
	int i;

	ASSERT(admin != NULL);
	ASSERT(pi > 1);

	ezcfg = admin->ezcfg;
	nvram = admin->nvram;
	html = admin->html;

	/* <p>Username : </p> */
	snprintf(buf, sizeof(buf), "%s&nbsp;:&nbsp;",
		ezcfg_locale_text(locale, "Username"));
	si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (si < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Username : <input type="text" maxlength="63" name="wan_pppoe_ppp_username" value=""/></p> */
	child_index = -1;
	input_index = ezcfg_html_add_body_child(html, si, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
	if (input_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_TEXT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_MAXLENGTH_ATTRIBUTE_NAME, "63", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(WAN, PPPOE_PPP_USERNAME), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, PPPOE_PPP_USERNAME), &p);
	if (p != NULL) {
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, p, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		free(p);
	}

	/* <p>Password : </p> */
	snprintf(buf, sizeof(buf), "%s&nbsp;:&nbsp;",
		ezcfg_locale_text(locale, "Password"));
	si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (si < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Password : <input type="text" maxlength="63" name="wan_pppoe_ppp_password" value=""/></p> */
	child_index = -1;
	input_index = ezcfg_html_add_body_child(html, si, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
	if (input_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_TEXT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_MAXLENGTH_ATTRIBUTE_NAME, "63", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(WAN, PPPOE_PPP_PASSWD), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, PPPOE_PPP_PASSWD), &p);
	if (p != NULL) {
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, p, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		free(p);
	}

	/* <p>Service Name : </p> */
	snprintf(buf, sizeof(buf), "%s&nbsp;:&nbsp;",
		ezcfg_locale_text(locale, "Service Name"));
	si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (si < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Service Name : <input type="text" maxlength="63" name="wan_pppoe_service" value=""/></p> */
	child_index = -1;
	input_index = ezcfg_html_add_body_child(html, si, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
	if (input_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_TEXT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_MAXLENGTH_ATTRIBUTE_NAME, "63", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(WAN, PPPOE_SERVICE), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, PPPOE_SERVICE), &p);
	if (p != NULL) {
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, p, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		free(p);
	}

	/* <p>Connecting Mode : </p> */
	snprintf(buf, sizeof(buf), "%s&nbsp;:&nbsp;",
		ezcfg_locale_text(locale, "Connecting Mode"));
	si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (si < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Connecting Mode : <select name="wan_pppoe_keep_enable"></select></p> */
	child_index = -1;
	select_index = ezcfg_html_add_body_child(html, si, child_index, EZCFG_HTML_SELECT_ELEMENT_NAME, NULL);
	if (select_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, select_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(WAN, PPPOE_KEEP_ENABLE), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <p>Connecting Mode : <select name="wan_pppoe_keep_enable"><option value="1" selected="selected">Keep Alive</option></select></p> */
	buf[0] = '\0';
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, PPPOE_KEEP_ENABLE), &p);
	if (p != NULL) {
		snprintf(buf, sizeof(buf), "%s", p);
		free(p);
	}
	child_index = -1;
	for (i = 0; i < 2; i++) {
		char tmp[2];
		snprintf(tmp, sizeof(tmp), "%d", i);
		child_index = ezcfg_html_add_body_child(html, select_index, child_index, EZCFG_HTML_OPTION_ELEMENT_NAME, ezcfg_locale_text(locale, ezcfg_util_text_get_pppoe_keep_enable(i)));
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, tmp, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		if (strcmp(tmp, buf) == 0) {
			ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		}
	}

	/* Connecting mode is Dial On Demand */
	if (strcmp(buf, "0") == 0) {
		/* <p>Max Idle Time : </p> */
		snprintf(buf, sizeof(buf), "%s&nbsp;:&nbsp;",
			ezcfg_locale_text(locale, "Max Idle Time"));
		si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_P_ELEMENT_NAME, buf);
		if (si < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}

		/* <p>Max Idle Time : <input type="text" maxlength="4" size="4" name="wan_pppoe_ppp_idle" value="600"/> seconds</p> */
		child_index = -1;
		input_index = ezcfg_html_add_body_child(html, si, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, ezcfg_locale_text(locale, " Seconds"));
		if (input_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_TEXT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_MAXLENGTH_ATTRIBUTE_NAME, "4", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_SIZE_ATTRIBUTE_NAME, "4", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(WAN, PPPOE_PPP_IDLE), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, PPPOE_PPP_IDLE), &p);
		if (p != NULL) {
			ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, p, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
			free(p);
		}
	}
	/* Connecting mode is Keep Alive */
	else {
		/* <p>Redial Period : </p> */
		snprintf(buf, sizeof(buf), "%s&nbsp;:&nbsp;",
			ezcfg_locale_text(locale, "Redial Period"));
		si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_P_ELEMENT_NAME, buf);
		if (si < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}

		/* <p>Redial Period : <input type="text" maxlength="4" size="4" name="wan_pppoe_ppp_redial" value="30"/> seconds</p> */
		child_index = -1;
		input_index = ezcfg_html_add_body_child(html, si, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, ezcfg_locale_text(locale, " Seconds"));
		if (input_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_TEXT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_MAXLENGTH_ATTRIBUTE_NAME, "4", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_SIZE_ATTRIBUTE_NAME, "4", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(WAN, PPPOE_PPP_LCP_ECHO_INTERVAL), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, PPPOE_PPP_LCP_ECHO_INTERVAL), &p);
		if (p != NULL) {
			ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, p, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
			free(p);
		}
	}

func_exit:
	return si;
}

static int set_setup_wan_pptp(
	struct ezcfg_http_html_admin *admin,
	struct ezcfg_locale *locale,
	int pi, int si)
{
	return si;
}

static int set_setup_wan_l2tp(
	struct ezcfg_http_html_admin *admin,
	struct ezcfg_locale *locale,
	int pi, int si)
{
	return si;
}

struct type_handler {
	char *name;
	int (*handler)(
		struct ezcfg_http_html_admin *admin,
		struct ezcfg_locale *locale,
		int pi, int si);
};

static struct type_handler wan_type_handlers[] = {
	{ "static", set_setup_wan_static },
	{ "pppoe", set_setup_wan_pppoe },
	{ "pptp", set_setup_wan_pptp },
	{ "l2tp", set_setup_wan_l2tp },
};

static int set_setup_wan_optional_settings(
	struct ezcfg_http_html_admin *admin,
	struct ezcfg_locale *locale,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	struct ezcfg_nvram *nvram;
	struct ezcfg_html *html;
	int child_index;
	int select_index, input_index;
	char buf[1024];
	char *p = NULL;
	int i;

	ASSERT(admin != NULL);
	ASSERT(pi > 1);

	ezcfg = admin->ezcfg;
	nvram = admin->nvram;
	html = admin->html;

	/* <h3>Optional Settings</h3> */
	si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_H3_ELEMENT_NAME, ezcfg_locale_text(locale, "Optional Settings"));
	if (si < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Host Name : </p> */
	snprintf(buf, sizeof(buf), "%s&nbsp;:&nbsp;",
		ezcfg_locale_text(locale, "Host Name"));
	si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (si < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Host Name : <input type="text" maxlength="39" name="wan_hostname" value=""/></p> */
	child_index = -1;
	input_index = ezcfg_html_add_body_child(html, si, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
	if (input_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_TEXT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_MAXLENGTH_ATTRIBUTE_NAME, "39", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(WAN, HOSTNAME), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, HOSTNAME), &p);
	if (p != NULL) {
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, p, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		free(p);
	}

	/* <p>Domain Name : </p> */
	snprintf(buf, sizeof(buf), "%s&nbsp;:&nbsp;",
		ezcfg_locale_text(locale, "Domain Name"));
	si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (si < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Domain Name : <input type="text" maxlength="63" name="wan_domain" value=""/></p> */
	child_index = -1;
	input_index = ezcfg_html_add_body_child(html, si, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
	if (input_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_TEXT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_MAXLENGTH_ATTRIBUTE_NAME, "63", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(WAN, DOMAIN), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, DOMAIN), &p);
	if (p != NULL) {
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, p, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		free(p);
	}

	/* <p>MTU : </p> */
	snprintf(buf, sizeof(buf), "%s&nbsp;:&nbsp;",
		ezcfg_locale_text(locale, "MTU"));
	si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (si < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>MTU : <select name="wan_mtu_enable"></select></p> */
	child_index = -1;
	select_index = ezcfg_html_add_body_child(html, si, child_index, EZCFG_HTML_SELECT_ELEMENT_NAME, NULL);
	if (select_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, select_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(WAN, MTU_ENABLE), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <p>MTU : <select name="wan_mtu_enable"><option value="0" selected="selected">Auto</option></select></p> */
	buf[0] = '\0';
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, MTU_ENABLE), &p);
	if (p != NULL) {
		snprintf(buf, sizeof(buf), "%s", p);
		free(p);
	}
	child_index = -1;
	for (i = 0; i < 2; i++) {
		char tmp[2];
		snprintf(tmp, sizeof(tmp), "%d", i);
		child_index = ezcfg_html_add_body_child(html, select_index, child_index, EZCFG_HTML_OPTION_ELEMENT_NAME, ezcfg_locale_text(locale, ezcfg_util_text_get_mtu_enable(i)));
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, tmp, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		if (strcmp(tmp, buf) == 0) {
			ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		}
	}

	/* MTU setting mode is manual */
	if (strcmp(buf, "1") == 0) {
		/* <p>MTU Size: </p> */
		snprintf(buf, sizeof(buf), "%s&nbsp;:&nbsp;",
			ezcfg_locale_text(locale, "MTU Size"));
		si = ezcfg_html_add_body_child(html, pi, si, EZCFG_HTML_P_ELEMENT_NAME, buf);
		if (si < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}

		/* <p>MTU Size : <input type="text" maxlength="6" name="wan_mtu" value="1500"/></p> */
		child_index = -1;
		input_index = ezcfg_html_add_body_child(html, si, child_index, EZCFG_HTML_INPUT_ELEMENT_NAME, NULL);
		if (input_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_TYPE_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_INPUT_TYPE_TEXT, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_MAXLENGTH_ATTRIBUTE_NAME, "6", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(WAN, MTU), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, MTU), &p);
		if (p != NULL) {
			ezcfg_html_add_body_child_attribute(html, input_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, p, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
			free(p);
		}
	}

func_exit:
	return si;
}

static int set_html_main_setup_wan(
	struct ezcfg_http_html_admin *admin,
	struct ezcfg_locale *locale,
	int pi, int si)
{
	struct ezcfg *ezcfg;
	struct ezcfg_nvram *nvram;
	struct ezcfg_html *html;
	int content_index, child_index;
	int p_index, select_index;
	char buf[1024];
	char wan_type[16];
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
	/* <h3>WAN Setup</h3> */
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_H3_ELEMENT_NAME, ezcfg_locale_text(locale, "WAN Setup"));
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* <p>Connection Type : </p> */
	snprintf(buf, sizeof(buf), "%s&nbsp;:&nbsp;",
		ezcfg_locale_text(locale, "Connection Type"));
	child_index = ezcfg_html_add_body_child(html, content_index, child_index, EZCFG_HTML_P_ELEMENT_NAME, buf);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}

	/* store child_index */
	p_index = child_index;

	/* <p>Connection Type : <select></select> </p> */
	child_index = -1;
	select_index = ezcfg_html_add_body_child(html, p_index, child_index, EZCFG_HTML_SELECT_ELEMENT_NAME, NULL);
	if (select_index < 0) {
		err(ezcfg, "ezcfg_html_add_body_child error.\n");
		goto func_exit;
	}
	ezcfg_html_add_body_child_attribute(html, select_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, NVRAM_SERVICE_OPTION(WAN, TYPE), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* <p>Connection Type : <select><option></option></select> </p> */
	wan_type[0] = '\0';
	ezcfg_nvram_get_entry_value(nvram, NVRAM_SERVICE_OPTION(WAN, TYPE), &p);
	if (p != NULL) {
		snprintf(wan_type, sizeof(wan_type), "%s", p);
		free(p);
	}
	child_index = -1;
	for (i = 0; i < ezcfg_util_wan_get_type_length(); i++) {
		child_index = ezcfg_html_add_body_child(html, select_index, child_index, EZCFG_HTML_OPTION_ELEMENT_NAME, ezcfg_locale_text(locale, ezcfg_util_wan_get_type_desc_by_index(i)));
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
		ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_VALUE_ATTRIBUTE_NAME, ezcfg_util_wan_get_type_name_by_index(i), EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		if (strcmp(wan_type, ezcfg_util_wan_get_type_name_by_index(i)) == 0) {
			ezcfg_html_add_body_child_attribute(html, child_index, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_HTML_SELECTED_ATTRIBUTE_NAME, EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
		}
	}

	/* restore index pointer */
	child_index = p_index;

	/* setup connection type mode special settings */
	for (i = 0; i < ARRAY_SIZE(wan_type_handlers); i++) {
		if (strcmp(wan_type, wan_type_handlers[i].name) == 0) {
			child_index = wan_type_handlers[i].handler(admin, locale, content_index, child_index);
			if (child_index < 0) {
				err(ezcfg, "set_status_wan_%s.\n", wan_type);
				goto func_exit;
			}
			/* stop loop */
			break;
		}
	}

	if (strcmp(wan_type, "disabled") != 0) {
		child_index = set_setup_wan_optional_settings(admin, locale, content_index, child_index);
		if (child_index < 0) {
			err(ezcfg, "ezcfg_html_add_body_child error.\n");
			goto func_exit;
		}
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

static int build_admin_setup_wan_response(struct ezcfg_http_html_admin *admin)
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
		ezcfg_locale_set_domain(locale, EZCFG_HTTP_HTML_ADMIN_SETUP_SYSTEM_DOMAIN);
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
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_NAME_ATTRIBUTE_NAME, "setup_wan", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_METHOD_ATTRIBUTE_NAME, "post", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);
	ezcfg_html_add_body_child_attribute(html, form_index, EZCFG_HTML_ACTION_ATTRIBUTE_NAME, EZCFG_HTTP_HTML_ADMIN_PREFIX_URI "setup_wan", EZCFG_XML_ELEMENT_ATTRIBUTE_TAIL);

	/* HTML div head */
	child_index = ezcfg_http_html_admin_set_html_head(admin, form_index, -1);
	if (child_index < 0) {
		err(ezcfg, "ezcfg_http_html_admin_set_html_head error.\n");
		rc = -1;
		goto func_exit;
	}

	/* HTML div main */
	child_index = set_html_main_setup_wan(admin, locale, form_index, child_index);
	if (child_index < 0) {
		err(ezcfg, "set_html_main_setup_wan error.\n");
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

static bool do_admin_setup_wan_action(struct ezcfg_http_html_admin *admin)
{
	struct ezcfg *ezcfg;
	struct ezcfg_link_list *list;
	char *tz_area, *tz_location;
	bool ret = false;

	ezcfg = admin->ezcfg;
	list = admin->post_list;

	if (ezcfg_http_html_admin_get_action(admin) == HTTP_HTML_ADMIN_ACT_SAVE) {
		/* set area and location */
		tz_area = ezcfg_link_list_get_node_value_by_name(list, NVRAM_SERVICE_OPTION(UI, TZ_AREA));
		tz_location = ezcfg_link_list_get_node_value_by_name(list, NVRAM_SERVICE_OPTION(UI, TZ_LOCATION));
		if ((tz_area != NULL) && (tz_location != NULL)) {
			if (ezcfg_util_tzdata_check_area_location(tz_area, tz_location) == true) {
				ret = ezcfg_link_list_insert(list, NVRAM_SERVICE_OPTION(SYS, TZ_AREA), tz_area);
				if (ret == false) {
					return false;
				}
				ret = ezcfg_link_list_insert(list, NVRAM_SERVICE_OPTION(SYS, TZ_LOCATION), tz_location);
				if (ret == false) {
					return false;
				}
			}
		}

		ret = ezcfg_http_html_admin_save_settings(admin);
	}
	return ret;
}

static bool handle_admin_setup_wan_post(struct ezcfg_http_html_admin *admin)
{
	struct ezcfg *ezcfg;
	bool ret = false;

	ezcfg = admin->ezcfg;

	if (ezcfg_http_html_admin_handle_post_data(admin) == true) {
		ret = do_admin_setup_wan_action(admin);
	}
	return ret;
}

/**
 * Public functions
 **/
int ezcfg_http_html_admin_setup_wan_handler(struct ezcfg_http_html_admin *admin)
{
	struct ezcfg *ezcfg;
	struct ezcfg_http *http;
	int ret = -1;

	ASSERT(admin != NULL);

	ezcfg = admin->ezcfg;
	http = admin->http;

	/* admin setup_wan uri=[/admin/setup_wan] */
	if (ezcfg_http_request_method_cmp(http, EZCFG_HTTP_METHOD_POST) == 0) {
		/* do post handling */
		info(ezcfg, "[%s]\n", ezcfg_http_get_message_body(http));
		handle_admin_setup_wan_post(admin);
	}

	ret = build_admin_setup_wan_response(admin);
	return ret;
}
