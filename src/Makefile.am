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
	-I../include -I../../include \
	-lpq \
	-I/usr/pgsql-9.4/include \
	-I/usr/include/postgresql \
	-L/usr/pgsql-9.4/lib

libzbxpgsql_la_LDFLAGS = \
	-shared \
	-avoid-version

# Prevent install of the redundant *.la files
install-exec-hook:
	rm -f $(DESTDIR)$(libdir)/libzbxpgsql.la

# Fix "files left after uninstall" issue
uninstall-local:
	rm -f $(DESTDIR)$(libdir)/libzbxpgsql.so
