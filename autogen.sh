#!/bin/sh

libtoolize --force &> /dev/null
aclocal
autoheader
autoconf
automake --add-missing
exec ./configure $@
