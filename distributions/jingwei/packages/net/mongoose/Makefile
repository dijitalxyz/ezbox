#
# Copyright (C) 2010 ezbox-project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mongoose
PKG_REV:=54c80baaddbf
PKG_VERSION:=20100625
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=hg
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://mongoose.googlecode.com/hg/

include $(INCLUDE_DIR)/package.mk

define Package/mongoose
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Mongoose web server
  URL:=http://code.google.com/p/mongoose/
  DEPENDS:=+libpthread +libopenssl
endef

define Package/mongoose/description
  Mongoose is an easy to use web server.
  It can be embedded into existing application to provide a web interface to it.
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) CC="$(TARGET_CC)" LIB="libmongoose.so" linux
endef

define Build/Install
	$(INSTALL_DIR) $(PKG_INSTALL_DIR)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mongoose $(PKG_INSTALL_DIR)/usr/bin
endef

define Package/mongoose/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/mongoose $(1)/usr/bin
endef

$(eval $(call BuildPackage,mongoose))
