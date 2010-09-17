# ------------------------------------------------------------------------------
# Copyright (C) 2010 ezbox project
# ------------------------------------------------------------------------------

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = COPYING exported_symbols

SUBDIRS = .

AM_CPPFLAGS = \
	-I$(srcdir)/include \
	-DSYSCONFDIR=\""$(sysconfdir)"\"

LDADD = \
	libezcfg.la

libezcfgincludedir = $(includedir)/libezcfg
libezcfginclude_HEADERS = \
	include/libezcfg-private.h \
	include/libezcfg.h

lib_LTLIBRARIES = libezcfg.la

libezcfg_la_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/api/include

libezcfg_la_LDFLAGS = \
	--version-info $(LIBEZCFG_CURRENT):$(LIBEZCFG_REVISION):$(LIBEZCFG_AGE) \
	-export-symbols $(srcdir)/exported_symbols

libezcfg_la_SOURCES = \
	src/libezcfg-private.h \
	src/libezcfg.h \
	src/libezcfg-list.c \
	src/libezcfg-util.c \
	src/libezcfg-thread.c \
	src/libezcfg.c

# ezcfg control
libezcfg_la_SOURCES += \
	src/libezcfg-ctrl.c 

# ezcfg nvram
libezcfg_la_SOURCES += \
	src/libezcfg-nvram.c

# ezcfg UUID
libezcfg_la_SOURCES += \
	src/libezcfg-uuid.c

# ezcfg socket
libezcfg_la_SOURCES += \
	src/libezcfg-socket.c

# ezcfg HTTP protocol
libezcfg_la_SOURCES += \
	src/libezcfg-http.c

# ezcfg XML doc
libezcfg_la_SOURCES += \
	src/libezcfg-xml.c

# ezcfg SOAP protocol
libezcfg_la_SOURCES += \
	src/libezcfg-soap.c

# ezcfg SOAP-HTTP binding
libezcfg_la_SOURCES += \
	src/libezcfg-soap_http.c

# ezcfg IGRS protocol
libezcfg_la_SOURCES += \
	src/libezcfg-igrs.c

# ezcfg ISDP protocol
libezcfg_la_SOURCES += \
	src/libezcfg-isdp.c

# ezcfg Master/Worker thread model
libezcfg_la_SOURCES += \
	src/libezcfg-master.c \
	src/libezcfg-worker.c

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libezcfg.pc

