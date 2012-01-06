## Process this file with automake to produce Makefile.in

TESTS = check_xml
check_PROGRAMS = check_xml
check_xml_SOURCES = check_xml.c $(top_builddir)/include/ezcfg.h
check_xml_CFLAGS = -I$(top_builddir)/include -I$(top_builddir)/api/include $(EZBOX_DISTRO_DEFINE) @CHECK_CFLAGS@
check_xml_LDADD = $(top_builddir)/libezcfg.la @CHECK_LIBS@

