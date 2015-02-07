# Prevent libtools libraries from being installed
# We only need the dlopen shared libraries (*.so)
install-exec-hook:
	rm -f $(DESTDIR)$(libdir)/*.la

lib_LTLIBRARIES = libzbxpgsql.la
libzbxpgsql_la_SOURCES = \
	libzbxpgsql.c \
	pg_server.c \
	pg_namespace.c \
	pg_tablespace.c \
	pg_database.c \
	pg_table.c \
	pg_index.c

libzbxpgsql_la_CFLAGS = \
	-I../zabbix-2.4.3/include \
	-lpq \
	-I/usr/pgsql-9.4/include \
	-I/usr/include/postgresql \
	-L/usr/pgsql-9.4/lib

libzbxpgsql_la_LDFLAGS = \
	-shared \
	-avoid-version
