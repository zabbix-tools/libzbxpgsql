---
layout: page
title: Release notes
menu: Release notes
permalink: /release-notes/
---

## v0.2.0
Release: Aug 16 2015

- Improved connections parameters on all item keys

  A `libpq` compatible keyword/value connection string can now be specified as
  the first parameter on all item keys. This allows additional configuration
  such as connection timeouts and SSL to be configured.

  Unfortunately this __breaks template compatibility__ with previous versions
  of `libzbxpgsql`.

- Add custom discovery rules via `pg.query.discovery`

  You can now transform any SQL query into a discovery rule. Each row is
  returned as a discovery instance, with each field returned as a discovery
  macro.

- Fixed compatability issues with < v9.2

  In earlier versions of `libzbxpgsql`, a number of items became unsupported
  on PostgreSQL servers older than v9.2. All items are now confirmed to be
  compatible with PostgreSQL v8.4 and above, except where noted in the
  documentation.

- Added support for OpenSUSE v13.2

  RPM packages are now available for OpenSUSE v13.2.

- Added SQL injection prevention

  All user input is now parametized and escaped to prevent SQL injection
  attacks via the Zabbix agent.

- Added `pg.uptime` and `pg.starttime` keys

  Server uptime and availability can now be monitored for change.

- Added `pg.modver` key to monitor the installed `libzbxpgsql` version

- Reduced required privileges for all keys to just `LOGIN`

  All item keys now only require the `LOGIN` privilege be granted to the
  role used for connecting to PostgreSQL. No additional privileges are
  required.

  This is a big win for security as no elevated privileges are required by
  Zabbix and therefore cannot be exploited or misused in this way.

  Any custom queries supplied to `pg.query.*` keys may still require additional
  privileges, as defined by the user.

- Fixed integer overflow issues on large objects

  Databases, tables, indexes, etc with sizes larger than 2GB are now reported
  correctly.

- Improved automated testing and packaging using Docker and `zabbix_agent_bench`

## v0.1.3
Release: Mar 17 2015

- Added configuration directive discovery

  All configuration directives in the `pg_settings` table can now be
  automatically discovered using a discovery rule and monitored for changes.


## v0.1.2 
Released: Feb 20 2015

- Fixed module installation path in RPM package

  The `.so` module file was being installed to `/usr/lib/` instead of the
  Zabbix agent default directory, `/usr/lib/module`.

- Added git reference to library version info

  The Git revision and state or the working directory is now included in the
  module build via the `Makefile`.

- Added project and RPM build to Travis CI

  Each push to GitHub now triggers and automated build of both the module and
  RPM package using the free (and fantastic) service [Travis CI](https://travis-ci.org)

- Improved detection of PostgreSQL OIDs and IP addresses in parameter values

  I was using basic pattern matching to differenciate between a hostname and IP
  address, and an OID or table/database/user name. This version now uses the
  POSIX function `inet_pton` to detect valid IPv4 or IPv6 addresses and accepts
  all-numeric strings as valid OIDs.


## v0.1.1
Release: Feb 16 2015

- Added `pg.queries.longest` key

  Returns the duration of the longest running currently active query.

- Added `pg.setting` key
  
  Returns the value of the specified run-time configuration setting. This key is
  useful for gathering configuration information such as the PostgreSQL data
  directory, maximum threshold, log file locations, etc.


- Added `pg.query.*` keys

  Execute a custom SQL query and return the value of the first column of the
  first row as a string, integer or double.


## v0.1.0
Release: Feb 8 2015

* Initial release
