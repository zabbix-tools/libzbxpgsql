---
layout: page
title: Known issues
permalink: /documentation/known-issues/
---

### Zabbix v3 compatibility

The release of Zabbix v3 broke compatibility for modules compiled against the
Zabbix v2 sources. Likewise, modules compiled against v3 break when loaded in
a v2 agent.

The issue results in no data being returned by the Zabbix agent when queried
for a module item. Alternatively, the following error may be seen:

    Check access restrictions in Zabbix agent configuration

Zabbix are working to resolve the issue in a later release of Zabbix v3. You
can track their progress in
[ZBX-10428](https://support.zabbix.com/browse/ZBX-10428).

Further comment is welcome on the
[GitHub issue](https://github.com/cavaliercoder/libzbxpgsql/issues/34).


### Segmentation fault on Zabbix v2.4

There is a known bug in the Zabbix agent v2.4.2 and v2.4.3 
([KZBX-9218](https://support.zabbix.com/browse/ZBX-9218)) that causes a
segmentation fault if any item in a loaded module becomes unsupported. The
following line is produced immediately following an unsupported item in the
zabbix agent log file:

    Got signal [signal:11(SIGSEGV),reason:1,refaddr:(nil)]. Crashing ...

To fix this issue, simply upgrade your Zabbix agent to v2.4.4 or higher.

Further comment is welcome on the
[GitHub issue](https://github.com/cavaliercoder/libzbxpgsql/issues/5).

### Third party Zabbix Agent packages

The packages published on this site are packaged to work with the Zabbix Agent
packages provided by Zabbix SIA at [http://repo.zabbix.com/](http://repo.zabbix.com/).

Unfortanately, packages provided by third parties (such as EPEL) may use
different package names (E.g. `zabbix32`) and file paths for the installed
agent.

If you prefer to use a third party package, please consider compiling and
installing `libzbxpgsql` from source instead. Instructions are provided on the
[libzbxpgsql documentation]({{ site.baseurl}}/documentation/module-installation/#install-from-source).

Further comment is welcome on the
[GitHub issue](https://github.com/cavaliercoder/libzbxpgsql/issues/18).