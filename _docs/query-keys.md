---
layout: page
title: Query keys
permalink: /documentation/reference/queries/
---

The keys described in the document allow for custom SQL queries to be executed
with the results displayed in Zabbix. 

For the `string`, `integer` and `double` keys, only *scalar* queries are valid;
I.E. queries that return a single row with a single column of the matching data
type. If multiple rows or columns are returned, only the first column of the
first row is evaluated. If no rows are returned, `ZBX_NOT_SUPPORTED` is
returned by the agent.

All query key support parametized PostgreSQL queries, where `$1` is the first
key parameter after the query parameter, `$2` is the second, etc.

If your query contains the `,` or `]` characters, the query must be enclosed in
`"` quotes. If you have quotes inside your query, they can be escaped as `\"`.

{% include keys.html keys=site.data.keys.query %}
