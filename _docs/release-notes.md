---
layout: page
title: Release notes
menu: Release notes
permalink: /release-notes/
---

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
