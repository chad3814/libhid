AC_DEFUN([MD_CHECK_GCC3],
  [
    if [[[ "$GCC" = "yes" ]]]; then
      case $(gcc -dD -E - < /dev/null | sed -ne 's,.*__GNUC__ ,,p') in
        [[3-9]]) :;;
        *) AC_MSG_ERROR([Please upgrade your compiler to gcc 3.x.]);;
      esac
    fi
  ])
