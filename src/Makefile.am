AM_CFLAGS =
AM_CXXFLAGS = $(AM_CFLAGS)
AM_CPPFLAGS = -I../include

if LINUX_API
OS_SUPPORT = linux.c
else
if BSD_API
OS_SUPPORT = bsd.c
else
if DARWIN_API
OS_SUPPORT = darwin.c
CFLAGS_EXT = -no-cpp-precomp
LDADDS = -lIOKit -framework "CoreFoundation"
else
OS_SUPPORT =
endif
endif
endif

lib_LTLIBRARIES = libhid.la
libhid_la_SOURCES = hid.c debug.c $(OS_SUPPORT)

include_HEADERS = \
  ../include/hid.h \
	../include/debug.h \
	../include/asser.h

#doc::
#	make -C ../doc

cvsclean:: mrproper
mrproper:: maintainer-clean
	rm -f Makefile.in

clean-local::
	rm -f $(wildcard *.loT)

.PHONY: debug run cvsclean mrproper clean-local
