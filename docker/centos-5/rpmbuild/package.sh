#!/bin/bash

docker build -t libzbxpgsql-centos5 .

docker run \
	-it \
	--rm \
	--name lizbxpgsql-centos5-run \
	-v ${PWD}/../..:/root/libzbxpgsql \
	libzbxpgsql-centos5

