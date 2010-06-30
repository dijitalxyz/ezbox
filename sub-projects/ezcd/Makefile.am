# ------------------------------------------------------------------------------
# Copyright (C) 2010 TANG HUI <zetalabs@gmail.com>
# ------------------------------------------------------------------------------

SUBDIRS = .

EXTRA_DIST = autogen.sh

ACLOCAL_AMFLAGS = -I m4

AM_MAKEFLAGS = --no-print-directory

AM_CPPFLAGS = \
	-include $(top_builddir)/include/ezcd_config.h \
	-I$(top_srcdir)/include \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-DLIBEXECDIR=\""$(libexecdir)"\"

AM_LDFLAGS = \
	-Wl,--as-needed -pthread

CLEANFILES =

# ------------------------------------------------------------------------------
# ezbox Configuration Daemon
# ------------------------------------------------------------------------------
sbin_PROGRAMS = \
        src/ezcd

src_ezcd_SOURCES = \
	src/main.c \
	src/ezcd.c
