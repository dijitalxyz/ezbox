#
# Copyright (C) 2011 ezbox project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=hisilicon-hi351x-mmz
#PKG_VERSION:=0.1
PKG_RELEASE:=1

#PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2

include $(INCLUDE_DIR)/package.mk

define Package/hisilicon-hi351x-mmz/Default
  SECTION:=kernel
  CATEGORY:=Kernel modules
  DEPENDS:=@PACKAGE_kmod-hi351x-mmz
  SUBMENU:=Proprietary HI351x media memory zone driver
  SUBMENUDEP:=@TARGET_hi351x
endef

define KernelPackage/hi351x-mmz/Default
  $(call Package/hisilicon-hi351x-mmz/Default)
  SECTION:=kernel
  DEPENDS:=@TARGET_hi351x
  TITLE:=Kernel driver for HI351x chipsets
  FILES:=$(PKG_BUILD_DIR)/driver/mmz.ko
  AUTOLOAD:=$(call AutoLoad,30,mmz)
endef

define KernelPackage/hi351x-mmz/Default/description
 This package contains the proprietary media memory zone driver for the Hisilicon
 HI351x chipset.
endef

define KernelPackage/hi351x-mmz
$(call KernelPackage/hi351x-mmz/Default,)
  TITLE+= (normal version)
endef

define KernelPackage/hi351x-mmz/description
$(call KernelPackage/hi351x-mmz/Default/description)
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
	$(CP) files/mmz.ko $(PKG_BUILD_DIR)/driver/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
endef

$(eval $(call KernelPackage,hi351x-mmz))
