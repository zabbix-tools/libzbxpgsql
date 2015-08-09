#!/bin/bash

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

sleep 3 # let the babies wake

# install sample databases
psql -U postgres -h 192.168.59.103 -p 5484 < dellstore2-normal-1.0/dellstore2-normal-1.0.sql
psql -U postgres -h 192.168.59.103 -p 5490 < dellstore2-normal-1.0/dellstore2-normal-1.0.sql
psql -U postgres -h 192.168.59.103 -p 5491 < dellstore2-normal-1.0/dellstore2-normal-1.0.sql
psql -U postgres -h 192.168.59.103 -p 5492 < dellstore2-normal-1.0/dellstore2-normal-1.0.sql
psql -U postgres -h 192.168.59.103 -p 5493 < dellstore2-normal-1.0/dellstore2-normal-1.0.sql
psql -U postgres -h 192.168.59.103 -p 5494 < dellstore2-normal-1.0/dellstore2-normal-1.0.sql
