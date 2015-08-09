#!/bin/bash
# copy files to /tmp
cd /tmp
cp -rvf /root/libzbxpgsql/* /tmp

# get package version from configure.ac 
APP=libzbxpgsql
VERSION=$(grep AC_INIT configure.ac | grep -Eo '\[[0-9]+\.[0-9]+\.[0-9]+\]' | grep -Eo '[0-9.]+')
ARCH=$(uname -m)
RPMROOT=/root/rpmbuild

# make tarball
./autogen.sh || exit 1
make dist || exit 1

# install rpmbuild sources
cp -vf ${APP}-${VERSION}.tar.gz ${RPMROOT}/SOURCES/ || exit 1
cp packaging/rpmbuild/${APP}.spec ${RPMROOT}/SPECS/ || exit 1

# build
rpmbuild -ba ${RPMROOT}/SPECS/${APP}.spec || exit 1

# copy out of container
cp -vf \
	${RPMROOT}/RPMS/${ARCH}/${APP}-${VERSION}-1.el7.centos.${ARCH}.rpm \
	/root/libzbxpgsql/${APP}-${VERSION}-1.el7.${ARCH}.rpm \
	|| exit 1
