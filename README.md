# libzbxpgsql

This project provides comprehensive monitoring of PostgreSQL servers using a
natively compiled Zabbix agent module and highly configurable item keys.

Sources in this project are used to compile `libzbxpgsql.so` which may be
loaded into a Zabbix agent for the discovery and monitoring of local or remote
PostgreSQL server assets; including databases, schema, table spaces, tables,
indices, etc.

Native Zabbix agent modules are advantageous over User Parameters and scripts
in that no process forking or code interpreter is required. This significantly
reduces the impact of monitoring on the agent system, particularly when
monitoring hundreds or thousands of checks.

* Read the [documentation](http://cavaliercoder.github.io/libzbxpgsql/)
* Clone the [sources](http://github.com/cavaliercoder/libzbxpgsql)
* Stalk the [author](http://cavaliercoder.com)

## License

libzbxpgsql - A PostgreSQL monitoring module for Zabbix
Copyright (C) 2015 - Ryan Armstrong <ryan@cavaliercoder.com>

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
