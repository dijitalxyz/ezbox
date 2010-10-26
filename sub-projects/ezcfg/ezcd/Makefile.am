# ------------------------------------------------------------------------------
# Copyright (C) 2010 ezbox project
# ------------------------------------------------------------------------------

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = COPYING exported_symbols

SUBDIRS = .

AM_CPPFLAGS = \
	-include $(top_builddir)/config.h \
	-I$(srcdir)/include

AM_LDFLAGS = -L.

#libezcfg depends on pthread
LIBS = -lezcfg

sbin_PROGRAMS = \
	ezcd

ezcd_SOURCES = \
	src/main.c \
	src/preinit.c \
	src/ezcd.c \
	src/ezcm.c \
	src/init.c \
	src/nvram.c

ezcd_DEPENDENCIES = \
	libezcfg.so

ezcd_CPPFLAGS = $(AM_CPPFLAGS) $(LIBEZCFG_INCLUDE) $(LIBEZCFG_API_INCLUDE)
#ezcd_LDADD = $(LIBEZCFG_LIBS)

# pkgconfigdir = $(libdir)/pkgconfig
# pkgconfig_DATA = ezcd.pc

libezcfg.so: $(LIBEZCFG_LIB)
	$(LN_S) $(LIBEZCFG_LIB)/libezcfg.so libezcfg.so

