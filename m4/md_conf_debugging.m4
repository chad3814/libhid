AC_DEFUN([MD_CONF_DEBUGGING],
  [
    MD_CHECK_ARG_ENABLE(debug,
      [
        DEBUG_FLAG=-g
        MD_CHECK_ARG_ENABLE(gdbdebug, [DEBUG_FLAG="-g3 -ggdb"])

        CFLAGS=`echo $CFLAGS | sed -e "s,-g[[^ ]]*,$DEBUG_FLAG,g"`
        CFLAGS=`echo $CFLAGS | sed -e 's,-O[[0-9]]*,-O0,g'`
        CXXFLAGS=`echo $CXXFLAGS | sed -e "s,-g[[^ ]]*,$DEBUG_FLAG,g"`
        CXXFLAGS=`echo $CXXFLAGS | sed -e 's,-O[[0-9]]*,-O0,g'`
      ],
      [
        CPPFLAGS="$CPPFLAGS -DNDEBUG"
      ])
  ])
