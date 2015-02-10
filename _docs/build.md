---
layout: page
title: Compilation/Build
menu: Build
permalink: /build/
---

To compile the agent module the following items are required:

* GNU build tools (`make`, `gcc`, `autoconf`, `automake`, `libtool`, `m4`, etc.)
* [libpq development headers](http://www.postgresql.org/download/)

To compile the module:

    # First time prep
    libtoolize
    aclocal
    autoheader
    automake --add-missing
    autoreconf

    # Compile
    ./configure && make

The `libzbxpgsql.so` will then be located under `src/.libs/`.

Call `make install` to install the module in `/usr/local/lib`
