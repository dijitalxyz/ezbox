# ------------------------------------------------------------------------------
# Copyright (C) 2010 ezbox project
# ------------------------------------------------------------------------------

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = COPYING exported_symbols

SUBDIRS = .

AM_CPPFLAGS = \
	-I$(srcdir)/include \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-DEZCFG_DEBUG

LDADD = libezcfg.la

libezcfgincludedir = $(includedir)/libezcfg

libezcfginclude_HEADERS = \
	api/include/ezcfg-api.h \
	api/include/ezcfg-api-errno.h

lib_LTLIBRARIES = libezcfg.la

libezcfg_la_CPPFLAGS = $(AM_CPPFLAGS) $(EZBOX_DISTRO_DEFINE) -I$(srcdir)/api/include

libezcfg_la_LDFLAGS = \
	--version-info $(LIBEZCFG_CURRENT):$(LIBEZCFG_REVISION):$(LIBEZCFG_AGE) \
	-export-symbols $(srcdir)/exported_symbols

libezcfg_la_SOURCES = src/common/ezcfg.c
libezcfg_la_SOURCES += src/common/list.c

# ezcfg Master/Worker thread model
libezcfg_la_SOURCES += src/thread/thread.c
libezcfg_la_SOURCES += src/thread/master.c
libezcfg_la_SOURCES += src/thread/worker.c

# ezcfg utils
libezcfg_la_SOURCES += src/util/util.c
libezcfg_la_SOURCES += src/util/util_crc32.c
libezcfg_la_SOURCES += src/util/util_adler32.c

# ezcfg control
libezcfg_la_SOURCES += src/ctrl/ctrl.c 

# ezcfg nvram
libezcfg_la_SOURCES += src/nvram/nvram.c
libezcfg_la_SOURCES += src/nvram/nvram_defaults.c

# ezcfg UUID
libezcfg_la_SOURCES += src/uuid/uuid.c

# ezcfg socket
libezcfg_la_SOURCES += src/socket/socket.c

# ezcfg HTTP protocol
libezcfg_la_SOURCES += src/http/http.c

# ezcfg XML doc
libezcfg_la_SOURCES += src/xml/xml.c

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

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libezcfg.pc

