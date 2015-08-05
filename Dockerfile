FROM centos:7

MAINTAINER Ryan Armstrong <ryan@cavaliercoder.com>

# docker run -d --name pg94 -p 5494:5432 postgres:9.4
# docker run -d --name pg90 -p 5490:5432 postgres:9.0
# docker run -d --name pg84 -p 5484:5432 postgres:8.4

RUN yum clean all && yum makecache && yum update --nogpgcheck -y

RUN rpm -i http://repo.zabbix.com/zabbix/2.4/rhel/7/x86_64/zabbix-release-2.4-1.el7.noarch.rpm

RUN yum install -y --nogpgcheck \
  zabbix-agent \
  postgresql-libs

RUN mkdir /usr/lib64/modules
COPY src/.libs/libzbxpgsql.so /usr/lib64/modules/libzbxpgsql.so
RUN echo "LoadModulePath=/usr/lib64/modules" >> /etc/zabbix/zabbix_agentd.conf 
RUN echo "LoadModule=libzbxpgsql.so" >> /etc/zabbix/zabbix_agentd.conf 

EXPOSE 10050

ENTRYPOINT /usr/sbin/zabbix_agentd -c /etc/zabbix/zabbix_agentd.conf 
