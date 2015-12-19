#!/bin/bash
#
# This script uses Docker on the local host to start an instance of each
# supported version of PostgreSQL. Each version will listen on a discrete port
# with a prefix of "54" and suffix of the major and minor version.
#
# E.g. for v8.4: tcp 5484
#      for v9.0: tcp 5490
#      for v9.1: tcp 5491
#
# A sample database is downloaded, cached and installed in each instance.
#
# You may stop all started instances with ./stop_postgresql.sh
#

# ensure PostgreSQL client is installed
which psql >/dev/null || exit 1

# ensure docker is running
docker version >/dev/null || exit 1

# download and extract sample database
if [[ ! -f dellstore2-normal-1.0.tar.gz ]]; then
	echo "==> Downloading Dell Store sample database..."
	curl -LO http://pgfoundry.org/frs/download.php/543/dellstore2-normal-1.0.tar.gz || exit 1
fi

if [[ ! -d dellstore2-normal-1.0 ]]; then
	echo "==> Extracing Dell store sample database..."
	tar xzvf dellstore2-normal-1.0.tar.gz || exit 1
fi

# start PostgreSQL instances
echo "==> Starting PostgreSQL docker instances..."
docker run -d -p 5484:5432 --name pg84 postgres:8.4
docker run -d -p 5490:5432 --name pg90 postgres:9.0
docker run -d -p 5491:5432 --name pg91 postgres:9.1
docker run -d -p 5492:5432 --name pg92 postgres:9.2
docker run -d -p 5493:5432 --name pg93 postgres:9.3
docker run -d -p 5494:5432 --name pg94 postgres:9.4
docker run -d -p 5495:5432 --name pg95 postgres:9.5

echo "==> Sleeping while instances start..."
sleep 30

# install sample databases
versions=(84 90 91 92 93 94 95)
for version in "${versions[@]}"; do
	echo "==> Installing Dell store sample database in PostgreSQL ${version}"
	psql -U postgres -h ${DOCKER_IP} -p 54${version} < dellstore2-normal-1.0/dellstore2-normal-1.0.sql
	psql -U postgres -h ${DOCKER_IP} -p 54${version} -c "CREATE ROLE monitoring WITH LOGIN NOSUPERUSER NOCREATEDB NOCREATEROLE;"
done

echo "==> Finished. Use ./stop_postgresql.sh to destroy all instances."
