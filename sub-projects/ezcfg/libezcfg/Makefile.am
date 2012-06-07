# ------------------------------------------------------------------------------
# Copyright (C) 2010 ezbox project
# ------------------------------------------------------------------------------

ACLOCAL_AMFLAGS = -I m4


if CONFIG_EZBOX_EZCFG_TEST
EXTRA_DIST = COPYING exported_symbols_test
SUBDIRS = . lib tests
else
EXTRA_DIST = COPYING exported_symbols
SUBDIRS = . lib
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libezcfg.pc

