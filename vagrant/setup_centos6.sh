#!/bin/bash
BULLET="==>"
cd /vagrant

# Install Zabbix, PostgreSQL and build tools
echo -e "${BULLET} Installing Zabbix, PostgreSQL and build tools..."
rpm -qa | grep pgdg >/dev/null || yum localinstall -y --nogpgcheck http://yum.postgresql.org/9.4/redhat/rhel-6-x86_64/pgdg-centos94-9.4-1.noarch.rpm
rpm -qa | grep zabbix-release >/dev/null || yum localinstall -y --nogpgcheck http://repo.zabbix.com/zabbix/2.4/rhel/6/x86_64/zabbix-release-2.4-1.el6.noarch.rpm
yum install -y --nogpgcheck \
    make \
    gcc \
    libtool \
    automake \
    autoconf \
    rpm-build \
    git \
    postgresql94-server \
    postgresql94-devel \
    zabbix-agent

# Configure PostgreSQL
echo -e "${BULLET} Configuring PostgreSQL server..."
/etc/init.d/postgresql-9.4 initdb
cat > /var/lib/pgsql/9.4/data/pg_hba.conf <<EOL
local   all             all                                     trust
host    all             all             127.0.0.1/32            trust
host    all             all             ::1/128                 trust
EOL
chkconfig postgresql-9.4 on
service postgresql-9.4 start

# Build module
echo -e "${BULLET} Building the libzbxpgsql agent module..."
libtoolize >/dev/null
aclocal >/dev/null
autoheader >/dev/null
automake --add-missing >/dev/null
autoreconf >/dev/null
./configure >/dev/null && make >/dev/null

# Install Zabbix Agent module
echo -e "${BULLET} Installing libzbxpgsql..."
echo "LoadModule=libzbxpgsql.so" > /etc/zabbix/zabbix_agentd.d/libzbxpgsql.conf

# Install add link to agent modules for built libzbxpgsql module
[[ -d /usr/lib64/modules ]] || mkdir /usr/lib64/modules
[[ -L /usr/lib64/modules/libzbxpgsql.so ]] || ln -s /vagrant/src/.libs/libzbxpgsql.so /usr/lib64/modules/libzbxpgsql.so

# MOTD
echo -e "${BULLET} Configuring message of the day..."
cat > /etc/motd <<MOTD
  _ _ _        _                               _ 
 | (_) |__ ___| |____  ___ __   __ _ ___  __ _| |
 | | | '_ \\_  / '_ \\ \\/ / '_ \\ / _\` / __|/ _\` | |
 | | | |_) / /| |_) >  <| |_) | (_| \\__ \\ (_| | |
 |_|_|_.__/___|_.__/_/\\_\\ .__/ \\__, |___/\\__, |_|
                        |_|    |___/        |_|  

            DΣVΣᄂӨPMΣПƬ VΛGЯΛПƬ BӨX

Test PostgreSQL module keys with:
    zabbix_agentd -t <key>

Print all key tests with:
    zabbix_agentd -p | grep '^pg\.'

All project sources are mapped in /vagrant

MOTD

echo -e "${BULLET} All done."
