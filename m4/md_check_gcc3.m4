AC_DEFUN([MD_CHECK_GCC3],
  [
    if [[[ "$GCC" = "yes" ]]]; then

      ver=$(gcc --version | head -1 | cut -d' ' -f3)
      if [[[ "$ver" = "2.96" ]]]; then
        AC_MSG_ERROR([WHAT? Dare you use gcc 2.96! Please get a proper compiler and bash RedHat at your earliest convenience.])
      elif ! echo $ver | grep -q '^3'; then
        AC_MSG_ERROR([Please upgrade your compiler to gcc 3.x.])
      fi

    fi
  ])
