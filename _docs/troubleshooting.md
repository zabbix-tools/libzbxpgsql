---
layout: page
title: Troubleshooting
permalink: /documentation/troubleshooting/
---

## Testing

To test all available items and see their default values, run the following on
a [correctly configured]({{ site.baseurl }}/documentation/module-installation/) Zabbix agent:

    zabbix_agentd -p | grep '^pg\.'

For more granular testing, key files are provided for
[zabbix_agent_bench](https://github.com/cavaliercoder/zabbix_agent_bench) in
the `fixtures/` subdirectory of the `libzbxpgsql-build`
[sources](https://github.com/cavaliercoder/libzbxpgsql-build/tree/master/fixtures).
