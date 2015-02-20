---
layout: page
title: Agent item keys
menu: Keys
permalink: /agent-keys/
---

This document describes all of the Zabbix agent item keys that are made
available for querying by `libzbxpgsql`.

These keys may be queried using `zabbix_get` or by adding Items, Discovery
Rules and Item Prototypes to Templates and/or Hosts in Zabbix.

Agent item keys are broken up into the following categories:

* [Server item keys]({{ site.baseurl }}/agent-keys/server/) - Monitor the
  server and its processes
* [Query item keys]({{ site.baseurl }}/agent-keys/queries/) - Execute custom
  queries
* [Tablespace item keys]({{ site.baseurl }}/agent-keys/tablespaces/) - Discover and
  monitor PostgreSQL Tablespaces
* [Database item keys]({{ site.baseurl }}/agent-keys/databases/) - Discover and
  monitor PostgreSQL Databases
* [Namespace item keys]({{ site.baseurl }}/agent-keys/namespaces/) - Discover and
  monitor PostgreSQL Namespaces (Schema)
* [Table item keys]({{ site.baseurl }}/agent-keys/tables/) - Discover and
  monitor PostgreSQL Tables
* [Index item keys]({{ site.baseurl }}/agent-keys/indexes/) - Discover and
  monitor PostgreSQL Table indexes

Most agent item keys are mapped directly to a PostgreSQL
[Statistics Collector view](http://www.postgresql.org/docs/9.4/static/monitoring-stats.html)
with the documentation copied loosely from the PostgreSQL manual with updates
for context.


## Connecting to PostgreSQL

All agent keys in `libzbxpgsql` share common connection parameters for the
first five parameters. These parameters are as follows:

1. Hostname     (default: localhost)
2. Port         (default: 5432)
3. Database     (default: postgres)
4. Username     (default: postgres)
5. Password     (default: blank)

E.g. `pg.connect[<host>,<port>,<database>,<username>,<password>]`

Any parameters specific to an item key will start from parameter 6.


## Security

The agent module will require read permissions to the PostgreSQL server
performance tables (i.e. `pg_stat*`) and any databases, tables, etc. you wish
to monitor. An account should be configured for monitoring and should not have
write access to any PostgreSQL assets.

All Zabbix agent traffic is transmitted unencrypted so it recommended you do
__NOT__ transmit connection passwords as parameters. 

Instead, use a [.pgpass](http://www.postgresql.org/docs/9.4/static/libpq-pgpass.html)
file or allow trusted connections for the PostgreSQL monitoring account you
created. Ideally the trusted connection should only be allowed for the agent IP
address and is configurable in your [pg_hba.conf](http://www.postgresql.org/docs/9.4/static/auth-pg-hba-conf.html)
file.

If you *must* use a password for connection authentication, also consider who
will have visibility of the password in Zabbix, as it will appear everywhere
the item key appears, such as trigger and event logs, host configuration, etc.


## Testing

To test all available items and see their default values, run the following on
a [correctly configured]({{ site.baseurl }}/installation/) Zabbix agent:

    zabbix_agentd -p | grep '^pg\.'
