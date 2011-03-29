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
export DEVICE_TYPE

# log settings
LOG_FILE ?= $(BASE_DIR)/$(shell date --iso=seconds)-$(DISTRO)-$(TARGET)-build.log
LOG_LEVEL ?= 0

ifeq ($(RT_TYPE),none)
SUFFIX:=$(TARGET)
else
SUFFIX:=$(TARGET)-$(RT_TYPE)
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

prepare-build:
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

clean-build:
	rm -rf $(WK_DIR)

prepare-realtime-kernel:
	[ ! -d $(RT_DIR)/target/linux/$(TARGET) ] || $(SCRIPTS_DIR)/symbol-link-source-dir.sh $(WK_DIR)/target/linux/$(TARGET) $(RT_DIR)/target/linux/$(TARGET) $(RT_DIR)/target/linux/$(TARGET)/patches-list.txt
	[ ! -d $(RT_DIR)/package ] || $(SCRIPTS_DIR)/symbol-link-source-dir.sh $(WK_DIR)/package $(RT_DIR)/package $(RT_DIR)/package/package-list.txt

quick-build:
	cd $(WK_DIR) && make ARCH=$(ARCH) oldconfig
	cd $(WK_DIR) && make V=$(LOG_LEVEL) 2>&1 | tee $(LOG_FILE)

quick-clean:
	cd $(WK_DIR) && make clean

$(DISTRO): build-info prepare-build prepare-realtime-kernel
	cp distro/$(DISTRO)/configs/defconfig-$(SUFFIX) $(WK_DIR)/.config
	cd $(WK_DIR) && make ARCH=$(ARCH) oldconfig
	cd $(WK_DIR) && make V=$(LOG_LEVEL) 2>&1 | tee $(LOG_FILE)

$(DISTRO)-clean:
	cd $(WK_DIR) && make clean

$(DISTRO)-distclean:
	rm -rf $(WK_DIR)

.PHONY: $(DISTRO) $(DISTRO)-all $(DISTRO)-clean $(DISTRO)-distclean
.PHONY: all dummy build-info prepare-build clean-build
.PHONY: quick-build quick-clean
