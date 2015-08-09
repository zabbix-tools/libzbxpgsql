#!/bin/bash

# tail agent log
touch /var/log/zabbix/zabbix_agentd.log
tail -f /var/log/zabbix/zabbix_agentd.log &

# start daemons
/bin/monit -c /root/monitrc
