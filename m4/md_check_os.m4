AC_DEFUN([MD_CHECK_OS],
  [
    AC_CANONICAL_HOST
    AC_MSG_CHECKING(for target OS)
    case $host in
      *-linux*)
        AC_DEFINE(OS_LINUX, [], [define to 1 if building for Linux])
        AC_SUBST(OS_LINUX)
        MD_OS=linux
        AC_MSG_RESULT(Linux)
        ;;
      *-freebsd*|*-openbsd*|*-netbsd*|*-k*bsd*-gnu)
        AC_DEFINE(OS_BSD, [], [define to 1 if building for BSD])
        AC_SUBST(OS_BSD)
        MD_OS=bsd
        AC_MSG_RESULT(*BSD)
        ;;
      *-darwin*)
        AC_DEFINE(OS_DARWIN, [], [define to 1 if building for OS X (Darwin)])
        AC_SUBST(OS_DARWIN)
        MD_OS=darwin
        OS_CFLAGS=-no-cpp-precomp
        OS_LDFLAGS="-lIOKit -framework \"CoreFoundation\""
        AC_MSG_RESULT(Darwin and/or MacOS 10)
        ;;
      *)
        AC_MSG_RESULT(unknown)
        AC_MSG_ERROR(libhid is not supported on $host)
    esac

    AC_SUBST(OS_CFLAGS)
    AC_SUBST(OS_LDFLAGS)
  ])
