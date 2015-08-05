FROM centos:7

MAINTAINER Ryan Armstrong <ryan@cavaliercoder.com>

RUN yum clean all && yum makecache && yum update --nogpgcheck -y

RUN rpm -i http://repo.zabbix.com/zabbix/2.4/rhel/7/x86_64/zabbix-release-2.4-1.el7.noarch.rpm

RUN yum install -y --nogpgcheck epel-release

RUN yum install -y --nogpgcheck \
  zabbix-agent \
  postgresql-libs \
  monit

RUN mkdir /usr/lib64/modules
# COPY ../src/.libs/libzbxpgsql.so /usr/lib64/modules/libzbxpgsql.so
RUN echo "LoadModulePath=/usr/lib64/modules" >> /etc/zabbix/zabbix_agentd.conf 
RUN echo "LoadModule=libzbxpgsql.so" >> /etc/zabbix/zabbix_agentd.conf 

COPY monitrc /root/monitrc
RUN chmod 600 /root/monitrc

EXPOSE 10050

CMD /bin/monit -c /root/monitrc
