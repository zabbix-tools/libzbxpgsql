#!/bin/bash
# copy files to /tmp
cd /tmp
cp -rvf /root/libzbxpgsql/* /tmp

# make tarball
./autogen.sh || exit 1
./configure || exit 1
make dist || exit 1

# copy out of container
cp -vf \
	/tmp/${PACKAGE}-${VERSION}.tar.gz \
	/root/${PACKAGE}/${PACKAGE}-${VERSION}.tar.gz \
	|| exit 1