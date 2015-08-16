#!/bin/bash
# copy files to /tmp
cd /tmp
cp -rvf /root/${PACKAGE}/* /tmp

# extract and install sources
tar -xf ${PACKAGE}-${VERSION}.tar.gz || exit 1
cp -rv /root/${PACKAGE}/packaging/debian ${PACKAGE}-${VERSION}/
mv ${PACKAGE}-${VERSION}.tar.gz ${PACKAGE}_${VERSION}.orig.tar.gz

# build
cd ${PACKAGE}-${VERSION}
debuild -us -uc
cd ..

# copy deb out of container
cp -vf \
	${PACKAGE}_${VERSION}-1_amd64.deb \
	/root/${PACKAGE}/${PACKAGE}_${VERSION}-1_amd64.deb \
	|| exit 1
