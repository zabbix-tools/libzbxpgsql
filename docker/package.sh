#!/bin/bash
# get package version info from ../configure.ac
PACKAGE=$(grep AC_INIT ../configure.ac | grep -Eo '\[.*?\]' | head -n 1 | grep -Eo '[a-z_\-]+')
VERSION=$(grep AC_INIT ../configure.ac | grep -Eo '\[[0-9]+\.[0-9]+\.[0-9]+\]' | grep -Eo '[0-9.]+')

BASE=$(pwd)
DOCKER_ARGS="-it --rm -v ${BASE}/../:/root/${PACKAGE} -e PACKAGE=${PACKAGE} -e VERSION=${VERSION}"
BULLET="\033[1;32m==>\033[0m"

# echo -e "${BULLET} Removing existing packages"
# rm -f ../${PACKAGE}*.tar.gz ../${PACKAGE}*.rpm ../${PACKAGE}*.deb

#
# CentOS 7
#
echo -e "${BULLET} Building CentOS 7 RPM and distro package"
cd "${BASE}/centos-7/rpmbuild"
make || exit 1
docker run ${DOCKER_ARGS} ${PACKAGE}/centos-7-rpmbuild /make_dist.sh || exit 1
docker run ${DOCKER_ARGS} ${PACKAGE}/centos-7-rpmbuild || exit 1

echo -e "${BULLET} Testing CentOS 7 RPM with Zabbix 3.0"
cd "${BASE}/centos-7/zabbix-3.0"
make || exit 1
docker run ${DOCKER_ARGS} ${PACKAGE}/centos-7-zabbix-3.0 || exit 1

#
# Ubuntu 14
#
echo -e "${BULLET} Building Ubuntu 14.04 deb package"
cd "${BASE}/ubuntu-14/debuild"
make || exit 1
docker run ${DOCKER_ARGS} ${PACKAGE}/ubuntu-12-debuild || exit 1

echo -e "${BULLET} Testing Ubuntu 14.04 Precise deb with Zabbix 3.0"
cd "${BASE}/ubuntu-14/zabbix-3.0"
make || exit 1
docker run ${DOCKER_ARGS} ${PACKAGE}/ubuntu-14-zabbix-3.0 || exit 1

#
# OpenSUSE 13.2
#
echo -e "${BULLET} Building OpenSUSE 13.2 RPM"
cd "${BASE}/opensuse-13.2/rpmbuild"
make || exit 1
docker run ${DOCKER_ARGS} ${PACKAGE}/opensuse-13.2-rpmbuild || exit 1

echo -e "${BULLET} Testing OpenSUSE 13.2 RPM with Zabbix 3.0"
cd "${BASE}/opensuse-13.2/zabbix-3.0"
make || exit 1
docker run ${DOCKER_ARGS} ${PACKAGE}/opensuse-13.2-zabbix-3.0 || exit 1

#
# Done
#
echo -e "${BULLET} All the win!"
