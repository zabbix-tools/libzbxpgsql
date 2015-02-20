Name        : libzbxpgsql
Vendor      : cavaliercoder
Version     : 0.1.1
Release     : 1%{?dist}
Summary     : PostgreSQL monitoring module for Zabbix

Group       : Applications/Internet
License     : GNU GPLv2
URL         : https://github.com/cavaliercoder/libzbxpgsql

Requires    : zabbix-agent >= 2.2.0

# Zabbix sources (Customized)
Source0     : %{name}-%{version}.tar.gz

Buildroot   : %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%description
libzbxpgsql is a comprehensive PostgreSQL discovery and monitoring module for the Zabbix monitoring agent written in C.

%prep
# Extract and configure sources into $RPM_BUILD_ROOT
%setup0 -q -n %{name}-%{version}

# fix up some lib64 issues
sed -i.orig -e 's|_LIBDIR=/usr/lib|_LIBDIR=%{_libdir}|g' configure

%build
# Configure and compile sources into $RPM_BUILD_ROOT
%configure --enable-dependency-tracking
make %{?_smp_mflags}

%install
# Install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

# Move lib into .../modules/
install -dm 755 $RPM_BUILD_ROOT%{_libdir}/modules
mv $RPM_BUILD_ROOT%{_libdir}/%{name}.so $RPM_BUILD_ROOT%{_libdir}/modules/%{name}.so

# Create agent config file
install -dm 755 $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_agentd.d
echo "LoadModule=libzbxpgsql.so" > $RPM_BUILD_ROOT%{_sysconfdir}/zabbix/zabbix_agentd.d/%{name}.conf

%clean
# Clean out the build root
rm -rf $RPM_BUILD_ROOT

%files
%{_libdir}/modules/libzbxpgsql.so
%{_sysconfdir}/zabbix/zabbix_agentd.d/%{name}.conf

%changelog
* Sat Feb 7 2015 Ryan Armstrong <ryan@cavaliercoder.com> 0.1.0-1
- Initial release
