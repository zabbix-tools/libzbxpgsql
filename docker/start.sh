#!/bin/bash
docker build -t libzbxpgsql .

# Start PostgreSQL instances
docker run -d --name pg94 -p 5494:5432 postgres:9.4
docker run -d --name pg90 -p 5490:5432 postgres:9.0
docker run -d --name pg84 -p 5484:5432 postgres:8.4

# start zabbix agent
docker run --name zabbix-agent --rm -p 10050:10050 -v ${PWD}/../src/.libs:/usr/lib64/modules libzbxpgsql
