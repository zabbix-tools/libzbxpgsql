#!/bin/bash
zypper \
	--non-interactive \
	--no-gpg-checks \
	install \
	/root/${PACKAGE}/${PACKAGE}-${VERSION}-1.sles13.$(uname -m).rpm \
	|| exit 1

zabbix_agentd -t pg.modver | grep --color "libzbxpgsql ${VERSION}" || exit 1
