#
# Copyright (C) 2008-2011 ezbox-project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ezcnc
PKG_REV:=1161
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=svn
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://ezbox.googlecode.com/svn/sub-projects/ezcnc
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)

PKG_FIXUP:=libtool
PKG_INSTALL:=1

PKG_BUILD_DEPENDS:=opkg/host

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

# set to 1 to enable debugging
DEBUG=0

define Package/ezcnc
  SECTION:=utils
  CATEGORY:=Real-Time
  MAINTAINER:=Zeta Labs <zetalabs@gmail.com>
  TITLE:=ezbox CNC interface
  DEPENDS:=+ezcfg +emc2
  URL:=http://ezbox.googlecode.com/svn/sub-projects/ezcfg/
endef

define Package/ezcnc/description
  ezcnc is an easy CNC interface for ezbox project.
  It can be used to control your ezbox-based CNC machine.
endef

#define Package/ezcnc/config
#	source "$(SOURCE)/Config.in"
#endef

define Build/Configure
	(cd $(PKG_BUILD_DIR); autoreconf --install --symlink );
	$(call Build/Configure/Default, )
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)
endef

define Build/InstallDev
endef

define Package/ezcnc/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libezcnc.so* $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/ezcnc $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,ezcnc))
