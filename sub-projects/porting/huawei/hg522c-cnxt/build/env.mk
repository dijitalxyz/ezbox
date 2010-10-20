PROJECTSRCDIR?=$(shell pwd)
export LINUX_IMAGE_FILE=$(PROJECTSRCDIR)/open-src/OS/linux-kernel/2.6/linux/arch/arm/boot/Image
export QUANTUM_BIN_DIR=$(PROJECTSRCDIR)/open-src/rd_templates/standalone/lib
export HOST_INCLUDE_DIR=$(PROJECTSRCDIR)/open-src/OS/linux-kernel/2.6/linux/include
export NETFILTER_DIR=$(PROJECTSRCDIR)/open-src/OS/linux-kernel/2.6/linux/net/ipv4/netfilter.12
export BOOT_DIR=BOOT_DIR=$(PROJECTSRCDIR)/Boot
export VIRATA_TOOLS=10
export UI_DIR=$(PROJECTSRCDIR)/UI
export APPS_DIR=$(PROJECTSRCDIR)/tk_apps
export WIRELESS_DRIVER_DIR=$(PROJECTSRCDIR)/cnxt-src/drivers/wireless
export GCC_CROSS_COMPILER=$(PROJECTSRCDIR)/build_arm_nofpu/staging_dir
export ISOS_TOOL_DIR=/usr/local/virata/tools_v10.1c/redhat-3-x86/bin
PATH=$(PROJECTSRCDIR)/tools/cnxt:/usr/local/bin:/usr/local/sbin:/sbin:/usr/bin:/bin:/usr/sbin:$(PROJECTSRCDIR)/tools/3rdparty/linux:$(PROJECTSRCDIR)/tools/cnxt:$(PROJECTSRCDIR)/build_arm_nofpu/staging_dir/bin:

export PATH

