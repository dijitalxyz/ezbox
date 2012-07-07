#
# Copyright (C) 2008-2011 ezbox-project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ezcfg
PKG_REV:=1624
PKG_VERSION:=r$(PKG_REV)
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=svn
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://ezbox.googlecode.com/svn/sub-projects/ezcfg
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)

PKG_FIXUP:=libtool
PKG_INSTALL:=1

PKG_BUILD_DEPENDS+=opkg/host
PKG_BUILD_DEPENDS+=PACKAGE_check:check
PKG_BUILD_DEPENDS+=BUILD_NLS:libiconv-full
PKG_BUILD_DEPENDS+=BUILD_NLS:gettext-full

include $(INCLUDE_DIR)/package.mk
ifeq ($(CONFIG_BUILD_NLS),y)
include $(INCLUDE_DIR)/nls.mk
endif

# set to 1 to enable debugging
DEBUG=0
SSL_DEPENDS:=+FEATURE_EZCFG_ezcfg-ssl-openssl:libopenssl

define Package/ezcfg
  SECTION:=base
  CATEGORY:=Base system
  MAINTAINER:=Zeta Labs <zetalabs@gmail.com>
  TITLE:=Ezbox Configuration Tools
  URL:=http://ezbox.googlecode.com/svn/sub-projects/ezcfg/
  MENU:=1
  DEPENDS:=+libpthread $(ICONV_DEPENDS) $(INTL_DEPENDS) $(SSL_DEPENDS)
endef

define Package/ezcfg/description
  Ezcfg is a collection of tools to setup the ezbox system.
  It makes system configuration easy.
endef

define Package/ezcfg/config
	source "$(SOURCE)/Config.in"
endef

# set distro name
ifneq ($(CONFIG_EZCFG_EZBOX_DISTRO_KUAFU),)
  EZBOX_DISTRO = "kuafu"
endif

ifneq ($(CONFIG_EZCFG_EZBOX_DISTRO_HUANGDI),)
  EZBOX_DISTRO = "huangdi"
endif

ifneq ($(CONFIG_EZCFG_EZBOX_DISTRO_JINGWEI),)
  EZBOX_DISTRO = "jingwei"
endif

ifneq ($(CONFIG_EZCFG_EZBOX_DISTRO_QIAOCHUI),)
  EZBOX_DISTRO = "qiaochui"
endif

ifneq ($(CONFIG_EZCFG_EZBOX_DISTRO_LILOU),)
  EZBOX_DISTRO = "lilou"
endif

ifneq ($(CONFIG_EZCFG_EZBOX_DISTRO_FUXI),)
  EZBOX_DISTRO = "fuxi"
endif

CONFIGURE_ARGS += \
	--with-ezbox-distro=$(EZBOX_DISTRO)

# set check unit test settings
ifneq ($(CONFIG_PACKAGE_check),)
CONFIGURE_ARGS += \
	--enable-ezcfg_test
else
CONFIGURE_ARGS += \
	--disable-ezcfg_test
endif

# set nls settings
#ifeq ($(EZBOX_DISTRO), "kuafu")
#CONFIGURE_ARGS += \
#	--disable-ezcfg_nls
#endif
ifneq ($(CONFIG_BUILD_NLS),)
CONFIGURE_ARGS += \
	--enable-ezcfg_nls
else
CONFIGURE_ARGS += \
	--disable-ezcfg_nls
endif


# set bridge NIC settings
CONFIGURE_ARGS += \
	--disable-bridge-nic

# set distro NIC settings
ifeq ($(EZBOX_DISTRO), "kuafu")
CONFIGURE_ARGS += \
	--disable-wan-nic
endif

ifeq ($(EZBOX_DISTRO), "qiaochui")
CONFIGURE_ARGS += \
	--disable-lan-nic
endif

# set syslog service
ifneq ($(CONFIG_BUSYBOX_CONFIG_SYSLOGD),)
CONFIGURE_ARGS += \
	--enable-service-syslogd
endif

# set klog service
ifneq ($(CONFIG_BUSYBOX_CONFIG_KLOGD),)
CONFIGURE_ARGS += \
	--enable-service-klogd
endif

# set ezcfg_httpd service
CONFIGURE_ARGS += \
	--enable-service-ezcfg_httpd

# set ezcfg_httpd_cgi_admin service
CONFIGURE_ARGS += \
	--enable-service-ezcfg_httpd_cgi_admin

# set ezcfg_httpd_cgi_index service
CONFIGURE_ARGS += \
	--enable-service-ezcfg_httpd_cgi_index

# set ezcfg_httpd_cgi_nvram service
CONFIGURE_ARGS += \
	--enable-service-ezcfg_httpd_cgi_nvram

# set ezcfg_httpd_ssi service
CONFIGURE_ARGS += \
	--enable-service-ezcfg_httpd_ssi

# set ezcfg_igrsd service
CONFIGURE_ARGS += \
	--enable-service-ezcfg_igrsd

# set ezcfg_upnpd service
CONFIGURE_ARGS += \
	--enable-service-ezcfg_upnpd

# set ezcfg_upnpd_igd1 service
CONFIGURE_ARGS += \
	--enable-service-ezcfg_upnpd_igd1

# set openssl service
ifneq ($(CONFIG_FEATURE_EZCFG_ezcfg-ssl-openssl),)
  ifneq ($(CONFIG_PACKAGE_libopenssl),)
CONFIGURE_ARGS += \
	--enable-service-openssl
  endif
endif

# set telnet service
ifneq ($(CONFIG_BUSYBOX_CONFIG_TELNETD),)
CONFIGURE_ARGS += \
	--enable-service-telnetd
endif

# set dnsmasq service
ifneq ($(CONFIG_PACKAGE_dnsmasq),)
CONFIGURE_ARGS += \
	--enable-service-dnsmasq
endif

# set iptables service
ifneq ($(CONFIG_PACKAGE_iptables),)
CONFIGURE_ARGS += \
	--enable-service-iptables \
	--enable-service-iptables_firewall \
	--enable-service-iptables_nat \
	--enable-service-iptables_dmz
endif

# set DHIS client service
ifneq ($(CONFIG_PACKAGE_dhid),)
CONFIGURE_ARGS += \
	--enable-service-dhid
endif

# set LUKS/dm-crypt for data partition service
ifneq ($(CONFIG_PACKAGE_cryptsetup),)
ifneq ($(CONFIG_PACKAGE_kmod-dm),)
CONFIGURE_ARGS += \
	--enable-service-dmcrypt_data_partition
endif
endif

# set nano-X service
ifneq ($(CONFIG_PACKAGE_microwin),)
CONFIGURE_ARGS += \
	--enable-service-nano_x
endif

# set TinyX/kdrive service
ifneq ($(CONFIG_PACKAGE_xserver-kdrive-xfbdev),)
CONFIGURE_ARGS += \
	--enable-service-kdrive
endif

# set fontconfig service
ifneq ($(CONFIG_PACKAGE_fontconfig),)
CONFIGURE_ARGS += \
	--enable-service-fontconfig
endif

# set dillo web browser service
ifneq ($(CONFIG_PACKAGE_dillo),)
CONFIGURE_ARGS += \
	--enable-service-dillo
endif

# set wpa_supplicant service
ifneq ($(CONFIG_PACKAGE_wpa-supplicant),)
CONFIGURE_ARGS += \
	--enable-service-wpa_supplicant
endif

# set hostapd service
ifneq ($(CONFIG_PACKAGE_hostapd),)
CONFIGURE_ARGS += \
	--enable-service-hostapd
endif

# set wireless STA and AP service
ifneq ($(CONFIG_PACKAGE_wpad),)
CONFIGURE_ARGS += \
	--enable-service-wpa_supplicant \
	--enable-service-hostapd
endif

# set  EMC2 (Enhanced Machine Controller) service
ifneq ($(CONFIG_PACKAGE_emc2),)
CONFIGURE_ARGS += \
	--enable-service-emc2
endif

CONFIGURE_VARS += \
	ac_cv_func_malloc_0_nonnull=yes

define Build/Configure
	(cd $(PKG_BUILD_DIR); autoreconf --install --symlink );
	$(call Build/Configure/Default, )
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/libezcfg
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libezcfg/*.h $(1)/usr/include/libezcfg
endef

define Package/ezcfg/install
	$(INSTALL_DIR) $(1)/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libezcfg.so* $(1)/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libezcd.so* $(1)/lib/
	$(INSTALL_DIR) $(1)/lib/rcso
	$(CP) $(PKG_BUILD_DIR)/ezcd/bin/rc_*.so* $(1)/lib/rcso/
	$(INSTALL_DIR) $(1)/lib/rcso/action
	$(CP) $(PKG_BUILD_DIR)/ezcd/action/* $(1)/lib/rcso/action/
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/ezcd $(1)/sbin/
	$(LN) sbin/ezcd $(1)/init
	$(LN) ezcd $(1)/sbin/ezcm
	$(LN) ezcd $(1)/sbin/nvram
	$(LN) ezcd $(1)/sbin/rc
	$(LN) ezcd $(1)/sbin/shelld
	$(LN) ezcd $(1)/sbin/upnp_monitor
	$(LN) ezcd $(1)/sbin/igrs_monitor
	$(LN) ezcd $(1)/sbin/ubootenv
	$(LN) ezcd $(1)/sbin/udhcpc.script
	$(LN) ezcd $(1)/sbin/upfw
	$(INSTALL_DIR) $(1)/usr/share/ezcfg/html/lang
	$(CP) $(PKG_BUILD_DIR)/i18n/html/lang/* $(1)/usr/share/ezcfg/html/lang/
	$(INSTALL_DIR) $(1)/boot
	$(CP) $(PKG_BUILD_DIR)/doc/ezbox_boot.cfg $(1)/boot/
	$(CP) $(PKG_BUILD_DIR)/doc/ezbox_rootfs.cfg $(1)/boot/
endef

define Feature/ezcfg-ssl-none
  TARGET_TITLE:=EZCFG SSL Backend
  TARGET_NAME:=EZCFG
  NAME:=none
endef

define Feature/ezcfg-ssl-openssl
  TARGET_TITLE:=EZCFG SSL Backend
  TARGET_NAME:=EZCFG
  NAME:=openssl
endef

$(eval $(call Feature,ezcfg-ssl-none))
$(eval $(call Feature,ezcfg-ssl-openssl))
$(eval $(call BuildPackage,ezcfg))
