AM_CPPFLAGS = @LIBUSBINCLUDE@ -I../include
AM_CFLAGS =
AM_LDFLAGS =

noinst_PROGRAMS = test_libhid

test_libhid_SOURCES = test_libhid.c
test_libhid_DEPENDENCIES = ../src/libhid.la
test_libhid_LDADD = ../src/libhid.la

../src/libhid.la::
	make -C ../src libhid.la

debug:: test_libhid
	libtool --mode=execute gdb -q $<

run:: test_libhid
	libtool --mode=execute ./$<

cvsclean:: mrproper
mrproper:: maintainer-clean
	rm -f Makefile.in

clean-local::
	rm -f $(wildcard *.loT)
	rm -f $(wildcard *.so.0*)

.PHONY: debug run cvsclean mrproper clean-local
