# -*- mode: ruby -*-
# vi: set ft=ruby :
VAGRANTFILE_API_VERSION = "2"

$script = <<script
BULLET="==>"
cd /vagrant

# Install zabbix sources
echo -e "${BULLET} Downloading and extracting Zabbix sources..."
sudo -Eu vagrant curl -LO http://sourceforge.net/projects/zabbix/files/ZABBIX%20Latest%20Stable/2.4.3/zabbix-2.4.3.tar.gz
sudo -Eu vagrant tar -xzf zabbix-2.4.3.tar.gz

# Install Zabbix, PostgreSQL and build tools
echo -e "${BULLET} Installing Zabbix, PostgreSQL and build tools..."
yum localinstall -y --nogpgcheck http://yum.postgresql.org/9.4/redhat/rhel-6-x86_64/pgdg-centos94-9.4-1.noarch.rpm
yum localinstall -y --nogpgcheck http://repo.zabbix.com/zabbix/2.4/rhel/7/x86_64/zabbix-release-2.4-1.el7.noarch.rpm
yum install -y --nogpgcheck make gcc libtool automake autoconf postgresql94-server postgresql94-devel phpPgAdmin zabbix-agent

# Build module
echo -e "${BULLET} Building the libzbxpgsql agent module..."
libtoolize
aclocal
autoheader
automake --add-missing
autoreconf
./configure
make

# Configure PostgreSQL
echo -e "${BULLET} Configuring PostgreSQL server..."
/usr/pgsql-9.4/bin/postgresql94-setup initdb
cat > /var/lib/pgsql/9.4/data/pg_hba.conf <<EOL
local   all             all                                     trust
host    all             all             127.0.0.1/32            trust
host    all             all             ::1/128                 trust
EOL
systemctl enable postgresql-9.4
systemctl start postgresql-9.4

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

# Configure Zabbix Agent
echo -e "${BULLET} Installing libzbxpgsql...'
echo "LoadModule=libzbxpgsql.so" > /etc/zabbix/zabbix_agentd.d/libzbxpgsql.conf

# Install add link to agent modules for built libzbxpgsql module
mkdir /usr/lib64/modules
ln -s /vagrant/src/.libs/libzbxpgsql.so /usr/lib64/modules/libzbxpgsql.so

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

MOTD

echo -e "${BULLET} All done!"
script

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  # Use DOE specific CentOS 7.0 image
  config.vm.box = "chef/centos-7.0"

  # Forward port for phpPgAdmin
  config.vm.network "forwarded_port", guest: 80, host: 8080

  # Pre-configuration
  config.vm.provision "shell", inline: $script
end
