---
layout: page
title: Server keys
permalink: /agent-keys/server/
---

Theses items collect data representing the PostgreSQL service itself, rather
than individual assets such as databases and tablespaces.

The majority of the following global items are gathered from the
[pg_stat_bgwriter](http://www.postgresql.org/docs/9.4/static/monitoring-stats.html#PG-STAT-BGWRITER-VIEW)
view.

## Table of contents

* [pg.connect](#pgconnect)
* [pg.version](#pgversion)
* [pg.checkpoints_timed](#pgcheckpointstimed)
* [pg.checkpoints_req](#pgcheckpointsreq)
* [pg.checkpoint_write_time](#pgcheckpointwritetime)
* [pg.checkpoint_sync_time](#pgcheckpointsynctime)
* [pg.buffers_checkpoint](#pgbufferscheckpoint)
* [pg.buffers_clean](#pgbuffersclean)
* [pg.maxwritten_clean](#pgmaxwrittenclean)
* [pg.buffers_backend](#pgbuffersbackend)
* [pg.buffers_backend_fsync](#pgbuffersbackendfsync)
* [pg.buffers_alloc](#pgbuffersalloc)
* [pg.stats_reset](#pgstatsreset)

### pg.connect

Monitor the availability of a PostgreSQL server. Returns 1 if the Zabbix Agent
can connect to the specified PostgreSQL instance.

*Type:* `Numeric (Unsigned)`

*Value map:* 0: Failure, 1: Success

*Example:*
    
    pg.connect[pg-server,5432,my-db,pg-monitor,p4ssw0rD]


### pg.version

Returns the version string of the connected PostgreSQL Server

*Type:* `Text`  

*Source:* `SELECT version();`


### pg.checkpoints_timed

Returns the number of scheduled checkpoints that have been performed

*Type:* `Numeric (unsigned)`

*Source:* `SELECT checkpoints_timed FROM pg_stat_bgwriter WHERE ...`
    

### pg.checkpoints_req

Returns the number of requested checkpoints that have been performed

*Type:* `Numeric (unsigned)`

*Source:* `SELECT checkpoints_req FROM pg_stat_bgwriter WHERE ...`
  

### pg.checkpoint_write_time

Returns the total amount of time that has been spent in the portion of
checkpoint processing where files are written to disk, in milliseconds

*Type:* `Numeric (float)`

*Units:* Milliseconds

*Source:* `SELECT checkpoint_write_time FROM pg_stat_bgwriter WHERE ...`
  

### pg.checkpoint_sync_time

Returns the total amount of time that has been spent in the portion of
checkpoint processing where files are synchronized to disk, in milliseconds

*Type:* `Numeric (float)`

*Units:* Milliseconds

*Source:* `SELECT checkpoint_sync_time FROM pg_stat_bgwriter WHERE ...`
    

### pg.buffers_checkpoint

Returns the Number of buffers written during checkpoints

*Type:* `Numeric (unsigned)`

*Source:* `SELECT buffers_checkpoint FROM pg_stat_bgwriter WHERE ...`


### pg.buffers_clean

Returns the number of buffers written by the background writer

*Type:* `Numeric (unsigned)`

*Source:* `SELECT buffers_clean FROM pg_stat_bgwriter WHERE ...`

 
### pg.maxwritten_clean

Returns the number of times the background writer stopped a cleaning scan
because it had written too many buffers

*Type:* `Numeric (unsigned)`

*Source:* `SELECT maxwritten_clean FROM pg_stat_bgwriter WHERE ...`


### pg.buffers_backend

Returns the number of buffers written directly by a backend

*Type:* `Numeric (unsigned)`

*Source:* `SELECT buffers_backend FROM pg_stat_bgwriter WHERE ...`


### pg.buffers_backend_fsync

Returns the number of times a backend had to execute its own fsync call
(normally the background writer handles those even when the backend does
its own write)

*Type:* `Numeric (unsigned)`

*Source:* `SELECT buffers_backend_fsync FROM pg_stat_bgwriter WHERE ...`


### pg.buffers_alloc

Returns the number of buffers allocated

*Type:* `Numeric (unsigned)`

*Source:* `SELECT buffers_alloc FROM pg_stat_bgwriter WHERE ...`

    
### pg.stats_reset

Returns the time at which these statistics were last reset

*Type:* `Text`

*Source:* `SELECT stats_reset FROM pg_stat_bgwriter WHERE ...`
