#!/bin/bash
BULLET="==>"

ZBX_MAJ=2
ZBX_MIN=2
ZBX_PATCH=10
ZBX_REL=1
ZBX_VER="${ZBX_MAJ}.${ZBX_MIN}.${ZBX_PATCH}-${ZBX_REL}"

cd /vagrant

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

Manage PostgreSQL server from:
    http://localhost:8080/phpPgAdmin (postgres:postgres)

Manage Zabbix server from:
    http://localhost:8080/zabbix (Admin:zabbix)

MOTD

# Install Zabbix, PostgreSQL and build tools
echo -e "${BULLET} Installing Zabbix, PostgreSQL and build tools..."
rpm -qa | grep pgdg >/dev/null || yum localinstall -y --nogpgcheck http://yum.postgresql.org/9.4/redhat/rhel-6-x86_64/pgdg-centos94-9.4-1.noarch.rpm
rpm -q zabbix-release >/dev/null || yum localinstall -y --nogpgcheck http://repo.zabbix.com/zabbix/${ZBX_MAJ}.${ZBX_MIN}/rhel/7/x86_64/zabbix-release-${ZBX_MAJ}.${ZBX_MIN}-1.el7.noarch.rpm
yum install -y --nogpgcheck \
    make \
    gcc \
    libtool \
    automake \
    autoconf \
    rpm-build \
    git \
    postgresql-devel \
    postgresql94-server \
    postgresql94-devel \
    phpPgAdmin \
    zabbix-agent \
    zabbix-get \
    zabbix-server-pgsql \
    zabbix-web-pgsql

rpm -q zabbix_agent_bench >/dev/null || yum localinstall -y --nogpgcheck http://sourceforge.net/projects/zabbixagentbench/files/rpm/zabbix_agent_bench-0.3.0-1.x86_64.rpm

# Configure PostgreSQL
echo -e "${BULLET} Configuring PostgreSQL server..."
/usr/pgsql-9.4/bin/postgresql94-setup initdb
systemctl enable postgresql-9.4
systemctl start postgresql-9.4
export PATH=$PATH:/usr/pgsql-9.4/bin

# Configure phpPgAdmin
echo -e "${BULLET} Configuring phpPgAdmin web console..."
cat > /etc/httpd/conf.d/phpPgAdmin.conf <<EOL
Alias /phpPgAdmin /usr/share/phpPgAdmin
<Location /phpPgAdmin>
    <IfModule mod_authz_core.c>
        # Apache 2.4
        Require all granted
    </IfModule>
    <IfModule !mod_authz_core.c>
        # Apache 2.2
        Order deny,allow
        Allow from all
    </IfModule>
</Location>
EOL

sed -i \
    -e "s/\$conf\['extra_login_security'\] = .*;/\$conf\['extra_login_security'\] = false;/" \
    -e "s/\$conf\['servers'\]\[0\]\['host'\] = .*/\$conf\['servers'\]\[0\]\['host'\] = 'localhost';/" \
    -e "s/\$conf\['owned_only'\] = .*/\$conf\['owned_only'\] = false;/" \
    /etc/phpPgAdmin/config.inc.php
systemctl enable httpd
systemctl start httpd

# Configure Zabbix server
pgscripts=/usr/share/doc/zabbix-server-pgsql-${ZBX_MAJ}.${ZBX_MIN}.${ZBX_PATCH}/create
dbname="zabbix"
dbuser="zabbix"
dbpasswd="zabbix"
dbschema="public"
sudo -u postgres psql -At -c "SELECT table_name FROM information_schema.tables WHERE table_name='hosts' AND table_schema='${dbschema}'" | grep '^hosts$' > /dev/null
if [[ $? ]]; then
    sudo -u postgres psql -c "CREATE ROLE \"${dbuser}\" WITH LOGIN PASSWORD '${dbpasswd}';"
    sudo -u postgres psql -c "CREATE DATABASE \"${dbname}\" WITH OWNER \"${dbuser}\" TEMPLATE \"template1\";"
    sudo -u zabbix psql -d $dbname -f $pgscripts/schema.sql
    sudo -u zabbix psql -d $dbname -f $pgscripts/images.sql
    sudo -u zabbix psql -d $dbname -f $pgscripts/data.sql

    chkconfig zabbix-server on
    service zabbix-server start
fi

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

echo -e "${BULLET} All done."
