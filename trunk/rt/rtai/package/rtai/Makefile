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

#TARGET_CFLAGS += -I$(LINUX_DIR)/arch/$(LINUX_KARCH)/include

CONFIGURE_ARGS += --with-linux-dir=$(LINUX_DIR)
CONFIGURE_ARGS += --disable-leds

#define Build/Configure
#endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		ARCH="$(ARCH)" \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		all
endef

define Build/Install
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		ARCH="$(ARCH)" \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/liblxrt.a $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/liblxrt.la $(1)/usr/lib/
	$(SED) 's,$(PKG_INSTALL_DIR),,g' $(1)/usr/lib/liblxrt.la
endef

define Package/rtai/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/calibration
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/modules
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/* $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/calibration/* $(1)/usr/calibration/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/modules/*.ko $(1)/usr/modules/
endef

$(eval $(call BuildPackage,rtai))

