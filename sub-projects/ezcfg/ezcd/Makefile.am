# ------------------------------------------------------------------------------
# Copyright (C) 2010 ezbox project
# ------------------------------------------------------------------------------

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = COPYING exported_symbols

SUBDIRS = .

AM_CPPFLAGS = \
	-include $(top_builddir)/config.h \
	-I$(srcdir)/include

sbin_PROGRAMS = \
	ezcd

ezcd_SOURCES = \
	src/main.c \
	src/ezcd.c \
	src/ezcm.c \
	src/init.c \
	src/nvram.c

ezcd_CPPFLAGS = $(AM_CPPFLAGS) $(LIBEZCFG_INCLUDE) $(LIBEZCFG_API_INCLUDE)
#ezcd_LDFLAGS = $(AM_LDFLAGS) -L$(LIBEZCFG_LIB) -lezcfg
#ezcd_LIBADD = $(LIBEZCFG_LTLIB)
ezcd_LDADD = $(LIBEZCFG_LTLIB)

# pkgconfigdir = $(libdir)/pkgconfig
# pkgconfig_DATA = ezcd.pc

