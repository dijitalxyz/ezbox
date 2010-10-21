#!/bin/bash

UBOOTDIR=$1
KERNELPATH=$2
IMAGEPATH=$3

MKIMAGE=${UBOOTDIR}/mkimage

ENTRY=`readelf -a $2/vmlinux|grep "Entry point"|cut -d":" -f 2`
LDADDR=`readelf -a $2/vmlinux|grep "\[ 1\]"|cut -d" " -f 26`

${MKIMAGE} -A mips -O linux -T kernel -C gzip \
	-a ${LDADDR} -e ${ENTRY} -n "Linux Kernel Image" -d ${KERNELPATH}/arch/mips/boot/vmlinux.bin.gz ${IMAGEPATH}/vmlinux.gz.uImage
