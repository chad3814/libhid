AM_CFLAGS =
AM_LDFLAGS = -lhidparser
AM_CPPFLAGS = -I../include

noinst_PROGRAMS = test_libhid test_libhid.static

test_libhid_SOURCES = test_libhid.c
test_libhid_DEPENDENCIES = ../src/libhid.la
test_libhid_LDADD = -L../src -lhid libusb.a -lefence

test_libhid_static_SOURCES = test_libhid.c
test_libhid_static_DEPENDENCIES = ../src/.libs/libhid.a
test_libhid_static_LDADD = ../src/.libs/libhid.a libusb.a

../src/.libs/libhid.a::
	make -C ../src libhid.la

../src/libhid.la::
	make -C ../src libhid.la

debug:: test_libhid.static
	gdb -q $<

run:: test_libhid
	./test_libhid

cvsclean:: mrproper
mrproper:: maintainer-clean
	rm -f Makefile.in

clean-local::
	rm -f $(wildcard *.loT)
	rm -f $(wildcard *.so.0*)

.PHONY: debug run cvsclean mrproper clean-local
