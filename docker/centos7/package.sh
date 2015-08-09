#!/bin/bash

docker build -t libzbxpgsql-centos7 .

docker run \
	-it \
	--rm \
	--name lizbxpgsql-centos7-run \
	-v ${PWD}/../..:/root/libzbxpgsql \
	libzbxpgsql-centos7

