/*
** libzbxpgsql - A PostgreSQL monitoring module for Zabbix
** Copyright (C) 2015 - Ryan Armstrong <ryan@cavaliercoder.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**/

#ifndef LIBZBXPGSQL_H
#define LIBZBXPGSQL_H

#include <ctype.h>
#include <stdarg.h>
#include <arpa/inet.h>

#ifdef __FreeBSD__
#include <netinet/in.h>
#include <sys/socket.h>
#endif

// PostgreSQL headers
#include <libpq-fe.h>

// Reading Config Files
#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include <glob.h>
#include <string.h>

// Zabbix source headers
#define HAVE_TIME_H 1
#include <sysinc.h>
#include <module.h>
#include <common.h>
#include <log.h>
#include <zbxjson.h>
#include <version.h>

// Default query config file location
#ifdef __FreeBSD__
#define DEFAULT_PG_QUERY_CONF_PATH       "/usr/local/etc/libzbxpgsql.d"
#else
#define DEFAULT_PG_QUERY_CONF_PATH       "/etc/libzbxpgsql.d"
#endif

int init_config();

// Default memory usage
#define MAX_GLOBBING_PATH_LENGTH         512
#define MAX_NUMBER_CONFIG_FILES          100
#define MAX_NUMBER_SQL_STATEMENT_IN_RAM  500

// Default connection settings
#define LOCALHOST       "localhost"
#define PSQL_PORT       "5432"
#define PSQL_USER       "postgres"

// Index of connection params in user requests
#define PARAM_CONN_STRING   0
#define PARAM_DBNAME        1
#define PARAM_FIRST         2

#define DEFAULT_CONN_STRING ""
#define DEFAULT_CONN_DBNAME NULL

#define PG_WHERE        "WHERE"
#define PG_AND          "AND"

#define PG_RELKIND_TABLE        "r"
#define PG_RELKIND_INDEX        "i"
#define PG_RELKIND_SEQUENCE     "s"
#define PG_RELKIND_VIEW         "v"
#define PG_RELKIND_MATVIEW      "m"
#define PG_RELKIND_COMPTYPE     "c"
#define PG_RELKIND_TOAST        "t"
#define PG_RELKIND_FGNTABLE     "f"

// Shared globals
extern char  *SQLkey[MAX_NUMBER_SQL_STATEMENT_IN_RAM+1];
extern char  *SQLstmt[MAX_NUMBER_SQL_STATEMENT_IN_RAM+1];

// function to determine if a string is null or empty
#define strisnull(c)            (NULL == c || '\0' == *c)

typedef char** PGparams;

// Local helper functions
int         set_err_result(AGENT_RESULT *result, const char *format, ...);
char        *build_connstring(const char *connstring, const char *dbname);
PGconn      *pg_connect(const char *connstring, AGENT_RESULT *result);
PGconn      *pg_connect_request(AGENT_REQUEST *request, AGENT_RESULT *result);
PGresult    *pg_exec(PGconn *conn, const char *command, PGparams params);
int         pg_scalar(AGENT_REQUEST *request, AGENT_RESULT *result, const char *query, PGparams params, char *buffer, size_t bufferlen);
long int    pg_version(AGENT_REQUEST *request, AGENT_RESULT *result);
char        *query_by_key(const char *key);

int     pg_get_result(AGENT_REQUEST *request, AGENT_RESULT *result, int type, const char *query, PGparams params);
int     pg_get_discovery(AGENT_REQUEST *request, AGENT_RESULT *result, const char *query, PGparams params);
int     pg_get_discovery_wide(AGENT_REQUEST *request, AGENT_RESULT *result, const char *query, PGparams params);
int     pg_get_percentage(AGENT_REQUEST *request, AGENT_RESULT *result, char *table, char *col1, char *col2, char *colFilter, char *filter);

#define pg_get_string(request, result, query, params)     pg_get_result(request, result, AR_STRING, query, params)
#define pg_get_int(request, result, query, params)        pg_get_result(request, result, AR_UINT64, query, params)
#define pg_get_dbl(request, result, query, params)        pg_get_result(request, result, AR_DOUBLE, query, params)

int     is_valid_ip(char *str);
int     is_oid(char *str);
char    *strcat2(char *destination, const char *source);

int         param_len(PGparams params);
PGparams    param_append(PGparams params, char *s);
void        param_free(PGparams params);
#define     param_new(s)    param_append(NULL, s)

int     PG_GET_CLASS_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result, char *relkind, char *relname);

// Define agent key functions
int     MODVER(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_CONNECT(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_VERSION(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_STARTTIME(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_UPTIME(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_PREPARED_XACTS_COUNT(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_PREPARED_XACTS_RATIO(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_PREPARED_XACTS_AGE(AGENT_REQUEST *request, AGENT_RESULT *result);

int     PG_SETTING(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_SETTING_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result);

int     PG_QUERY(AGENT_REQUEST *request, AGENT_RESULT *result);

int     PG_BACKENDS_COUNT(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_BACKENDS_FREE(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_BACKENDS_RATIO(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_QUERIES_LONGEST(AGENT_REQUEST *request, AGENT_RESULT *result);

int     PG_STAT_BGWRITER(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_BG_AVG_INTERVAL(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_BG_TIME_RATIO(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_BG_STATS_RESET_INTERVAL(AGENT_REQUEST *request, AGENT_RESULT *result);

int     PG_DB_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_NAMESPACE_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_TABLESPACE_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_TABLE_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_TABLE_CHILDREN_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_INDEX_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result);

int     PG_STAT_DATABASE(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_DB_BLKS_RATIO(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_DB_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_DB_XID_AGE(AGENT_REQUEST *request, AGENT_RESULT *result);

int     PG_TABLESPACE_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result);

int     PG_NAMESPACE_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result);

int     PG_STAT_ALL_TABLES(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_TABLE_IDX_SCAN_RATIO(AGENT_REQUEST *request, AGENT_RESULT *result);

int     PG_STATIO_ALL_TABLES(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_TABLE_HEAP_BLKS_RATIO(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_TABLE_IDX_BLKS_RATIO(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_TABLE_TOAST_BLKS_RATIO(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_TABLE_TIDX_BLKS_RATIO(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_TABLE_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_TABLE_ROWS(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_TABLE_CHILDREN(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_TABLE_CHILDREN_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_TABLE_CHILDREN_ROWS(AGENT_REQUEST *request, AGENT_RESULT *result);

int     PG_STAT_ALL_INDEXES(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_STATIO_ALL_INDEXES(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_INDEX_IDX_BLKS_RATIO(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_INDEX_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_INDEX_ROWS(AGENT_REQUEST *request, AGENT_RESULT *result);

#endif
