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
	src/nvram.c \
	src/rc.c \
	src/rc/rc_system.c \
	src/rc/rc_func.c \
	src/utils.c

# base files
ezcd_SOURCES += \
	src/pop/pop_etc_banner.c \
	src/pop/pop_etc_hostname.c \
	src/pop/pop_etc_profile.c \
	src/rc/rc_base_files.c

# ubootenv support
ezcd_SOURCES += src/ubootenv.c

# ezcd
ezcd_SOURCES += \
	src/rc/rc_ezcd.c

# hotplug2
ezcd_SOURCES += \
	src/pop/pop_etc_hotplug2_rules.c \
	src/rc/rc_hotplug2.c

# kernel modules
ezcd_SOURCES += \
	src/pop/pop_etc_modules.c \
	src/rc/rc_load_modules.c

# init
ezcd_SOURCES += \
	src/pop/pop_etc_inittab.c \
	src/rc/rc_init.c

# login
ezcd_SOURCES += \
	src/pop/pop_etc_passwd.c \
	src/pop/pop_etc_group.c \
	src/rc/rc_login.c

# netbase
ezcd_SOURCES += \
	src/pop/pop_etc_hosts.c \
	src/pop/pop_etc_network_interfaces.c \
	src/pop/pop_etc_protocols.c \
	src/pop/pop_etc_resolv_conf.c \
	src/rc/rc_netbase.c

# network
ezcd_SOURCES += \
	src/rc/rc_loopback.c \
	src/rc/rc_lan_if.c

ezcd_DEPENDENCIES = \
	libezcfg.so

ezcd_CPPFLAGS = $(AM_CPPFLAGS) $(LIBEZCFG_INCLUDE) $(LIBEZCFG_API_INCLUDE)
#ezcd_LDADD = $(LIBEZCFG_LIBS)

# pkgconfigdir = $(libdir)/pkgconfig
# pkgconfig_DATA = ezcd.pc

libezcfg.so: $(LIBEZCFG_LIB)
	$(LN_S) $(LIBEZCFG_LIB)/libezcfg.so libezcfg.so

