#!/bin/bash
cd /tmp
cp -rvf /root/libzbxpgsql/* /tmp

# get package version from configure.ac 
VERSION=$(grep AC_INIT configure.ac | grep -Eo '\[[0-9]+\.[0-9]+\.[0-9]+\]' | grep -Eo '[0-9.]+')
ARCH=$(uname -m)

./autogen.sh || exit 1
make dist || exit 1
cp -vf libzbxpgsql-${VERSION}.tar.gz /usr/src/redhat/SOURCES/ || exit 1
cp packaging/rpmbuild/libzbxpgsql.spec /usr/src/redhat/SPECS/ || exit 1
rpmbuild -ba /usr/src/redhat/SPECS/libzbxpgsql.spec || exit 1
cp -vf /usr/src/redhat/RPMS/x86_64/libzbxpgsql-*.rpm /root/libzbxpgsql/ || exit 1
