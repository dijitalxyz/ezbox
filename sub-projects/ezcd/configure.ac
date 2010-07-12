#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.62])
AC_INIT([ezcfg], [112], [ezbox@gmail.com])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_DISABLE_STATIC
AM_INIT_AUTOMAKE(ezcfg, 0.1)
AC_CONFIG_MACRO_DIR([m4])
LT_INIT
AC_PREFIX_DEFAULT([/usr])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h netinet/in.h stddef.h stdlib.h string.h sys/mount.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT64_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_HEADER_MAJOR
AC_FUNC_MALLOC
AC_CHECK_FUNCS([dup2 memset mkdir select socket strchr strerror strrchr])

AC_CONFIG_HEADERS(config.h)
AC_CONFIG_FILES([
	Makefile
	libezcfg/libezcfg.pc
])
AC_OUTPUT
AC_MSG_RESULT([
	ezcfg $VERSION
	========

	prefix:			${prefix}
	sysconfdir:		${sysconfdir}
	sbindir:		${sbinfir}
	libdir:			${libdir}
	rootlibdir:		${rootlib_execdir}
	libexecdir:		${libexecdir}

	datarootdir:		${datarootdir}
	mandir:			${mandir}
	includedir:		${includedir}

	include_prefix:		${INCLUDE_PREFIX}

	logging:		${enable_logging}
	debug:			${enable_debug}

	compiler:		${CC}
	cflags:			${CFLAGS}
	ldflags:		${LDFLAGS}
])
