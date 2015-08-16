FROM centos:5

RUN yum update -y

# need epel for git
RUN yum install -y epel-release

RUN yum install -y \
	automake \
	gcc \
	git \
	libtool \
	make \
	postgresql-devel \
	rpm-build \
	tar

RUN mkdir -p /usr/src/redhat/{SPECS,RPMS,SRPMS,SOURCES} /root/libzbxpgsql

COPY entrypoint.sh /entrypoint.sh
CMD /entrypoint.sh
