#!/bin/bash
THREADS=8

export PGHOST=192.168.59.103
export PGUSER="postgres"
export PGDATABASE="postgres"

# test v8.4
export PGCONN="host=${PGHOST} user=${PGUSER} port=5484"
zabbix_agent_bench \
	-threads ${THREADS} \
	-iterations 1 \
	-strict \
	-keys ../fixtures/postgresql-8.4.keys \
	|| exit 1

# test v9.0
export PGCONN="host=${PGHOST} user=${PGUSER} port=5490"
zabbix_agent_bench \
	-threads ${THREADS} \
	-iterations 1 \
	-strict \
	-keys ../fixtures/postgresql-8.4.keys \
	|| exit 1

# test v9.1
export PGCONN="host=${PGHOST} user=${PGUSER} port=5491"
zabbix_agent_bench \
	-threads ${THREADS} \
	-iterations 1 \
	-strict \
	-keys ../fixtures/postgresql-9.1.keys \
	|| exit 1

# test v9.2
export PGCONN="host=${PGHOST} user=${PGUSER} port=5492"
zabbix_agent_bench \
	-threads ${THREADS} \
	-iterations 1 \
	-strict \
	-keys ../fixtures/postgresql-9.2.keys \
	|| exit 1

# test v9.3
export PGCONN="host=${PGHOST} user=${PGUSER} port=5493"
zabbix_agent_bench \
	-threads ${THREADS} \
	-iterations 1 \
	-strict \
	-keys ../fixtures/postgresql-9.2.keys \
	|| exit 1

# test v9.4
export PGCONN="host=${PGHOST} user=${PGUSER} port=5494"
zabbix_agent_bench \
	-threads ${THREADS} \
	-iterations 1 \
	-strict \
	-keys ../fixtures/postgresql-9.2.keys \
	|| exit 1

echo "SUCCESS! All test passed."
