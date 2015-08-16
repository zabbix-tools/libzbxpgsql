#!/bin/bash
BULLET="==>"
cd /vagrant

# Install PostgreSQL package repo
echo -e "${BULLET} Installing Zabbix, PostgreSQL and build tools..."
if [[ ! -f /etc/apt/sources.list.d/pgdg.list ]]; then
	echo "deb http://apt.postgresql.org/pub/repos/apt/ precise-pgdg main" > /etc/apt/sources.list.d/pgdg.list
	wget --quiet -O - https://www.postgresql.org/media/keys/ACCC4CF8.asc | apt-key add -
fi

# Install Zabbix package repo
[[ -f zabbix-release_2.2-1+precise_all.deb ]] || wget -nv http://repo.zabbix.com/zabbix/2.2/ubuntu/pool/main/z/zabbix-release/zabbix-release_2.2-1+precise_all.deb
dpkg -l zabbix-release > /dev/null || dpkg -i zabbix-release_2.2-1+precise_all.deb

# Update package list and install
apt-get -q update
apt-get -y install \
	git \
	devscripts \
	debhelper \
	dh-make \
	rpm \
	zabbix-agent \
	make \
	gcc \
	autoconf \
	automake \
	m4 \
	libtool \
	postgresql-9.4 \
	postgresql-client-9.4 \
	libpq-dev

# Configure PostgreSQL
echo -e "${BULLET} Configuring PostgreSQL server..."
cat > /etc/postgresql/9.4/main/pg_hba.conf <<EOL
local   all             all                                     trust
host    all             all             127.0.0.1/32            trust
host    all             all             ::1/128                 trust
EOL
/etc/init.d/postgresql reload

# Build module
echo -e "${BULLET} Building the libzbxpgsql agent module..."
./autogen.sh >/dev/null
./configure >/dev/null && make >/dev/null

# Configure Zabbix Agent
echo -e "${BULLET} Installing libzbxpgsql..."
echo "LoadModule=libzbxpgsql.so" > /etc/zabbix/zabbix_agentd.d/libzbxpgsql.conf

# Install add link to agent modules for built libzbxpgsql module
[[ -d /usr/lib/modules ]] || mkdir /usr/lib/modules
[[ -L /usr/lib/modules/libzbxpgsql.so ]] || ln -s /vagrant/src/.libs/libzbxpgsql.so /usr/lib/modules/libzbxpgsql.so

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