---
layout: page
title: Table item keys
permalink: /agent-keys/tables/
---

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