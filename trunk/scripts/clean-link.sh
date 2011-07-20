#!/bin/bash

BUILD_DIR=$1
PACKAGE_LIST=$2

usage() {
  echo "usage: ./clean-link.sh <build directory> <packages list>"
}

dbg() {
  echo $1
}

if [ "x$BUILD_DIR" = "x" ] ; then
  usage
  exit -1
fi

if [ "x$PACKAGE_LIST" = "x" ] ; then
  usage
  exit -1
fi

while read LINE
do
  echo $LINE
  if [ "x$LINE" = "x" ] ; then
    echo "LINE is empty."
    continue;
  fi
  FIRST_CHAR=${LINE:0:1}
  if [ $FIRST_CHAR = "#" ] ; then
    echo "it's a comment"
  else
    SOURCE=${LINE%:*}
    TARGET=${LINE#*:}
    dbg "SOURCE=[$SOURCE]"
    dbg "TARGET=[$TARGET]"
    if [ "x$SOURCE" = "x" ] ; then
      echo "file format error @ [$LINE] !!!"
      exit -1
    fi
    if [ "x$TARGET" = "x" ] ; then
      echo "file format error @ [$LINE] !!!"
      exit -1
    fi
    dbg "rm -rf $BUILD_DIR/$TARGET"
    rm -rf $BUILD_DIR/$TARGET
  fi
done < $PACKAGE_LIST

