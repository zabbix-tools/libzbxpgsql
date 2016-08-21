---
layout: page
title: Module item keys
permalink: /documentation/reference/
---

This document describes all of the Zabbix agent item keys that are made
available for querying by `libzbxpgsql`. These keys may be queried using
`zabbix_get` or by adding Items, Discovery Rules and Item Prototypes to
Templates and/or Hosts in Zabbix.

Agent item keys are broken up into the following categories:

<ul>
{% for category_hash in site.data.keys.categories %}
{% assign category = category_hash[1] %}
  <li><a href="{{ category.link | prepend: site.baseurl }}">{{ category.title }}</a> - {{ category.description }}</li>
{% endfor %}
</ul>

A number of agent item keys map directly to a PostgreSQL
[Statistics Collector view](http://www.postgresql.org/docs/current/static/monitoring-stats.html)
with the documentation copied loosely from the PostgreSQL manual with updates
for context.
