---
layout: page
title: Agent item keys
menu: Keys
permalink: /agent-keys/
---

This document describes all of the Zabbix agent item keys that are made
available for querying by `libzbxpgsql`. These keys may be queried using
`zabbix_get` or by adding Items, Discovery Rules and Item Prototypes to
Templates and/or Hosts in Zabbix.

Agent item keys are broken up into the following categories:

* [Server item keys]({{ site.baseurl }}/agent-keys/server/) - Monitor the
  server and its processes
* [Query item keys]({{ site.baseurl }}/agent-keys/queries/) - Execute custom
  queries
* [Tablespace item keys]({{ site.baseurl }}/agent-keys/tablespaces/) - Discover and
  monitor PostgreSQL Tablespaces
* [Database item keys]({{ site.baseurl }}/agent-keys/databases/) - Discover and
  monitor PostgreSQL Databases
* [Schema item keys]({{ site.baseurl }}/agent-keys/schema/) - Discover and
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

The first two parameters of all agent keys in `libzbxpgsql` specify the desired
connection string and target database as follows:

1. Connection string     (default: "")
2. Connection satabase   (default: same as connection string user name)

E.g. `pg.connect[<connection_string>,<database>]`

Any parameters specific to an item key will start from parameter 3, following
the common connection parameters.

E.g. `pg.setting[<connection_string>,<database>,<setting>]`

The connection string must be a valid libpq [keyword/value connection string](http://www.postgresql.org/docs/9.4/static/libpq-connect.html#LIBPQ-PARAMKEYWORDS)
(connection URIs are not supported). The `dbname` parameter should be omitted
and instead specified as the second parameter if it the desired database name
differs from the connection user name.

*Note:* It may seem counter intuitive to specify the connected database
in a separate parameter to the connection string. Behind the scenes,
`libzbxpgsql` appends `dbname=<param2>` to the connection string specified in
the first parameter. There is a reason for this!

PostgreSQL will only expose some information regarding a database (such as the
list of tables and indexes) for the connected database. Other RDBMs might allow
multiple databases to be queried in a single connection (e.g. with a `USE`
statement) but unfortunately PostgreSQL requires a new connection for each
database.

This is typically not an issue, except in the case of Zabbix, where we want to
discover connectable databases and discover assets within those databases.
For discovery prototypes to gather information about a discovered database, the
connection string needs to be manipulated to connect to the desired database.
Within Zabbix there is currently no smarter way to do this in a discovery rule,
so the database connection is specified as a second parameter, which can be
populated by a discovery rule (and then the connection string modified by
`libzbxpgsql`).


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
