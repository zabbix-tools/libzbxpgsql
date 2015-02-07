# Prevent libtools libraries from being installed
# We only need the dlopen shared libraries (*.so)
install-exec-hook:
	rm -f $(DESTDIR)$(libdir)/*.la

lib_LTLIBRARIES = libzbxpgsql.la
libzbxpgsql_la_SOURCES = libzbxpgsql.c pg_server.c pg_namespace.c pg_tablespace.c pg_database.c pg_table.c pg_index.c
libzbxpgsql_la_CFLAGS = -lpq -I/usr/pgsql-9.4/include -L/usr/pgsql-9.4/lib -I../zabbix-2.4.3/include
libzbxpgsql_la_LDFLAGS = -shared -avoid-version
