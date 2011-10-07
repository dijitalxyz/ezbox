#
# Copyright (C) 2011 ezbox project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=hisilicon-hi351x-adec
#PKG_VERSION:=0.1
PKG_RELEASE:=1

#PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2

include $(INCLUDE_DIR)/package.mk

define Package/hisilicon-hi351x-adec/Default
  SECTION:=kernel
  CATEGORY:=Kernel modules
  DEPENDS:=@PACKAGE_kmod-hi351x-adec
  SUBMENU:=Proprietary HI351x Audio Decode driver
  SUBMENUDEP:=@TARGET_hi351x
endef

define KernelPackage/hi351x-adec/Default
  $(call Package/hisilicon-hi351x-adec/Default)
  SECTION:=kernel
  DEPENDS:=@TARGET_hi351x
  TITLE:=Kernel driver for HI351x chipsets
  FILES:=$(PKG_BUILD_DIR)/driver$(1)/hi351x_adec.ko
  AUTOLOAD:=$(call AutoLoad,30,hi351x_adec)
endef

define KernelPackage/hi351x-adec/Default/description
 This package contains the proprietary Audio Decode driver for the Hisilicon
 HI351x chipset.
endef

define KernelPackage/hi3515-adec
$(call KernelPackage/hi351x-adec/Default,-hi3515)
  TITLE+= (hi3515)
endef

define KernelPackage/hi3515-adec/description
$(call KernelPackage/hi351x-adec/Default/description)
endef

MAKE_KMOD := $(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		PATH="$(TARGET_PATH)" \
		SUBDIRS="$(PKG_BUILD_DIR)/kmod" \

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/driver-hi3515
endef

define Build/Compile
	# Compile the kernel part
	# $(MAKE_KMOD) \
		SUBDIRS="$(PKG_BUILD_DIR)/driver" \
		MODFLAGS="-DMODULE -mlong-calls" \
		modules
	$(CP) files/hi3515_adec.ko $(PKG_BUILD_DIR)/driver-hi3515/hi351x_adec.ko
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
endef

$(eval $(call KernelPackage,hi3515-adec))
