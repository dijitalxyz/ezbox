#
# Copyright (C) 2011 ezbox project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tk
PKG_VERSION:=8.5.8
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)$(PKG_VERSION)-src.tar.gz
PKG_SOURCE_URL:=@SF/tk
#PKG_MD5SUM:=7f123e53b3daaaba2478d3af5a0752e3

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)$(PKG_VERSION)

PKG_FIXUP:=libtool
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/tk
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=TCL Lang
  URL:=http://www.tcl.tk
endef

define Package/tk/description
Tk is a graphical user interface toolkit that takes developing desktop 
applications to a higher level than conventional approaches. Tk is the 
standard GUI not only for Tcl, but for many other dynamic languages, 
and can produce rich, native applications that run unchanged across 
Windows, Mac OS X, Linux and more. 
endef

CONFIGURE_PATH := unix

CONFIGURE_VARS += \
	tk_cv_strtod_unbroken=ok

CONFIGURE_ARGS += \
	--enable-threads

MAKE_PATH := unix

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/*.h $(1)/usr/include/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libtk*.{a,so*} $(1)/usr/lib/
endef

define Package/tk/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so $(1)/usr/lib

	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/* $(1)/usr/bin
endef

$(eval $(call BuildPackage,tk))
