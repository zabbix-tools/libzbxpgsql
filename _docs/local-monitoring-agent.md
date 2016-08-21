---
layout: page
title: Local monitoring agent
permalink: /documentation/local-monitoring-agent/
---

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

