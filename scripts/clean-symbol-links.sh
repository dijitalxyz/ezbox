#!/bin/bash

SOURCE_DIR=$1
SOURCE_LIST=$2

usage() {
  echo "usage: ./clean-symbol-links.sh <build directory> <source file list>"
}

dbg() {
  echo $1
}

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
    dbg "rm -rf $SOURCE_DIR/$LINE"
    rm -rf $SOURCE_DIR/$LINE
  fi
done < $SOURCE_LIST

