#!/bin/bash

docker build -t libzbxpgsql-opensuse .

docker run \
	-it \
	--rm \
	--name lizbxpgsql-opensuse-run \
	-v ${PWD}/../..:/root/libzbxpgsql \
	libzbxpgsql-opensuse

