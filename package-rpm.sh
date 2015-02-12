#!/bin/bash
APP=libzbxpgsql
VERSION=0.1.1

# Create RPM build directories
[[ -d ~/rpmbuild ]] || mkdir -p ~/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS} || exit 1

# Build source tarball
[[ -f Makefile ]] || ./configure || exit 1
make dist || exit 1
cp ${APP}-${VERSION}.tar.gz ~/rpmbuild/SOURCES || exit 1

# Build rpm
rpmbuild -ba ${APP}.spec
