AC_DEFUN([SWIG_PROG],
  [
    AC_PATH_PROG([SWIG],[swig])

    if [[[ -z $SWIG ]]]; then

      AC_MSG_WARN([cannot find 'swig' program, you may have a look at http://www.swig.org])
      SWIG='echo "error: SWIG is not installed, you may have a look at http://www.swig.org" ; false'

    elif [[[ -n $1 ]]]; then

      AC_MSG_CHECKING([for SWIG version])
      swig_version=`$SWIG -version 2>&1 | grep 'SWIG Version' | sed -e 's,.* \([[[:digit:]]]*\.[[[:digit:]]]*\.[[[:digit:]]]*\).*,\1,'`
      AC_MSG_RESULT([$swig_version])

      if [[[ -n $swig_version ]]]; then
        set -- `echo $1 | sed 's/[[^[:digit:]]]/ /g' | tr -s ' '`
        swig_required_version=$(( 1000000 * ${1:-0} + 1000 * ${2:-0} + ${3:-0} ))

        set -- `echo $swig_version | sed 's/[[^[:digit:]]]/ /g' | tr -s ' '`
        swig_available_version=$(( 1000000 * ${1:-0} + 1000 * ${2:-0} + ${3:-0} ))

        if [[[ $swig_required_version -gt $swig_available_version ]]]; then
          AC_MSG_WARN([SWIG version $1 is required, you have $swig_version])
        fi
      else
        AC_MSG_WARN([cannot determine SWIG version])
      fi
      SWIG_RUNTIME_LIBS_DIR="${SWIG%/bin*}/lib"
      AC_MSG_NOTICE([SWIG runtime library directory is '$SWIG_RUNTIME_LIBS_DIR'])

    fi
    AC_SUBST([SWIG_RUNTIME_LIBS_DIR])
  ])
