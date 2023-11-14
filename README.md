# libzbxpgsql [![Build Status](https://travis-ci.org/cavaliercoder/libzbxpgsql.svg?branch=master)](https://travis-ci.org/cavaliercoder/libzbxpgsql)

This project provides comprehensive monitoring of PostgreSQL servers using a
natively compiled Zabbix agent module, written in C.

**N.B.** Zabbix 6+ users will probably prefer the official support for PostrgreSQL
available in `zabbix_agent2`, [documented here](https://git.zabbix.com/projects/AP/repos/postgresql/browse/README.md).

A preconfigured Zabbix Template is also included for your convenience.

Sources in this project are used to compile `libzbxpgsql.so` which may be
loaded by a Zabbix agent using the `LoadModule` directive. The module enables
discovery and monitoring of tablespaces, databases, namespaces, tables,
indexes, etc.

* Read the [documentation](http://cavaliercoder.com/libzbxpgsql/)
* Download the [packages](http://cavaliercoder.com/libzbxpgsql/download)
* Clone the [sources](http://github.com/cavaliercoder/libzbxpgsql)
* Follow the [author](http://cavaliercoder.com)


## Installation

To compile the agent module the following items are required:

* GNU build tools (`make`, `gcc`, `autoconf`, `automake`, `libtool`, `m4`, etc.)
* [Zabbix sources](http://www.zabbix.com/download.php)
* [libpq development headers](http://www.postgresql.org/download/)

If you are building from sources cloned from GitHub, you first need to
regenerate the build scripts using `./autogen.sh`. Otherwise:

    ./configure --with-zabbix=/usr/src/zabbix
    make
    sudo make install

Module file `libzbxpgsql.so` will then be installed in `/usr/local/lib`.

If you are using a packaged version of Zabbix, you may with to redirect the
installation directories as follows:

    $ sudo make prefix=/usr sysconfdir=/etc libdir=/usr/lib64 install

__Note:__ Please use a clean copy of the Zabbix source code. Once you configure
or build the Zabbix sources, they are no longer useful for building this module.

To build the RPM package on a RHEL6+ family system with `rpm-build` installed:

    make rpm


## License

libzbxpgsql - A PostgreSQL monitoring module for Zabbix
Copyright (C) 2016 - Ryan Armstrong <ryan@cavaliercoder.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
