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
	debian/libhid-dev.docs \
	debian/libhid-dev.install \
	debian/libhid0.install \
	debian/python-hid.install \
	debian/rules

DEBIAN_VERSION = ${shell head -1 debian/changelog 2>/dev/null | sed -e '1s,.*(\([^)]\+\)).*,\1,'}
DEBIAN_ARCH = $(shell test -x `which dpkg-architecture 2>/dev/null` && dpkg-architecture -qDEB_BUILD_ARCH 2>/dev/null || echo i386)

TARBALL = $(distdir).tar.gz
DEBDIR = .debian
STAMP = $(DEBDIR)/stamp
DISTDIR = $(DEBDIR)/$(distdir)
CHANGES = $(DEBDIR)/@PACKAGE@_$(DEBIAN_VERSION)_$(DEBIAN_ARCH).changes
UPLOAD = $(DEBDIR)/@PACKAGE@_$(DEBIAN_VERSION)_$(DEBIAN_ARCH).upload

$(TARBALL):
	$(error run 'make dist' to create the tarball)

$(STAMP): $(TARBALL)
	mkdir -p $(DEBDIR)
	tar xfz $(TARBALL) -C $(DEBDIR)
	cp -r $(DISTDIR) $(DISTDIR).orig
	rm -rf $(DISTDIR).orig/debian
	touch $@

$(CHANGES): $(STAMP)
	cd $(DISTDIR) && dpkg-buildpackage -rfakeroot -k$(DEBSIGN_KEYID)

$(DEBDIR)/dupload.conf: dupload.conf
	ln -f $< $@

$(UPLOAD): $(CHANGES) $(DEBDIR)/dupload.conf
	cd $(DEBDIR) && dupload -c --to pdo

.PHONY: debian
debian: $(CHANGES)
	@echo "Debian packages for version $(DEBIAN_VERSION) built in .debian subdirectory"

.PHONY: dupload
dupload: $(UPLOAD)
	@echo "Debian packages uploaded."

.PHONY: debclean
debclean:
	rm -rf $(DEBDIR)

.PHONY: clean-local
clean-local:
	rm -f $(wildcard $(PACKAGE)-*.tar.gz)

