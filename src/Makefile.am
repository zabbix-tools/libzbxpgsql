GIT_VERSION = $(shell git describe --abbrev=8 --dirty --always --tags)

# TODO: Move pg_config call from Makefile into configure script
PSQL_INCLUDE = $(shell pg_config --includedir)
PSQL_LIB = $(shell pg_config --libdir)

lib_LTLIBRARIES = libzbxpgsql.la

libzbxpgsql_la_SOURCES = \
	libzbxpgsql.h \
	libzbxpgsql.c \
	pg_server.c \
	pg_query.c \
	pg_backends.c \
	pg_setting.c \
	pg_namespace.c \
	pg_tablespace.c \
	pg_database.c \
	pg_table.c \
	pg_index.c

libzbxpgsql_la_CFLAGS = \
	-lpq \
	-I$(PSQL_INCLUDE) \
	-L$(PSQL_LIB) \
	-I../.. \
	-DGIT_VERSION=\"$(GIT_VERSION)\"

libzbxpgsql_la_LDFLAGS = \
	-shared \
	-avoid-version

# Prevent install of the redundant *.la files
install-exec-hook:
	rm -f $(DESTDIR)$(libdir)/libzbxpgsql.la

# Fix "files left after uninstall" issue
uninstall-local:
	rm -f $(DESTDIR)$(libdir)/libzbxpgsql.so
