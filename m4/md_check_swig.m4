AC_DEFUN([MD_CHECK_SWIG],
  [
    MD_CHECK_ARG_ENABLE(swig,
    [
      AC_PROG_SWIG($1)
      if [[ -x $SWIG ]]; then
        SWIG_BIN=$SWIG
        AC_SUBST(SWIG_BIN)
        MD_SAVED_PACKAGE=$PACKAGE
        PACKAGE=hid
        AM_PATH_PYTHON([],
          [ AM_CONDITIONAL([PYTHONWRAPPER], [true]) ],
          [ AM_CONDITIONAL([PYTHONWRAPPER], [false]) ])
        PACKAGE=$MD_SAVED_PACKAGE
      else
        AM_CONDITIONAL([PYTHONWRAPPER], [false])
      fi
    ],
    [
      AM_CONDITIONAL([PYTHONWRAPPER], [false])
    ])
  ])
