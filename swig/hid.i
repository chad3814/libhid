%module(docstring="libhid is a user-space USB HID access library built on libusb.") hid 
%{
#include <compiler.h>
#include <hid.h>
%}

%feature("autodoc","1");

%include "exception.i"

%typemap(in) FILE* {
  if (PyFile_Check($input)) {
      $1 = PyFile_AsFile($input);
  } else {
      SWIG_exception(SWIG_TypeError, "file expected");
  }
}

%include "cstring.i"

// hid_interrupt_write()
%apply (char *STRING, int LENGTH) { (const char* const bytes, unsigned int const size) }

// Python-specific; this should be moved to another file which includes this
// (to-be generic) hid.i file.

// Convert tuples or lists to paths (and depth)
// Ref: http://www.swig.org/Doc1.3/Python.html
%typemap(in) (int const path[], unsigned int const depth) {
  int i, size;
  int *temp;

  if (!PySequence_Check($input)) {
    PyErr_SetString(PyExc_TypeError,"Expecting a sequence");
    return NULL;
  }

  size = PySequence_Size($input);
  temp = (int *)calloc(size, sizeof(int));

  for (i =0; i < size; i++) {
    PyObject *o = PySequence_GetItem($input,i);
    if (!PyInt_Check(o)) {
      PyErr_SetString(PyExc_ValueError,"Expecting a sequence of integers");
      return NULL;
    }
    temp[i] = (int)PyInt_AsLong(o);
  }

  $1 = temp;
  $2 = size;
}
%typemap(freearg) (int const path[], unsigned int const depth) {
  free((char *) $1);
}

%include "hid.h"

/* COPYRIGHT --
 *
 * This file is part of libhid, a user-space HID access library.
 * libhid is (c) 2003-2005
 *   Martin F. Krafft <libhid@pobox.madduck.net>
 *   Charles Lepple <clepple@ghz.cc>
 *   Arnaud Quette <arnaud.quette@free.fr> && <arnaud.quette@mgeups.com>
 * and distributed under the terms of the GNU General Public License.
 * See the file ./COPYING in the source distribution for more information.
 *
 * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES
 * OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
