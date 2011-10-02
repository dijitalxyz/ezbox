#!/bin/bash

POOL_DIR=$1
WORK_DIR=$2

usage() {
  echo "usage: ./customize.sh <pool directory> <work directory>"
}

dbg() {
  echo $1
}

if [ "x$POOL_DIR" = "x" ] ; then
  usage
  exit -1
fi

if [ "x$WORK_DIR" = "x" ] ; then
  usage
  exit -1
fi

do_customize() {
  [ ! -d $POOL_DIR/customize/target/linux/hi351x ] || cp -af $POOL_DIR/customize/target/linux/hi351x/* $WORK_DIR/target/linux/hi351x/
  # customize kernel build
  [ ! -f $POOL_DIR/customize/include/customize-rootfs-kernel-defaults.mk ] || cp -af $POOL_DIR/customize/include/customize-rootfs-kernel-defaults.mk $WORK_DIR/include/kernel-defaults.mk
  # mesa math patches
  ln -sf $POOL_DIR/customize/packages/Xorg/lib/mesa/mesa-7.9/patches/901-fix-math.patch $POOL_DIR/packages/Xorg/lib/mesa/mesa-7.9/patches/901-fix-math.patch
  ln -sf $POOL_DIR/customize/packages/Xorg/lib/mesa/mesa-7.9/patches/902-fix-pipe-atomic.patch $POOL_DIR/packages/Xorg/lib/mesa/mesa-7.9/patches/902-fix-pipe-atomic.patch
  # kdrive math patches
  ln -sf $POOL_DIR/customize/packages/Xorg/xorg/xserver/xorg-kdrive/xorg-kdrive-1.10.2/Makefile-hi351x $POOL_DIR/packages/Xorg/xorg/xserver/xorg-kdrive/xorg-kdrive-1.10.2/Makefile
}

do_customize
