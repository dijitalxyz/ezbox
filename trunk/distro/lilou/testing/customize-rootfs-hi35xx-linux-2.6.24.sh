#!/bin/bash

ROOTFS_DIR=$1

usage() {
  echo "usage: ./customize-rootfs.sh <rootfs directory>"
}

dbg() {
  echo $1
}

if [ "x$ROOTFS_DIR" = "x" ] ; then
  usage
  exit -1
fi

if [ ! -d $ROOTFS_DIR ] ; then
  echo "$ROOTFS_DIR does not exist!"
  exit -1
fi

# libintl symbol link
rm -rf $ROOTFS_DIR/lib/libintl.so.8
ln -sf ../usr/lib/libintl.so.8 $ROOTFS_DIR/lib/libintl.so.8

# clean /etc
rm -rf $ROOTFS_DIR/etc
mkdir -p $ROOTFS_DIR/etc


