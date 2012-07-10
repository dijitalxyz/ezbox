# 
# Copyright (C) 2012 ezbox project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

export PATH=$(TARGET_PATH):/sbin

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build/gzip
	gzip -f9 $(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).img
	gzip -f9 $(BIN_DIR)/$(IMG_PREFIX)-rootfs-$(1).img
endef

ifneq ($(CONFIG_TARGET_IMAGES_GZIP),)
  define Image/Build/gzip/ext4
	$(call Image/Build/gzip,ext4)
  endef
  ifneq ($(CONFIG_TARGET_IMAGES_PAD),)
    define Image/Build/gzip/squashfs
	$(call Image/Build/gzip,squashfs)
    endef
    define Image/Build/gzip/jffs2-64k
	$(call Image/Build/gzip,jffs2-64k)
    endef
    define Image/Build/gzip/jffs2-128k
	$(call Image/Build/gzip,jffs2-128k)
    endef
  endif
endif

define Image/BuildKernel
	$(CP) $(KDIR)/bzImage $(BIN_DIR)/$(IMG_PREFIX)-vmlinuz
endef

define Image/Prepare
	$(CP) $(LINUX_DIR)/arch/x86/boot/bzImage $(KDIR)/bzImage
	$(call Image/Prepare/bootscript)
	$(call Image/Prepare/grub)
endef

define Image/Build
	$(call Image/Build/$(1))
	$(call Image/Build/bootscript,$(1))
	$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-rootfs-$(1).img
	$(CP) $(KDIR)/bzImage $(BIN_DIR)/$(IMG_PREFIX)-vmlinuz
	$(call Image/Build/gzip/$(1))
endef

$(eval $(call BuildImage))
