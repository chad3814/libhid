#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.58)

# Making releases:
#   Version string: major.minor.micro.iface_age.bin_age
#
#   on release: [major/minor/micro]++, iface_age++, bin_age++
#     if functions were added, iface_age = 0
#     if not backward compatible, iface_age = bin_age = 0
#
AC_INIT(libhid, 0.1.90.0.0, libhid@lists.ailab.ch)
AC_REVISION($Revision: 1.2 $)

AM_INIT_AUTOMAKE

#AC_DEFINE_UNQUOTED(AC_PACKAGE_VERSION, $AC_PACKAGE_VERSION)

LIBHID_MAJOR_VERSION=`echo $AC_PACKAGE_VERSION | cut -d. -f1`
LIBHID_MINOR_VERSION=`echo $AC_PACKAGE_VERSION | cut -d. -f2`
LIBHID_MICRO_VERSION=`echo $AC_PACKAGE_VERSION | cut -d. -f3`
LIBHID_INTERFACE_AGE=`echo $AC_PACKAGE_VERSION | cut -d. -f4`
LIBHID_BINARY_AGE=`echo $AC_PACKAGE_VERSION | cut -d. -f5`
LIBHID_VERSION=$LIBHID_MAJOR_VERSION.$LIBHID_MINOR_VERSION.$LIBHID_MICRO_VERSION

AC_SUBST(LIBHID_MAJOR_VERSION)
AC_SUBST(LIBHID_MINOR_VERSION)
AC_SUBST(LIBHID_MICRO_VERSION)
AC_SUBST(LIBHID_INTERFACE_AGE)
AC_SUBST(LIBHID_BINARY_AGE)
AC_SUBST(LIBHID_VERSION)

#AC_DEFINE_UNQUOTED(LIBHID_MAJOR_VERSION, $LIBHID_MAJOR_VERSION)
#AC_DEFINE_UNQUOTED(LIBHID_MINOR_VERSION, $LIBHID_MINOR_VERSION)
#AC_DEFINE_UNQUOTED(LIBHID_MICRO_VERSION, $LIBHID_MICRO_VERSION)
#AC_DEFINE_UNQUOTED(LIBHID_INTERFACE_AGE, $LIBHID_INTERFACE_AGE)
#AC_DEFINE_UNQUOTED(LIBHID_BINARY_AGE, $LIBHID_BINARY_AGE)

LIBHID_MICRO_VERSION_NUM=`echo $LIBHID_MICRO_VERSION | sed 's/[[a-zA-Z]]//g'`

LT_RELEASE=$LIBHID_MAJOR_VERSION.$LIBHID_MINOR_VERSION
LT_CURRENT=`expr $LIBHID_MICRO_VERSION_NUM - $LIBHID_INTERFACE_AGE`
LT_REVISION=$LIBHID_INTERFACE_AGE
LT_AGE=`expr $LIBHID_BINARY_AGE - $LIBHID_INTERFACE_AGE`

AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

VERSION=$LIBHID_VERSION

AC_CONFIG_HEADER([config.h])

AC_CANONICAL_HOST
AC_MSG_CHECKING(for target OS)
case $host in
  *-linux*)
    AC_DEFINE(LINUX_API, [], [define if building for Linux])
    AC_SUBST(LINUX_API)
    os_support=linux
    AC_MSG_RESULT(Linux)
    OSLIBS=""
    ;;
  *-freebsd*|*-openbsd*|*-netbsd*)
    AC_DEFINE(BSD_API, [], [define if building for BSD])
    AC_SUBST(BSD_API)
    os_support=bsd
    AC_MSG_RESULT(FreeBSD, OpenBSD and/or NetBSD)
    OSLIBS=""
    ;;
  *-darwin*)
    AC_DEFINE(DARWIN_API, [], [define if building for OS X (Darwin)])
    AC_SUBST(DARWIN_API)
    os_support=darwin
    AC_MSG_RESULT(Darwin and/or MacOS 10)
    OSLIBS="-lIOKit"
    ;;
  *)
    AC_MSG_RESULT(unknown);;
esac

AM_CONDITIONAL(LINUX_API, test "$os_support" = "linux")
AM_CONDITIONAL(BSD_API, test "$os_support" = "bsd")
AM_CONDITIONAL(DARWIN_API, test "$os_support" = "darwin")

AC_SUBST(OSLIBS)

dnl declare --enable-* args and collect ac_help strings
debug_default=yes
AC_ARG_ENABLE(debug, [  --enable-debug=[no/yes] turn on debugging [default=$debug_default]],,enable_debug=$debug_default)

if test "x$enable_debug" = "xyes"; then
  test "$cflags_set" = set || CFLAGS="$CFLAGS -gstabs+ -O0"
fi

AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

#AC_HEADER_DIRENT
AC_HEADER_STDC
#AC_CHECK_HEADERS([fcntl.h limits.h stdlib.h string.h sys/ioctl.h sys/time.h unistd.h])
#AC_CHECK_HEADERS([usb.h])

AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
#AC_TYPE_SIZE_T

#AC_PROG_GCC_TRADITIONAL
#AC_FUNC_MMAP
#AC_FUNC_MALLOC
#AC_FUNC_MEMCMP
#AC_CHECK_FUNCS([memset strchr strdup strtoul])

AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(void *)
AC_CHECK_SIZEOF(long long)

if test "x$GCC" = "xyes"; then
  if ! echo " $CFLAGS " | grep -q '[ 	]-Wall[ 	]'; then
    CFLAGS="$CFLAGS -Wall"
  fi
fi

AC_C_BIGENDIAN

if test "$ac_cv_c_bigendian" = "yes"; then
  BIGENDIAN="1"
else
  BIGENDIAN="0"
fi
AC_SUBST(BIGENDIAN)

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([test/Makefile])

AC_OUTPUT
