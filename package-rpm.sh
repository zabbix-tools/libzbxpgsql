#!/bin/bash
APP=libzbxpgsql
VERSION=0.1.1

RPMBASE=~/rpmbuild

# Check for rpmbuild
which rpmbuild > /dev/null 2>&1
if [[ $? -ne "0" ]]; then
	echo "rpmbuild not found"
	exit 1
fi

# Create RPM build directories
[[ -d ~/rpmbuild ]] || mkdir -p ${RPMBASE}/{BUILD,RPMS,SOURCES,SPECS,SRPMS} || exit 1

# Build source tarball
[[ -f Makefile.in ]] || ./autogen.sh || exit 1
[[ -f Makefile ]] || ./configure || exit 1
make dist || exit 1
cp ${APP}-${VERSION}.tar.gz ${RPMBASE}/SOURCES || exit 1

# Build rpm
rpmbuild -ba ${APP}.spec

# Copy to host
[[ -d /vagrant ]] && for rpm in $(find ${RPMBASE} -name '*.rpm' | grep -v debuginfo); do cp ${rpm} /vagrant/; done
