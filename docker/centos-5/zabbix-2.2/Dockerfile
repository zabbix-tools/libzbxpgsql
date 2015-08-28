FROM centos:5

RUN yum update -y

RUN rpm -ivh http://repo.zabbix.com/zabbix/2.2/rhel/5/x86_64/zabbix-release-2.2-1.el5.noarch.rpm

RUN yum install -y zabbix-agent postgresql-libs

COPY entrypoint.sh /entrypoint.sh

CMD /entrypoint.sh
