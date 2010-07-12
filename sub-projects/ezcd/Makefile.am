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
	-I$(top_srcdir)/libezcfg \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-DLIBEXECDIR=\""$(libexecdir)"\"

AM_LDFLAGS = \
	-Wl,--as-needed -pthread

CLEANFILES =

# ------------------------------------------------------------------------------
# ezbox Configuration Library
# ------------------------------------------------------------------------------
LIBEZCFG_CURRENT=1
LIBEZCFG_REVISION=1
LIBEZCFG_AGE=1

include_HEADERS = libezcfg/libezcfg.h
lib_LTLIBRARIES = libezcfg/libezcfg.la

libezcfg_libezcfg_la_SOURCES = \
	libezcfg/libezcfg-private.h \
	libezcfg/libezcfg.h \
	libezcfg/libezcfg-list.c \
	libezcfg/libezcfg.c

libezcfg_libezcfg_la_LDFLAGS = \
	--version-info $(LIBEZCFG_CURRENT):$(LIBEZCFG_REVISION):$(LIBEZCFG_AGE) \
	-export-symbols $(top_srcdir)/libezcfg/exported_symbols

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libezcfg/libezcfg.pc

EXTRA_DIST += libezcfg/exported_symbols libezcfg/COPYING
# move lib from $(libdir) to $(rootlib_execdir) and update devel link, if needed
libezcfg-install-move-hook:
	if test "$(libdir)" != "$(rootlib_execdir)"; then \
		mkdir -p $(DESTDIR)$(rootlib_execdir) && \
		so_img_name=$$(readlink $(DESTDIR)$(libdir)/libezcfg.so) && \
		so_img_rel_target_prefix=$$(echo $(libdir) | sed 's,\(^/\|\)[^/][^/]*,..,g') && \
		ln -sf $$so_img_rel_target_prefix$(rootlib_execdir)/$$so_img_name $(DESTDIR)$(libdir)/libezcfg.so && \
		mv $(DESTDIR)$(libdir)/libezcfg.so.* $(DESTDIR)$(rootlib_execdir); \
	fi

libezcfg-uninstall-move-hook:
	rm -f $(DESTDIR)$(rootlib_execdir)/libezcfg.so*

INSTALL_EXEC_HOOKS = libezcfg-install-move-hook
UNINSTALL_EXEC_HOOKS = libezcfg-uninstall-move-hook

# ------------------------------------------------------------------------------
# ezbox Configuration Daemon
# ------------------------------------------------------------------------------
sbin_PROGRAMS = \
        src/ezcd

src_ezcd_SOURCES = \
	src/main.c \
	src/ezcd.c

# ------------------------------------------------------------------------------
# ezbox config tests
# ------------------------------------------------------------------------------
TESTS = test/ezcfg-test.pl

check_PROGRAMS = \
	libezcfg/test-libezcfg

libezcfg_test_libezcfg_SOURCES = libezcfg/test-libezcfg.c
libezcfg_test_libezcfg_LDADD = libezcfg/libezcfg.la
