#
# ezbox project Makefile
#

# language settings
LC_ALL:=C
LANG:=C
export LC_ALL LANG

# base dir
BASE_DIR:=${CURDIR}/..

ifneq ($(wildcard $(BASE_DIR)/default.mk),)
  include $(BASE_DIR)/default.mk
endif

# build info
DISTRO ?= huangdi
TARGET ?= x86
DEVICE_TYPE ?= ezbox
ARCH ?= i386
RT_TYPE ?= none
DRAWING_BACKEND ?= none
GUI_TOOLKIT ?= none
export DEVICE_TYPE

# log settings
LOG_FILE ?= $(BASE_DIR)/$(shell date --iso=seconds)-$(DISTRO)-$(TARGET)-build.log
LOG_LEVEL ?= 0

# initialize suffix
SUFFIX:=$(TARGET)

# setting realtime suffix
ifneq ($(RT_TYPE),none)
SUFFIX:=$(SUFFIX)-$(RT_TYPE)
endif

# setting drawing backend suffix
ifneq ($(DRAWING_BACKEND),none)
SUFFIX:=$(SUFFIX)-$(DRAWING_BACKEND)
endif

# setting gui toolkit suffix
ifneq ($(GUI_TOOLKIT),none)
SUFFIX:=$(SUFFIX)-$(GUI_TOOLKIT)
endif

# basic directories
CUR_DIR:=${CURDIR}
WK_DIR:=$(CUR_DIR)/bootstrap.$(DISTRO)-$(SUFFIX)
DISTRO_DIR:=$(CUR_DIR)/distro/$(DISTRO)
DL_DIR:=$(BASE_DIR)/dl
RT_DIR:=$(CUR_DIR)/realtime/$(RT_TYPE)
SCRIPTS_DIR:=$(CUR_DIR)/scripts

all: $(DISTRO)-all

$(DISTRO)-all: $(DISTRO)-distclean $(DISTRO)

build-info:
	echo "DISTRO=$(DISTRO)"
	echo "TARGET=$(TARGET)"
	echo "DEVICE_TYPE=$(DEVICE_TYPE)"
	echo "ARCH=$(ARCH)"
	echo "RT_TYPE=$(RT_TYPE)"
	echo "DRAWING_BACKEND=$(DRAWING_BACKEND)"
	echo "GUI_TOOLKIT=$(GUI_TOOLKIT)"

prepare-basic-structure:
	mkdir -p $(WK_DIR)
	cp -af bootstrap/* $(WK_DIR)/
	rm -rf $(WK_DIR)/target/linux/$(TARGET)
	ln -s $(DISTRO_DIR)/target/linux/$(TARGET) $(WK_DIR)/target/linux/$(TARGET)
	[ ! -d $(DISTRO_DIR)/package ] || $(SCRIPTS_DIR)/symbol-link-source-dir.sh $(WK_DIR)/package $(DISTRO_DIR)/package $(DISTRO_DIR)/package/package-list.txt
	cp $(DISTRO_DIR)/feeds.conf $(WK_DIR)/feeds.conf
	cd $(WK_DIR) && ./scripts/feeds update -a
	cd $(WK_DIR) && ./scripts/feeds install -a
	sleep 3
	mkdir -p $(DL_DIR)
	rm -rf $(WK_DIR)/dl
	ln -s $(DL_DIR) $(WK_DIR)/dl

prepare-realtime-kernel: prepare-basic-structure
	[ ! -d $(RT_DIR)/target/linux/generic ] || $(SCRIPTS_DIR)/symbol-link-source-dir.sh $(WK_DIR)/target/linux/generic $(RT_DIR)/target/linux/generic $(RT_DIR)/target/linux/generic/patches-list.txt
	[ ! -d $(RT_DIR)/target/linux/$(TARGET) ] || $(SCRIPTS_DIR)/symbol-link-source-dir.sh $(WK_DIR)/target/linux/$(TARGET) $(RT_DIR)/target/linux/$(TARGET) $(RT_DIR)/target/linux/$(TARGET)/patches-list.txt
	[ ! -d $(RT_DIR)/package ] || $(SCRIPTS_DIR)/symbol-link-source-dir.sh $(WK_DIR)/package $(RT_DIR)/package $(RT_DIR)/package/package-list.txt

prepare-build: prepare-basic-structure prepare-realtime-kernel
	echo "prepare-build OK!"

clean-build:
	echo "start to clean build!!!"
	rm -rf $(WK_DIR)
	echo "clean-build is finished!"

quick-build:
	echo "start to quick build..."
	cd $(WK_DIR) && make ARCH=$(ARCH) oldconfig
	cd $(WK_DIR) && make V=$(LOG_LEVEL) 2>&1 | tee $(LOG_FILE)
	echo "quick-build is finished!"

quick-clean:
	echo "start to quick clean!!!"
	cd $(WK_DIR) && make clean
	echo "quick-clean is finished!"

$(DISTRO): build-info prepare-build
	cp distro/$(DISTRO)/configs/defconfig-$(SUFFIX) $(WK_DIR)/.config
	cd $(WK_DIR) && make ARCH=$(ARCH) oldconfig
	cd $(WK_DIR) && make V=$(LOG_LEVEL) 2>&1 | tee $(LOG_FILE)

$(DISTRO)-clean:
	cd $(WK_DIR) && make clean

$(DISTRO)-distclean:
	rm -rf $(WK_DIR)

.PHONY: all dummy
.PHONY: $(DISTRO) $(DISTRO)-all $(DISTRO)-clean $(DISTRO)-distclean
.PHONY: prepare-basic-structure prepare-realtime-kernel
.PHONY: build-info prepare-build clean-build
.PHONY: quick-build quick-clean
