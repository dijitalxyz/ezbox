## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-include $(top_builddir)/config.h \
	-I$(top_builddir)/include \
	-I$(top_builddir)/api/include \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-DDATADIR=\""$(datadir)"\" \
	-DEZCFG_DEBUG \
	-Wall -Werror

#AM_LDFLAGS = -L$(top_builddir)/.libs

TESTS = check_xml
check_PROGRAMS = check_xml
check_xml_SOURCES = check_xml.c $(top_builddir)/include/ezcfg.h
check_xml_CFLAGS = $(AM_CPPFLAGS) $(EZBOX_DISTRO_DEFINE) @CHECK_CFLAGS@
check_xml_LDADD = @CHECK_LIBS@ $(top_builddir)/libezcfg.la
#check_xml_LDFLAGS = $(AM_LDFLAGS) -L$(top_builddir)/.libs -lezcfg @CHECK_LIBS@
