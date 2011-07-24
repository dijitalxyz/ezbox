#
# Copyright (C) 2008-2011 ezbox project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

BLOCK_MENU:=Block Devices


define KernelPackage/pata-sis
  TITLE:=SiS PATA support
  DEPENDS:=@PCI_SUPPORT
  KCONFIG:=CONFIG_PATA_SIS
  FILES=$(LINUX_DIR)/drivers/ata/pata_sis.ko
  AUTOLOAD:=$(call AutoLoad,30,pata-sis,1)
  $(call AddDepends/ata)
endef

define KernelPackage/pata-sis/description
  Kernel module for the SiS PATA controllers
endef

$(eval $(call KernelPackage,pata-sis))

