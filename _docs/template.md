---
layout: page
title: Zabbix Template
menu: Template
permalink: /template/
---

A comprehensive Zabbix template is included to demonstrate the use of the agent
keys made available in this module. It is not recommended to use this template
out-of-the-box in production without first configuring the check intervals,
configuration macros, discovery rule filters, etc. to suit your environment.

Import `template_postgresql_server.xml` (from 
[GitHub](https://raw.githubusercontent.com/cavaliercoder/libzbxpgsql/498670967bbc4e99606d58589aff0c8f9c71d829/template_postgresql_server.xml)
or from the [source tarball]({{ site.baseurl }}/download)
into Zabbix to get started. See the [Zabbix manual](https://www.zabbix.com/documentation/2.4/manual/xml_export_import)
for details on importing templates from XML.

Once applied to a host, the template will discover all available tablespaces,
databases, tables and indexes accessible to the configured user account and
create items and graphs for effective monitoring.

For a typical, PostgreSQL server hosting only Zabbix, this will create around
**6000 item checks and 2500 graphs** if no filtering is applied.

Please note that the template uses [Active checks](https://www.zabbix.com/documentation/2.4/manual/appendix/items/activepassive)
and so a restart of the agent may be required for the new template items to be
made known to the agent.

## Usage

* Import [template_postgresql_server.xml](https://raw.githubusercontent.com/cavaliercoder/libzbxpgsql/master/template_postgresql_server.xml)
  from the source repository. For information on importing Templates, see the [Zabbix manual](https://www.zabbix.com/documentation/2.4/manual/xml_export_import).

* Modify the update intervals of each Discovery Rule, Item Prototype and Item
  to suit your needs. Also, enable and disables items as desired.

* Configure your PostgreSQL connection details on the desired host using the
  provided [macros](#macros).

* Apply the newly imported `Template PostgreSQL Server` template to your
  selected host.

* Add [Discovery Filters](https://www.zabbix.com/documentation/2.4/manual/discovery/low_level_discovery?s[]=filters)
  to the Discovery Rules on the __Host__ so that only the desired databases,
  tables, etc. are created during discovery.

  For example, to monitor only the `zabbix` database, add a Filter to the
  `Discover PostgreSQL Databases` discovery rule with:

  `{#DATABASE}` matches `^zabbix$`

  To monitor only the History and Trends tables for Zabbix, add a Filter to the
  `Discover PostgreSQL Tables` discovery rule with:

  `{#DATABASE}` *matches* `^zabbix$` *and* `{#TABLE}` *matches* `^history.*|^trend.*`

  ![Discovery rule filter screenshot]({{ site.baseurl }}/assets/filters.png)

* Restart the Zabbix Agent (or wait for [RefreshActiveChecks](https://www.zabbix.com/documentation/2.4/manual/appendix/config/zabbix_agentd?s[]=RefreshActiveChecks)
  to expire) so the agent becomes aware of the new configuration.

* Explore *Latest Data* and *Graphs* to make use of the captured data.
  
  ![Latest data screenshot]({{ site.baseurl }}/assets/latestdata.png)


### Macros

The template includes configurable macros which should be configured on all
hosts to which the template is applied. These macros are substituted into the
connection parameters for all PostgreSQL item keys in the template.

* `{$PG_CONN}` - libpq compatible connection string
* `{$PG_DB}`   - database to connect to (appended to the connection
                 string as per [Connecting to PostgreSQL]({{ site.baseurl }}/agent-keys/#connecting-to-postgresql)

