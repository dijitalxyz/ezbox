#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.62])
AC_INIT([ezcfg], [0.1], [ezbox@ezidc.net])
AC_CANONICAL_SYSTEM
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_DISABLE_STATIC
AC_CONFIG_MACRO_DIR([m4])
AC_PREFIX_DEFAULT([/usr])

# Configure variables: EZBOX_DISTRO.
AC_ARG_VAR([EZBOX_DISTRO_DEFINE], ["which distro ezcfg is built for."])

# Not specified?
if test -z "$EZBOX_DISTRO"; then
	# Set to kuafu distro
	EZBOX_DISTRO_DEFINE="-DEZCFG_EZBOX_DISTRO=EZCFG_EZBOX_DISTRO_KUAFU"
fi

AC_CONFIG_SUBDIRS([libezcfg ezcd])

AC_CONFIG_FILES([
	Makefile
])
AC_OUTPUT
AC_MSG_RESULT([
	ezcfg $VERSION
	========

	distro define:	${EZBOX_DISTRO_DEFINE}
])
