#!/bin/bash

SRC_DIR=$1
DST_DIR=$2

DIFF_DIR="diff"
ITEM_LIST="itemlist.txt"

usage() {
  echo "usage: ./gen_dir.sh <source dir> <dest dir>"
}

if [ "x$SRC_DIR" = "x" ] ; then
  usage
  exit 0
fi

if [ "x$DST_DIR" = "x" ] ; then
  usage
  exit 0
fi

while read LINE
do
  echo $LINE
  FIRST_CHAR=${LINE:0:1}
  if [ $FIRST_CHAR = "#" ] ; then
    echo "it's a comment"
  else
    DIFF_FILE=${LINE//\//-}
    diff -urNd $SRC_DIR/$LINE $DST_DIR/$LINE > $DIFF_DIR/$DIFF_FILE.diff
  fi
done < $ITEM_LIST

