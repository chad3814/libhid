SUBDIRS = hidparser src test

dist_noinst_DATA = \
	debian/changelog \
	debian/compat \
	debian/control \
	debian/copyright \
	debian/libhid-dev.dirs \
	debian/libhid-dev.docs \
	debian/libhid-dev.files \
	debian/libhid0.dirs \
	debian/libhid0.files \
	debian/rules

dist_noinst_HEADERS = \
  include/assert.h \
	include/constants.h \
	include/debug.h \
	include/hid_helpers.h \
	include/macros.h \
	include/os.h

test:: test/test_libhid
	make -C test run

test/test_libhid::
	make -C test

debian:: dist
	mkdir -p .debinstall
	cd .debinstall && tar xfz ../$(distdir).tar.gz
	cd .debinstall/$(distdir) && fakeroot debian/rules binary
	mv --target-directory=. $(wildcard .debinstall/*.deb)
	rm -rf .debinstall

cvsclean:: mrproper
mrproper:: maintainer-clean
	rm -f Makefile.in aclocal.m4 config.guess config.sub configure config.h.in
	rm -f depcomp install-sh ltmain.sh missing mkinstalldirs
	rm -f COPYING INSTALL
	rm -rf install
	rm -f $(wildcard */Makefile.in)
	rm -f $(wildcard *~)
	rm -f $(distdir).tar.gz
	rm -f $(wildcard *.deb)

.PHONY: cvsclean mrproper test
