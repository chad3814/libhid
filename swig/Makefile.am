AM_MAKEFLAGS = @MAKEFLAGS@
ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I/usr/include/python2.3/ -I../include -DHID_INTERNAL
AM_CFLAGS = -fpic
AM_LDFLAGS = -shared ../src/libhid.la -lusb -lswigpy

SWIG = swig -python -c
SWIGFLAGS = -I../include

dist_noinst_DATA = hid.i

pkgpyexec_LTLIBRARIES = libpython_hid.la
libpython_hid_la_SOURCES = hid_wrap.c
libpython_hid_la_DEPENDENCIES = ../src/libhid.la

pkgpython_PYTHON = __init__.py

__init__.py: hid.py
	ln -f $< $@

%.py: %.i
	$(SWIG) $(SWIGFLAGS) $<
	sed -e "s,_$(patsubst %.py,%,$@),libpython&,g" $@ > $@.new;
	mv $@.new $@;

%_wrap.c: %.py
	for i in $(patsubst %.i,%,$<); do \
		sed -e "s,SWIG_init[ 	]\+init_$$i,SWIG_init initlibpython_$$i,g" \
				-e "s,SWIG_name[ 	]\+\"_$$i\",SWIG_name \"libpython_hid\",g" \
			$@ > $@.new; \
		mv $@.new $@; \
	done

../src/libhid.la::
	@$(MAKE) -sC ../hidparser libhidparser.la

startup.py: $(pyexec_LTLIBRARIES)
	echo "#!/usr/bin/python" > $@
	echo "from sys import path" >> $@
	echo "from os import getcwd" >> $@
	echo "path.append(getcwd() + '/.libs')" >> $@
	for i in *.i; do \
		echo "from $${i%%.i} import *"; \
	done >> $@

.PHONY: run
run: $(pyexec_LTLIBRARIES) startup.py
	env PYTHONSTARTUP=./startup.py python

.PHONY: clean-local
clean-local:
	rm -f $(wildcard *.py) $(wildcard *.pyc)
	rm -f $(wildcard *.c)
	rm -f $(wildcard *.o)
	rm -f $(wildcard *.so)
	rm -f $(wildcard *.lo)
	rm -f $(wildcard *.la)
