---
layout: page
title: Download libzbxpgsql
menu: Download
permalink: /download
---

__Thanks for downloading!__

Before using the PostgreSQL Template, you must install the `libzbxpgsql` package
on a host running the Zabbix agent.

The `libzbxpgsql` package requires the `zabbix-agent` package provided by
Zabbix SIA at [http://repo.zabbix.com/](http://repo.zabbix.com/). If are using
packages provided by a third party (E.g. EPEL) you may run into dependency and
file structure issues. In this case it is recommended instead to compile and
install from source.

Please select your operating system and Zabbix version below to download the
correct package for your platform.

## Next Steps

* Read the [docs]({{ site.baseurl }}/documentation/)
* Raise any issues on [GitHub](https://github.com/cavaliercoder/libzbxpgsql/issues)

## Download packages

<table class="downloads">
  <thead>
    <tr>
      <th>Operating System</th>
      <th>Downloads</th>
    </tr>
  </thead>
  <tbody>
{% for download in site.data.downloads %}
    <tr>
      <td>{{ download.target }}</td>
      <td>
{% for file in download.files %}
        <a href="{{ file.url }}">{{ file.label }}</a>
{% endfor %}
      </td>
    </tr>
{% endfor %}
  </tbody>
</table>
