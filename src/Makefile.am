AM_MAKEFLAGS = @MAKEFLAGS@
ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS =
AM_CFLAGS = @OS_CFLAGS@
AM_LDFLAGS = @OS_LDFLAGS@

if OS_LINUX
OS_SUPPORT_SOURCE = linux.c
else
if OS_BSD
OS_SUPPORT_SOURCE = bsd.c
else
if OS_DARWIN
OS_SUPPORT_SOURCE = darwin.c
AM_CFLAGS += -no-cpp-precomp
AM_LDFLAGS += -lIOKit -framework "CoreFoundation"
else
OS_SUPPORT =
endif
endif
endif

lib_LTLIBRARIES = libhid.la
libhid_la_DEPENDENCIES = ../hidparser/libhidparser.la
libhid_la_LIBADD = ../hidparser/libhidparser.la
libhid_la_SOURCES = \
  $(OS_SUPPORT_SOURCE) \
	hid_initialisation.c \
	hid_preparation.c \
	hid_opening.c \
	hid_parsing.c \
	hid_exchange.c \
	hid_presentation.c \
	debug.c

include_HEADERS = ../include/hid.h

../hidparser/libhidparser.la::
	@$(MAKE) -sC ../hidparser libhidparser.la

.PHONY: clean-local
clean-local:
	rm -f $(wildcard *.loT)
