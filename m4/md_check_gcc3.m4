AC_DEFUN([MD_CHECK_GCC3],
  [
    if [[[ "$GCC" = "yes" ]]]; then

      ver=$(gcc --version | head -1 | sed -e 's,.* \([[0-9.]]*\) .*,\1,')
      if [[[ "$ver" = "2.96" ]]]; then
        AC_MSG_ERROR([WHAT? dare you use gcc 2.96. Please get a proper compiler and bash RedHat at your earliest convenience.])
      elif ! echo $ver | grep -q '^3'; then
        AC_MSG_ERROR([Please upgrade your compiler to gcc 3.x.])
      fi

    fi
  ])
