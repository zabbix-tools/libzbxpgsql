FROM opensuse:13.2

RUN zypper --non-interactive update

RUN zypper \
	--non-interactive \
	--no-gpg-checks \
	install \
	http://repo.zabbix.com/zabbix/2.2/rhel/7/x86_64/zabbix-2.2.10-1.el7.x86_64.rpm \
	http://repo.zabbix.com/zabbix/2.2/rhel/7/x86_64/zabbix-agent-2.2.10-1.el7.x86_64.rpm \
	libpq5

COPY entrypoint.sh /entrypoint.sh

CMD /entrypoint.sh
