#
# Copyright (C) 2010 ezbox-project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ezcfg
PKG_REV:=301
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

# set to 1 to enable debugging
DEBUG=0

define Package/ezcfg/Default
  SUBMENU:=ezcfg
  TITLE:=ezbox config utility
  URL:=http://ezbox.googlecode.com/svn/sub-projects/ezcfg/
endef

define Package/ezcfg/Default/description
  ezcfg is an easy to use config utility for ezbox.
  It can be used to configurate your ezbox system.
endef

define Package/libezcfg
  $(call Package/ezcfg/Default)
  SECTION:=libs
  CATEGORY:=Base system
  DEPENDS:=+libpthread
  TITLE+= (library)
endef

define Package/libezcfg/description
  $(call Package/ezcfg/Default/description)
  This package contains the ezcfg shared library, used by other
  programs.
endef

define Package/ezcd
  $(call Package/ezcfg/Default)
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=+libezcfg
  TITLE+= (binary -- ezbox config daemon (EZCD))
endef

define Package/ezcd/description
  $(call Package/ezcfg/Default/description)
  This package contains the ezcfg utility binary.
endef

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

define Package/libezcfg/install
	$(INSTALL_DIR) $(1)/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libezcfg.so* $(1)/lib
endef

define Package/ezcd/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/ezcd $(1)/sbin/
	$(LN) ./sbin/ezcd $(1)/init
	$(LN) ezcd $(1)/sbin/ezcm
	$(LN) ezcd $(1)/sbin/nvram
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,libezcfg))
$(eval $(call BuildPackage,ezcd))
