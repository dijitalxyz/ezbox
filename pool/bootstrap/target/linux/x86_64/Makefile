#
# Copyright (C) 2012-2013 ezbox project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=x86_64
BOARD:=x86_64
BOARDNAME:=x86_64
FEATURES:=squashfs jffs2 ext4 vdi vmdk pcmcia targz
SUBTARGETS=generic

LINUX_VERSION:=3.3.8

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))

$(eval $(call $(if $(CONFIG_TARGET_ROOTFS_ISO),RequireCommand,Ignore),mkisofs, \
   	Please install mkisofs. \
))
