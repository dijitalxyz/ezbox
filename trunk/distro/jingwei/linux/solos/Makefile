#
# Copyright (C) 2010 ezbox project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=solos
BOARDNAME:=Conexant Solos
FEATURES:=squashfs jffs2 ext2
SUBTARGETS=generic
CFLAGS:=-O2 -pipe -march=armv5te -mtune=arm1026ej-s -funit-at-a-time

LINUX_VERSION:=2.6.32.14

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))
