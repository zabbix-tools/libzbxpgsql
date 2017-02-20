---
layout: page
title: Module installation
permalink: /documentation/module-installation/
---

The easiest way to install the agent module, is to use the prebuilt packages
available on the [downloads page]({{ site.baseurl}}/download/).

The `libzbxpgsql` package requires the `zabbix-agent` package provided by
Zabbix SIA at [http://repo.zabbix.com/](http://repo.zabbix.com/).

For all other purposes, you may compile and install `libzbxpgsql` yourself from
source.

## Install from package

* Install the `zabbix-agent` package as per the
  [Zabbix documentation](https://www.zabbix.com/documentation/3.0/manual/installation/install_from_packages)

* Install the PostgreSQL client library package as per the
  [PostgreSQL documentation](https://www.postgresql.org/download/) (typically
  available as `libpq5` or `postgresql-libs`)

* Download the `libzbxpgsql` package for your operating system and Zabbix
  version from the [downloads page]({{ site.baseurl}}/download/)

* Install with your native package manager

  * On Debian family systems: `$ dpkg -i libzbxpgsql-*.deb`

  * On RedHat family systems: `$ rpm -i libzbxpgsql-*.rpm`

* Restart the Zabbix agent


## Install from source

* Download and install the
  [libpq development headers](http://www.postgresql.org/download/) (typically
  `libpq-dev` or `postgresql-devel`)

* Download and extract the Zabbix source tarball from the Zabbix
  [downloads page](http://www.zabbix.com/download.php)

* Download the latest `libzbxpgsql` source tarball from the
  [downloads page]({{ site.baseurl}}/download/)

* Extract, configure and compile the sources with:
  
      $ tar -xzf libzbxpgsql-*.tar.gz
      $ cd libzbxpgsql-*/
      $ ./configure --with-zabbix=/usr/src/zabbix
      $ make && sudo make install

  The module will be installed to `/usr/local/lib/libzbxpgsql.so`

* Copy or link the module into your Zabbix agent
  [LoadModulePath](https://www.zabbix.com/documentation/3.0/manual/appendix/config/zabbix_agentd?s[]=loadmodulepath)
  directory (typically `/var/lib/zabbix/modules`)

      $ sudo mkdir /var/lib/zabbix/modules
      $ sudo ln -s /usr/local/lib/libzbxpgsql.so /var/lib/zabbix/modules/libzbxpgsql.so

* Add the module to your `zabbix_agentd.conf` file with:

  `LoadModule=libzbxpgsql.so`

* Restart the Zabbix agent with

## Validate installation

* If the module is loaded correctly, you will see the following line in your
  Zabbix agent log file (`/var/log/zabbix/zabbix_agentd.log`):

      loaded modules: libzbxpgsql.so

* Check the installed version with:

      $ zabbix_agentd -t pg.modver
      pg.modver          [s|libzbxpgsql 1.0.0, compiled for Zabbix 3.0.2]

![Agent tests screenshot]({{ site.baseurl }}/assets/agenttests.png)
