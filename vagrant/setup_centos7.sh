#!/bin/bash
BULLET="<><><>"

ZBX_MAJ=3
ZBX_MIN=0
ZBX_PATCH=1
ZBX_REL=1
ZBX_VER="${ZBX_MAJ}.${ZBX_MIN}.${ZBX_PATCH}"

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
    http://localhost:9000/phpPgAdmin (postgres:postgres)

Manage Zabbix server from:
    http://localhost:9000/zabbix (Admin:zabbix)

MOTD

# Fix issue where wrong package versions are cached in vagrant-cachier
if [[ ! -f "/var/cache/yum/x86_64/7/zabbix/packages/zabbix-server-${ZBX_VER}-${ZBX_REL}.el7.x86_64.rpm" ]]; then
    echo -e "${BULLET} Clearing invalid package cache..."
    rm -rvf /var/cache/yum/x86_64/7/zabbix
    rm -rvf /var/cache/yum/x86_64/7/zabbix-non-supported
fi

# Install packages
echo -e "${BULLET} Installing Zabbix, PostgreSQL and build tools..."
rpm -qa | grep pgdg >/dev/null || yum localinstall -y --nogpgcheck \
    https://download.postgresql.org/pub/repos/yum/9.5/redhat/rhel-7-x86_64/pgdg-centos95-9.5-2.noarch.rpm

rpm -q zabbix-release >/dev/null || yum localinstall -y --nogpgcheck \
    http://repo.zabbix.com/zabbix/${ZBX_MAJ}.${ZBX_MIN}/rhel/7/x86_64/zabbix-release-${ZBX_MAJ}.${ZBX_MIN}-1.el7.noarch.rpm

yum install -y --nogpgcheck \
    augeas \
    autoconf \
    automake \
    gcc \
    git \
    libtool \
    make \
    phpPgAdmin \
    postgresql-devel \
    postgresql95-devel \
    postgresql95-server \
    rpm-build \
    vim-enhanced \
    zabbix-agent-${ZBX_VER} \
    zabbix-get-${ZBX_VER} \
    zabbix-server-pgsql-${ZBX_VER} \
    zabbix-web-pgsql-${ZBX_VER}

rpm -q zabbix_agent_bench >/dev/null || yum localinstall -y --nogpgcheck \
    http://sourceforge.net/projects/zabbixagentbench/files/rpm/zabbix_agent_bench-0.4.0-1.x86_64.rpm

# Configure Zabbix home directory
if [[ ! -d /var/lib/zabbix ]]; then
    echo -e "${BULLET} Configuring Zabbix home directory..."
    mkdir \
        --mode=0700 \
        --parents \
        --context=unconfined_u:object_r:user_home_dir_t:s0 \
        --verbose \
        /var/lib/zabbix 
    chown -v zabbix:zabbix /var/lib/zabbix
fi

cat > /var/lib/zabbix/.pgpass <<EOL
# see: http://www.postgresql.org/docs/9.5/static/libpq-pgpass.html
# hostname:port:database:username:password
*:*:*:monitoring:monitoring

EOL
chmod -v 0600 /var/lib/zabbix/.pgpass
chown -v zabbix:zabbix /var/lib/zabbix/.pgpass

# Configure PostgreSQL
echo -e "${BULLET} Configuring PostgreSQL server..."
export PATH=$PATH:/usr/pgsql-9.5/bin
postgresql95-setup initdb
cat >/var/lib/pgsql/9.5/data/pg_hba.conf <<EOL
# TYPE  DATABASE        USER            ADDRESS                 METHOD
local   all             monitoring                              peer map=monitoring
local   all             all                                     peer
host    all             all             127.0.0.1/32            md5
host    all             all             ::1/128                 md5

EOL
cat >/var/lib/pgsql/9.5/data/pg_ident.conf <<EOL
# MAPNAME       SYSTEM-USERNAME         PG-USERNAME
monitoring      zabbix                  monitoring

EOL

echo -e "${BULLET} Starting PostgreSQL server..."
systemctl enable postgresql-9.5
systemctl start postgresql-9.5

# set password and monitoring account
echo -e "${BULLET} Configuring PostgreSQL server roles..."
sudo -u postgres psql \
    -c "ALTER USER postgres WITH PASSWORD 'postgres';"

sudo -u postgres psql \
    -c "CREATE ROLE \"monitoring\" WITH LOGIN NOSUPERUSER NOCREATEDB NOCREATEROLE PASSWORD 'monitoring';"


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

#
# Create Zabbix database
#
echo -e "${BULLET} Installing Zabbix database..."
dbname="zabbix"
dbuser="zabbix"
dbpasswd="zabbix"
dbschema="public"
pgscripts=/usr/share/doc/zabbix-server-pgsql-${ZBX_MAJ}.${ZBX_MIN}.${ZBX_PATCH}

sudo -u postgres psql -At -c "SELECT table_name FROM information_schema.tables WHERE table_name='hosts' AND table_schema='${dbschema}'" | grep '^hosts$' > /dev/null
if [[ $? ]]; then
    sudo -u postgres psql -c "CREATE ROLE \"${dbuser}\" WITH LOGIN PASSWORD '${dbpasswd}';"
    sudo -u postgres psql -c "CREATE DATABASE \"${dbname}\" WITH OWNER \"${dbuser}\" TEMPLATE \"template1\";"

    if [[ -d "${pgscripts}/create" ]]; then
        sudo -u zabbix psql -d $dbname -f $pgscripts/schema.sql >/root/db.schema.log
        sudo -u zabbix psql -d $dbname -f $pgscripts/images.sql >/root/db.images.log
        sudo -u zabbix psql -d $dbname -f $pgscripts/data.sql >/root/db.data.log
    fi

    if [[ -f "${pgscripts}/create.sql.gz" ]]; then
        zcat $pgscripts/create.sql.gz | sudo -u zabbix psql -d $dbname >/root/db.create.log
    fi

    sudo -u postgres psql -c "GRANT CONNECT ON DATABASE \"${dbname}\" TO monitoring;"
fi

#
# Configure Zabbix server
#
echo -e "${BULLET} Configuring Zabbix server..."
augtool -s set /files/etc/zabbix/zabbix_server.conf/DBName ${dbname}
augtool -s set /files/etc/zabbix/zabbix_server.conf/DBUser ${dbuser}
echo "DBPassword=${dbpasswd}" >> /etc/zabbix/zabbix_server.conf

echo -e "${BULLET} Starting Zabbix server..."
systemctl enable zabbix-server
systemctl start zabbix-server

#
# Configure web server
#
echo -e "${BULLET} Configuring Zabbix web console..."
cat > /etc/zabbix/web/zabbix.conf.php <<EOL
<?php
// Zabbix GUI configuration file
global \$DB;

\$DB['TYPE']     = 'POSTGRESQL';
\$DB['SERVER']   = 'localhost';
\$DB['PORT']     = '0';
\$DB['DATABASE'] = '${dbname}';
\$DB['USER']     = '${dbuser}';
\$DB['PASSWORD'] = '${dbpasswd}';

// SCHEMA is relevant only for IBM_DB2 database
\$DB['SCHEMA'] = '';

\$ZBX_SERVER      = 'localhost';
\$ZBX_SERVER_PORT = '10051';
\$ZBX_SERVER_NAME = '';

\$IMAGE_FORMAT_DEFAULT = IMAGE_FORMAT_PNG;
?>

EOL

augtool -s set /files/etc/php.ini/Date/date.timezone UTC

echo -e "${BULLET} Starting Zabbix web console..."
systemctl enable httpd
systemctl start httpd

#
# Install Zabbix sources
#
if [[ ! -f "/vagrant/zabbix-${ZBX_VER}.tar.gz" ]]; then
    echo -e "${BULLET} Installing Zabbix sources..."
    curl -sL -o /vagrant/zabbix-${ZBX_VER}.tar.gz \
        "http://sourceforge.net/projects/zabbix/files/ZABBIX%20Latest%20Stable/${ZBX_VER}/zabbix-${ZBX_VER}.tar.gz"
fi

if [[ ! -d "/usr/src/zabbix-${ZBX_VER}" ]]; then
    tar -xzC /usr/src -f /vagrant/zabbix-${ZBX_VER}.tar.gz
fi

#
# Build module
#
echo -e "${BULLET} Building the libzbxpgsql agent module..."
./autogen.sh >/dev/null&& \
    ./configure --with-zabbix=/usr/src/zabbix-${ZBX_VER} >/dev/null && \
    make >/dev/null

# Install Zabbix Agent module
echo -e "${BULLET} Installing libzbxpgsql..."
echo "LoadModule=libzbxpgsql.so" > /etc/zabbix/zabbix_agentd.d/libzbxpgsql.conf

# Install add link to agent modules for built libzbxpgsql module
[[ -d /usr/lib64/modules ]] || mkdir /usr/lib64/modules
[[ -L /usr/lib64/modules/libzbxpgsql.so ]] || ln -s /vagrant/src/.libs/libzbxpgsql.so /usr/lib64/modules/libzbxpgsql.so

# for v3+
mkdir -p /usr/lib64/zabbix/modules
[[ -L /usr/lib64/zabbix/modules/libzbxpgsql.so ]] || ln -s /vagrant/src/.libs/libzbxpgsql.so /usr/lib64/zabbix/modules/libzbxpgsql.so


#
# Configure Zabbix agent
#
echo -e "${BULLET} Starting Zabbix agent..."
systemctl enable zabbix-agent
systemctl start zabbix-agent

#
# Message done
#
echo -e "${BULLET} All done."
