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
export DEVICE_TYPE

# log settings
LOG_FILE ?= $(BASE_DIR)/$(shell date --iso=seconds)-$(DISTRO)-$(TARGET)-build.log
LOG_LEVEL ?= 0

# basic directories
CUR_DIR:=${CURDIR}
WK_DIR:=$(CUR_DIR)/bootstrap.$(DISTRO)-$(TARGET)
LCDL_DIR:=$(BASE_DIR)/dl

all: $(DISTRO)

build-info:
	echo "DISTRO="$(DISTRO)
	echo "TARGET="$(TARGET)
	echo "DEVICE_TYPE="$(DEVICE_TYPE)
	echo "ARCH="$(ARCH)

$(DISTRO): build-info
	mkdir -p $(WK_DIR)
	cp -af bootstrap/* $(WK_DIR)/
	rm -rf $(WK_DIR)/target/linux/$(TARGET)
	ln -s $(CUR_DIR)/distro/$(DISTRO)/target/linux/$(TARGET) $(WK_DIR)/target/linux/$(TARGET)
	cp distro/$(DISTRO)/feeds.conf $(WK_DIR)/feeds.conf
	cd $(WK_DIR) && ./scripts/feeds update -a
	cd $(WK_DIR) && ./scripts/feeds install -a
	sleep 3
	mkdir -p $(LCDL_DIR)
	rm -rf $(WK_DIR)/dl
	ln -s $(LCDL_DIR) $(WK_DIR)/dl
	cp distro/$(DISTRO)/configs/defconfig-$(TARGET) $(WK_DIR)/.config
	cd $(WK_DIR) && make ARCH=$(ARCH) oldconfig
	cd $(WK_DIR) && make V=$(LOG_LEVEL) 2>&1 | tee $(LOG_FILE)

$(DISTRO)-clean:
	cd $(WK_DIR) && make clean

.PHONY: $(DISTRO) $(DISTRO)-clean
.PHONY: dummy build-info
