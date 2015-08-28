#!/bin/bash
dpkg -i /root/${PACKAGE}/${PACKAGE}_${VERSION}-1_amd64.deb || exit 1
zabbix_agentd -t pg.modver | grep --color "libzbxpgsql ${VERSION}" || exit 1
