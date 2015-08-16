FROM centos:7

RUN yum update -y

RUN yum install -y \
	automake \
	gcc \
	git \
	libtool \
	make \
	postgresql-devel \
	rpm-build

RUN mkdir -p /root/rpmbuild/{SPECS,RPMS,SRPMS,SOURCES} /root/libzbxpgsql

COPY make_dist.sh /make_dist.sh

COPY entrypoint.sh /entrypoint.sh

CMD /entrypoint.sh
