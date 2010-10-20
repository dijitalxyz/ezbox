# ############################################################################
# (c) Copyright Conexant 2006
#
# Conexant Limited Confidential and Proprietary
#
# The following software source code ("Software") is strictly confidential and
# is proprietary to Conexant Limited ("Conexant").  It may only be read, used,
# copied, adapted, modified or otherwise dealt with by you if you have
# entered into a confidentiality agreement with Conexant and then subject to the
# terms of that confidentiality agreement and any other applicable agreement
# between you and Conexant.  If you are in any doubt as to whether you are
# entitled to access, read, use, copy, adapt, modify or otherwise deal with
# the Software or whether you are entitled to disclose the Software to any
# other person you should contact Conexant.  If you have not entered into a
# confidentiality agreement with Conexant granting access to this Software you
# should forthwith return all media, copies and printed listings containing
# the Software to Conexant.
#
# Conexant reserves the right to take legal action against you should you breach
# the above provisions.
#
# If you are unsure, or to report violations, please contact
# support@conexant.com
# ############################################################################
# Makefile for compiling Linux Kernel, Buildroot, client application and 
# conexant drivers

# Environment Variables 
#     ATMOS_DIR: atmos directory and  default is current_directory/atmos
#     LINUX_DIR  linux kernel directory and default is current_directory/
#                linux_kernel/2.6/linux
#     BUILDROOT_DIR buildroot directory and default is current_directory/buildroot
#     BUILDROOT package does not contain complete source code, only compiler and config file.
#     APPLICATION_DIR application directory and no default
#Arguments are used for mkproduct 
#   


#########Configuration file
include ./Rules.mk
include ./env.mk
#Arguments to be passed
ARGS_KRNL1=arm
ARGS_KRNL2=arm-linux- Image modules
KRNL_DOT_CONFIG=.config
BOARD_TYPE=solosW
GEN_WEB_BOOT_IMAGE=0

#Directory variables
CURR_DIR =$(PROJECTSRCDIR)
LINUX_DIR_DEF=$(HOST_INCLUDE_DIR)/..
ATMOS_DIR_DEF=$(CURR_DIR)/cnxt-src
BUILDROOT_DIR_DEF=$(CURR_DIR)/buildroot

BUILDROOT_DIR?=$(BUILDROOT_DIR_DEF)
ATMOS_DIR?=$(ATMOS_DIR_DEF)
LINUX_DIR?=$(LINUX_DIR_DEF)
DRIVER_DIR?=$(ATMOS_DIR)/drivers
BOOT_DIR=../Boot
IMAGE_DIR=$(CURR_DIR)/image

#Compilation flags for Open source Application
CONFIG_FLASH_UTIL=y
CONFIG_ETHTOOL=y
CONFIG_AUTOMOUNT=y
CONFIG_UVCCAPTURE=n
CONFIG_BFTPD=y
CONFIG_VSFTPD=n
CONFIG_DELTAPP=n
CONFIG_CUPS=n
CONFIG_TURBOSTATE=n
CONFIG_DRVCTL=n
CONFIG_BR2684CTL=y
CONFIG_DSLAPP=n
CONFIG_ATMCTL=n
CONFIG_DUALIMG=n
CONFIG_HTTPBOOT=n
CONFIG_IPOA=n
CONFIG_SCANPVC=n
#End
export CONFIG_DUALIMG

export PATH := $(PATH):$(CURR_DIR)/virata/tools_v10.1c/redhat-3-x86/bin/


#Flags to implement Conexant Features
ifeq ($(CONFIG_DELTAPP),y)
OPTION_DELT=-W -Sdelt_ioctl_support
else
OPTION_DELT=
endif

ifeq ($(CONFIG_DSLAPP),y)
OPTION_DELT=-W -Sdelt_ioctl_support
else
OPTION_DELT=
endif

ifeq ($(CONFIG_DUALIMG),y)
OPTION_DUALIMG=-W -Sdual_image_support
else
OPTION_DUALIMG=
endif

ifeq ($(CONFIG_HTTPBOOT),y)
OPTION_HTTPBOOT=-W -Soption_web_boot
else
OPTION_HTTPBOOT=
endif

#End

#Single OPTION variable to collect all  Conexant Feature Flags
OPTION=$(OPTION_DUALIMG) $(OPTION_DELT) $(OPTION_HTTPBOOT)

ifndef FLASH_BOOT
BOOT_OPTION=-n
endif

ifeq ("$(BOARD_TYPE)","solosW")
PRODUCT=linux-gateway
BOARD=so4610-rd
ifeq ("$(ANNEX)","A")
ARGS_MKP?= $(BOOT_OPTION) $(OPTION) -W -Sbsp_fast_boot -W -Sgallus_adsl_2_a $(PRODUCT) $(BOARD)
endif
ifeq ("$(ANNEX)","B")
ARGS_MKP?= $(BOOT_OPTION) $(OPTION) -W -Sbsp_fast_boot -W -Sgallus_adsl_2_b $(PRODUCT) $(BOARD)
endif
ARGS_MKP_BIN?= -W -N -n linux-binary-package so4610-rd
WIRELESS_DIR=$(ATMOS_DIR)/drivers/wireless/stung
KRNL_DEF_CONFIG?=so4610_defconfig
KRNL_SYS_TYPE="CONFIG_ARCH_SOLOS_208PIN"
else
ifeq ("$(BOARD_TYPE)","solosWV")
PRODUCT=linux-voip-gateway
BOARD=so4615-rd
ARGS_MKP?= $(BOOT_OPTION) -W -Sgallus_adsl_2_a $(PRODUCT) $(BOARD)
ARGS_MKP_BIN?= -W -N -n linux-binary-package so4615-rd
WIRELESS_DIR=$(ATMOS_DIR)/drivers/wireless/stung
KRNL_DEF_CONFIG?=so4615_defconfig
KRNL_SYS_TYPE="CONFIG_ARCH_SOLOS_324PIN"
else
ifeq ("$(BOARD_TYPE)","solosWVV")
PRODUCT=linux-voip-gateway
BOARD=so4615-vidmot-rd
ARGS_MKP?= $(BOOT_OPTION) -W -Sgallus_adsl_2_a -W -Soption_voice_t38_fax $(PRODUCT) $(BOARD)
#ARGS_MKP?= $(BOOT_OPTION) -W -Sgallus_adsl_2_a  $(PRODUCT) $(BOARD)
ARGS_MKP_BIN?= -W -N -n linux-binary-package so4615-rd
WIRELESS_DIR=$(ATMOS_DIR)/drivers/wireless/stung
KRNL_DEF_CONFIG?=so4615_defconfig
KRNL_SYS_TYPE="CONFIG_ARCH_SOLOS_324PIN"
else
ifeq ("$(BOARD_TYPE)","solos")
ARGS_MKP?= $(BOOT_OPTION)  -W -Sgallus_adsl_2_a linux-gateway so4515-rd
WIRELESS_DIR=$(ATMOS_DIR)/drivers/wireless/cohiba
KRNL_DEF_CONFIG?=so4515_defconfig
KRNL_SYS_TYPE="CONFIG_ARCH_SOLOS_GALLUS"
else
$(error  "Unknown Board type: $(BOARD_TYPE)")
endif
endif
endif
endif

export DRIVER_DIR BOARD_TYPE   

BUILD_DIR=$(ATMOS_DIR_DEF)/build/products/$(BOARD)-$(PRODUCT)
COPY_LIB_DIR=$(ATMOS_DIR_DEF)/build/$(BOARD)-$(PRODUCT)

ifeq ("$(SUB_VER)","00")
FW_SUB_VERSION?=
else
FW_SUB_VERSION?= \($(SUB_VER)\)
endif

#Rules defination
#dirs used for cleaning the source code from stale .o files
dirs := $(UI_DIR) $(APPS_DIR) $(LINUX_DIR) $(WIRELESS_DIR)

#default Makefile build rule 
all: rootfs kernel mkp apps appsi fs bin
premake: compiler tools attach_chksum
kernel: check_krnl_config krnl_build 
mkp: wlan
#clean_dependencies wlan os_app mkproduct  web_boot
	@if [ ! -d $(CURR_DIR)/build] ;then mkdir -p $(CURR_DIR)/build; fi
	cp $(CURR_DIR)/open-src/OS/linux-kernel/2.6/linux/arch/arm/boot/Image ./build
	bin2aout -e 0x28000 -c 0x0b -t 0x05010015 -b 434792 $(CURR_DIR)/build/Image $(CURR_DIR)/build/image
	cp $(CURR_DIR)/build/image $(CURR_DIR)/build/image.aout
	echo "Calling LZMA compression script: "
#	perl config/software/build/scripts/lzma-compress.pl -o isos-objcopy -s build/so4610-rd-lzma_boot/image.elf build/so4610-rd-linux-gateway/image build/so4610-rd-linux-gateway/image.comp
	perl ./scripts/lzma-compress.pl -o isos-objcopy -s ./build/image.elf ./build/image ./build/image.comp
	mkhfimage  -p 0 -i $(CURR_DIR)/build/image -e $(CURR_DIR)/build/image $(CURR_DIR)/build/image.comp  $(CURR_DIR)/build/image.val
	perl $(CURR_DIR)/scripts/mkhfimage-fixup.pl $(CURR_DIR)/build/image.val
	echo "file $(CURR_DIR)/build/image.val image" > $(CURR_DIR)/build/flashfs_update.cfg
	mkhfflashfs -ffs 638 -off 130 -g 256 -o $(CURR_DIR)/build/kernel.bin -i $(CURR_DIR)/build/flashfs_update.cfg
	$(CURR_DIR)/scripts/mkhfconfig -fill 0xff -o $(CURR_DIR)/build/padForImage.bin 2K
	cat $(CURR_DIR)/build/padForImage.bin $(CURR_DIR)/build/kernel.bin > $(CURR_DIR)/build/flashfs_update.bin
	rm -f $(CURR_DIR)/build/kernel.bin
	




.PHONY:compiler
compiler:
#	tar -zvxf compiler.tgz
	cd open-src/applications && find ./ -type f -name "*.tgz" -exec tar zxf {} \;
	cd $(APPS_DIR) && find ./ -type f -name "*.tgz" -exec tar zxf {} \;

.PHONY:tools
tools:
#	cd tools/cnxt && tar -zvxf mkfs.jffs2.tgz

.PHONY:attach_chksum
attach_chksum:
	cd image/ && gcc -Wall -o attach_chksum attach_chksum.o md5c.c && rm -f md5c.o

.PHONY:rootfs
rootfs:
	@echo "+++++++++++++++PATH=$(PATH)"
	cd open-src/rd_templates/ && rm -rf standalone && tar -zvxf standalone.tgz

.PHONY:bin
bin:
	cp -f open-src/rd_templates/fs.sqsh $(IMAGE_DIR)
#	cp -f open-src/rd_templates/fs.cramfs $(IMAGE_DIR)
	cp -f build/flashfs_update.bin $(IMAGE_DIR)

ifeq ("$(ANNEX)","A")
	cd $(IMAGE_DIR) && cp -f wag54g2.pid.a wag54g2.pid && cp -f fs.sqsh squashfs.bin && ./bb && mv wag54g2.bin wag54g2_$(ANNEX)_$(FW_VER)$(FW_SUB_VERSION).bin && cp wag54g2_$(ANNEX)_$(FW_VER)$(FW_SUB_VERSION).bin ../Results/
endif
ifeq ("$(ANNEX)","B")
	cd $(IMAGE_DIR) && cp -f wag54g2.pid.b wag54g2.pid && cp -f fs.sqsh squashfs.bin && ./bb && mv wag54g2.bin wag54g2_$(ANNEX)_$(FW_VER)$(FW_SUB_VERSION).bin && cp wag54g2_$(ANNEX)_$(FW_VER)$(FW_SUB_VERSION).bin ../Results/
endif


.PHONY: fs
fs:
	cp -f build_arm_nofpu/staging_dir/lib/libutil.so.0 open-src/rd_templates/standalone/lib/
#	./delete_file.sh
#	$(GCC_CROSS_COMPILER)/bin/arm-linux-uclibc-strip --strip-debug --strip-unneeded $(QUANTUM_BIN_DIR)/*
	cd $(QUANTUM_BIN_DIR)/../../ && ./mksquashfs standalone fs.sqsh -le -root-owned -noappend -D ./device_table.standalone

root:
	@echo "-------------------------------"
	@echo "Make root file system"
	@echo "-------------------------------"
	cd ./open-src/rd_templates && rm -rf standalone && tar -zvxf standalone.tgz


check_krnl_config:
	@echo "-------------------------------"
	@echo "Checking kernel confiuration"
	@echo "-------------------------------"
	@if [ "$(LINUX_DIR)" ]; then if [ -d "$(LINUX_DIR)" ]; then cd $(LINUX_DIR); if [ -f $(KRNL_DOT_CONFIG) ]; then echo "Checking .config for $(BOARD_TYPE)" ; grep "$(KRNL_SYS_TYPE)=y" $(KRNL_DOT_CONFIG); if [ $$? -eq 0 ]; then echo "Correct $(KRNL_DOT_CONFIG) file found for $(BOARD_TYPE)"; else echo "$(KRNL_DOT_CONFIG) file is not valid for $(BOARD_TYPE). Removing $(LINUX_DIR)/$(KRNL_DOT_CONFIG)"; rm $(KRNL_DOT_CONFIG); echo "Making the appropriate .config"; make ARCH=arm $(KRNL_DEF_CONFIG) ; fi; else echo "No .config file found, using $(KRNL_DEF_CONFIG)"; make ARCH=arm $(KRNL_DEF_CONFIG) ;fi; else echo "$(LINUX_DIR) does not exist... Aborting compilation.." ; exit 1; fi; else echo "Linux Directory Path is not set..Aborting Compilation" ; exit 1;fi

	@echo "----------------------------------------"

#linux kernel build rule
krnl_build:
	@echo "---------------------------------------"
	@echo "Kernel Module compilation started..."
	@echo "---------------------------------------"
	@echo "$(LINUX_DIR)"
	@if [  "$(LINUX_DIR)" ] ;then  echo "The Linux Directory Path is set..." ; if [ -d $(LINUX_DIR) ] ;then echo "$(LINUX_DIR) exists...";cd $(LINUX_DIR); if [ $$? -eq 0  ] ;then echo "Directory change to $(LINUX_DIR) successful!!....Kernel Compilation begins...";make ARCH=$(ARGS_KRNL1) CROSS_COMPILE=$(ARGS_KRNL2);if [ $$? -eq 0 ] ;then echo "Kernel compiled successfully !!! " ;else echo "Kernel Compilation failed !!! "; exit 1;fi;else echo "Could not change directory to $(LINUX_DIR)..Aborting..."; exit 1;fi;  else echo "$(LINUX_DIR) does not exist...Aborting Compilation..."; exit 1 ;fi;else echo "Linux Directory Path is not set..Aborting Compilation" ; exit 1;fi
	@echo "----------------------------------------"


#buildroot build rule
bldrt:	
	@echo "----------------------------------------"
	@echo "BuildRoot Module Started..."
	@echo "present working directory : $(CURR_DIR)"
	@if [ ! "$(BUILDROOT_DIR)" ];then echo "BuildRoot Directory path is not set..."; exit 1;else echo "BuildRoot Directory path is set..."; if [ -d $(BUILDROOT_DIR) ] ;then echo "$(BUILDROOT_DIR) exists ...Starting execution " ;$(MAKE) -C $(BUILDROOT_DIR);if [ $$? -eq 0 ] ;then echo "makefile in buildroot executed successfully";else echo "Buildroot makefile failed" ;fi;else echo "$(BUILDROOT_DIR) does not exist"; exit 1;fi;fi;


	@echo "---------------------------------------"

#User application build rule
.PHONY: apps
apps:
	@echo "----------------------------------------"
	@echo "Compilation of client applications..."
	$(MAKE) -C $(APPS_DIR)
	@echo "----------------------------------------"

.PHONY: appsi
appsi:
	@echo "----------------------------------------"
	@echo "Install apps"
	cd $(APPS_DIR) && make install
	@echo "----------------------------------------"
	find $(QUANTUM_BIN_DIR)/../ -name CVS | xargs rm -rf
	find $(QUANTUM_BIN_DIR)/../ -name Makefile | xargs rm -rf
	find $(QUANTUM_BIN_DIR)/../ -name _desktop.ini | xargs rm -rf

.PHONY: appsc
appsc:
	@echo "----------------------------------------"
	@echo "Clean apps"
	cd $(APPS_DIR) && make clean
	@echo "----------------------------------------"

.PHONY: ui
ui:
	@echo "----------------------------------------"
	@echo "Compilation of  UI..."
	$(MAKE) -C UI
	@echo "----------------------------------------"

.PHONY: uic
uic :
	@echo "----------------------------------------"
	@echo "make clean  UI..."
	$(MAKE) -C UI clean
	@echo "----------------------------------------"

os_app:flash_util ethtool automount cups turbostate uvccapture bftpd vsftpd drvctl deltapp br2684ctl dslapp atmctl ipoa scanpvc
	@echo "----------------------------------------"
	@echo "Compilation of open source applications..."
	@echo "----------------------------------------"
scanpvc:
ifeq ($(CONFIG_SCANPVC),y)
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/scanpvc all
else
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/scanpvc clean
endif
atmctl:
ifeq ($(CONFIG_ATMCTL),y)
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/atmctl all 
else
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/atmctl clean
endif
dslapp:
ifeq ($(CONFIG_DSLAPP),y)
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/dslconf/lib all install
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/dslconf/app all install
else
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/dslconf/lib clean
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/dslconf/app clean

endif


ethtool:
ifeq ($(CONFIG_ETHTOOL),y)
	@grep -q "CONFIG_ARM_SOLOS_ETHER=" $(HOST_INCLUDE_DIR)/../.config ; if [ $$? -eq 0 ] ; then $(MAKE) -C $(CURR_DIR)/open-src/applications/ethtool-5 all; fi
else
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/ethtool-5 clean
endif

flash_util:
ifeq ($(CONFIG_FLASH_UTIL),y)
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/flash_util all install
else
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/flash_util clean
endif

drvctl:
ifeq ($(CONFIG_DRVCTL),y)
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/drvctl all install
else
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/drvctl clean
endif
automount:
ifeq ($(CONFIG_AUTOMOUNT),y)
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/automount all install
else
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/automount clean
endif

uvccapture:
ifeq ($(CONFIG_UVCCAPTURE),y)
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/uvccapture all install
else
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/uvccapture clean
endif

cups:
ifeq ($(CONFIG_CUPS),y)
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/cups 
	@$(MAKE) cupsinstall -C $(CURR_DIR)/open-src/applications/cups 
  else
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/cups clean cupsuninstall

endif

bftpd:
ifeq ($(CONFIG_BFTPD),y)
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/bftpd/bftpd-1.6.6 all install
else
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/bftpd/bftpd-1.6.6 clean
endif

vsftpd:
ifeq ($(CONFIG_VSFTPD),y)
	@chmod 777 $(CURR_DIR)/open-src/applications/vsftpd/vsftpd-2.0.5/vsf_findlibs.sh
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/vsftpd/vsftpd-2.0.5 all install
else
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/vsftpd/vsftpd-2.0.5 clean
endif

deltapp:
ifeq ($(CONFIG_DELTAPP),y)
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/delt_app all
else
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/delt_app clean
endif

br2684ctl:
ifeq ($(CONFIG_BR2684CTL),y)
	@if [ -e $(QUANTUM_BIN_DIR)/libatm.so.1.0.0 ] ; then cd $(QUANTUM_BIN_DIR); sln libatm.so.1.0.0 libatm.so; cd -; $(MAKE) -C $(CURR_DIR)/open-src/applications/br2684ctl-20040226 all; else echo No libatm; fi
else
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/br2684ctl-20040226 clean
endif

turbostate:
ifeq ($(CONFIG_TURBOSTATE),y)
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/turbo_op_state all install
else
	@$(MAKE) -C $(CURR_DIR)/open-src/applications/turbo_op_state clean
endif

ipoa:
ifeq ($(CONFIG_IPOA),y)
	@cp -f $(QUANTUM_BIN_DIR)/../../optional_utility/sbin/atmarp $(QUANTUM_BIN_DIR)/../sbin/
	@cp -f $(QUANTUM_BIN_DIR)/../../optional_utility/sbin/atmarpd $(QUANTUM_BIN_DIR)/../sbin/
else
	@rm -f $(QUANTUM_BIN_DIR)/../sbin/atmarp
	@rm -f $(QUANTUM_BIN_DIR)/../sbin/atmarpd

endif

TOUCH_FILE=cnxt-src/build/$(BOARD)-$(PRODUCT)/atmos_world/cnxt_fiq.o

clean_dependencies:
	@rm -f $(QUANTUM_BIN_DIR)/../bin/ethtool $(QUANTUM_BIN_DIR)/*.o $(QUANTUM_BIN_DIR)/*.ko
	@if [ -e $(TOUCH_FILE) ] ; then touch $(TOUCH_FILE) ; fi
	@rm -f $(QUANTUM_BIN_DIR)/../bin/jffs2
	@rm -f $(QUANTUM_BIN_DIR)/../bin/boot.bin
	@rm -f $(QUANTUM_BIN_DIR)/../bin/flashfs_update.bin

#mkproduct build rule
.PHONY: mkproduct
mkproduct:
	@echo "-----------------------------------------"
	@echo "Mkproduct..."
	@echo "-----------------------------------------"
	@echo $(LINUX_IMAGE_FILE)
	@echo "ATMOS_DIR=$(ATMOS_DIR)"
	@rm -f $(QUANTUM_BIN_DIR)/*.o
	@rm -f $(QUANTUM_BIN_DIR)/../isfs/gwlc_aic_w.bin
	@rm -f $(QUANTUM_BIN_DIR)/../isfs/gwlc_dic_w.bin
	@if [ -e $(TOUCH_FILE) ] ; then touch $(TOUCH_FILE) ; fi
	@grep -q "CONFIG_ARM_SOLOS_ETHER=m" $(HOST_INCLUDE_DIR)/../.config; if [ $$? -eq 0 ] ;then cp $(HOST_INCLUDE_DIR)/../drivers/net/arm/solos_ethernet.ko $(QUANTUM_BIN_DIR) ;fi
#	@if [ ! "$(ATMOS_DIR)" ] ;then echo "The Atmos Directory path is not set..."; exit 1; else echo "Atmos Directory path is set" ;if [ -d $(ATMOS_DIR) ] ;then echo "$(ATMOS_DIR) exists...Starting mkproduct..." ; echo "mkproduct options:$(ARGS_MKP)";cd $(ATMOS_DIR); grep -q "CONFIG_ARM_SOLOS_ETHER=" $(HOST_INCLUDE_DIR)/../.config ; if [ $$? -eq 0 ] ; then grep -q "CONFIG_ATM_BR2684=" $(HOST_INCLUDE_DIR)/../.config; if [ $$? -eq 0 ] ; then mkproduct $(ARGS_MKP); else mkproduct -W -SsolosW_rfc1483 $(ARGS_MKP); fi ; else grep -q "CONFIG_ATM_BR2684=" $(HOST_INCLUDE_DIR)/../.config; if [ $$? -eq 0 ] ; then mkproduct -W -SsolosW_ethernet $(ARGS_MKP); else mkproduct -W -SsolosW_ethernet -W -SsolosW_rfc1483$(ARGS_MKP); fi; fi; if [ $$? -eq 0 ] ;then echo "Mkproduct ended successfully..." ;else echo "Error during mkproduct..." ;fi ;else echo "$(ATMOS_DIR) does not exist"; exit 1;fi;fi
	@if [ ! "$(ATMOS_DIR)" ] ;then echo "The Atmos Directory path is not set..."; exit 1; else echo "Atmos Directory path is set" ;if [ -d $(ATMOS_DIR) ] ;then echo "$(ATMOS_DIR) exists...Starting mkproduct..." ; echo "mkproduct options:$(ARGS_MKP)";cd $(ATMOS_DIR); grep -q "CONFIG_ARM_SOLOS_ETHER=" $(HOST_INCLUDE_DIR)/../.config ; if [ $$? -eq 0 ] ; then mkproduct $(ARGS_MKP); else mkproduct -W -SsolosW_ethernet $(ARGS_MKP); fi; if [ $$? -eq 0 ] ;then echo "Mkproduct ended successfully..." ;else echo "Error during mkproduct..." ;fi ;else echo "$(ATMOS_DIR) does not exist"; exit 1;fi;fi
	@if [ ! -e $(QUANTUM_BIN_DIR)/hsl_mod_gpl.o ] ; then cp $(COPY_LIB_DIR)/gpl_abstraction/hsl_mod_gpl.o $(QUANTUM_BIN_DIR) ;fi
	@if [ ! -e $(QUANTUM_BIN_DIR)/cnxt_fiq.o ] ; then cp $(COPY_LIB_DIR)/atmos_world/cnxt_fiq.o $(QUANTUM_BIN_DIR) ;fi
	@if [ ! -e $(QUANTUM_BIN_DIR)/cnxt_drv.o ] ; then cp $(COPY_LIB_DIR)/cnxt_driver/cnxt_drv.o $(QUANTUM_BIN_DIR) ;fi

	@echo "-----------------------------------------"

#mkproduct build rule
binary:
	@echo "-----------------------------------------"
	@echo "Making Binary Product..."
	@echo "-----------------------------------------"
	export BOOT_DIR
	@if [ ! "$(ATMOS_DIR)" ] ;then echo "The Atmos Directory path is not set..."; exit 1; else echo "Atmos Directory path is set" ;if [ -d $(ATMOS_DIR) ] ;then  echo "$(ATMOS_DIR) exists...Starting mkproduct..." ; echo "mkproduct options:$(ARGS_MKP_BIN)";cd $(ATMOS_DIR); mkproduct $(ARGS_MKP_BIN);if [ $$? -eq 0 ] ;then echo "Mkproduct binary ended successfully..." ;else echo "Error during mkproduct..."; exit 1 ;fi ;else echo "$(ATMOS_DIR) does not exist"; exit 1 ;fi;fi
	@echo "-----------------------------------------"
wlan:
	@echo "-----------------------------------------"
	@echo "Wireless Driver Sources..."
	@echo "-----------------------------------------"
	@echo "$(WIRELESS_DIR)"
	make -C $(WIRELESS_DIR) all
	make -C $(WIRELESS_DIR) install 
	@echo "-----------------------------------------"

uboot:
	@echo "-----------------------------------------"
	@echo "Building uboot ..."
	make -C open-src/bootloader/u-boot-1.1.4 solos_config
	make -C open-src/bootloader/u-boot-1.1.4 arch=ARM CROSS_COMPILE=arm-linux- 
	@echo "-----------------------------------------"

ifeq ("$(GEN_WEB_BOOT_IMAGE)","1")
web_boot: wb_boot wb_kernel wb_jffs2
	@echo "-----------------------------------------"
	@echo "Build Web Boot ..."
	@echo "$(BUILD_DIR)"
	@mv -f  $(BUILD_DIR)/flash.bin $(BUILD_DIR)/flash_old.bin
	@mv -f $(BUILD_DIR)/jffs2.bin $(BUILD_DIR)/jffs2_old.bin
	@mv -f $(BUILD_DIR)/fw_images/fw_flash.bin $(BUILD_DIR)/fw_images/fw_flash_old.bin
	make mkproduct
	@mv -f $(BUILD_DIR)/flash.bin $(BUILD_DIR)/flash_web_boot.bin
	@mv -f $(BUILD_DIR)/fw_images/fw_flash_old.bin $(BUILD_DIR)/fw_images/fw_flash.bin 
	@mv -f $(BUILD_DIR)/jffs2_old.bin $(BUILD_DIR)/jffs2.bin
	@mv -f $(BUILD_DIR)/flash_old.bin $(BUILD_DIR)/flash.bin
	@rm -f $(QUANTUM_BIN_DIR)/../bin/jffs2
	@rm -f $(QUANTUM_BIN_DIR)/../bin/boot.bin
	@rm -f $(QUANTUM_BIN_DIR)/../bin/flashfs_update.bin
	@echo "-----------------------------------------"
else
web_boot:
endif

wb_boot:
	@echo "-----------------------------------------"
	@echo "Coping boot.bin to initrd ..."
	@cp $(BUILD_DIR)/boot.bin $(QUANTUM_BIN_DIR)/../bin
	@echo "-----------------------------------------"

wb_kernel:
	@echo "-----------------------------------------"
	@echo "Coping flash_update.bin to initrd ..."
	@cp $(BUILD_DIR)/flashfs_update.bin $(QUANTUM_BIN_DIR)/../bin
	@echo "-----------------------------------------"

wb_jffs2:
	@echo "-----------------------------------------"
	@echo "Creating jffs2 directory ..."
	@echo "Dummy JFFS2" > $(QUANTUM_BIN_DIR)/../bin/jffs2
	@echo "-----------------------------------------"

#clean rule
clean:
	@echo "-----------------------------------------"
	@echo "Cleaning up files ..."
	$(foreach dir,$(dirs),cd $(dir);make clean;)
#	rm -rf cnxt-src/build
	rm -rf open-src/rd_templates/standalone/
#	make -C $(WIRELESS_DIR) clean
#	if [ -d open-src/bootloader/u-boot-1.1.4 ] ; then make -C open-src/bootloader/u-boot-1.1.4 distclean ; fi


	@echo "Files removed.."
	@echo "-----------------------------------------"

#default installation rule
install:
	@echo "-----------------------------------------"
	@echo "Installation...."
	@echo "Unzipping Open Source Package : DO-422260-LS CX9461x (Solos-W) OpenSource Package"
	unzip DO-422260-LS*.zip
	@echo "Unzipping core package DO-422258-LS - CX9461x(Solos-W)Core Package"
	unzip DO-422258-LS*.zip
	@echo "Unzipping package: DO-422259-LS CX9461x(Solos-W)DSL Package"
	unzip DO-422259-LS*.zip 
	@echo "Installation done..";
	@echo "-----------------------------------------"
	

