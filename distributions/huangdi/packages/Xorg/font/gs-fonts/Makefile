#
# Copyright (C) 2008-2011 ezbox project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=gs-fonts
PKG_VERSION:=8.11
PKG_RELEASE:=1

PKG_SOURCE:=ghostscript-fonts-std-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR=$(BUILD_DIR)/Xorg/$(_CATEGORY)/$(PKG_NAME)-$(PKG_VERSION)/fonts/
PKG_SOURCE_URL:=@SF/gs-fonts
# PKG_MD5SUM:=4c6c3f4e902dd5ee0a121e8c41d040bd

include $(INCLUDE_DIR)/package.mk

define Package/gs-fonts
  SECTION:=xorg-font
  CATEGORY:=Xorg
  SUBMENU:=font
  DEPENDS:= @DISPLAY_SUPPORT
  TITLE:=gs-fonts
  URL:=http://sourceforge.net/projects/gs-fonts/
endef

define Build/Compile
	true
endef

define Build/Configure
	true
endef

define Package/gs-fonts/install
	$(INSTALL_DIR) $(1)/usr/share/fonts/gs-fonts
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/*.afm $(1)/usr/share/fonts/gs-fonts
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/*.pfb $(1)/usr/share/fonts/gs-fonts
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/*.pfm $(1)/usr/share/fonts/gs-fonts
endef

$(eval $(call BuildPackage,gs-fonts))

