#
# ezbox project Makefile
#

BASE_DIR:=${CURDIR}/..

ifneq ($(wildcard $(BASE_DIR)/default.mk),)
  include $(BASE_DIR)/default.mk
endif

DISTRO ?= huangdi
TARGET ?= x86
DEVICE_TYPE ?= ezbox
ARCH ?= i386
export DEVICE_TYPE

# language settings
LC_ALL:=C
LANG:=C
export LC_ALL LANG

# log settings
LOG_FILE ?= $(BASE_DIR)/$(shell date --iso=seconds)-$(DISTRO)-$(TARGET)-build.log
LOG_LEVEL ?=

# basic directories
CUR_DIR:=${CURDIR}
WK_DIR:=$(CUR_DIR)/bootstrap.$(TARGET)
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
	ln -s $(CUR_DIR)/distro/$(DISTRO)/target/linux/$(TARGET) $(WK_DIR)/target/linux/$(TARGET)
	cp distro/$(DISTRO)/feeds.conf $(WK_DIR)/feeds.conf
	cd $(WK_DIR) && ./scripts/feeds update -a
	cd $(WK_DIR) && ./scripts/feeds install -a
	sleep 3
	mkdir -p $(LCDL_DIR)
	ln -s $(LCDL_DIR) $(WK_DIR)/dl
	cp distro/$(DISTRO)/configs/defconfig-$(TARGET) $(WK_DIR)/.config
	cd $(WK_DIR) && make ARCH=$(ARCH) oldconfig
	cd $(WK_DIR) && make DEVICE_TYPE=$(DEVICE_TYPE) $(LOG_LEVEL) 2>&1 | tee $(LOG_FILE)

$(DISTRO)-clean:
	cd $(WK_DIR) && make clean

.PHONY: $(DISTRO) $(DISTRO)-clean
.PHONY: dummy build-info
