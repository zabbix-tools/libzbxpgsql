if HAVE_GIT
GIT_REVISION = $(shell $(GIT) rev-parse --verify HEAD)
GIT_DESCRIPTION = $(shell $(GIT) describe --abbrev=8 --dirty --always --tags)
GIT_CFLAGS = -DGIT_REVISION=\"$(GIT_REVISION)\" -DGIT_DESCRIPTION=\"$(GIT_DESCRIPTION)\"
endif

lib_LTLIBRARIES = libzbxpgsql.la

libzbxpgsql_la_SOURCES = \
	libzbxpgsql.h \
	libzbxpgsql.c \
	pg_connect.c \
	pg_discovery.c \
	pg_backends.c \
	pg_bgwriter.c \
	pg_database.c \
	pg_index.c \
	pg_namespace.c \
	pg_params.c \
	pg_query.c \
	pg_server.c \
	pg_setting.c \
	pg_table.c \
	pg_tablespace.c

libzbxpgsql_la_CFLAGS = \
	-I../include \
	$(POSTGRESQL_CPPFLAGS) \
	$(POSTGRESQL_LDFLAGS) \
	$(POSTGRESQL_LIBS) \
	$(GIT_CFLAGS)

libzbxpgsql_la_LDFLAGS = \
	-shared \
	-avoid-version

# Prevent install of the redundant *.la files
install-exec-hook:
	rm -f $(DESTDIR)$(libdir)/libzbxpgsql.la

# Fix "files left after uninstall" issue
uninstall-local:
	rm -f $(DESTDIR)$(libdir)/libzbxpgsql.so
