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
  `postmaster` server and its processes
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

A number of agent item keys map directly to a PostgreSQL
[Statistics Collector view](http://www.postgresql.org/docs/9.4/static/monitoring-stats.html)
with the documentation copied loosely from the PostgreSQL manual with updates
for context.


## Connecting to PostgreSQL

The first two parameters of all agent keys in `libzbxpgsql` specify the desired
connection string and target database as follows:

1. Connection string     (default: "")
2. Connection database   (default: same as connection string user name)

E.g. `pg.connect[<connection_string>,<database>]`

Any parameters specific to an item key will start from parameter 3, following
the common connection parameters.

E.g. `pg.setting[<connection_string>,<database>,<setting>]`

The connection string must be a valid libpq [keyword/value connection string](http://www.postgresql.org/docs/9.4/static/libpq-connect.html#LIBPQ-PARAMKEYWORDS)
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

### Monitoring role

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

### Local monitoring agent

If the Zabbix agent is running on the PostgreSQL server with `libzbxpgsql`
configured, you can take advantage of Unix sockets connections and `ident`
(or `peer` in v9.1+) authentication which identifies the Zabbix agent using the
identity of the process as reported by the operating system

If the name of the monitoring role you created does not match the identity of
the Zabbix agent running on the PostgreSQL server, you can use a mapping to
allow the agent to impersonate the monitoring role.

First, grant the monitoring role access to connect locally by adding the
following to the __top__ of your `pg_hba.conf`:

    # TYPE  DATABASE        USER            ADDRESS                 METHOD
    local   all             monitoring                              ident map=monitoring

Allow the Zabbix agent identity (typically `zabbix` or `root`) to connect as
the monitoring role by adding the following to your `pg_indent.conf`:

    # MAPNAME       SYSTEM-USERNAME         PG-USERNAME
    monitoring      zabbix                  monitoring

Configure Zabbix to use the `monitoring` role by specifying the `user` keyword
in the first parameter of your item keys. E.g.

    pg.connect[user=monitoring,postgres]

### Remote monitoring agent

The module can also monitor remote PostgreSQL servers without the need to
install the agent on those servers directly. This is useful for restrictive
vendor appliances or if using the module loaded directly from the Zabbix
server.

A number of [authentication methods](http://www.postgresql.org/docs/9.4/static/auth-methods.html)
are a available. The simplest to configure is `md5` password authentication.

First, allow the monitoring role to connect by adding the following __before__
any `host` type access rules:

    # TYPE  DATABASE        USER            ADDRESS                 METHOD
    host    all             monitoring      10.0.0.0/8              md5

Be sure to replace `10.0.0.0/8` with your agent's IP address or range. You can
also limit which databases may be monitored by changing the seconds column from
`all` to a comma separated list of valid databases.

All Zabbix agent traffic is transmitted unencrypted (although optional in v3+)
so it recommended you do __NOT__ transmit connection passwords as parameters.
Instead, use a [.pgpass](http://www.postgresql.org/docs/9.4/static/libpq-pgpass.html)
file, located in the home directory of the Zabbix agent process (E.g.
`/var/lib/zabbix/.pgpass`). To store the password for the `monitoring` role,
appending the following to the password file:

    # hostname:port:database:username:password
    *:*:*:monitoring:Password123

*Note:* The password file should have an access mask of 0600 and can be set with

    chmod 0600 .pgpass

## Connection pooling

Monitoring your PostgreSQL server will consume additional backend connections.
While these connections are shortlived, and typically sparse (with each item
check spread over the configured check intervals), it is good practice to
implement connection pooling. This will minimize memory and semaphore
identifiers consumed by connections for the monitoring agent.

Rather than duplicate the efforts of more specialized projects and complicating
item key configuration, connection pooling has not been built directly into
`libzbxpgsql`. Instead we recommend:

 * [PgBouncer](https://pgbouncer.github.io/) or
 * [Pgpool](http://www.pgpool.net/)

Please see the 
[PostgreSQL wiki](https://wiki.postgresql.org/wiki/Replication,_Clustering,_and_Connection_Pooling#Connection_Pooling_and_Acceleration)
for details on pooling.

*NOTE:* If availability monitoring is your primary concern, connection pooling
introduces the potential for false alerts if the pooling daemon fails, while
your PostgreSQL services may still be available. You can mitigate this issue by
using a separate connection string for your availability monitoring keys (e.g.
`pg.connect`) which connects directly to PostgreSQL, bypassing the connection
pooling daemon.

## Testing

To test all available items and see their default values, run the following on
a [correctly configured]({{ site.baseurl }}/installation/) Zabbix agent:

    zabbix_agentd -p | grep '^pg\.'

For more granular testing, key files are provided for
[zabbix_agent_bench](https://github.com/cavaliercoder/zabbix_agent_bench) in
the `fixtures/` subdirectory of the `libzbxpgsql` sources.
