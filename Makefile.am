ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src
EXTRA_DIST = \
	include \
	README.md \
	template_postgresql_server.xml \
	$(PACKAGE).spec

# RPM package build target
RPMBASE = ~/rpmbuild
RPMBUILD = rpmbuild
RPMBUILD_FLAGS = -ba
RPMFILE = $(PACKAGE)-$(PACKAGE_VERSION)-*.$(build_cpu).rpm

rpm:
	sed -i 's/^Version\s*:.*/Version     : $(PACKAGE_VERSION)/' $(PACKAGE).spec
	mkdir -p $(RPMBASE)/{BUILD,RPMS,SOURCES,SPECS,SRPMS} || :
	$(MAKE) dist
	cp $(PACKAGE)-$(PACKAGE_VERSION).tar.gz $(RPMBASE)/SOURCES/$(PACKAGE)-$(PACKAGE_VERSION).tar.gz
	$(RPMBUILD) $(RPMBUILD_FLAGS) $(PACKAGE).spec
	cp $(RPMBASE)/RPMS/$(build_cpu)/$(RPMFILE) .
