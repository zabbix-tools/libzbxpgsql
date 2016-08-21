---
layout: page
title: Remote monitoring agent
permalink: /documentation/remote-monitoring-agent/
---

The module can also monitor remote PostgreSQL servers without the need to
install the agent on those servers directly. This is useful for restrictive
vendor appliances or if using the module loaded directly from the Zabbix
server.

A number of [authentication methods](http://www.postgresql.org/docs/current/static/auth-methods.html)
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
Instead, use a [.pgpass](http://www.postgresql.org/docs/current/static/libpq-pgpass.html)
file, located in the home directory of the Zabbix agent process (E.g.
`/var/lib/zabbix/.pgpass`). To store the password for the `monitoring` role,
appending the following to the password file:

    # hostname:port:database:username:password
    *:*:*:monitoring:Password123

*Note:* The password file should have an access mask of 0600 and can be set with

    chmod 0600 .pgpass

