AM_MAKEFLAGS = @MAKEFLAGS@
ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I/usr/include/python2.3/ -I../include -DHID_INTERNAL -DSWIG
AM_CFLAGS = -fPIC
AM_LDFLAGS = -shared ../src/libhid.la -lusb -lswigpy

SWIG = @SWIG_BIN@
SWIGFLAGS = -python -noruntime -I../include

# swig produces crap code :)
override CFLAGS := $(subst -Werror,,$(CFLAGS))

dist_noinst_DATA = hid.i test_libhid.py.in

pkgpyexec_LTLIBRARIES = _hid.la
nodist__hid_la_SOURCES = hid_wrap.c
dist__hid_la_SOURCES = hid_swig_helpers.c
_hid_la_DEPENDENCIES = ../src/libhid.la
_hid_la_LDFLAGS = -module

dist_noinst_HEADERS = hid_swig_helpers.h

nodist_pkgpython_PYTHON = __init__.py

EXTRA_DIST = README

__init__.py: hid.py
	ln -f $< $@

%.py: %.i
	$(SWIG) $(SWIGFLAGS) $<
	
%_wrap.c: %.py
	sed -i -e 's/PyObject \*self/PyObject *self UNUSED/' $@

../src/libhid.la::
	@$(MAKE) -sC ../src libhid.la

startup.py: $(pkgpyexec_LTLIBRARIES)
	echo "#!/usr/bin/python" > $@
	echo >> $@
	echo "# preparation" >> $@
	echo "import sys" >> $@
	echo "import os" >> $@
	echo "libsdir = os.getcwd() + '/.libs'" >> $@
	echo "if os.path.isdir(libsdir):" >> $@
	echo "  sys.path.append(libsdir)" >> $@
	for i in *.i; do \
		echo "from $${i%%.i} import *"; \
	done >> $@

.PHONY: python
python: startup.py
	env PYTHONSTARTUP=./startup.py python

test_libhid.py: startup.py test_libhid.py.in
	cat $< > $@
	echo >> $@
	cat test_libhid.py.in >> $@
	chmod +x $@

.PHONY: run test
run test: test_libhid.py
	./$<

.PHONY: all-local
all-local: test_libhid.py

.PHONY: clean-local
clean-local:
	rm -f $(wildcard *.py) $(wildcard *.pyc)
	rm -f $(wildcard *.s)
# only delete _wrap.[ci] files, not the python sources or helpers
	rm -f $(wildcard *_wrap.c) $(wildcard *_wrap.i)
	rm -f $(wildcard *.o)
	rm -f $(wildcard *.so)
	rm -f $(wildcard *.lo)
	rm -f $(wildcard *.la)

# COPYRIGHT --
#
# This file is part of libhid, a user-space HID access library.
# libhid is (c) 2003-2004
#   Martin F. Krafft <libhid@pobox.madduck.net>
#   Charles Lepple <clepple@ghz.cc>
#   Arnaud Quette <arnaud.quette@free.fr> && <arnaud.quette@mgeups.com>
# and distributed under the terms of the GNU General Public License.
# See the file ./COPYING in the source distribution for more information.
#
# THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES
# OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.

