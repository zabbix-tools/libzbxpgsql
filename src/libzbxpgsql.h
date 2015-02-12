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

#include <ctype.h>
#include <stdarg.h>

#include "sysinc.h"
#include "module.h"
#include "common.h"
#include "log.h"
#include "zbxjson.h"

#include <libpq-fe.h>

// Default connection settings
#define LOCALHOST       "localhost"
#define PSQL_PORT       "5432"
#define PSQL_USER       "postgres"

// Index of connection params in user requests
#define PARAM_HOST      0
#define PARAM_PORT      1
#define PARAM_DB        2
#define PARAM_USER      3
#define PARAM_PASSWD    4
#define PARAM_FIRST     5

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

// Local helper functions
PGconn  *pg_connect(AGENT_REQUEST *request);
int     pg_get_string(AGENT_REQUEST *request, AGENT_RESULT *result, const char *query);
int     pg_get_int(AGENT_REQUEST *request, AGENT_RESULT *result, const char *query);
int     pg_get_dbl(AGENT_REQUEST *request, AGENT_RESULT *result, const char *query);

int     PG_GET_CLASS_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result, char *relkind, char *relname);

// Define agent key functions
int     PG_CONNECT(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_VERSION(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_SETTING(AGENT_REQUEST *request, AGENT_RESULT *result);

int     PG_QUERY(AGENT_REQUEST *request, AGENT_RESULT *result);

int     PG_BACKENDS_COUNT(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_QUERIES_LONGEST(AGENT_REQUEST *request, AGENT_RESULT *result);

int     PG_STAT_BGWRITER(AGENT_REQUEST *request, AGENT_RESULT *result);

int     PG_DB_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_NAMESPACE_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_TABLESPACE_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_TABLE_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_TABLE_CHILDREN_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_INDEX_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result);

int     PG_STAT_DATABASE(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_DB_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result);

int     PG_TABLESPACE_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result);

int     PG_NAMESPACE_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result);

int     PG_STAT_ALL_TABLES(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_STATIO_ALL_TABLES(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_TABLE_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_TABLE_ROWS(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_TABLE_CHILDREN(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_TABLE_CHILDREN_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_TABLE_CHILDREN_TUPLES(AGENT_REQUEST *request, AGENT_RESULT *result);

int     PG_STAT_ALL_INDEXES(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_STATIO_ALL_INDEXES(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_INDEX_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_INDEX_ROWS(AGENT_REQUEST *request, AGENT_RESULT *result);