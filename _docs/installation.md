---
layout: page
title: Installation
menu: Installation
permalink: /installation/
---

This document describes how to install and configure the `libzbxpgsql` module
onto a PostgreSQL server with the Zabbix monitoring agent
[already installed](https://www.zabbix.com/documentation/2.4/manual/installation/getting_zabbix).

This module is required to be installed on all agents on which you wish to
utilize the PostgreSQL agent item keys.

To configure the Zabbix server to monitor an agent with the module correctly
installed, see the [pre-built template]({{ site.baseurl }}/template/) or create
you own template using the keys described in [Agent keys]({{ site.baseurl }}/agent-keys).

![Agent tests screenshot]({{ site.baseurl }}/assets/agenttests.png)

## From source

* Download the latest `libzbxpgsql` source tarball

* Extract, configure and compile the sources with:
  
      tar -xzf libzbxpgsql-*.tar.gz
      cd libzbxpgsql-*/
      ./configure && make && sudo make install

  The module will be installed to `/usr/local/lib/libzbxpgsql.so`

* Copy or link the module into your Zabbix agent [LoadModulePath](https://www.zabbix.com/documentation/2.4/manual/appendix/config/zabbix_agentd?s[]=loadmodulepath)
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
package, `zabbix-agent` and install the `libzbxpgsql` module and configuration
in the default directories used by the Zabbix agent packages.

See [Zabbix - Installation from packages](https://www.zabbix.com/documentation/2.4/manual/installation/install_from_packages).

* Download the latest `libzbxpgsql` RPM packages for your distribution

* Install using RPM with: `rpm -i libzbxpgsql-*.rpm`

* The module will be installed to:
 
  `/usr/{lib|lib64}/modules/libzbxpgsql.so` 

  and the agent configuration file will be installed to:

  `/etc/zabbix/zabbix_agentd.d/libzbxpgsql.conf`

* Restart the Zabbix agent with:
  
  `sudo /etc/init.d/zabbix-agent restart`

* You should see a `loaded modules: libzbxpgsql.so` entry in your Zabbix agent
  log file (`/var/log/zabbix/zabbix_agentd.conf`)

* Test for successful installation with `zabbix_agentd -p | grep '^pg\.'`
