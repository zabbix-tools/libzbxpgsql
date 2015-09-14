FROM centos:5

RUN yum update -y

RUN \
	yum install -y curl && \
	curl -sLO http://repo.zabbix.com/zabbix/2.2/rhel/5/x86_64/zabbix-release-2.2-1.el5.noarch.rpm && \
	rpm -ivh zabbix-release-2.2-1.el5.noarch.rpm

RUN yum install -y zabbix-agent postgresql-libs

COPY entrypoint.sh /entrypoint.sh

CMD /entrypoint.sh
