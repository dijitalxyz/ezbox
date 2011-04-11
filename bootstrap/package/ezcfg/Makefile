#
# Copyright (C) 2010 ezbox-project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ezcfg
PKG_REV:=763
PKG_VERSION:=r$(PKG_REV)
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=svn
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://ezbox.googlecode.com/svn/sub-projects/ezcfg
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)

PKG_FIXUP:=libtool
PKG_INSTALL:=1

PKG_BUILD_DEPENDS:=opkg/host

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

# set to 1 to enable debugging
DEBUG=0

define Package/ezcfg
  SECTION:=base
  CATEGORY:=Base system
  MAINTAINER:=Zeta Labs <zetalabs@gmail.com>
  TITLE:=ezbox config utility
  URL:=http://ezbox.googlecode.com/svn/sub-projects/ezcfg/
  MENU:=1
  DEPENDS:=+libpthread $(ICONV_DEPENDS) $(INTL_DEPENDS)
endef

define Package/ezcfg/description
  ezcfg is an easy to use config utility for ezbox.
  It can be used to configurate your ezbox system.
endef

define Package/ezcfg/config
	source "$(SOURCE)/Config.in"
endef

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

CONFIGURE_ARGS += \
	--with-ezbox-distro=$(EZBOX_DISTRO)

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
	$(INSTALL_DIR) $(1)/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libezcfg.so* $(1)/lib
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/ezcd $(1)/sbin/
	$(LN) ./sbin/ezcd $(1)/init
	$(LN) ezcd $(1)/sbin/ezcm
	$(LN) ezcd $(1)/sbin/nvram
	$(LN) ezcd $(1)/sbin/rc
	$(LN) ezcd $(1)/sbin/ubootenv
	$(LN) ezcd $(1)/sbin/udhcpc.script
	$(LN) ezcd $(1)/sbin/upfw
	$(INSTALL_DIR) $(1)/usr/share/ezcfg/html/lang
	$(CP) $(PKG_BUILD_DIR)/i18n/html/lang/* $(1)/usr/share/ezcfg/html/lang/
endef

$(eval $(call BuildPackage,ezcfg))
