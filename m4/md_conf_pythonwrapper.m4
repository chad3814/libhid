AC_DEFUN([MD_CONF_PYTHONWRAPPER],
  [
    if test ! -z "$2"; then
      SAVE_PACKAGE=$PACKAGE
      PACKAGE=$2
    fi
    AM_PATH_PYTHON($1,
      [
        AM_CONDITIONAL([PYTHONWRAPPER], [true])
      ],
      [
        AM_CONDITIONAL([PYTHONWRAPPER], [false])
      ])
    test ! -z "$SAVE_PACKAGE" && PACKAGE=$SAVE_PACKAGE
  ])
