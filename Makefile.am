SUBDIRS = hidparser src test

dist_noinst_DATA = \
	debian/changelog \
	debian/compat \
	debian/control \
	debian/copyright \
	debian/libhid-dev.dirs \
	debian/libhid-dev.docs \
	debian/libhid0.dirs \
	debian/rules

dist_noinst_HEADERS = \
  include/assert.h \
	include/constants.h \
	include/debug.h \
	include/hid_helpers.h \
	include/macros.h \
	include/os.h \
	hidparser/hidparser.h \
	hidparser/hidtypes.h

test:: test/test_libhid
	make -C test run

test/test_libhid::
	make -C test

cvsclean:: mrproper
mrproper:: maintainer-clean
	rm -f Makefile.in aclocal.m4 config.guess config.sub configure config.h.in
	rm -f depcomp install-sh ltmain.sh missing mkinstalldirs
	rm -f COPYING INSTALL
	rm -rf install
	rm -f $(wildcard */Makefile.in)
	rm -f $(wildcard *~)

.PHONY: cvsclean mrproper test
