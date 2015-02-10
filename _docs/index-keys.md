---
layout: page
title: Index keys
permalink: /agent-keys/indexes/
---

The agent item keys described in this document allow you to discover and
monitor all Table Indexes on a PostgreSQL server.

## Table of contents

* [pg.index.discovery](#pgindexdiscovery)
* [pg.index.size](#pgindexsizeindex)
* [pg.index.rows](#pgindexrowsindex)

### pg.index.discovery

Discover all Indexes visible to the configured user account.

*Type:* `Discovery`

*Macros:*

  * `{#OID}` Index unique identifier
  * `{#INDEX}` Index name
  * `{#TABLE}` Parent table of the index
  * `{#DATABASE}` Parent database of the index
  * `{#SCHEMA}` Parent namespace of the index
  * `{#OWNER}` Owner name
  * `{#ACCESS}` Index access method (e.g. `btree`, `hash`, `gin`, etc.)

*Source:*

    SELECT
        ic.oid
        , ic.relname
        , current_database()
        , n.nspname
        , t.relname
        , a.rolname
        ,m.amname
    FROM pg_index i
    JOIN pg_class ic ON ic.oid = i.indexrelid
    JOIN pg_namespace n ON n.oid = ic.relnamespace
    JOIN pg_authid a ON a.oid = ic.relowner
    JOIN pg_class t ON t.oid = i.indrelid
    JOIN pg_am m ON m.oid = ic.relam;


### pg.index.size[,,,,,<<index>>]

Returns the size in bytes of a PostgreSQL Index

*Type:* `Numeric (unsigned)`

*Parameters:*

  * `index` The name or OID of the desired Index. If not specified, the sum
    size of all accessible indexes is returned.

*Units:* Bytes

*Source:*

    SELECT (relpages * 8192)
    FROM pg_class
    WHERE (
      relkind='i' 
      AND relname = $1
    );

    -- or for sum values:

    SELECT (SUM(relpages) * 8192)
    FROM pg_class
    WHERE relkind='i';

### pg.index.rows[,,,,,<<index>>]

Returns the estimated row count of a PostgreSQL Index

*Type:* `Numeric (unsigned)`

*Parameters:*

  * `index` The name or OID of the desired Index. If not specified, the sum
    estimated row count of all accessible indexes is returned.

*Source*:

    SELECT reltuples
    FROM pg_class
    WHERE (
      relkind='i'
      AND relname = $1
    );

    -- or for sum values:

    SELECT SUM(reltuples)
    FROM pg_class
    WHERE relkind='i';