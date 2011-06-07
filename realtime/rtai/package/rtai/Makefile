# 
# Copyright (C) 2008-2011 ezbox project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=rtai
# PKG_VERSION:=3.8.1
PKG_VERSION:=20110603
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=cvs
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
# PKG_SOURCE_URL:=http://www.rtai.org/RTAI/
PKG_SOURCE_URL:=:pserver:anonymous@cvs.gna.org:/cvs/rtai
PKG_SOURCE_SUBDIR:=magma
PKG_SOURCE_VERSION:=-D$(PKG_VERSION)

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)/magma

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
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/modules
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib/
	$(SED) 's,$(STAGING_DIR),,g' $(1)/usr/lib/liblxrt.la
	$(CP) $(PKG_INSTALL_DIR)/usr/modules/* $(1)/usr/modules/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/rtai-config $(1)/usr/bin/
	$(SED) \
		's,DESTDIR,$(STAGING_DIR),g' \
		$(1)/usr/bin/rtai-config
	$(INSTALL_DIR) $(2)/bin
	for f in rtai-config; do \
		$(LN) ../../usr/bin/$$$$f $(2)/bin/ ; \
	done
endef

define Package/rtai/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/calibration
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/modules
	$(INSTALL_DIR) $(1)/usr/share/rtai
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/* $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/calibration/* $(1)/usr/calibration/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/modules/*.ko $(1)/usr/modules/
	$(CP) $(PKG_INSTALL_DIR)/usr/share/rtai/* $(1)/usr/share/rtai/
	$(SED) 's,DESTDIR,,g' $(1)/usr/bin/rtai-config
	$(SED) 's,$(KERNEL_BUILD_DIR),/usr/src,g' $(1)/usr/bin/rtai-config
endef

$(eval $(call BuildPackage,rtai))

