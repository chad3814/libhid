AM_CFLAGS =
AM_CPPFLAGS = @LIBUSBINCLUDE@ -I../include
AM_LDFLAGS = -L. @LIBUSBLIB@ -lusb -lhidparser

if LINUX_API
OS_SUPPORT = linux.c
else
if BSD_API
OS_SUPPORT = bsd.c
else
if DARWIN_API
OS_SUPPORT = darwin.c
AM_CFLAGS += -no-cpp-precomp
AM_LDFLAGS += -lIOKit -framework "CoreFoundation"
else
OS_SUPPORT =
endif
endif
endif

AM_CPPFLAGS_ := $(AM_CPPFLAGS)
CPPFLAGSFILLER := $(shell test ! -z "$(CPPFLAGS)" && echo ' ')
CFLAGSFILLER := $(shell test ! -z "$(CFLAGS)" && echo ' ')
LDFLAGSFILLER := $(shell test ! -z "$(LDFLAGS)" && echo ' ')

override AM_CPPFLAGS = $(AM_CPPFLAGS_) \
  -DCPPFLAGS="\"$(CPPFLAGS)$(CPPFLAGSFILLER)$(AM_CPPFLAGS_)\"" \
	-DCFLAGS="\"$(CFLAGS)$(CFLAGSFILLER)$(AM_CFLAGS)\"" \
	-DLDFLAGS="\"$(LDFLAGS)$(LDFLAGSFILLER)$(AM_LDFLAGS)\"" \
	-DCOMPILEDATE="\"`date`\""

lib_LTLIBRARIES = libhid.la
libhid_la_SOURCES = \
  $(OS_SUPPORT) \
	hid_initialisation.c \
	hid_preparation.c \
	hid_opening.c \
	hid_parsing.c \
	hid_exchange.c \
	hid_presentation.c \
	config.c \
	debug.c

bin_PROGRAMS = libhid-config
libhid_config_SOURCES = libhid-config.c
libhid_config_DEPENDENCIES = libhid.la
libhid_config_LDADD = -lhid

include_HEADERS = ../include/hid.h

#doc::
#	make -C ../doc

cvsclean:: mrproper
mrproper:: maintainer-clean
	rm -f Makefile.in

clean-local::
	rm -f $(wildcard *.loT)

.PHONY: debug run cvsclean mrproper clean-local
