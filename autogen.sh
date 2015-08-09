#!/bin/sh
libtoolize -c -f || exit 1
aclocal --force --verbose || exit 1
autoheader -fv || exit 1
automake -acfv || exit 1
autoreconf -ifv || exit 1
# ./configure
