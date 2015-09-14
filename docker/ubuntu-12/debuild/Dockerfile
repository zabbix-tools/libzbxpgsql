FROM ubuntu:precise

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get -y install \
	git \
	devscripts \
	debhelper \
	dh-make \
	rpm \
	make \
	gcc \
	autoconf \
	automake \
	m4 \
	libtool \
	libpq-dev

COPY entrypoint.sh /entrypoint.sh
CMD /entrypoint.sh
