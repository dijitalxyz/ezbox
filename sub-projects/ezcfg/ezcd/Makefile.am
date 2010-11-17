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

sbin_PROGRAMS =	ezcd

# basic functions
ezcd_SOURCES = src/main.c
ezcd_SOURCES += src/preinit.c
ezcd_SOURCES += src/ezcd.c
ezcd_SOURCES += src/ezcm.c
ezcd_SOURCES += src/nvram.c
ezcd_SOURCES += src/rc.c
ezcd_SOURCES += src/rc/rc_system.c
ezcd_SOURCES += src/rc/rc_func.c

# utils
ezcd_SOURCES += src/utils/utils_get_kernel_version.c
ezcd_SOURCES += src/utils/utils_find_pid_by_name.c
ezcd_SOURCES += src/utils/utils_crc32.c

# base files
ezcd_SOURCES += src/pop/pop_etc_banner.c
ezcd_SOURCES += src/pop/pop_etc_hostname.c
ezcd_SOURCES += src/pop/pop_etc_profile.c
ezcd_SOURCES += src/rc/rc_base_files.c

# ubootenv support
ezcd_SOURCES += src/ubootenv.c

# ezcd
ezcd_SOURCES += src/rc/rc_ezcd.c

# hotplug2
ezcd_SOURCES += src/pop/pop_etc_hotplug2_rules.c
ezcd_SOURCES += src/rc/rc_hotplug2.c

# kernel modules
ezcd_SOURCES += src/pop/pop_etc_modules.c
ezcd_SOURCES += src/rc/rc_load_modules.c

# init
ezcd_SOURCES += src/pop/pop_etc_inittab.c
ezcd_SOURCES += src/rc/rc_init.c

# login
ezcd_SOURCES += src/pop/pop_etc_passwd.c
ezcd_SOURCES += src/pop/pop_etc_group.c
ezcd_SOURCES += src/rc/rc_login.c

# netbase
ezcd_SOURCES += src/pop/pop_etc_hosts.c
ezcd_SOURCES += src/pop/pop_etc_network_interfaces.c
ezcd_SOURCES += src/pop/pop_etc_protocols.c
ezcd_SOURCES += src/pop/pop_etc_resolv_conf.c
ezcd_SOURCES += src/rc/rc_netbase.c

# network
ezcd_SOURCES += src/rc/rc_loopback.c
ezcd_SOURCES += src/rc/rc_lan_if.c
ezcd_SOURCES += src/rc/rc_lan.c
ezcd_SOURCES += src/rc/rc_wan_if.c
ezcd_SOURCES += src/rc/rc_wan.c

# syslog service
ezcd_SOURCES += src/rc/rc_syslog.c

# telnet service
ezcd_SOURCES += src/rc/rc_telnetd.c

# dhcp/dns service
ezcd_SOURCES += src/pop/pop_etc_dnsmasq_conf.c
ezcd_SOURCES += src/rc/rc_dnsmasq.c

ezcd_DEPENDENCIES = libezcfg.so

ezcd_CPPFLAGS = $(AM_CPPFLAGS) $(EZBOX_DISTRO_DEFINE) $(LIBEZCFG_INCLUDE) $(LIBEZCFG_API_INCLUDE)
#ezcd_LDADD = $(LIBEZCFG_LIBS)

# pkgconfigdir = $(libdir)/pkgconfig
# pkgconfig_DATA = ezcd.pc

libezcfg.so: $(LIBEZCFG_LIB)
	-rm -f libezcfg.so
	$(LN_S) $(LIBEZCFG_LIB)/libezcfg.so libezcfg.so

