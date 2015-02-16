#!/bin/sh
#libtoolize -vc || exit 1
#aclocal --verbose || exit 1
#autoheader -v || exit 1
#automake -acv || exit 1
autoreconf -iv || exit 1
./configure
