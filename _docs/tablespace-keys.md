---
layout: page
title: Tablespace keys
permalink: /agent-keys/tablespaces/
---

The agent item keys described in this document allow you to discover and
monitor all Tablespaces on a PostgreSQL server.

## Table of contents

* [pg.tablespace.discovery](#pgtablespacediscovery)
* [pg.tablespace.size](#pgtablespacesizetablespace)

### pg.tablespace.discovery

Discover all Tablespaces visible to the configured user account.

*Type:* `Discovery`

*Macros:*

  * `{#OID}` Tablespace unique identifier
  * `{#TABLESPACE}` Tablespace name
  * `{#OWNER}` Owner name

*Source:*

    SELECT
      t.oid
      , t.spcname
      , a.rolname
    FROM pg_tablespace t 
    JOIN pg_authid a ON a.oid = t.spcowner

### pg.tablespace.size[,,,,,<<tablespace>>]

Returns the size in bytes of a PostgreSQL Tablespace

*Type:* `Numeric (unsigned)`

*Parameters:*

  * `tablespace` The name or OID of the desired Tablespace (required)

*Units:* Bytes

*Source:*

    SELECT pg_tablespace_size($1)
