---
layout: page
title: PostgreSQL monitoring for Zabbix
menu: About
permalink: /
---

libzbxpgsql (*Lib-Zabbix-PostgreSQL*) provides comprehensive monitoring of
PostgreSQL servers using a natively compiled Zabbix agent module, written in C,
with highly configurable item keys and a complimentary Template.

Native Zabbix agent modules are advantageous over User Parameters and scripts
in that no process forking, code interpreter or external dependencies are
required. This significantly reduces the impact of monitoring on the agent
system, particularly when monitoring hundreds or thousands of checks.

## Features

* Powerful discovery of Tablespaces, Databases, Namespaces, Tables and Indexes
* Exhaustive coverage of PostgreSQL Statistics Collector views
* Execute custom queries specific to your applications
* Significant performance benefit over `UserParameter` scripts
* No `UserParameter` declarations required
* No external dependencies (such as PostgreSQL client tools)
* Simple installation or bundling with your agent packages
* Detailed documentation
* Comprehensive PostgreSQL monitoring Template included

![Zabbix Screen example]({{ site.baseurl }}/assets/screen.png)

## Compatibility

All monitoring keys are confirmed to work with PostgreSQL v8.4 to 9.4 except
where otherwise noted.

The agent module is confirmed to work with Zabbix agent v2.2+ on various
Linux based operating systems including RedHat, CentOS, openSUSE and Ubuntu.

__WARNING__ There is a known bug [KZBX-9218](https://support.zabbix.com/browse/ZBX-9218)
in Zabbix agent v2.4.2 - 2.4.3 (and possibly earlier 2.4.x versions) which
causes a segmentation fault if any item in a loaded module becomes
unsupported. The issue is fixed in v2.4.4 and above.

The issue produces the following line immediately following an unsupported item
in the zabbix agent log file:

	Got signal [signal:11(SIGSEGV),reason:1,refaddr:(nil)]. Crashing ...

Further comment is welcome on the [GitHub issue](https://github.com/cavaliercoder/libzbxpgsql/issues/5).

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

You should have received a copy of the [GNU General Public License](http://www.gnu.org/licenses/gpl-2.0.html)
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.


## Further information

* [libzbxpgsql Sources](https://github.com/cavaliercoder/libzbxpgsql)

* [Zabbix Loadable Modules](https://www.zabbix.com/documentation/2.4/manual/config/items/loadablemodules)

* [PostgreSQL Statistics Collector](http://www.postgresql.org/docs/9.4/static/monitoring-stats.html)

* [libpq - C Library](http://www.postgresql.org/docs/9.4/static/libpq.html)

* [Vagrant](https://www.vagrantup.com/)
