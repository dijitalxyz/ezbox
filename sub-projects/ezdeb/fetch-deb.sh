#!/bin/bash

PACKAGE_LIST=$1

usage() {
  echo "usage: ./fetch-deb.sh <package list file>"
  echo "       for example"
  echo "       sudo ./fetch-deb.sh deb-list.txt"
}

dbg() {
  echo $1
}

if [ "x$PACKAGE_LIST" = "x" ] ; then
  usage
  exit -1
fi

do_fetch_deb() {
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
      dbg "apt-get -d -y --reinstall install $LINE"
      apt-get -d -y --reinstall install $LINE
      cp -af /var/cache/apt/archives/$LINE\_*.deb ./
    fi
  done < $PACKAGE_LIST
}

do_fetch_deb

