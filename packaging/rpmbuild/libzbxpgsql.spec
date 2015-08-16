Name        : libzbxpgsql
Vendor      : cavaliercoder
Version     : 0.2.0
Release     : 1%{?dist}
Summary     : PostgreSQL monitoring module for Zabbix

Group       : Applications/Internet
License     : GNU GPLv2
URL         : https://github.com/cavaliercoder/libzbxpgsql

# Zabbix sources (Customized)
Source0     : %{name}-%{version}.tar.gz

Buildroot   : %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%if "%{_vendor}" == "redhat" || "%{_vendor}" == "suse"
BuildRequires : libtool
BuildRequires : postgresql-devel
%endif
# BuildRequires don't seem to work with rpmbuild on Ubuntu...

# package dependencies
Requires    : zabbix-agent >= 2.2.0

%if "%{_vendor}" == "redhat"
Requires    : postgresql-libs >= 8.1.23
%endif
%if "%{_vendor}" == "suse"
Requires    : libpq5
%endif

%description
libzbxpgsql is a comprehensive PostgreSQL discovery and monitoring module for the Zabbix monitoring agent written in C.

%prep
# Extract and configure sources into $RPM_BUILD_ROOT
%setup0 -q -n %{name}-%{version}

# fix up some lib64 issues
sed -i.orig -e 's|_LIBDIR=/usr/lib|_LIBDIR=%{_libdir}|g' configure

# make it build on openSUSE too (pgsql/libpq-fe.h)
%if 0%{?suse_version}
sed -i.orig -e 's|libpq-fe.h|pgsql/libpq-fe.h|g' src/libzbxpgsql.h
%endif

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
* Sun Aug 16 2015 Ryan Armstrong <ryan@cavaliercoder.com> 0.2.0-1
- Improved connections parameters on all item keys
- Add custom discovery rules via `pg.query.discovery`
- Fixed compatability issues with < v9.2
- Added support for OpenSUSE v13.2
- Added SQL injection prevention
- Added `pg.uptime` and `pg.starttime` keys
- Added `pg.modver` key to monitor the installed `libzbxpgsql` version
- Reduced required privileges for all keys to just `LOGIN`
- Fixed integer overflow issues on large objects
- Improved automated testing and packaging using Docker and `zabbix_agent_bench`

* Tue Mar 17 2015 Ryan Armstrong <ryan@cavaliercoder.com> 0.1.3-1
- Added configuration directive discovery

* Fri Feb 20 2015 Ryan Armstrong <ryan@cavaliercoder.com> 0.1.2-1
- Fixed module installation path
- Added git reference to library version info
- Added project and RPM build to Travis CI
- Improved detection of PostgreSQL OIDs and IP addresses in parameter values

* Mon Feb 16 2015 Ryan Armstrong <ryan@cavaliercoder.com> 0.1.1-1
- Added `pg.queries.longest` key
- Added `pg.setting` key
- Added `pg.query.*` keys
- Improved documentation

* Sat Feb 7 2015 Ryan Armstrong <ryan@cavaliercoder.com> 0.1.0-1
- Initial release
