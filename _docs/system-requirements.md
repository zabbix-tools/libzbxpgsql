---
layout: page
title: System requirements
menu: About
permalink: /documentation/system-requirements/
---

The following minimum components are required:

### Operating system

Libzbxpgsql is supported on all operating systems currently supported by Zabbix
for loading agent modules.

In essence, any Linux based operating system.

### PostgreSQL

PostgreSQL server v8.4 or above is required. All item keys are supported except
where otherwise noted in the
[module item key reference]({{ site.baseurl }}/documentation/reference/)
documentation.

The PostgreSQL client library, libpq v.5 or above, is required to be installed
on the same system as the Zabbix agent and module. This client library is
typically available as a package named `libpq5` or `postgresql-libs`.

### Zabbix

Zabbix v2.2 or above is required.

Zabbix v3 broke compatibility with Zabbix v2 modules, so ensure you install the
correct libzbxpgsql package for your Zabbix version. See
[Known issues]({{ site.baseurl }}/documentation/known-issues/#zabbix-v3-compatibility)
for more information.
