AM_MAKEFLAGS = @MAKEFLAGS@
ACLOCAL_AMFLAGS = -I m4

if PYTHONWRAPPER
MAYBE_PYTHONWRAPPER = swig
endif
SUBDIRS = hidparser src $(MAYBE_PYTHONWRAPPER) test

dist_noinst_HEADERS = \
	include/assert.h \
	include/constants.h \
	include/debug.h \
	include/hid_helpers.h \
	include/os.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = pkgconfig/libhid.pc

dist_noinst_DATA = \
	debian/changelog \
	debian/compat \
	debian/control \
	debian/copyright \
	debian/docs \
	debian/libhid-dev.install \
	debian/libhid0.install \
	debian/python-hid.install \
	debian/rules

.PHONY: clean-local
clean-local:
	rm -f $(wildcard $(PACKAGE)-*.tar.gz)

include Makefile.debian
