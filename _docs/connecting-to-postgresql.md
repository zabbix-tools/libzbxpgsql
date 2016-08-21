---
layout: page
title: Connecting to PostgreSQL
permalink: /documentation/connecting-to-postgresql
---

The first two parameters of all agent keys in `libzbxpgsql` specify the desired
connection string and target database as follows:

1. Connection string     (default: "")
2. Connection database   (default: same as connection string user name)

E.g. `pg.connect[<connection_string>,<database>]`

Any parameters specific to an item key will start from parameter 3, following
the common connection parameters.

E.g. `pg.setting[<connection_string>,<database>,<setting>]`

The connection string must be a valid libpq [keyword/value connection string](http://www.postgresql.org/docs/current/static/libpq-connect.html#LIBPQ-PARAMKEYWORDS)
(connection URIs are not supported). The `dbname` parameter should be omitted
and instead specified as the second parameter if the desired database name
differs from the connection user name which PostgreSQL uses by default.

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


## Securing the connection

__WARNING:__ Do not use `superadmin` or highly privileged accounts for
monitoring.

Consider that any user on your Zabbix server can use `pg.query.*` keys to
execute any query they wish on a monitored PostgreSQL server (such as 
`DROP DATABASE`) if the configured connection role is not suitably restricted.

Configuing a role and HBA rules for secure operation is simple, and detailed
in the following sections.


## Monitoring role

The agent module will require read permissions on the PostgreSQL server
performance tables (i.e. `pg_stat*`) and any databases, tables, etc. you wish
to monitor. A discrete role should be created for monitoring with limited
privileges and should not have write access to any PostgreSQL resources.

To create a role named `monitoring` with the required privileges, execute the
following:

{% highlight sql %}
CREATE ROLE monitoring WITH LOGIN NOSUPERUSER NOCREATEDB NOCREATEROLE;

-- and for each <dbname> you wish to monitor:
GRANT CONNECT ON DATABASE <dbname> TO monitoring;

{% endhighlight %}

You can then connect as the `monitoring` account by passing it in your
connection string item parameters. For example:

    pg.connect[host=10.1.2.3 user=monitoring,postgres]

With the privileges granted above, all module keys should work except
`pg.tablespace.size[,,pg_global]` and potentially any queries you custom define
in the `pg.query.*` keys that may require additional privileges such as
`SELECT`.