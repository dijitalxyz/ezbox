#!/bin/bash

TARGET_DIR=$1
SOURCE_DIR=$2
SOURCE_LIST=$3

usage() {
  echo "usage: ./symbol-link-source-file.sh <target file directory> <source file directory path> <source file list>"
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

LINE1=
LINE2=

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
    if [ "x$LINE1" = "x" ] ; then
      LINE1=$LINE
    elif [ "x$LINE2" = "x" ] ; then
      LINE2=$LINE
      dbg "mkdir -p $TARGET_DIR/$LINE2"
      mkdir -p $TARGET_DIR/$LINE2
      dbg "rm -rf $TARGET_DIR/$LINE2"
      rm -rf $TARGET_DIR/$LINE2
      dbg "ln -sf $SOURCE_DIR/$LINE1 $TARGET_DIR/$LINE2"
      ln -sf $SOURCE_DIR/$LINE1 $TARGET_DIR/$LINE2
      LINE1=
      LINE2=
    fi
  fi
done < $SOURCE_LIST

