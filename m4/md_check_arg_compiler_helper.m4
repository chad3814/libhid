AC_DEFUN([MD_CHECK_ARG_COMPILER_HELPER],
  AC_BEFORE([$0], [AC_PROG_LIBTOOL])
  [
    AC_BEFORE([MD_CHECK_ARG_COMPILER_HELPER], [AC_PROG_LIBTOOL])
    MD_CHECK_ARG_WITH($1, $2,
      [
        AC_CHECK_PROG(HAVE_$1, $path, yes)
        if test "$HAVE_$1" = "yes"; then
          AC_MSG_NOTICE([$path found. using $1.])
          CC="$path $CC"
          CXX="$path $CXX"
          $3
        else
          AC_MSG_WARN([$path not found. not using $1.])
          $4
        fi
      ])
  ])
