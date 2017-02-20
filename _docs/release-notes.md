---
layout: page
title: Release notes
menu: Release notes
permalink: /release-notes/
---

## v1.1.0
Release: Feb 20 2017

- Added a config file (`/etc/zabbix/libzbxpgsql.conf`) to enable named SQL
  queries to be used in `pg.query.*` keys - Thanks to Rob Brucks

- Fixed the `PostgreSQL Backend connections are exhausted on Zabbix server`
  trigger expression

- Added Makefile to backport templates to older version of Zabbix using the
  [Zabbix Template Convertor](https://github.com/cavaliercoder/zabbix-template-convertor)

## v1.0.0
Release: Jun 26 2016

This release includes breaking changes from previous versions.

- Added support for Zabbix v3

- Added error messages to failed requests

- Added `pg.db.xid_age` to monitor the allocation of Transaction IDs

- Added `pg.stats_reset_interval` to return seconds since background writer
  stats were reset

- Added `pg.table.n_mod_since_analyze` to return the estimated number of rows
  that have been modified since the last table analyze

- Added support for `pg.queries.longest` in PostgreSQL versions prior to 9.2

- Added `pg.prepared_xacts_count` to return the number of transactions currently
  prepared for two phase commit

- Added `pg.prepared_xacts_ratio` to return the number of transactions currently
  prepared for two phase commit as a ratio of the maximum permitted prepared
  transaction count

- Added `pg.prepared_xacts_age` to return the age of the oldest transaction
  currently prepared for two phase commit

- Added `pg.backends.free` to return the number of available backend connections

- Added `pg.backends.ratio` to return the ratio of used available backend
  connections

- Added `--with-postgresql` switch to source configuration script

- Added `--with-zabbix` switch to source configuration script

- Fixed misreporting in `pg.queries.longest` when no queries were in progress

- Fixed build dependencies on Debian (thanks darkweaver87)

- Moved build scripts to a new repository
  ([libzbxpgsql-build](https://github.com/cavaliercoder/libzbxpgsql-build))

- Enabled "deep" discovery of database specific assets

  Some assets such as namespaces, tables and indexes are only discoverable when
  connected to the parent database. Discovery keys for these items have been
  updated to determine which databases are accessible to the connected user,
  connect to each database separately and build an amalgamated list of
  discoverable items across all accessible databases.

  The result is that a discovery rule will use multiple backend connections,
  but will in turn return results for all accessible databases.

  To disable this feature and only return discovery items for the connected
  database, each key supports a new `search mode` parameter which can be set to
  `shallow`.

  The following keys will now discover across all accessible databases:

  * `pg.schema.discovery` and `pg.namespace.discovery`
  * `pg.table.discovery`
  * `pg.index.discovery`


- Added cache hit rate keys

  To determine the effectiveness of caches on databases, tables and indexes,
  convenience keys have been added to measure cache hits against disk hits where
  `result = cache hits / (disk hits + cache hits)`. An effective cache or index
  on a busy relation should return a value above 0.99.

  Keys include:

  * `pg.db.blks_ratio`: cache hit rate for a database
  * `pg.table.idx_scan_ratio`: index hit rate of all scans on a table
  * `pg.table.heap_blks_ratio`: cache hit rate for a table's data
  * `pg.table.idx_blks_ratio`: cache hit rate for a table's indexes
  * `pg.table.toast_blks_ratio`: cache hit rate for a table's TOAST data
  * `pg.table.tidx_blks_ratio`: cache hit rate for a table's TOAST indexes


- Added keys for additional checkpoint monitoring

  Checkpoint writing and syncing is an important aspect of PostgreSQL
  performance and stability. Additional keys have been added to give better
  insight into the configuration and performance of your background checkpoint
  writer process.

  * `pg.checkpoint_avg_interval`: average interval in seconds between each
    checkpoint since statistics were reset by the server

  * `pg.checkpoint_time_ratio`: percentage of time spent writing and/or syncing
    checkpoints since statistics were reset by the server

## v0.2.1
Release: Sep 14 2015

- Fixed connection leak in pg_version()
  
  A PostgreSQL backend connection was being opened by `pg_version()` and never
  closed. This function was called by the `pg.index.rows` agent key, resulting
  in a leaked file handle and backend connection with each call from Zabbix.

  See GitHub issue [#12](https://github.com/cavaliercoder/libzbxpgsql/issues/12)

- Fixed query error in pg.index.rows key

- Removed noisy logging in pg.query.* keys


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

- Initial release
