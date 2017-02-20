---
layout: page
title: Zabbix template installation
permalink: /documentation/template-installation/
---

This guide will step you through the import of the PostgreSQL monitoring
template.

## Import the template

The latest version of PostgreSQL monitoring template is available on
[GitHub](https://github.com/cavaliercoder/libzbxpgsql/tree/v1.1.0/templates).
It can also be found in the `/templates` directory in the source tarball
available from the [downloads page]({{ site.baseurl }}/download/).

* Save the XML template file to a convenient location

* Open the Zabbix console

* Navigate to `Configuration` > `Templates`

* Click the `Import` button

* Select the downloaded `template_postgresql_server.xml` file

* Tick the `Create new / Screens` checkbox

* Click `Import`
