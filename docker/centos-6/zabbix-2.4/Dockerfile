FROM centos:6

RUN yum update -y

RUN rpm -ivh http://repo.zabbix.com/zabbix/2.4/rhel/6/x86_64/zabbix-release-2.4-1.el6.noarch.rpm

RUN yum install -y zabbix-agent postgresql-libs

COPY entrypoint.sh /entrypoint.sh

ENTRYPOINT /entrypoint.sh