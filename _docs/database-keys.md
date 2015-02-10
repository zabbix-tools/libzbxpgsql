---
layout: page
title: Database item keys
permalink: /agent-keys/databases/
---

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


