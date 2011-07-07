# ------------------------------------------------------------------------------
# Copyright (C) 2010 ezbox project
# ------------------------------------------------------------------------------

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = COPYING exported_symbols

SUBDIRS = .

AM_CPPFLAGS = \
	-include $(top_builddir)/config.h \
	-I$(srcdir)/include \
        -DSYSCONFDIR=\""$(sysconfdir)"\" \
        -DDATADIR=\""$(datadir)"\" \
        -DEZCFG_DEBUG \
        -Wall

AM_LDFLAGS = -L.

# ezcd depends on libezcfg
# LIBS += -lezcfg

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
ezcd_SOURCES += src/utils/utils_check_ezcd.c
ezcd_SOURCES += src/utils/utils_check_service.c
if CONFIG_EZBOX_WAN_NIC
ezcd_SOURCES += src/utils/utils_check_wan_interface.c
endif
ezcd_SOURCES += src/utils/utils_file_get_keyword.c
ezcd_SOURCES += src/utils/utils_get_kernel_modules.c
ezcd_SOURCES += src/utils/utils_get_kernel_version.c
ezcd_SOURCES += src/utils/utils_get_device_info.c
if CONFIG_EZBOX_WAN_NIC
ezcd_SOURCES += src/utils/utils_get_wan_type.c
endif
ezcd_SOURCES += src/utils/utils_file_get_line.c
ezcd_SOURCES += src/utils/utils_find_pid_by_name.c
ezcd_SOURCES += src/utils/utils_crc32.c

# base files
ezcd_SOURCES += src/pop/pop_etc_banner.c
ezcd_SOURCES += src/pop/pop_etc_hostname.c
ezcd_SOURCES += src/pop/pop_etc_mtab.c
ezcd_SOURCES += src/pop/pop_etc_profile.c
ezcd_SOURCES += src/rc/rc_base_files.c

# ubootenv support
ezcd_SOURCES += src/ubootenv.c

# udhcpc script support
if CONFIG_EZBOX_WAN_NIC
ezcd_SOURCES += src/udhcpc_script.c
endif

# upgrade firmware support
ezcd_SOURCES += src/upfw.c

# ezcd
ezcd_SOURCES += src/pop/pop_etc_ezcd_conf.c
ezcd_SOURCES += src/rc/rc_ezcd.c
ezcd_SOURCES += src/rc/rc_ezcfg_httpd.c

# nvram
ezcd_SOURCES += src/pop/pop_etc_nvram_conf.c
ezcd_SOURCES += src/rc/rc_nvram.c

# ezcm
ezcd_SOURCES += src/pop/pop_etc_ezcm_conf.c
ezcd_SOURCES += src/rc/rc_ezcm.c

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
if CONFIG_EZBOX_LAN_NIC
ezcd_SOURCES += src/rc/rc_lan_if.c
ezcd_SOURCES += src/rc/rc_lan.c
endif
if CONFIG_EZBOX_WAN_NIC
ezcd_SOURCES += src/rc/rc_wan_if.c
ezcd_SOURCES += src/rc/rc_wan.c
endif

# syslog service
if CONFIG_EZBOX_SERVICE_SYSLOG
ezcd_SOURCES += src/rc/rc_syslog.c
endif

# telnet service
if CONFIG_EZBOX_SERVICE_TELNETD
ezcd_SOURCES += src/rc/rc_telnetd.c
endif

# dhcp/dns service
if CONFIG_EZBOX_SERVICE_DNSMASQ
ezcd_SOURCES += src/pop/pop_etc_dnsmasq_conf.c
ezcd_SOURCES += src/rc/rc_dnsmasq.c
endif

# iptables service
if CONFIG_EZBOX_SERVICE_IPTABLES
ezcd_SOURCES += src/pop/pop_etc_l7_protocols.c
ezcd_SOURCES += src/rc/rc_iptables.c
endif

# LAN side binding services
if CONFIG_EZBOX_LAN_NIC
ezcd_SOURCES += src/rc/rc_lan_services.c
endif

# WAN side binding services
if CONFIG_EZBOX_WAN_NIC
ezcd_SOURCES += src/rc/rc_wan_services.c
endif

# nano-X service
if CONFIG_EZBOX_SERVICE_NANO_X
ezcd_SOURCES += src/rc/rc_nano_x.c
endif

# TinyX/kdrive service
if CONFIG_EZBOX_SERVICE_KDRIVE
ezcd_SOURCES += src/rc/rc_kdrive.c
endif

# dillo web browser service
if CONFIG_EZBOX_SERVICE_DILLO
ezcd_SOURCES += src/pop/pop_etc_dillo_dillorc.c
ezcd_SOURCES += src/rc/rc_dillo.c
endif

# EMC2 Enhanced Machine Controller service
if CONFIG_EZBOX_SERVICE_EMC2
ezcd_SOURCES += src/pop/pop_etc_emc2_rtapi_conf.c
ezcd_SOURCES += src/pop/pop_etc_emc2_configs.c
ezcd_SOURCES += src/rc/rc_emc2.c
endif

ezcd_DEPENDENCIES = libezcfg.so

ezcd_CPPFLAGS = $(AM_CPPFLAGS) $(EZBOX_DISTRO_DEFINE) $(LIBEZCFG_INCLUDE) $(LIBEZCFG_API_INCLUDE)
#ezcd_LDADD = $(LIBEZCFG_LIBS)

# pkgconfigdir = $(libdir)/pkgconfig
# pkgconfig_DATA = ezcd.pc

libezcfg.so: $(LIBEZCFG_LIB)
	-rm -f libezcfg.so
	$(LN_S) $(LIBEZCFG_LIB)/libezcfg.so libezcfg.so

