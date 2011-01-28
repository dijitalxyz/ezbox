#!/bin/bash

TARGET_DIR=$1
SOURCE_DIR=$2
SOURCE_LIST=$3

usage() {
  echo "usage: ./symbol-link-source.sh <target file directory> <source file directory path> <source file list>"
}

dbg() {
  echo $1
}

if [ "x$TARGET_DIR" = "x" ] ; then
  usage
  exit 0
fi

if [ "x$SOURCE_DIR" = "x" ] ; then
  usage
  exit 0
fi

if [ "x$SOURCE_LIST" = "x" ] ; then
  usage
  exit 0
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
    echo "It's a comment."
  else
    dbg "mkdir -p $TARGET_DIR/$LINE"
    mkdir -p $TARGET_DIR/$LINE
    dbg "rm -rf $TARGET_DIR/$LINE"
    rm -rf $TARGET_DIR/$LINE
    dbg "ln -sf $SOURCE_DIR/$LINE $TARGET_DIR/$LINE"
    ln -sf $SOURCE_DIR/$LINE $TARGET_DIR/$LINE
  fi
done < $SOURCE_LIST

