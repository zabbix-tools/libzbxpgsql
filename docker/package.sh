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

echo -e "${BULLET} Testing CentOS 7 RPM with Zabbix 2.2"
cd "${BASE}/centos-7/zabbix-2.2"
make || exit 1
docker run ${DOCKER_ARGS} ${PACKAGE}/centos-7-zabbix-2.2 || exit 1

echo -e "${BULLET} Testing CentOS 7 RPM with Zabbix 2.4"
cd "${BASE}/centos-7/zabbix-2.4"
make || exit 1
docker run ${DOCKER_ARGS} ${PACKAGE}/centos-7-zabbix-2.4 || exit 1

#
# CentOS 6
#
echo -e "${BULLET} Building CentOS 6 RPM"
cd "${BASE}/centos-6/rpmbuild"
make || exit 1
docker run ${DOCKER_ARGS} ${PACKAGE}/centos-6-rpmbuild || exit 1

echo -e "${BULLET} Testing CentOS 6 RPM with Zabbix 2.2"
cd "${BASE}/centos-6/zabbix-2.2"
make || exit 1
docker run ${DOCKER_ARGS} ${PACKAGE}/centos-6-zabbix-2.2 || exit 1

echo -e "${BULLET} Testing CentOS 6 RPM with Zabbix 2.4"
cd "${BASE}/centos-6/zabbix-2.4"
make || exit 1
docker run ${DOCKER_ARGS} ${PACKAGE}/centos-6-zabbix-2.4 || exit 1

#
# CentOS 5
#
echo -e "${BULLET} Building CentOS 5 RPM"
cd "${BASE}/centos-5/rpmbuild"
make || exit 1
docker run ${DOCKER_ARGS} ${PACKAGE}/centos-5-rpmbuild || exit 1

echo -e "${BULLET} Testing CentOS 5 RPM with Zabbix 2.2"
cd "${BASE}/centos-5/zabbix-2.2"
make || exit 1
docker run ${DOCKER_ARGS} ${PACKAGE}/centos-5-zabbix-2.2 || exit 1

#
# Ubuntu 12
#
echo -e "${BULLET} Building Ubuntu 12.04 deb package"
cd "${BASE}/ubuntu-12/debuild"
make || exit 1
docker run ${DOCKER_ARGS} ${PACKAGE}/ubuntu-12-debuild || exit 1

echo -e "${BULLET} Testing Ubuntu 12.04 Precise deb with Zabbix 2.2"
cd "${BASE}/ubuntu-12/zabbix-2.2"
make || exit 1
docker run ${DOCKER_ARGS} ${PACKAGE}/ubuntu-12-zabbix-2.2 || exit 1

echo -e "${BULLET} Testing Ubuntu 14.04 Trusty deb with Zabbix 2.2"
cd "${BASE}/ubuntu-14/zabbix-2.2"
make || exit 1
docker run ${DOCKER_ARGS} ${PACKAGE}/ubuntu-14-zabbix-2.2 || exit 1

echo -e "${BULLET} Testing Ubuntu 14.04 Trusty deb with Zabbix 2.4"
cd "${BASE}/ubuntu-14/zabbix-2.4"
make || exit 1
docker run ${DOCKER_ARGS} ${PACKAGE}/ubuntu-14-zabbix-2.4 || exit 1

#
# OpenSUSE 13.2
#
echo -e "${BULLET} Building OpenSUSE 13.2 RPM"
cd "${BASE}/opensuse-13.2/rpmbuild"
make || exit 1
docker run ${DOCKER_ARGS} ${PACKAGE}/opensuse-13.2-rpmbuild || exit 1

echo -e "${BULLET} Testing OpenSUSE 13.2 RPM with Zabbix 2.2"
cd "${BASE}/opensuse-13.2/zabbix-2.2"
make || exit 1
docker run ${DOCKER_ARGS} ${PACKAGE}/opensuse-13.2-zabbix-2.2 || exit 1

#
# Done
#
echo -e "${BULLET} All the win!"
