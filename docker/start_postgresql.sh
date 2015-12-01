#!/bin/bash
PSQL_CREATE_ROLE="CREATE ROLE monitoring WITH LOGIN NOSUPERUSER NOCREATEDB NOCREATEROLE;"

# download and extract sample database
[[ -f dellstore2-normal-1.0.tar.gz ]] || curl -LO http://pgfoundry.org/frs/download.php/543/dellstore2-normal-1.0.tar.gz
[[ -d dellstore2-normal-1.0 ]] || tar xzvf dellstore2-normal-1.0.tar.gz

# start PostgreSQL instances
docker run -d -p 5484:5432 --name pg84 postgres:8.4
docker run -d -p 5490:5432 --name pg90 postgres:9.0
docker run -d -p 5491:5432 --name pg91 postgres:9.1
docker run -d -p 5492:5432 --name pg92 postgres:9.2
docker run -d -p 5493:5432 --name pg93 postgres:9.3
docker run -d -p 5494:5432 --name pg94 postgres:9.4

sleep 10 # let the babies wake

# install sample databases
psql -U postgres -h ${DOCKER_IP} -p 5484 < dellstore2-normal-1.0/dellstore2-normal-1.0.sql
psql -U postgres -h ${DOCKER_IP} -p 5484 -c "${PSQL_CREATE_ROLE}"

psql -U postgres -h ${DOCKER_IP} -p 5490 < dellstore2-normal-1.0/dellstore2-normal-1.0.sql
psql -U postgres -h ${DOCKER_IP} -p 5490 -c "${PSQL_CREATE_ROLE}"

psql -U postgres -h ${DOCKER_IP} -p 5491 < dellstore2-normal-1.0/dellstore2-normal-1.0.sql
psql -U postgres -h ${DOCKER_IP} -p 5491 -c "${PSQL_CREATE_ROLE}"

psql -U postgres -h ${DOCKER_IP} -p 5492 < dellstore2-normal-1.0/dellstore2-normal-1.0.sql
psql -U postgres -h ${DOCKER_IP} -p 5492 -c "${PSQL_CREATE_ROLE}"

psql -U postgres -h ${DOCKER_IP} -p 5493 < dellstore2-normal-1.0/dellstore2-normal-1.0.sql
psql -U postgres -h ${DOCKER_IP} -p 5493 -c "${PSQL_CREATE_ROLE}"

psql -U postgres -h ${DOCKER_IP} -p 5494 < dellstore2-normal-1.0/dellstore2-normal-1.0.sql
psql -U postgres -h ${DOCKER_IP} -p 5494 -c "${PSQL_CREATE_ROLE}"
