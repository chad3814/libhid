AC_DEFUN([MD_CONF_FLAGS],
  [
    CPPFLAGS="`echo $1 $CPPFLAGS | tr -s '[[  ]]' | sed -e 's,^[[   ]]*\\(.*\\)[[   ]]*$,\\1,'`"
    CFLAGS="`echo $2 $CFLAGS | tr -s '[[  ]]' | sed -e 's,^[[   ]]*\\(.*\\)[[   ]]*$,\\1,'`"
    CXXFLAGS="`echo $3 $CXXFLAGS | tr -s '[[  ]]' | sed -e 's,^[[   ]]*\\(.*\\)[[   ]]*$,\\1,'`"
    LDFLAGS="`echo $4 $LDFLAGS | tr -s '[[  ]]' | sed -e 's,^[[   ]]*\\(.*\\)[[   ]]*$,\\1,'`"
  ])
