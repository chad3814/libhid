AM_MAKEFLAGS = @MAKEFLAGS@
ACLOCAL_AMFLAGS = -I m4

PYTHONINCLUDES = @PYTHON_PREFIX@/include/python@PYTHON_VERSION@

AM_CPPFLAGS = -I$(PYTHONINCLUDES) -I../include -DHID_INTERNAL -DSWIG
AM_CFLAGS = -fPIC -fno-strict-aliasing
AM_LDFLAGS = -shared ../src/libhid.la -lusb

SWIG = @SWIG_BIN@
SWIGFLAGS = -python -I../include

# swig produces crap code :)
override CFLAGS := $(subst -Werror,,$(CFLAGS))

dist_noinst_DATA = hid.i

pkgpyexec_LTLIBRARIES = _hid.la

nodist__hid_la_SOURCES = hid_wrap.c
_hid_la_DEPENDENCIES = ../src/libhid.la
_hid_la_LDFLAGS = -module

nodist_pkgpython_PYTHON = __init__.py

EXTRA_DIST = README

__init__.py: hid.py
	ln -f $< $@

%.py %_wrap.c: %.i ../include/hid.h
	$(SWIG) $(SWIGFLAGS) $<
	echo '#include <compiler.h>' > tmp.c
	sed -e 's/PyObject \*self/& UNUSED/' \
	    -e 's/int flags)/int flags UNUSED)/' < $(patsubst %.i,%_wrap.c,$<) >> tmp.c \
	&& mv tmp.c $(patsubst %.i,%_wrap.c,$<)

../src/libhid.la::
	@$(MAKE) -sC ../src libhid.la

dist_noinst_PYTHON = test_libhid.py

.PHONY: run test
run test: test_libhid.py $(pkgpyexec_LTLIBRARIES)
	python ./$<

startup.py: $(pkgpyexec_LTLIBRARIES) Makefile
	echo "import sys,os" > $@
	echo "libsdir = os.path.join(os.getcwd(), '.libs')" >> $@
	echo "if os.path.isdir(libsdir) and \\" >> $@
	echo "        os.path.isfile(os.path.join(libsdir,'_hid.so')):" >> $@
	echo "    sys.path.insert(0, libsdir)" >> $@
	echo "from hid import *" >> $@

.PHONY: python
python: startup.py
	env PYTHONSTARTUP=./startup.py python

.PHONY: all-local
all-local: test_libhid.py

.PHONY: clean-local
clean-local:
	rm -f startup.py $(wildcard *.pyc)
	rm -f $(patsubst %.i,%.py,$(wildcard *.i))
	rm -f $(wildcard *.s)
# only delete _wrap.[ci] files, not the python sources or helpers
	rm -f $(wildcard *_wrap.c) $(wildcard *_wrap.i) tmp.c
	rm -f $(wildcard *.o)
	rm -f $(wildcard *.so)
	rm -f $(wildcard *.lo *.loT)
	rm -f $(wildcard *.la)

# COPYRIGHT --
#
# This file is part of libhid, a user-space HID access library.
# libhid is (c) 2003-2005
#   Martin F. Krafft <libhid@pobox.madduck.net>
#   Charles Lepple <clepple+libhid@ghz.cc>
#   Arnaud Quette <arnaud.quette@free.fr> && <arnaud.quette@mgeups.com>
# and distributed under the terms of the GNU General Public License.
# See the file ./COPYING in the source distribution for more information.
#
# THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES
# OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.

