#!/bin/bash

docker build -t libzbxpgsql-centos6 .

docker run \
	-it \
	--rm \
	--name lizbxpgsql-centos6-run \
	-v ${PWD}/../..:/root/libzbxpgsql \
	libzbxpgsql-centos6

