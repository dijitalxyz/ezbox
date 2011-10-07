#
# Copyright (C) 2011 ezbox project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=hisilicon-hi351x-hifb
#PKG_VERSION:=0.1
PKG_RELEASE:=1

#PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2

include $(INCLUDE_DIR)/package.mk

define Package/hisilicon-hi351x-hifb/Default
  SECTION:=kernel
  CATEGORY:=Kernel modules
  DEPENDS:=@PACKAGE_kmod-hi351x-hifb
  SUBMENU:=Proprietary HI351x Frame Buffer driver
  SUBMENUDEP:=@TARGET_hi351x
endef

define KernelPackage/hi351x-hifb/Default
  $(call Package/hisilicon-hi351x-hifb/Default)
  SECTION:=kernel
  DEPENDS:=@TARGET_hi351x
  TITLE:=Kernel driver for HI351x chipsets
  FILES:=$(PKG_BUILD_DIR)/driver/hifb.ko
  AUTOLOAD:=$(call AutoLoad,30,hifb)
endef

define KernelPackage/hi351x-hifb/Default/description
 This package contains the proprietary frame buffer driver for the Hisilicon
 HI351x chipset.
endef

define KernelPackage/hi351x-hifb
$(call KernelPackage/hi351x-hifb/Default,)
  TITLE+= (normal version)
endef

define KernelPackage/hi351x-hifb/description
$(call KernelPackage/hi351x-hifb/Default/description)
endef

MAKE_KMOD := $(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		PATH="$(TARGET_PATH)" \
		SUBDIRS="$(PKG_BUILD_DIR)/kmod" \

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/driver
endef

define Build/Compile
	# Compile the kernel part
	# $(MAKE_KMOD) \
		SUBDIRS="$(PKG_BUILD_DIR)/driver" \
		MODFLAGS="-DMODULE -mlong-calls" \
		modules
	$(CP) files/hifb.ko $(PKG_BUILD_DIR)/driver/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
endef

$(eval $(call KernelPackage,hi351x-hifb))
