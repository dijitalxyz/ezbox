## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-include $(top_builddir)/config.h \
	-I$(top_builddir)/include \
	-I$(top_builddir)/api/include \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-DDATADIR=\""$(datadir)"\" \
	-DEZCFG_DEBUG \
	-Wall -Werror

AM_LDFLAGS = -L. -lezcfg

libezcfg.so: $(top_builddir)/lib/.libs/libezcfg.so
	-rm -f libezcfg.so
	$(LN_S) $(top_builddir)/lib/.libs/libezcfg.so libezcfg.so

TESTS = check_xml
check_PROGRAMS = check_xml
check_xml_DEPENDENCIES = libezcfg.so
check_xml_SOURCES = xml/check_xml.c
check_xml_CFLAGS = $(AM_CPPFLAGS) $(EZBOX_DISTRO_DEFINE) @CHECK_CFLAGS@
#check_xml_LDADD = @CHECK_LIBS@ $(top_builddir)/lib/libezcfg.la
check_xml_LDFLAGS = $(AM_LDFLAGS) @CHECK_LIBS@

