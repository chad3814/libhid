%module hid
%{
#include <hid.h>
%}

%include "exception.i"

%typemap(in) FILE* {
  if (PyFile_Check($input)) {
      $1 = PyFile_AsFile($input);
  } else {
      SWIG_exception(SWIG_TypeError, "file expected");
  }
}

%include "hid.h"
