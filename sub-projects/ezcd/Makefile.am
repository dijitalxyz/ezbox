# ------------------------------------------------------------------------------
# Copyright (C) 2010 TANG HUI <zetalabs@gmail.com>
# ------------------------------------------------------------------------------

SUBDIRS = .

EXTRA_DIST = autogen.sh

ACLOCAL_AMFLAGS = -I m4

AM_MAKEFLAGS = --no-print-directory

AM_CPPFLAGS = \
	-include $(top_builddir)/config.h \
	-I$(top_srcdir)/include \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-DLIBEXECDIR=\""$(libexecdir)"\"

AM_LDFLAGS = \
	-Wl,--as-needed

CLEANFILES =

# ------------------------------------------------------------------------------
# ezbox Configuration Daemon
# ------------------------------------------------------------------------------
sbin_PROGRAMS = \
        src/ezcd

src_ezcd_SOURCES = \
	src/main.c
