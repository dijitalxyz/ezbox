#
# Copyright (C) 2010 ezbox-project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luasp
PKG_REV:=11
PKG_VERSION:=r$(PKG_REV)
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=svn
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://luasp.googlecode.com/svn/trunk/

PKG_FIXUP:=libtool
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/luasp/Default
  SUBMENU:=Lua Server Pages (luasp)
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=Lua Server Pages (luasp) scripting language
  URL:=http://code.google.com/p/luasp/
endef

define Package/luasp/Default/description
  Lua Server Pages (luasp) is an easy open source scripting language
  that follows the PHP syntax.
endef

define Package/libluasp
  $(call Package/luasp/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+liblua
  TITLE+= (library)
endef

define Package/libluasp/description
$(call Package/luasp/Default/description)
  This package contains the Lua Server Pages(luasp) support shared library,
  used by other programs.
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) CC="$(TARGET_CC)"
endef

define Build/Install
	$(INSTALL_DIR) $(PKG_INSTALL_DIR)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libluasp.so* $(PKG_INSTALL_DIR)/usr/lib/
	$(INSTALL_DIR) $(PKG_INSTALL_DIR)/usr/include
	$(CP) $(PKG_BUILD_DIR)/llsplib.h $(PKG_INSTALL_DIR)/usr/include/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/llsplib.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libluasp.so* $(1)/usr/lib/
endef

define Package/libluasp/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libluasp.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libluasp))
