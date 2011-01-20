#
# Copyright (C) 2010 Zeta Labs
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nxlib
PKG_VERSION:=20100410
PKG_REV:=6a3e586142572897ef08f38be41e8ced772c587d
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=git://microwindows.org/nxlib
PKG_SOURCE_PROTO:=git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_BUILD_DIR=$(BUILD_DIR)/Xorg/$(_CATEGORY)/$(PKG_NAME)-$(PKG_VERSION)/

PKG_INSTALL:=1
#PKG_FIXUP:=libtool

include $(INCLUDE_DIR)/package.mk

define Package/nxlib
  SECTION:=xorg-libraries
  CATEGORY:=Xorg
  SUBMENU:=libraries
  TITLE:=NXLIB X11 Emulation Library for Nano-X.
  DEPENDS:=+microwin
  URL:=http://www.microwindows.org/
endef

define Package/nxlib/description
  NXLIB build an X11 -> Nano-X conversion library,
  allowing binaries compiled for X11 to run unmodified on a
  Microwindows based system, using the Nano-X API.
endef

define Build/Configure
	$(CP) ./files/nxlib-Makefile $(PKG_BUILD_DIR)/Makefile
	$(CP) ./files/X11/*.h $(PKG_BUILD_DIR)/X11/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		AR=$(TARGET_CROSS)ar \
		MWIN_INCLUDE=$(STAGING_DIR)/usr/include/microwin \
		MWIN_LIB=$(STAGING_DIR)/usr/lib
endef

define Build/Install
	$(INSTALL_DIR) $(PKG_INSTALL_DIR)/usr/include/X11/extensions $(PKG_INSTALL_DIR)/usr/lib
	$(MAKE) -C $(PKG_BUILD_DIR) \
		INSTALL_DIR=$(PKG_INSTALL_DIR)/usr/lib \
		CC=$(TARGET_CC) \
		AR=$(TARGET_CROSS)ar \
		MWIN_INCLUDE=$(STAGING_DIR)/usr/include/microwin \
		MWIN_LIB=$(STAGING_DIR)/usr/lib \
		install
	$(CP) $(PKG_BUILD_DIR)/nxlib.h $(PKG_INSTALL_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/X11/*.h $(PKG_INSTALL_DIR)/usr/include/X11/
	$(CP) $(PKG_BUILD_DIR)/X11/*.h $(PKG_INSTALL_DIR)/usr/include/X11/extensions/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib/
endef

define Package/nxlib/install
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/bin $(1)/etc
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,nxlib))
