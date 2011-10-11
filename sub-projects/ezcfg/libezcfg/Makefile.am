# ------------------------------------------------------------------------------
# Copyright (C) 2010 ezbox project
# ------------------------------------------------------------------------------

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = COPYING exported_symbols

LIBEZCFG_CURRENT = 1
LIBEZCFG_REVISION = 0
LIBEZCFG_AGE = 0

SUBDIRS = .

AM_CPPFLAGS = \
	-include $(top_builddir)/config.h \
	-I$(srcdir)/include \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-DDATADIR=\""$(datadir)"\" \
	-DEZCFG_DEBUG \
	-Wall

LDADD = libezcfg.la

libezcfgincludedir = $(includedir)/libezcfg

libezcfginclude_HEADERS = \
	api/include/ezcfg-api.h \
	api/include/ezcfg-api-errno.h

lib_LTLIBRARIES = libezcfg.la

libezcfg_la_CPPFLAGS = $(AM_CPPFLAGS) $(EZBOX_DISTRO_DEFINE) -I$(srcdir)/api/include

libezcfg_la_LDFLAGS = \
	-version-info $(LIBEZCFG_CURRENT):$(LIBEZCFG_REVISION):$(LIBEZCFG_AGE) \
	-export-symbols $(srcdir)/exported_symbols

libezcfg_la_SOURCES = src/common/ezcfg.c
libezcfg_la_SOURCES += src/common/list.c
libezcfg_la_SOURCES += src/common/link_list.c

# ezcfg Master/Worker thread model
libezcfg_la_SOURCES += src/thread/thread.c
libezcfg_la_SOURCES += src/thread/master.c
libezcfg_la_SOURCES += src/thread/worker.c

# ezcfg utils
libezcfg_la_SOURCES += src/util/util.c
libezcfg_la_SOURCES += src/util/util_adler32.c
libezcfg_la_SOURCES += src/util/util_base64.c
libezcfg_la_SOURCES += src/util/util_crc32.c
libezcfg_la_SOURCES += src/util/util_conf.c
libezcfg_la_SOURCES += src/util/util_language.c
libezcfg_la_SOURCES += src/util/util_rc.c
libezcfg_la_SOURCES += src/util/util_service_binding.c
libezcfg_la_SOURCES += src/util/util_text.c
libezcfg_la_SOURCES += src/util/util_tzdata.c
libezcfg_la_SOURCES += src/util/util_wan.c

# ezcfg control
libezcfg_la_SOURCES += src/ctrl/ctrl.c 

# ezcfg nvram
libezcfg_la_SOURCES += src/nvram/nvram.c
libezcfg_la_SOURCES += src/nvram/nvram_defaults.c
libezcfg_la_SOURCES += src/nvram/nvram_validators.c

# ezcfg uevent
libezcfg_la_SOURCES += src/uevent/uevent.c

# ezcfg authentication
libezcfg_la_SOURCES += src/auth/auth.c

# ezcfg locale
libezcfg_la_SOURCES += src/locale/locale.c

# ezcfg UUID
libezcfg_la_SOURCES += src/uuid/uuid.c

# ezcfg socket
libezcfg_la_SOURCES += src/socket/socket.c

# ezcfg HTTP/socket handler
libezcfg_la_SOURCES += src/socket/socket_http.c

# ezcfg HTTP protocol
libezcfg_la_SOURCES += src/http/http.c

# ezcfg HTML/HTTP index handling
libezcfg_la_SOURCES += src/http/http_html_index.c

# ezcfg HTML/HTTP admin handling
libezcfg_la_SOURCES += src/http/http_html_admin.c

# ezcfg HTML/HTTP admin common head handling
libezcfg_la_SOURCES += src/http/admin/http_html_admin_common_head.c

# ezcfg HTML/HTTP admin head handling
libezcfg_la_SOURCES += src/http/admin/http_html_admin_head.c

# ezcfg HTML/HTTP admin foot handling
libezcfg_la_SOURCES += src/http/admin/http_html_admin_foot.c

# ezcfg HTML/HTTP admin button handling
libezcfg_la_SOURCES += src/http/admin/http_html_admin_button.c


# ezcfg HTML/HTTP admin menu handling
libezcfg_la_SOURCES += src/http/admin/http_html_admin_menu.c

# ezcfg HTML/HTTP admin menu status handling
libezcfg_la_SOURCES += src/http/admin/http_html_admin_menu_status.c

# ezcfg HTML/HTTP admin menu setup handling
libezcfg_la_SOURCES += src/http/admin/http_html_admin_menu_setup.c

# ezcfg HTML/HTTP admin menu management handling
libezcfg_la_SOURCES += src/http/admin/http_html_admin_menu_management.c


# ezcfg HTML/HTTP admin layout_css handling
libezcfg_la_SOURCES += src/http/admin/http_html_admin_layout_css.c

# ezcfg HTML/HTTP admin status_system handling
libezcfg_la_SOURCES += src/http/admin/http_html_admin_status_system.c

# ezcfg HTML/HTTP admin status_lan handling
if CONFIG_EZBOX_LAN_NIC
libezcfg_la_SOURCES += src/http/admin/http_html_admin_status_lan.c
endif

# ezcfg HTML/HTTP admin view_dhcp_client_table handling
libezcfg_la_SOURCES += src/http/admin/http_html_admin_view_dhcp_client_table.c

# ezcfg HTML/HTTP admin status_wan handling
if CONFIG_EZBOX_WAN_NIC
libezcfg_la_SOURCES += src/http/admin/http_html_admin_status_wan.c
endif

# ezcfg HTML/HTTP admin setup_system handling
libezcfg_la_SOURCES += src/http/admin/http_html_admin_setup_system.c

# ezcfg HTML/HTTP admin setup_lan handling
if CONFIG_EZBOX_LAN_NIC
libezcfg_la_SOURCES += src/http/admin/http_html_admin_setup_lan.c
endif

# ezcfg HTML/HTTP admin setup_wan handling
if CONFIG_EZBOX_WAN_NIC
libezcfg_la_SOURCES += src/http/admin/http_html_admin_setup_wan.c
endif

# ezcfg HTML/HTTP admin management_authz handling
libezcfg_la_SOURCES += src/http/admin/http_html_admin_management_authz.c

# ezcfg HTML/HTTP admin management_default handling
libezcfg_la_SOURCES += src/http/admin/http_html_admin_management_default.c

# ezcfg HTML/HTTP admin management_upgrade handling
libezcfg_la_SOURCES += src/http/admin/http_html_admin_management_upgrade.c

if CONFIG_EZBOX_SERVICE_EMC2
libezcfg_la_SOURCES += src/http/admin/http_html_admin_menu_cnc.c
# ezcfg HTML/HTTP admin cnc_setup handling
libezcfg_la_SOURCES += src/http/admin/http_html_admin_cnc_setup.c
# ezcfg HTML/HTTP admin cnc_default handling
libezcfg_la_SOURCES += src/http/admin/http_html_admin_cnc_default.c
# ezcfg HTML/HTTP admin cnc_latency handling
libezcfg_la_SOURCES += src/http/admin/http_html_admin_cnc_latency.c
endif

# ezcfg XML doc
libezcfg_la_SOURCES += src/xml/xml.c

# ezcfg HTML handling
libezcfg_la_SOURCES += src/html/html.c

# ezcfg CSS handling
libezcfg_la_SOURCES += src/css/css.c

# ezcfg SOAP protocol
libezcfg_la_SOURCES += src/soap/soap.c

# ezcfg SOAP-HTTP binding
libezcfg_la_SOURCES += src/soap/soap_http.c

# ezcfg NVRAM/SOAP-HTTP binding
libezcfg_la_SOURCES += src/soap/soap_http_nvram.c

# ezcfg IGRS protocol
libezcfg_la_SOURCES += src/igrs/igrs.c
libezcfg_la_SOURCES += src/igrs/igrs_create_session.c
libezcfg_la_SOURCES += src/igrs/igrs_invoke_service.c

# ezcfg ISDP protocol
libezcfg_la_SOURCES += src/igrs/isdp.c

# ezcfg API wrap
libezcfg_la_SOURCES += api/src/api-nvram.c
libezcfg_la_SOURCES += api/src/api-ubootenv.c
libezcfg_la_SOURCES += api/src/api-firmware.c

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libezcfg.pc

