#!/bin/bash
#
# This script destroys PostgreSQL container instances started from
# ./start_postgresql.sh
#
versions=(84 90 91 92 93 94 95)
for version in "${versions[@]}"; do
	echo "==> Destroying PostgreSQL ${version} instance..."
	docker kill pg${version}; docker rm pg${version}
done
