#!/bin/sh -ex

libtoolize --force &> /dev/null
aclocal -I m4
autoheader
automake --add-missing
autoconf
exec ./configure $@
