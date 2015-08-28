#!/bin/bash
rpm -ivh /root/libzbxpgsql/libzbxpgsql-${VERSION}-1.el7.x86_64.rpm || exit 1
zabbix_agentd -t pg.modver | grep --color "libzbxpgsql ${VERSION}" || exit 1
