#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.62])
AC_INIT([ezcd], [0.1], [ezbox@ezidc.net])
AC_CANONICAL_SYSTEM
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
LT_INIT

AC_DISABLE_STATIC
AC_CONFIG_MACRO_DIR([m4])
AC_PREFIX_DEFAULT([/usr])

AC_LANG(C)

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Configure variables: LIBEZCFG_LIB and LIBEZCFG_INC.
AC_ARG_VAR([LIBEZCFG_LIB], [where libezcfg.la is at])
AC_ARG_VAR([LIBEZCFG_INC], [where libezcfg.h is at])

# Ensure that both or neither are specified.
if (test -n "$LIBEZCFG_LIB" && test -z "$LIBEZCFG_INC") || \
   (test -n "$LIBEZCFG_INC" && test -z "$LIBEZCFG_LIB"); then
	AC_MSG_ERROR([both or neither libezcfg variables])
fi

# Not specified? Check for libezcfg in standard places.
if test -z "$LIBEZCFG_LIB"; then
  # Check for libezcfg as a sub-project.
  if test -d "$srcdir/libezcfg"; then
	AC_CONFIG_SUBDIRS([libezcfg])
	LIBEZCFG_INC='$(top_srcdir)/libezcfg/include'
	LIBEZCFG_LIB='$(top_srcdir)/libezcfg/.libs'
  else
    # Check for libezcfg as a super-project.
    if test -d "$srcdir/../libezcfg"; then
	LIBEZCFG_INC='$(top_srcdir)/../libezcfg/include'
	LIBEZCFG_LIB='$(top_srcdir)/../libezcfg/.libs'
    fi
  fi
fi

# Still empty? Check for installed libezcfg.
if test -z "$LIBEZCFG_LIB"; then
	AC_CHECK_LIB([libezcfg], [ezcfg_new], 
		[AC_CHECK_HEADERS([libezcfg.h])
			LIBS="-lezcfg $LIBS"],
	[AC_MSG_ERROR([No libezcfg found.])])
fi

# AC_SUBST command line variables.
if test -n "$LIBEZCFG_LIB"; then
	AC_SUBST([LIBEZCFG_LTLIB], ["$LIBEZCFG_LIB/libezcfg.la"])
	AC_SUBST([LIBEZCFG_INCLUDE], ["-I$LIBEZCFG_INC"])
fi



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
])
AC_OUTPUT
AC_MSG_RESULT([
	ezcd $VERSION
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

	libezcfg_lib:		${LIBEZCFG_LIB:-INSTALLED}
	libezcfg_inc:		${LIBEZCFG_INC:-INSTALLED}
])
