#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.58)

AH_TOP(
#undef LINUX_API
#undef BSD_API
#undef DARWIN_API

#undef LIBHID_VERSION
)

# Making releases:
#   Version string: major.minor.micro.iface_age.bin_age
#
#   on release: [major/minor/micro]++, iface_age++, bin_age++
#     if functions were added, iface_age = 0
#     if not backward compatible, iface_age = bin_age = 0
#
AC_INIT(libhid, 0.1.10.0.0, libhid@lists.ailab.ch)
AC_REVISION($Revision: 1.9 $)

AM_INIT_AUTOMAKE

LIBHID_MAJOR_VERSION=`echo "AC_PACKAGE_VERSION" | cut -d. -f1`
LIBHID_MINOR_VERSION=`echo "AC_PACKAGE_VERSION" | cut -d. -f2`
LIBHID_MICRO_VERSION=`echo "AC_PACKAGE_VERSION" | cut -d. -f3`
LIBHID_INTERFACE_AGE=`echo "AC_PACKAGE_VERSION" | cut -d. -f4`
LIBHID_BINARY_AGE=`echo "AC_PACKAGE_VERSION" | cut -d. -f5`
LIBHID_VERSION=$LIBHID_MAJOR_VERSION.$LIBHID_MINOR_VERSION.$LIBHID_MICRO_VERSION

AC_DEFINE_UNQUOTED(LIBHID_VERSION, $LIBHID_VERSION)

AC_SUBST(LIBHID_MAJOR_VERSION)
AC_SUBST(LIBHID_MINOR_VERSION)
AC_SUBST(LIBHID_MICRO_VERSION)
AC_SUBST(LIBHID_INTERFACE_AGE)
AC_SUBST(LIBHID_BINARY_AGE)
AC_SUBST(LIBHID_VERSION)

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
PACKAGE=AC_PACKAGE_NAME

AM_MAINTAINER_MODE

AC_CONFIG_HEADER([config.h])
AC_PROG_LIBTOOL

debug_default=yes
AC_ARG_ENABLE(debug, AC_HELP_STRING([--enable-debug=yes/no],
      [turn on debugging (default=$debug_default)]),
      ,
      enable_debug=$debug_default
)

if test "x$enable_debug" = "xyes"; then
  CFLAGS=`echo $CFLAGS | sed -e 's,-g[[^ ]]\+,-g,g'`
  CFLAGS=`echo $CFLAGS | sed -e 's,-O[[0-9]]\+,-O0,g'`
fi

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
    AC_MSG_RESULT(unknown)
    AC_MSG_ERROR(libhid is not supported on $host)
esac

AM_CONDITIONAL(LINUX_API, test "$os_support" = "linux")
AM_CONDITIONAL(BSD_API, test "$os_support" = "bsd")
AM_CONDITIONAL(DARWIN_API, test "$os_support" = "darwin")

AC_SUBST(OSLIBS)

AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AC_HEADER_STDC

AC_HEADER_STDBOOL
AC_C_CONST

AC_CHECK_FUNCS([strncmp])

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

AC_ARG_WITH(usb-prefix, AC_HELP_STRING([--with-usb-prefix=PATH],
      [specify path to libusb installation ]),
  [
    if test -d "$withval/lib"; then
      LIBUSBLIB="-L${withval}/lib"
    else
      LIBUSBLIB="-L${withval}"
    fi

    if test -d "$withval/include"; then
      LIBUSBINCLUDE="-I${withval}/include"
    elif test -d "$withval/../include"; then
      LIBUSBINCLUDE="-I${withval}/../include"
    else
      LIBUSBINCLUDE="-I${withval}"
    fi
  ]
)

AC_SUBST(LIBUSBLIB)
AC_SUBST(LIBUSBINCLUDE)

saved_CPPFLAGS=$CPPFLAGS
saved_LDFLAGS=$LDFLAGS

AC_CHECK_HEADERS([usb.h])
CPPFLAGS="$LIBUSBINCLUDE $CPPFLAGS"
LDFLAGS="$LIBUSBLIB $LDFLAGS -lusb"

test "$os_support" = "linux" && {
  AC_MSG_CHECKING(for libusb version)
  AC_LINK_IFELSE(
    [
      #include <usb.h>

      int main(void)
      {
        usb_dev_handle* dev;
        usb_detach_kernel_driver_np(dev, 0);
        return 0;
      }
    ],
    AC_MSG_RESULT(ok (>= 0.1.8beta)),
    AC_MSG_ERROR(libhid requires libusb version 0.1.8beta or greater)
  )
}

CPPFLAGS=$saved_CPPFLAGS
LDFLAGS=$saved_LDFLAGS

AC_CHECK_HEADERS([hidparser.h])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([hidparser/Makefile])
AC_CONFIG_FILES([src/config.c])
AC_CONFIG_FILES([test/Makefile])

AC_OUTPUT
