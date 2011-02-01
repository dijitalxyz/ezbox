#
# Copyright (C) 2011 ezbox project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=emc2
PKG_VERSION:=20110131
PKG_REV:=7bc299e60228754e3460baecdb020e77ac37399a
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=git://git.linuxcnc.org/git/emc2.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)

PKG_INSTALL:=1
#PKG_FIXUP:=libtool

include $(INCLUDE_DIR)/package.mk

define Package/emc2
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=EMC (the Enhanced Machine Control)
  DEPENDS:=+rtai
  URL:=http://www.linuxcnc.org/
endef

define Package/emc2/description
  EMC (the Enhanced Machine Control) is a software system for
  computer control of machine tools such as milling machines
  and lathes.
endef

define Build/Configure
	touch $(PKG_BUILD_DIR)/Makefile
	mv $(PKG_BUILD_DIR)/Makefile $(PKG_BUILD_DIR)/Makefile.orig
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		AR=$(TARGET_CROSS)ar \
		MWIN_INCLUDE=$(STAGING_DIR)/usr/include/microwin \
		MWIN_LIB=$(STAGING_DIR)/usr/lib
endef

define Build/Install
	$(MAKE) -C $(PKG_BUILD_DIR) \
		INSTALL_DIR=$(PKG_INSTALL_DIR)/usr/lib \
		CC=$(TARGET_CC) \
		AR=$(TARGET_CROSS)ar \
		MWIN_INCLUDE=$(STAGING_DIR)/usr/include/microwin \
		MWIN_LIB=$(STAGING_DIR)/usr/lib \
		install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib/
endef

define Package/emc2/install
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/bin $(1)/etc
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,emc2))
