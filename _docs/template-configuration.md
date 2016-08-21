---
layout: page
title: Template configuration
permalink: /documentation/template-configuration/
---

This guide will step you through configuring your PostgreSQL monitoring template
to connect to a PostgreSQL server via the Zabbix agent. It assumes that the
agent modules is correctly installed and the monitoring template has been
imported to Zabbix.

* [Apply the template to a host](#apply-the-template-to-a-host)
* [Configure monitoring items](#configure-monitoring-items)
* [Configure connection macros](#configure-connection-macros)

## Apply the template to a host

__Tip:__ It is recommended to disable the monitored host, before applying the
template. This way, you can configure your PostgreSQL connection details and
monitoring thresholds before reenabling the host.

* Select the desired PostgreSQL server under `Configuration` > `Hosts`

* Navigate to the `Templates` tab

* In `Link new templates`, select the `Template App PostgreSQL` template

* Click `Add` and then `Update`

## Configure monitoring items

Navigate in the Zabbix console to the configuration of your host.

* Modify the polling intervals of each Item, Discovery Rule, Item Prototype to
  suit your needs. You may also enable or disable any Items or Triggers as
  desired.

* Add [Discovery Filters](https://www.zabbix.com/documentation/3.0/manual/discovery/low_level_discovery?s[]=filters)
  to the Discovery Rules on the __Host__ so that only the desired databases,
  tables, etc. are created during discovery.

  For example, to monitor only the `zabbix` database, add a Filter to the
  `Discover PostgreSQL Databases` discovery rule with:

  `{#DATABASE}` matches `^zabbix$`

  To monitor only the History and Trends tables for Zabbix, add a Filter to the
  `Discover PostgreSQL Tables` discovery rule with:

  `{#DATABASE}` *matches* `^zabbix$` *and* `{#TABLE}` *matches* `^history.*|^trend.*`

  ![Discovery rule filter screenshot]({{ site.baseurl }}/assets/filters.png)

## Configure connection macros

Your PostgreSQL connection details are supplied to the template using User
Macros that you add the Host.

To specify connection parameters, navigate in the Zabbix console to the
configuration of your Host and select the `Macros` tab.

Add the following macros:

* `{$PG_CONN}` - libpq compatible connection string. This includes the hostname,
  port, username, etc. required to connect to PostgreSQL. For syntax details,
  see the [PostgreSQL documentation](https://www.postgresql.org/docs/current/static/libpq-connect.html#LIBPQ-CONNSTRING).

  E.g.

  ```plain
  {$PG_CONN} => host=pgdb.local port=5432 user=zabbix connect_timeout=10
  ```

  __Please note__ that you must not include the `dbname=` directive in your
  connection string. This is instead defined in the next macro, to enable
  discovery of PostgreSQL resources across multiple databases.

  __Please also note__ that you should not supply a password in your connection
  string macro. You should instead use `.pgpass` file which is described in a
  later section (TODO).

* `{$PG_DB}`   - database to connect to. Behind the scenes, this is appended to
  your connection string on your behalf.

  E.g.

  ```plain
  {$PG_DB} => postgres
  ```

