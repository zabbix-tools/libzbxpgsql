#!/bin/bash
rpm -ivh /root/${PACKAGE}/${PACKAGE}-${VERSION}-1.el5.x86_64.rpm || exit 1
zabbix_agentd -t pg.modver | grep --color "libzbxpgsql ${VERSION}" || exit 1
