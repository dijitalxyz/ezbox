#!/bin/bash

SRC_DIR=$1
DST_DIR=$2
ITEM_LIST=$3

DIFF_DIR="diff"

usage() {
  echo "usage: ./gen_dir.sh <source dir> <dest dir> <item-list file>"
}

if [ "x$SRC_DIR" = "x" ] ; then
  usage
  exit 0
fi

if [ "x$DST_DIR" = "x" ] ; then
  usage
  exit 0
fi

if [ "x$ITEM_LIST" = "x" ] ; then
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
    DIFF_FILE=${LINE//\//-}.diff
    diff -urNd $SRC_DIR/$LINE $DST_DIR/$LINE > $DIFF_DIR/$DIFF_FILE
    LINE_COUNT=$(cat $DIFF_DIR/$DIFF_FILE | wc -l)
    # remove empty diff file
    if [ $LINE_COUNT -lt 1 ] ; then
      rm -f $DIFF_DIR/$DIFF_FILE
    fi
  fi
done < $ITEM_LIST

