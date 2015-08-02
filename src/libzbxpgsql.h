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

#include "include/sysinc.h"
#include "include/module.h"
#include "include/common.h"
#include "include/log.h"
#include "include/zbxjson.h"

#include <libpq-fe.h>

// Version info
#ifdef GIT_VERSION
    #define STRVER  PACKAGE " " GIT_VERSION
#else
    #define STRVER  PACKAGE " " PACKAGE_VERSION
#endif

// Default connection settings
#define LOCALHOST       "localhost"
#define PSQL_PORT       "5432"
#define PSQL_USER       "postgres"

// Index of connection params in user requests
#define PARAM_CONN_STRING   0
#define PARAM_DBNAME        1
#define PARAM_FIRST         2

#define DEFAULT_CONN_STRING "\0"
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

// function to determine if a string is null or empty
#define strisnull(c)            (NULL == c || '\0' == *c)

// Local helper functions
PGconn  *pg_connect(AGENT_REQUEST *request);
int     pg_get_string(AGENT_REQUEST *request, AGENT_RESULT *result, const char *query);
int     pg_get_int(AGENT_REQUEST *request, AGENT_RESULT *result, const char *query);
int     pg_get_dbl(AGENT_REQUEST *request, AGENT_RESULT *result, const char *query);
int     is_valid_ip(char *str);
int     is_oid(char *str);
char    *strcat2(char *destination, const char *source);

int     PG_GET_CLASS_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result, char *relkind, char *relname);

// Define agent key functions
int     PG_CONNECT(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_VERSION(AGENT_REQUEST *request, AGENT_RESULT *result);

int     PG_SETTING(AGENT_REQUEST *request, AGENT_RESULT *result);
int     PG_SETTING_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result);

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

#endif