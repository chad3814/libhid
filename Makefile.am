SUBDIRS = src test

#dist_noinst_DATA = \
#	debian/changelog \
#	debian/compat \
#	debian/control \
#	debian/copyright \
#	debian/docs \
#	debian/libhid-dev.dirs \
#	debian/libhid-dev.files \
#	debian/libhid1.dirs \
#	debian/libhid1.files \
#	debian/rules

test:: test/test_libhid
	$<

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
