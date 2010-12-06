#
# ezbox project Makefile
#

ifneq ($(wildcard ../default.mk),)
  include ../default.mk
endif

DISTRO ?= huangdi
TARGET ?= x86
DEVICE_TYPE ?= ezbox
ARCH ?= i386

# export DEVICE_TYPE=$(DEVICE_TYPE)
# export ARCH=$(ARCH)

$(DISTRO):
	rm -rf bootstrap.$(TARGET)
	cp -af bootstrap bootstrap.$(TARGET)
	ln -s `pwd`/distro/$(DISTRO)/target/linux/$(TARGET) bootstrap.$(TARGET)/target/linux/$(TARGET)
	cp distro/$(DISTRO)/feeds.conf bootstrap.$(TARGET)/feeds.conf
	cp distro/$(DISTRO)/configs/defconfig-$(TARGET) bootstrap.$(TARGET)/.config
	cd bootstrap.$(TARGET) && ./scripts/feeds update -a
	cd bootstrap.$(TARGET) && ./scripts/feeds install -a
	cd bootstrap.$(TARGET) && make ARCH=$(ARCH) oldconfig
	cd bootstrap.$(TARGET) && make DEVICE_TYPE=$(DEVICE_TYPE) V=99 2>&1 | tee build.log

