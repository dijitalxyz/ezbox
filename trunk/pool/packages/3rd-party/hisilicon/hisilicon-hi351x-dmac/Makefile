#
# Copyright (C) 2011 ezbox project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=hisilicon-hi351x-dmac
#PKG_VERSION:=0.1
PKG_RELEASE:=1

#PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2

include $(INCLUDE_DIR)/package.mk

define Package/hisilicon-hi351x-dmac/Default
  SECTION:=kernel
  CATEGORY:=Kernel modules
  DEPENDS:=@PACKAGE_kmod-hi351x-dmac
  SUBMENU:=Proprietary HI351x DMA Controller driver
  SUBMENUDEP:=@TARGET_hi351x
endef

define KernelPackage/hi351x-dmac/Default
  $(call Package/hisilicon-hi351x-dmac/Default)
  SECTION:=kernel
  DEPENDS:=@TARGET_hi351x
  TITLE:=Kernel driver for HI351x chipsets
  FILES:=$(PKG_BUILD_DIR)/driver/hidmac.ko
  AUTOLOAD:=$(call AutoLoad,30,hidmac)
endef

define KernelPackage/hi351x-dmac/Default/description
 This package contains the proprietary DMA controller driver for the Hisilicon
 HI351x chipset.
endef

define KernelPackage/hi351x-dmac
$(call KernelPackage/hi351x-dmac/Default,)
  TITLE+= (normal version)
endef

define KernelPackage/hi351x-dmac/description
$(call KernelPackage/hi351x-dmac/Default/description)
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
	$(CP) files/hidmac.ko $(PKG_BUILD_DIR)/driver/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
endef

$(eval $(call KernelPackage,hi351x-dmac))
