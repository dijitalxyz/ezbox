#
# Copyright (C) 2010 ZetaLabs
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ezcd
PKG_REV:=84
PKG_VERSION:=r$(PKG_REV)
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=svn
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://ezbox.googlecode.com/svn/sub-projects/ezcd
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)

PKG_FIXUP:=libtool

include $(INCLUDE_DIR)/package.mk

# set to 1 to enable debugging
DEBUG=

define Package/ezcd
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=
  TITLE:=EZbox Configuration Daemon (EZCD)
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR); autoreconf --install --symlink );
	$(call Build/Configure/Default, )

endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)
endef

define Package/ezcd/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/ezcd $(1)/sbin/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/include/ezcd{,_config}.h $(1)/usr/include
endef

$(eval $(call BuildPackage,ezcd))
