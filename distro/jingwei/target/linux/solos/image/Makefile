# 
# Copyright (C) 2010 ezbox project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

export PATH=$(TARGET_PATH):/sbin

ROOTPART:=$(call qstrip,$(CONFIG_TARGET_ROOTFS_PARTNAME))

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/BuildKernel
	$(CP) $(KDIR)/zImage $(BIN_DIR)/$(IMG_PREFIX)-vmlinuz
endef

define Image/Prepare
	$(CP) $(LINUX_DIR)/arch/arm/boot/zImage $(KDIR)/zImage
	$(call Image/Prepare/bootscript)
endef

define Image/Build
	$(call Image/Build/$(1))
	$(call Image/Build/bootscript,$(1))
	$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-rootfs-$(1).img
	$(CP) $(KDIR)/zImage $(BIN_DIR)/$(IMG_PREFIX)-vmlinuz
  ifeq ($(1),ext2)
	gzip -f9 $(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).img
	gzip -f9 $(BIN_DIR)/$(IMG_PREFIX)-rootfs-$(1).img
  endif
endef

$(eval $(call BuildImage))
