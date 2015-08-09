#!/bin/bash

docker run -d -p 5484:5432 --name pg84 postgres:8.4
docker run -d -p 5490:5432 --name pg90 postgres:9.0
docker run -d -p 5491:5432 --name pg91 postgres:9.1
docker run -d -p 5492:5432 --name pg92 postgres:9.2
docker run -d -p 5493:5432 --name pg93 postgres:9.3
docker run -d -p 5494:5432 --name pg94 postgres:9.4
