# ------------------------------------------------------------------------------
# Copyright (C) 2010-2011 ezbox project
# ------------------------------------------------------------------------------

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = COPYING exported_symbols

SUBDIRS = .

AM_CPPFLAGS = \
	-include $(top_builddir)/config.h \
	-I$(srcdir)/include \
        -DSYSCONFDIR="$(sysconfdir)" \
        -DDATADIR="$(datadir)" \
        -DEZCFG_DEBUG \
        -Wall

AM_LDFLAGS = -L. -ldl

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

#ezcd_SOURCES += src/rc/rc_preboot.c
#ezcd_SOURCES += src/rc/rc_data_storage.c
#ezcd_SOURCES += src/rc/rc_system.c
#ezcd_SOURCES += src/rc/rc_func.c
#ezcd_SOURCES += src/rc/rc_debug.c

# ubootenv support
ezcd_SOURCES += src/ubootenv.c

# udhcpc script support
if CONFIG_EZBOX_WAN_NIC
ezcd_SOURCES += src/udhcpc_script.c
endif

# upgrade firmware support
ezcd_SOURCES += src/upfw.c

# hotplug2
#ezcd_SOURCES += src/pop/pop_etc_hotplug2_rules.c
#ezcd_SOURCES += src/rc/rc_hotplug2.c

# mount system fs
#ezcd_SOURCES += src/rc/rc_mount_system_fs.c

# network
if CONFIG_EZBOX_LAN_NIC
#ezcd_SOURCES += src/rc/rc_lan_if.c
#ezcd_SOURCES += src/rc/rc_lan.c
endif
if CONFIG_EZBOX_WLAN_NIC
#ezcd_SOURCES += src/rc/rc_wlan_if.c
if CONFIG_EZBOX_SERVICE_WPA_SUPPLICANT
#ezcd_SOURCES += src/rc/rc_wpa_supplicant.c
#ezcd_SOURCES += src/pop/pop_etc_wpa_supplicant_conf.c
endif
endif
if CONFIG_EZBOX_WAN_NIC
#ezcd_SOURCES += src/rc/rc_wan_if.c
#ezcd_SOURCES += src/rc/rc_wan.c
endif

# telnet service
if CONFIG_EZBOX_SERVICE_TELNETD
#ezcd_SOURCES += src/rc/rc_telnetd.c
endif

# dhcp/dns service
if CONFIG_EZBOX_SERVICE_DNSMASQ
#ezcd_SOURCES += src/pop/pop_etc_dnsmasq_conf.c
#ezcd_SOURCES += src/rc/rc_dnsmasq.c
endif

# LAN side binding services
if CONFIG_EZBOX_LAN_NIC
#ezcd_SOURCES += src/rc/rc_lan_services.c
endif

# WAN side binding services
if CONFIG_EZBOX_WAN_NIC
#ezcd_SOURCES += src/rc/rc_wan_services.c
endif

# nano-X service
if CONFIG_EZBOX_SERVICE_NANO_X
#ezcd_SOURCES += src/rc/rc_nano_x.c
endif

# TinyX/kdrive service
if CONFIG_EZBOX_SERVICE_KDRIVE
#ezcd_SOURCES += src/rc/rc_kdrive.c
endif

# fontconfig service
if CONFIG_EZBOX_SERVICE_FONTCONFIG
#ezcd_SOURCES += src/pop/pop_etc_fonts.c
#ezcd_SOURCES += src/rc/rc_fontconfig.c
endif

# dillo web browser service
if CONFIG_EZBOX_SERVICE_DILLO
#ezcd_SOURCES += src/pop/pop_etc_dillo_dillorc.c
#ezcd_SOURCES += src/rc/rc_dillo.c
endif

# EMC2 Enhanced Machine Controller service
if CONFIG_EZBOX_SERVICE_EMC2
#ezcd_SOURCES += src/pop/pop_etc_emc2_rtapi_conf.c
#ezcd_SOURCES += src/pop/pop_etc_emc2_configs.c
#ezcd_SOURCES += src/rc/rc_realtime.c
#ezcd_SOURCES += src/rc/rc_emc2.c
#ezcd_SOURCES += src/rc/rc_emc2_latency_test.c
endif

ezcd_DEPENDENCIES = libezcfg.so $(lib_LTLIBRARIES) $(RCSO_SO) $(RCSO_EXE)

ezcd_CPPFLAGS = $(AM_CPPFLAGS) $(EZBOX_DISTRO_DEFINE) $(LIBEZCFG_INCLUDE) $(LIBEZCFG_API_INCLUDE)
#ezcd_LDADD = $(LIBEZCFG_LIBS)

ezcd_LDFLAGS = $(AM_LDFLAGS) -L.libs -lezcd

libezcfg.so: $(LIBEZCFG_LIB)
	-rm -f libezcfg.so
	$(LN_S) $(LIBEZCFG_LIB)/libezcfg.so libezcfg.so

###############################
# libezcd
###############################
LIBEZCD_CURRENT = 1
LIBEZCD_REVISION = 0
LIBEZCD_AGE = 0

libezcdincludedir = $(includedir)/libezcd

libezcdinclude_HEADERS = \
        include/utils.h \
        include/ezcd.h

lib_LTLIBRARIES = libezcd.la

libezcd_la_DEPENDENCIES = libezcfg.so

libezcd_la_CPPFLAGS = $(AM_CPPFLAGS) $(EZBOX_DISTRO_DEFINE) $(LIBEZCFG_INCLUDE) $(LIBEZCFG_API_INCLUDE)

libezcd_la_LDFLAGS = \
        -version-info $(LIBEZCD_CURRENT):$(LIBEZCD_REVISION):$(LIBEZCD_AGE) \
        -export-symbols $(srcdir)/exported_symbols

# utils
libezcd_la_SOURCES = src/utils/utils_check_ezcd.c
libezcd_la_SOURCES += src/utils/utils_check_service.c
if CONFIG_EZBOX_WAN_NIC
libezcd_la_SOURCES += src/utils/utils_check_wan_interface.c
endif
libezcd_la_SOURCES += src/utils/utils_nvram_match.c
libezcd_la_SOURCES += src/utils/utils_file_get_keyword.c
libezcd_la_SOURCES += src/utils/utils_get_kernel_modules.c
libezcd_la_SOURCES += src/utils/utils_get_kernel_version.c
libezcd_la_SOURCES += src/utils/utils_handle_kernel_module.c
libezcd_la_SOURCES += src/utils/utils_mount_partition.c
libezcd_la_SOURCES += src/utils/utils_get_bootcfg_keyword.c
libezcd_la_SOURCES += src/utils/utils_get_device_info.c
libezcd_la_SOURCES += src/utils/utils_sync_cfg_with_nvram.c
libezcd_la_SOURCES += src/utils/utils_sync_nvram_with_cfg.c
libezcd_la_SOURCES += src/utils/utils_get_rc_act_type.c
if CONFIG_EZBOX_WAN_NIC
libezcd_la_SOURCES += src/utils/utils_get_wan_type.c
endif
libezcd_la_SOURCES += src/utils/utils_file_get_line.c
libezcd_la_SOURCES += src/utils/utils_find_pid_by_name.c
libezcd_la_SOURCES += src/utils/utils_crc32.c
libezcd_la_SOURCES += src/utils/utils_udev_trigger_pop.c

# login
libezcd_la_SOURCES += src/pop/pop_etc_passwd.c
libezcd_la_SOURCES += src/pop/pop_etc_group.c

# mdev
#libezcd_la_SOURCES += src/pop/pop_etc_mdev_conf.c

# kernel modules
libezcd_la_SOURCES += src/pop/pop_etc_modules.c

# nvram
libezcd_la_SOURCES += src/pop/pop_etc_nvram_conf.c

# ezcd
libezcd_la_SOURCES += src/pop/pop_etc_ezcd_conf.c

# ezcm
libezcd_la_SOURCES += src/pop/pop_etc_ezcm_conf.c

# netbase
libezcd_la_SOURCES += src/pop/pop_etc_hosts.c
libezcd_la_SOURCES += src/pop/pop_etc_network_interfaces.c
libezcd_la_SOURCES += src/pop/pop_etc_protocols.c
libezcd_la_SOURCES += src/pop/pop_etc_mactab.c
libezcd_la_SOURCES += src/pop/pop_etc_resolv_conf.c

# iptables service
if CONFIG_EZBOX_SERVICE_IPTABLES
libezcd_la_SOURCES += src/pop/pop_etc_l7_protocols.c
endif

# init
libezcd_la_SOURCES += src/pop/pop_etc_inittab.c

# base files
libezcd_la_SOURCES += src/pop/pop_etc_banner.c
libezcd_la_SOURCES += src/pop/pop_etc_hostname.c
libezcd_la_SOURCES += src/pop/pop_etc_mtab.c
libezcd_la_SOURCES += src/pop/pop_etc_profile.c

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ezcd.pc


###############################
# rcso
###############################
RCSO_CFLAGS = -Wall -Werror -rdynamic -fPIC
RCSO_CFLAGS += $(ezcd_CPPFLAGS)
RCSO_SRC = rc_action.c
RCSO_SRC += rc_exec.c
RCSO_SRC += rc_login.c
#RCSO_SRC += rc_mdev.c
RCSO_SRC += rc_preboot.c
RCSO_SRC += rc_load_modules.c
RCSO_SRC += rc_mount_system_fs.c
RCSO_SRC += rc_data_storage.c
RCSO_SRC += rc_nvram.c
RCSO_SRC += rc_ezcd.c
RCSO_SRC += rc_ezcfg_httpd.c
RCSO_SRC += rc_ezcm.c
RCSO_SRC += rc_netbase.c
if CONFIG_EZBOX_SERVICE_IPTABLES
RCSO_SRC += rc_iptables.c
endif
RCSO_SRC += rc_init.c
RCSO_SRC += rc_base_files.c
RCSO_SRC += rc_ldconfig.c
# syslogd service
if CONFIG_EZBOX_SERVICE_SYSLOGD
RCSO_SRC += rc_syslogd.c
endif
# klogd service
if CONFIG_EZBOX_SERVICE_KLOGD
RCSO_SRC += rc_klogd.c
endif
RCSO_SRC += rc_loopback.c


RCSO_SO = $(subst .c,.so,$(RCSO_SRC))
RCSO_EXE = $(subst .c,.exe,$(RCSO_SRC))

%.so : src/rcso/%.c
	$(CC) $(RCSO_CFLAGS) -L.libs -lezcd -shared $< -o $@

%.exe : src/rcso/%.c
	$(CC) $(RCSO_CFLAGS) $(CFLAGS) $(LDFLAGS) $(LIBS) -D_EXEC_ -L.libs -lezcd $< -o $@

