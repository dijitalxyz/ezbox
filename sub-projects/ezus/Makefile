#
# Makefile for ezus
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
VENDOR ?= edsim51
BOARD ?= edsim51
TARGET ?= 8051
ARCH ?= mcs51
export ARCH

CC ?= gcc
export CC

# log settings
LOG_FILE ?= $(BASE_DIR)/$(shell date --iso=seconds)-$(VENDOR)-$(BOARD)-$(TARGET)-$(ARCH)-build.log
LOG_LEVEL ?= 0

# basic directories
CUR_DIR:=${CURDIR}
TOP_DIR:=$(CUR_DIR)
CORE_DIR:=$(TOP_DIR)/core
WK_DIR:=$(CUR_DIR)/build.$(BOARD)/$(VENDOR)/$(BOARD)
export TOP_DIR CORE_DIR WK_DIR

all: $(BOARD)

clean: $(BOARD)-clean

build-info:
	echo "VENDOR="$(VENDOR)
	echo "BOARD="$(BOARD)
	echo "TARGET="$(TARGET)
	echo "ARCH="$(ARCH)

$(BOARD): build-info
	mkdir -p $(WK_DIR)
	cp -af board/$(VENDOR)/$(BOARD)/* $(WK_DIR)/
	cd $(WK_DIR) && make 2>&1 | tee $(LOG_FILE)

$(BOARD)-clean:
	cd $(WK_DIR) && make clean

.PHONY: $(BOARD) $(BOARD)-clean
.PHONY: dummy build-info
.PHONY: all clean
