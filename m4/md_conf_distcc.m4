AC_DEFUN([MD_CONF_DISTCC],
  AC_BEFORE([MD_CONF_DISTCC], [MD_CONF_CCACHE])
  [
    MD_CHECK_ARG_COMPILER_HELPER(distcc, distcc,
      [ 
        MAKEFLAGS="-j -l1"
        AC_SUBST(MAKEFLAGS)
      ])
  ])
