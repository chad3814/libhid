AM_CFLAGS =
AM_CPPFLAGS = 
AM_LDFLAGS = 

noinst_LTLIBRARIES = libhidparser.la
libhidparser_la_SOURCES = hidparser.c
libhidparser_la_LDFLAGS = -static

#doc::
#	make -C ../doc

cvsclean:: mrproper
mrproper:: maintainer-clean
	rm -f Makefile.in

clean-local::
	rm -f $(wildcard *.loT)

.PHONY: debug run cvsclean mrproper clean-local
