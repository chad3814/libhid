#!/bin/sh -ex

libtoolize --force &> /dev/null
aclocal
autoheader
automake --add-missing --no-force
autoconf
exec ./configure $@
