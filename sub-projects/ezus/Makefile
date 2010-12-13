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
BOARD ?= edsim51
TARGET ?= 8051
ARCH ?= mcs51
export ARCH

# log settings
LOG_FILE ?= $(BASE_DIR)/$(shell date --iso=seconds)-$(BOARD)-$(TARGET)-$(ARCH)-build.log
LOG_LEVEL ?= 0

# basic directories
CUR_DIR:=${CURDIR}
WK_DIR:=$(CUR_DIR)/src.$(BOARD)-$(TARGET)-$(ARCH)
LCDL_DIR:=$(BASE_DIR)/dl

all: $(BOARD)

build-info:
	echo "BOARD="$(BOARD)
	echo "TARGET="$(TARGET)
	echo "ARCH="$(ARCH)

$(BOARD): build-info
	mkdir -p $(WK_DIR)
	cp -af src/* $(WK_DIR)/
	cd $(WK_DIR) && make 2>&1 | tee $(LOG_FILE)

$(BOARD)-clean:
	cd $(WK_DIR) && make clean

.PHONY: $(BOARD) $(BOARD)-clean
.PHONY: dummy build-info
