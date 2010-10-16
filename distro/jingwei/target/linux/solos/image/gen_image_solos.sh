#!/usr/bin/env bash
# Copyright (C) 2010 ezbox project
set -x 
[ $# == 5 ] || {
    echo "SYNTAX: $0 <file> <kernel size> <kernel directory> <rootfs size> <rootfs image>"
    exit 1
}

OUTPUT="$1"
KERNELSIZE="$2"
KERNELDIR="$3"
ROOTFSSIZE="$4"
ROOTFSIMAGE="$5"

rm -f "$OUTPUT"

head=16
sect=63
cyl=$(( ($KERNELSIZE + $ROOTFSSIZE) * 1024 * 1024 / ($head * $sect * 512)))

# create partition table
set `ptgen -o "$OUTPUT" -h $head -s $sect -p ${KERNELSIZE}m -p ${ROOTFSSIZE}m`

KERNELOFFSET="$(($1 / 512))"
KERNELSIZE="$(($2 / 512))"
ROOTFSOFFSET="$(($3 / 512))"
ROOTFSSIZE="$(($4 / 512))"

BLOCKS="$((($KERNELSIZE / 2) - 1))"

[ -n "$PADDING" ] && dd if=/dev/zero of="$OUTPUT" bs=512 seek="$ROOTFSOFFSET" conv=notrunc count="$ROOTFSSIZE"
dd if="$ROOTFSIMAGE" of="$OUTPUT" bs=512 seek="$ROOTFSOFFSET" conv=notrunc

