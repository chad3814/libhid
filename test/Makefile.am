AM_MAKEFLAGS = @MAKEFLAGS@
ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS =
AM_CFLAGS =
AM_LDFLAGS =

noinst_PROGRAMS = test_libhid
test_libhid_SOURCES = test_libhid.c
test_libhid_DEPENDENCIES = ../src/libhid.la
test_libhid_LDADD = ../src/libhid.la

../src/libhid.la::
	@$(MAKE) -sC ../src libhid.la

../hidparser/libhidparser.la::
	@$(MAKE) -sC ../hidparser libhidparser.la

.PHONY: debug
debug: test_libhid
	libtool --mode=execute gdb -q $<

.PHONY: run
run: test_libhid
	libtool --mode=execute ./$<

clean-local::
	rm -f $(wildcard *.loT)
