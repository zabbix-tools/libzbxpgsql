#!/bin/bash
#
# This script executes a test of all libzbxpgsql agent item keys against all
# supported versions of PostgreSQL.
#
# Requirements:
#   * zabbix_agent_bench must be install in $PATH
#   * the zabbix agent daemon must be listening on tcp://127.0.0.1:10050
#   * libzbxpgsql must be loaded in the agent
#   * each supported version of PostgreSQL must be listening from $PGHOST, on a
#     different port for each version (e.g. 5484 for v8.4, 5492 for v9.2)
#   * PostgreSQL must allow trusted connections in pg_hba.conf for $PGUSER
#   * each PostgreSQL instance should have sample databases for testing
#
# Agent item keys are defined in ../fixtures/*.keys
#
# Scripts are provided for starting and stopping PostgreSQL versions via Docker.
# See: start_postgresql.sh and stop_postgresql.sh
#
ARGS="-threads 8 -iterations 1 -strict"
PGUSER="postgres"
PGDATABASE="postgres"

# ensure zabbix_agent_bench is installed
which zabbix_agent_bench >/dev/null || exit 1

# ensure libzbxpgsql is loaded in local zabbix agent
LIBVER=$(zabbix_get -s 127.0.0.1 -k pg.modver | grep ^libzbxpgsql)
if [[ -z "${LIBVER}" ]]; then
	echo "Zabbix agent is not running or libzbxpgsql is not loaded" >&2
	exit 1
fi
echo "==> Zabbix agent loaded with ${LIBVER}"

# ensure $PGHOST is set
if [[ -z "${PGHOST}" ]]; then
	echo "PGHOST is not set" >&2
	exit 1
fi

# test function for all versions
function test(){
	version=$1
	key_version=$2
	port=$3

	echo "==> Testing keys for PostgreSQL v${version}..."
	PGCONN="host=${PGHOST} user=${PGUSER} port=${port}" \
		PGDATABASE="${PGDATABASE}" \
		zabbix_agent_bench ${ARGS} \
		-keys ../fixtures/postgresql-${key_version}.keys

	if [[ $? -ne 0 ]]; then
		echo "FAIL! All tests were not successful :'(" >&2
		exit 1
	fi
}

# test each version
test 8.4 8.4 5484
test 9.0 8.4 5490
test 9.1 9.1 5491
test 9.2 9.2 5492
test 9.3 9.2 5493
test 9.4 9.4 5494
test 9.5 9.4 5495

echo "SUCCESS! All test passed."
