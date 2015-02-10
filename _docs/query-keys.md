---
layout: page
title: Query keys
permalink: /agent-keys/queries/
---

The keys described in the document allow for custom SQL queries to be executed
with the results displayed in Zabbix. Only *scalar* queries are valid; i.e.
queries that return a single row with a single column of the matching data
type.

If multiple rows or columns are returned, only the first column of the first
row is evaluated.

If no rows are returned, `ZBX_NOT_SUPPORTED` is returned by the agent.

## Table of contents

* [pg.query.string](#pgquerystringquery)
* [pg.query.integer](#pgqueryintegerquery)
* [pg.query.double](#pgquerydoublequery)


### pg.query.string[,,,,,<<query>>]

Execute a custom SQL query and return the value of the first column of the
first row as a string of text.

*Type:* `Text`

*Parameters:*

  * `query` The scalar SQL query to execute

*Example:*

    pg.query.string[pg-server,5432,postgres,postgres,P4ssw0rd,SELECT 'Lorem ipsem dolor';]


### pg.query.integer[,,,,,<<query>>]

Execute a custom SQL query and return the value of the first column of the
first row as an unsigned integer.

*Type:* `Numeric (unsigned)`

*Parameters:*

  * `query` The scalar SQL query to execute

*Example:*

    pg.query.string[pg-server,5432,postgres,postgres,P4ssw0rd,SELECT 1234;]


### pg.query.double[,,,,,<<query>>]

Execute a custom SQL query and return the value of the first column of the
first row as a double precision floating point integer.

*Type:* `Numeric (float)`

*Parameters:*

  * `query` The scalar SQL query to execute

*Example:*

    pg.query.string[pg-server,5432,postgres,postgres,P4ssw0rd,SELECT 12.34;]

