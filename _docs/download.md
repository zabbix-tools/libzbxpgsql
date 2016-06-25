---
layout: page
title: Download libzbxpgsql
menu: Download
permalink: /download
---

__Thanks for downloading!__

Before using the PostgreSQL Template, you must install the `libzbxpgsql` package
on a host running the Zabbix agent.

Please select your operating system and Zabbix version below to download the
correct package for your platform.

## Next Steps

* Read the [installation docs]({{ site.baseurl }}/installation)
* Install the [PostgreSQL template]({{ site.baseurl}}/template)
* Configure a connection using the [connection guide]({{ site.baseurl }}/agent-keys)
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
