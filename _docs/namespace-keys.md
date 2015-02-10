---
layout: page
title: Namespace keys
permalink: /agent-keys/namespaces/
---

The agent item keys described in this document allow you to discover and
monitor all Namespaces (a.k.a *Schema*) on a PostgreSQL server.

## Table of contents

* [pg.namespace.discovery](#pgnamespacediscovery)
* [pg.namespace.size](#pgnamespacesizenamespace)

### pg.namespace.discovery

Discover all Namespaces visible to the configured user account.

*Type:* `Discovery`

*Macros:*

  * `{#OID}` Namespace unique identifier
  * `{#NAMESPACE}` Namespace name
  * `{#DATABASE}` Parent database of the namespace
  * `{#OWNER}` Owner name

*Source:*

    SELECT
      n.oid
      , n.nspname
      , current_database()
      , a.rolname
    FROM pg_namespace n
    JOIN pg_authid a ON a.oid = n.nspowner


### pg.namespace.size[,,,,,<<namespace>>]

Returns the size in bytes of a PostgreSQL Namespace

*Type:* `Numeric (unsigned)`

*Parameters:*

  * `namespace` The name or OID of the desired Namespace (required)

*Units:* Bytes

*Source:*

    SELECT
      sum(pg_relation_size(quote_ident(schemaname) || '.' || quote_ident(tablename)))::bigint
    FROM pg_tables
    WHERE schemaname = $1