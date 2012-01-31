#!/bin/sh -e

autoreconf --install --symlink --force

EZCFG_CFLAGS="-g -Wall \
-Wmissing-declarations -Wmissing-prototypes \
-Wnested-externs -Wpointer-arith \
-Wpointer-arith -Wsign-compare -Wchar-subscripts \
-Wstrict-prototypes -Wshadow \
-Wformat=2 -Wtype-limits"

case "$CFLAGS" in
	*-O[0-9]*)
		;;
	*)
		EZCFG_CFLAGS="$EZCFG_CFLAGS -O2"
		;;
esac

libdir() {
	echo $(cd $1/$(gcc -print-multi-os-directory); pwd)
}

#args="--prefix=/usr/local \
#--sysconfdir=/etc \
#--sbindir=/sbin \
#--libdir=$(libdir /lib) \
#--libexecdir=/lib/ezcfg \
#--includedir=/usr/include"

args="--prefix=/usr/local \
--sysconfdir=/etc \
--enable-service-ezcfg_httpd \
--enable-service-ezcfg_httpd_cgi_index \
--enable-service-ezcfg_httpd_cgi_admin \
--enable-service-ezcfg_httpd_cgi_nvram \
--enable-service-ezcfg_httpd_ssi \
--enable-service-ezcfg_upnpd \
--enable-service-ezcfg_upnpd_igd1 \
--enable-service-iptables \
"

export CFLAGS="$CFLAGS $EZCFG_CFLAGS"
./configure $args $@
