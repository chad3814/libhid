AM_CFLAGS =
AM_CPPFLAGS = -I/usr/local/include -I../include
AM_LDFLAGS = 

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
override AM_CPPFLAGS = $(AM_CPPFLAGS_) -DCPPFLAGS="\"$(AM_CPPFLAGS_)\"" \
	-DCFLAGS="\"$(AM_CFLAGS)\"" -DLDFLAGS="\"$(AM_LDFLAGS)\"" \
	-DCOMPILEDATE="\"`date -R`\""

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

include_HEADERS = ../include/hid.h

#doc::
#	make -C ../doc

cvsclean:: mrproper
mrproper:: maintainer-clean
	rm -f Makefile.in

clean-local::
	rm -f $(wildcard *.loT)

.PHONY: debug run cvsclean mrproper clean-local
