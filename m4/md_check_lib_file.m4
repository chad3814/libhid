AC_DEFUN([MD_CHECK_LIB_FILE],
  [
    MD_CHECK_ARG_WITH($1, $2,
      [
        AC_CHECK_FILE([$path/$3],
          [
            AC_DEFINE([HAVE_$1], [1], [Define to 1 if you have the $1 library.])
            if test "$path" != "/usr"; then
              CPPFLAGS="$CPPFLAGS -I$path"
              LDFLAGS="$LDFLAGS -L$path"
            fi
            $4
          ],
          [
            $5
          ])
      ])
  ])
            
AC_DEFUN([MD_CHECK_LIB_FILE_WARN],
  [
    MD_CHECK_LIB_FILE($1, $2, $3, $4,
      [
        AC_MSG_WARN([$1 not found.])
        $5
      ])
  ])

AC_DEFUN([MD_CHECK_LIB_FILE_ERROR],
  [
    MD_CHECK_LIB_FILE($1, $2, $3, $4,
      [
        AC_MSG_ERROR([$1 not found.])
        $5
      ])
  ])
