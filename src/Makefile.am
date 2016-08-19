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
	pg_query2.c \
	pg_server.c \
	pg_setting.c \
	pg_table.c \
	pg_tablespace.c

libzbxpgsql_la_CFLAGS = \
	$(ZABBIX_CPPFLAGS) \
	$(POSTGRESQL_CPPFLAGS) \
	$(POSTGRESQL_LDFLAGS) \
	$(POSTGRESQL_LIBS)


libzbxpgsql_la_LDFLAGS = \
	-shared \
	-lconfig \
	-avoid-version

# Prevent install of the redundant *.la files
install-exec-hook:
	rm -f $(DESTDIR)$(libdir)/libzbxpgsql.la

# Fix "files left after uninstall" issue
uninstall-local:
	rm -f $(DESTDIR)$(libdir)/libzbxpgsql.so
