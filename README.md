# libzbxpgsql

This project provides comprehensive monitoring of PostgreSQL servers using a
natively compiled Zabbix agent module and highly configurable item keys.

Sources in this project are used to compile `libzbxpgsql.so` which may be loaded
into a Zabbix agent for the discovery and monitoring of local or remote
PostgreSQL server assets; including databases, schema, table spaces, tables,
indices, etc.

Native Zabbix agent modules are advantageous over User Parameters and scripts
in that no process forking or code interpreter is required. This significantly
reduces the impact of monitoring on the agent system, particularly when
monitoring hundreds or thousands of checks.

## Table of contents

* [Open source license](#license)
* [Supported environments](#support)
* [Building from source](#building)
* [Module installation](#installation)
* [Monitoring template](#template)
* [Vagrant development VM](#vagrant-vm)
* [Further reading](#further-reading)
* [Connecting to PostgreSQL server](#connecting-to-postgresql)
* [Security considerations](#security)
* [Agent item keys](#agent-keys)

  * [Server keys](#server-keys)
  * [Database keys](#database-keys)
  * [Table keys](#table-keys)


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


## Support

All monitoring keys are confirmed to work with PostgreSQL v8.1 to 9.4 except
where otherwise noted.

The agent module is confirmed to work with Zabbix agent v1.8 to 2.4 on various
Linux based operating systems including CentOS and Ubuntu.


## Building

To compile the agent module the following packages are required:

* GNU build tools including `make` and `gcc`
* `autoconf` and `automake`
* `libtool`
* PostgreSQL 9.4 development headers
* Zabbix 2.4.3 sources

The Zabbix sources should be extracted into the project root directory or the
`CFLAGS` directive modified in `src/Makefile.am`.

To compile the module:

```bash
# First time prep
libtoolize
aclocal
autoheader
automake --add-missing
autoreconf

# Compile
./configure
make
```

The `libzbxpgsql.so` will then be located under `src/.libs/`.


## Installation

* Copy `libzbxpgsql.so` to your agent modules directory (e.g. `/usr/zabbix/lib`)

* Set `LoadModulePath` to your modules path in `zabbix_agentd.conf`

* Add a `LoadModule=libzbxpgsql.so` declaration to `zabbix_agentd.conf`

* Restart the Zabbix agent

* Test for successful installation with `zabbix_agentd -t pg.version`
   (connection parameters may be required).


## Template

A comprehensive Zabbix template is included to demonstrate the use of the agent
keys made available in this module. It is not recommended to use this template
out-of-the-box in production without first configuring the check intervals, 
configuration macros, etc. to suit your environment.

Import the `template_postgresql_server.xml` template into Zabbix get started.

## Vagrant VM

A `Vagrantfile` is provided for your convenience which builds a CentOS 7.0
VirtualBox machine ready for building and testing the agent module.


## Further reading

* [Zabbix Loadable Modules](https://www.zabbix.com/documentation/2.4/manual/config/items/loadablemodules)

* [PostgreSQL Statistics Collector](http://www.postgresql.org/docs/9.4/static/monitoring-stats.html)

* [libpq - C Library](http://www.postgresql.org/docs/9.4/static/libpq.html)

* [Vagrant](https://www.vagrantup.com/)


## Connecting to PostgreSQL

All agent keys in `lizbxpgsql.so` share common connection parameters for the
first five parameters. These parameters are as follows:

0. Hostname     (default: localhost)

1. Port         (default: 5432)

2. Database     (default: postgres)

3. Username     (default: postgres)

4. Password     (default: blank)

E.g. `pg.connect[<host>,<port>,<database>,<username>,<password>]`

Any parameters specific to an item key will start from index 5.


## Security

The agent module will require read permissions to the PostgreSQL server
performance tables (i.e. `pg_stat*`) and any databases, tables, etc. you wish
to monitor. An account should be configured for monitoring and should not have
write access to any PostgreSQL assets.

All Zabbix agent traffic is transmitted unencrypted so it recommended you do
__NOT__ transmit connection passwords as parameters. 

Instead, use a `.pgpass` file or allow trusted connections for the PostgreSQL
monitoring account you created. Ideally the trusted connection should only be
allowed for the agent IP address and is configurable in your `pg_bha.conf`
file.

If you do use a password for connection authentication, also consider who will
have visibility of the password in Zabbix is will appear everywhere the item
key appears, such as trigger and event logs, host configuration, etc.


## Agent keys

See the `keys[]` declaration in `libzbxpgsql.c` for a definitive list of
available agent keys and their associated functions.


### Server keys

* `pg.connect`

    Returns 1 if the Zabbix Agent can connect to PostgreSQL instance
  
* `pg.version`

    Returns the version string of the connection PostgreSQL Server
  
* `pg.checkpoints_timed`

    Returns the number of scheduled checkpoints that have been performed
    
* `pg.checkpoints_req`

    Returns the number of requested checkpoints that have been performed
    
* `pg.checkpoint_write_time`

    Returns the total amount of time that has been spent in the portion of
    checkpoint processing where files are written to disk, in milliseconds
    
* `pg.checkpoint_sync_time`

    Returns the total amount of time that has been spent in the portion of
    checkpoint processing where files are synchronized to disk, in milliseconds
    
* `pg.buffers_checkpoint`

    Returns the Number of buffers written during checkpoints
    
* `pg.buffers_clean`

    Returns the number of buffers written by the background writer
    
* `pg.maxwritten_clean`

    Returns the number of times the background writer stopped a cleaning scan
    because it had written too many buffers
    
* `pg.buffers_backend`

    Returns the number of buffers written directly by a backend
    
* `pg.buffers_backend_fsync`

    Returns the number of times a backend had to execute its own fsync call
    (normally the background writer handles those even when the backend does
    its own write)
    
* `pg.buffers_alloc`

    Returns the number of buffers allocated
    
* `pg.stats_reset`

    Returns the time at which these statistics were last reset


### Database keys

* `pg.db.discovery`

    Returns a JSON array of all known PostgreSQL databases which are available for connection
    
* `pg.db.size[,,<database>]`

    Returns the size in bytes of a PostgreSQL database

* `pg.db.numbackends[,,<database>]`

    Returns the number of backends currently connected to this database. This is the only column in this view that returns a value reflecting current state; all other columns return the accumulated values since the last reset.
    
* `pg.db.xact_commit[,,<database>]`

    Returns the number of transactions in this database that have been committed
    
* `pg.db.xact_rollback[,,<database>]`

    Returns the number of transactions in this database that have been rolled back
    
* `pg.db.blks_read[,,<database>]`

    Returns the number of disk blocks read in this database
    
* `pg.db.blks_hit[,,<database>]`

    Returns the number of times disk blocks were found already in the buffer cache, so that a read was not necessary (this only includes hits in the PostgreSQL buffer cache, not the operating system's file system cache)
    
* `pg.db.tup_returned[,,<database>]`

    Returns the number of rows returned by queries in this database
    
* `pg.db.tup_fetched[,,<database>]`

    Returns the number of rows fetched by queries in this database
    
* `pg.db.tup_inserted[,,<database>]`

    Returns the number of rows inserted by queries in this database
    
* `pg.db.tup_updated[,,<database>]`

    Returns the number of rows updated by queries in this database
    
* `pg.db.tup_deleted[,,<database>]`

    Returns the number of rows deleted by queries in this database
    
* `pg.db.conflicts[,,<database>]`

    Returns the number of queries canceled due to conflicts with recovery in this database. (Conflicts occur only on standby servers; see pg_stat_database_conflicts for details.)
    
* `pg.db.temp_files[,,<database>]`

    Returns the number of temporary files created by queries in this database. All temporary files are counted, regardless of why the temporary file was created (e.g., sorting or hashing), and regardless of the log_temp_files setting.
    
* `pg.db.temp_bytes[,,<database>]`

    Returns the total amount of data written to temporary files by queries in this database. All temporary files are counted, regardless of why the temporary file was created, and regardless of the log_temp_files setting.
    
* `pg.db.deadlocks[,,<database>]`

    Returns the number of deadlocks detected in this database
    
* `pg.db.blk_read_time[,,<database>]`

    Returns the time spent reading data file blocks by backends in this database, in milliseconds
    
* `pg.db.blk_write_time[,,<database>]`

    Returns the time spent writing data file blocks by backends in this database, in milliseconds
    
* `pg.db.stats_reset[,,<database>]`

    Returns the time at which these statistics were last reset


### Table keys

* `pg.table.discovery`

    Returns a JSON array of all known PostgreSQL tables in the current database
    
* `pg.table.size[,,,,,<table>]`

    Returns the size in bytes of a PostgreSQL table
    
* `pg.table.rows[,,,,,<table>]`

    Returns the number of rows in the table. This is only an estimate used by
    the planner. It is updated by `VACUUM`, `ANALYZE`, and a few DDL commands such
    as `CREATE INDEX`. 

* `pg.table.seq_scan[,,,,,<table>]`

	Returns the number of sequential scans initiated on this table

* `pg.table.seq_tup_read[,,,,,<table>]`

	Returns the number of live rows fetched by sequential scans

* `pg.table.idx_scan[,,,,,<table>]`

	Returns the number of index scans initiated on this table

* `pg.table.idx_tup_fetch[,,,,,<table>]`

	Returns the number of live rows fetched by index scans

* `pg.table.n_tup_ins[,,,,,<table>]`

	Returns the number of rows inserted

* `pg.table.n_tup_upd[,,,,,<table>]`

	Returns the number of rows updated

* `pg.table.n_tup_del[,,,,,<table>]`

	Returns the number of rows deleted

* `pg.table.n_tup_hot_upd[,,,,,<table>]`

	Returns the number of rows HOT updated (i.e., with no separate index update required)

* `pg.table.n_live_tup[,,,,,<table>]`

	Returns the estimated number of live rows

* `pg.table.n_dead_tup[,,,,,<table>]`

	Returns the estimated number of dead rows

* `pg.table.last_vacuum[,,,,,<table>]`

	Returns the last time at which this table was manually vacuumed (not counting VACUUM FULL)

* `pg.table.last_autovacuum[,,,,,<table>]`

	Returns the last time at which this table was vacuumed by the autovacuum daemon

* `pg.table.last_analyze[,,,,,<table>]`

	Returns the last time at which this table was manually analyzed

* `pg.table.last_autoanalyze[,,,,,<table>]`

	Returns the last time at which this table was analyzed by the autovacuum daemon

* `pg.table.vacuum_count[,,,,,<table>]`

	Returns the number of times this table has been manually vacuumed (not counting VACUUM FULL)

* `pg.table.autovacuum_count[,,,,,<table>]`

	Returns the number of times this table has been vacuumed by the autovacuum daemon

* `pg.table.analyze_count[,,,,,<table>]`

	Returns the number of times this table has been manually analyzed

* `pg.table.autoanalyze_count[,,,,,<table>]`

    Returns the number of times this table has been analyzed by the autovacuum daemon
        
* `pg.table.heap_blks_read[,,,,,<table>]`

	Returns the number of disk blocks read from this table

* `pg.table.heap_blks_hit[,,,,,<table>]`

	Returns the number of buffer hits in this table

* `pg.table.idx_blks_read[,,,,,<table>]`

	Returns the number of disk blocks read from all indexes on this table

* `pg.table.idx_blks_hit[,,,,,<table>]`

	Returns the number of buffer hits in all indexes on this table

* `pg.table.toast_blks_read[,,,,,<table>]`

	Returns the number of disk blocks read from this table's TOAST table (if any)

* `pg.table.toast_blks_hit[,,,,,<table>]`

	Returns the number of buffer hits in this table's TOAST table (if any)

* `pg.table.tidx_blks_read[,,,,,<table>]`

	Returns the number of disk blocks read from this table's TOAST table index (if any)

* `pg.table.tidx_blks_hit[,,,,,<table>]`

	Returns the number of buffer hits in this table's TOAST table index (if any)