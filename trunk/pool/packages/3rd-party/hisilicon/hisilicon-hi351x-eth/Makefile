#
# Copyright (C) 2011 ezbox project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=hisilicon-hi351x-eth
#PKG_VERSION:=0.1
PKG_RELEASE:=1

#PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2

include $(INCLUDE_DIR)/package.mk

define Package/hisilicon-hi351x-eth/Default
  SECTION:=kernel
  CATEGORY:=Kernel modules
  DEPENDS:=@PACKAGE_kmod-hi351x-eth
  SUBMENU:=Proprietary HI351x Ethernet driver
  SUBMENUDEP:=@TARGET_hi351x
endef

define KernelPackage/hi351x-eth/Default
  $(call Package/hisilicon-hi351x-eth/Default)
  SECTION:=kernel
  DEPENDS:=@TARGET_hi351x
  TITLE:=Kernel driver for HI351x chipsets
  FILES:=$(PKG_BUILD_DIR)/driver/hiether.ko
  AUTOLOAD:=$(call AutoLoad,30,hiether)
endef

define KernelPackage/hi351x-eth/Default/description
 This package contains the proprietary ethernet driver for the Hisilicon
 HI351x chipset.
endef

define KernelPackage/hi351x-eth
$(call KernelPackage/hi351x-eth/Default,)
  TITLE+= (normal version)
endef

define KernelPackage/hi351x-eth/description
$(call KernelPackage/hi351x-eth/Default/description)
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
	$(CP) files/hiether.ko $(PKG_BUILD_DIR)/driver/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
endef

$(eval $(call KernelPackage,hi351x-eth))
