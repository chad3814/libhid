AC_DEFUN([MD_CHECK_GCC3],
  [
    if [[[ "$GCC" = "yes" ]]]; then

      ver=$(gcc --version | sed -ne 's,^gcc .* \([[[:digit:]]]*.[[[:digit:]]]*.[[[:digit:]]]*\) .*,\1,p')
      echo $ver
      case $ver in
        2.96)
          AC_MSG_ERROR([WHAT? Dare you use gcc 2.96! Please get a proper compiler and bash RedHat at your earliest convenience.])
          ;;
        [[3-9]].*)
          :
          ;;
        *)
          AC_MSG_ERROR([Please upgrade your compiler to gcc 3.x.])
          ;;
      esac
    fi
  ])
