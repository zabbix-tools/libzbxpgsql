---
layout: page
title: Installation
menu: Installation
permalink: /installation/
---

This document describes how to install and configure the `libzbxpgsql` module
onto a PostgreSQL server with the Zabbix monitoring agent version 2.2 or above
[already installed](https://www.zabbix.com/documentation/2.4/manual/installation/getting_zabbix).

The module is installed with the Zabbix agent, typically on the PostgreSQL
server to be monitored (although monitoring remote servers is possible).

Once the agent is configured, you can add PostgreSQL items to your Hosts and
Templates in the Zabbix administrative console, using the `libzbxpgsql`
[Agent item keys]({{ site.baseurl }}/agent-keys). Alternatively, you can make
use of the included [pre-built template]({{ site.baseurl }}/template/).

The following installation methods are available:

* [Compile from source](#from-source) on Linux and comparable operating systems

* [RPM Packages](#rpm-package) for RedHat family operating systems

* [Deb Packages](#deb-packages) for Debian family operating systems

![Agent tests screenshot]({{ site.baseurl }}/assets/agenttests.png)

## From source

* Download and install the [libpq development headers](http://www.postgresql.org/download/)
  (`postgresql-devel` package on RHEL family servers or `libpq-dev` on Debian
  family servers)

* Download the latest `libzbxpgsql` source tarball from
  [SourceForge](https://sourceforge.net/projects/libzbxpgsl/files/sources/)

* Extract, configure and compile the sources with:
  
      tar -xzf libzbxpgsql-*.tar.gz
      cd libzbxpgsql-*/
      ./configure && make && sudo make install

  The module will be installed to `/usr/local/lib/libzbxpgsql.so`

* Copy or link the module into your Zabbix agent
  [LoadModulePath](https://www.zabbix.com/documentation/2.4/manual/appendix/config/zabbix_agentd?s[]=loadmodulepath)
  directory (typically `/usr/lib/modules` or `/usr/lib64/modules`)

      sudo mkdir /usr/lib64/modules
      sudo ln -s /usr/local/lib/libzbxpgsql.so /usr/lib64/modules/libzbxpgsql.so

* Add the module to your `zabbix_agentd.conf` file with:

  `LoadModule=libzbxpgsql.so`

* Restart the Zabbix agent with:
  
  `sudo /etc/init.d/zabbix-agent restart`

* You should see a `loaded modules: libzbxpgsql.so` entry in your Zabbix agent
  log file (`/var/log/zabbix/zabbix_agentd.conf`)

* Test for successful installation with `zabbix_agentd -p | grep '^pg\.'`


## RPM Package

RPM packages are made available for simpler installation on RHEL family systems
such as RedHat, Fedora, CentOS, Scientific Linux, etc.

The packages require the installation of the Zabbix SIA provided agent RPM
package, `zabbix-agent` and installs the `libzbxpgsql` module and configuration
in the default directories used by the Zabbix agent packages.

See [Zabbix - Installation from packages](https://www.zabbix.com/documentation/2.4/manual/installation/install_from_packages).

* Install PostgreSQL client libraries with: `yum install postgresql-libs`

* Download the latest `libzbxpgsql` RPM package for your distribution from
  [SourceForge](https://sourceforge.net/projects/libzbxpgsl/files/rpms/)

* Install using `rpm` with: `rpm -i libzbxpgsql-*.rpm`

* The module will be installed to:
 
  `/usr/{lib|lib64}/modules/libzbxpgsql.so` 

  and the agent configuration file will be installed to:

  `/etc/zabbix/zabbix_agentd.d/libzbxpgsql.conf`

* Restart the Zabbix agent with:
  
  `sudo /etc/init.d/zabbix-agent restart`

* You should see a `loaded modules: libzbxpgsql.so` entry in your Zabbix agent
  log file (`/var/log/zabbix/zabbix_agentd.log`)

* Test for successful installation with `zabbix_agentd -p | grep '^pg\.'`


## Deb Packages

Debian packages are made available for simpler installation on Debian family
systems such as Ubuntu.

The packages require the installation of the Zabbix SIA provided agent Deb
package, `zabbix-agent` and install the `libzbxpgsql` module and configuration
in the default directories used by the Zabbix agent packages.

See [Zabbix - Installation from packages](https://www.zabbix.com/documentation/2.4/manual/installation/install_from_packages).

* Install PostgreSQL client libraries with: `apt-get install libpq5`

* Download the latest `libzbxpgsql` Deb package for your distribution from
  [SourceForge](https://sourceforge.net/projects/libzbxpgsl/files/debs/)

* Install using `dpkg` with: `dpkg -i libzbxpgsql-*.deb`

* The module will be installed to:
 
  `/usr/lib/modules/libzbxpgsql.so` 

  and the agent configuration file will be installed to:

  `/etc/zabbix/zabbix_agentd.d/libzbxpgsql.conf`

* Restart the Zabbix agent with:
  
  `sudo /etc/init.d/zabbix-agent restart`

* You should see a `loaded modules: libzbxpgsql.so` entry in your Zabbix agent
  log file (`/var/log/zabbix/zabbix_agentd.log`)

* Test for successful installation with `zabbix_agentd -p | grep '^pg\.'`
