FROM opensuse:13.2

RUN zypper --non-interactive update

RUN zypper --non-interactive install \
	automake \
	libtool \
	postgresql-devel \
	rpmbuild

RUN mkdir -p /root/rpmbuild/{SPECS,RPMS,SRPMS,SOURCES}

COPY entrypoint.sh /entrypoint.sh

CMD /entrypoint.sh