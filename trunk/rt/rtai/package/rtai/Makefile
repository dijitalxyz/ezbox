# 
# Copyright (C) 2011 ezbox project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=rtai
PKG_VERSION:=3.8.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.rtai.org/RTAI/

include $(INCLUDE_DIR)/package.mk

define Package/rtai/Default
  SUBMENU:=RTAI
  SECTION:=utils
  CATEGORY:=Real-Time
  TITLE:=Real-Time Application Interface
  URL:=http://www.rtai.org/
endef

define Package/rtai/Default/description
 RTAI - the RealTime Application Interface for Linux -
 which lets you write applications with strict timing constraints for your
 favourite operating system.
endef

define Package/rtai
$(call Package/rtai/Default)
  DEPENDS:=
  TITLE+= (interpreter)
endef

define Package/rtai/description
$(call Package/rtai/Default/description)
 This package contains the RTAI kernel modules.
endef

TARGET_CFLAGS += -I$(LINUX_DIR)/arch/$(LINUX_KARCH)/include

CONFIGURE_ARGS += --with-linux-dir=$(LINUX_DIR)
# CONFIGURE_ARGS += --disable-leds

#define Build/Configure
#endef

#define Build/Compile
#endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/lua{,lib,conf}.h $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/lauxlib.h $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/lnum_config.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/liblua.{a,so*} $(1)/usr/lib/
	ln -sf liblua.so.$(PKG_VERSION) $(1)/usr/lib/liblualib.so
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_BUILD_DIR)/etc/lua.pc $(1)/usr/lib/pkgconfig/
endef

define Package/rtai/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/lua $(1)/usr/bin/
endef

$(eval $(call BuildPackage,rtai))

