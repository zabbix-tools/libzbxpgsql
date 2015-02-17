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

{% include keys.html keys=site.data.keys.query %}
