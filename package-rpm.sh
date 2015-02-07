#!/bin/bash
APP=libzbxpgsql
VERSION=0.1.0

# Build source tarball
[[ -f Makefile ]] || ./configure
make dist || exit 1
cp ${APP}-${VERSION}.tar.gz ~/rpmbuild/SOURCES

# Build rpm
rpmbuild -bb ${APP}.spec