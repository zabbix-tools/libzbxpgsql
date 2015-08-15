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

#include "libzbxpgsql.h"

#define PGSQL_DISCOVER_TABLES       "\
SELECT \
    c.oid AS oid \
    , current_database() AS database \
    , n.nspname AS schema \
    , CASE c.reltablespace \
        WHEN 0 THEN (SELECT ds.spcname FROM pg_tablespace ds JOIN pg_database d ON d.dattablespace = ds.oid WHERE d.datname = current_database()) \
        ELSE (SELECT spcname FROM pg_tablespace WHERE oid = c.reltablespace) \
        END AS tablespace \
    , c.relname AS table \
    ,t.typname AS type \
    , pg_catalog.pg_get_userbyid(c.relowner) AS owner \
    , (SELECT COUNT(inhparent) FROM pg_inherits WHERE inhrelid = c.oid) AS issubclass \
    , pg_catalog.obj_description(c.oid, 'pg_class') as description \
FROM pg_class c \
JOIN pg_namespace n ON c.relnamespace = n.oid \
JOIN pg_type t ON c.reltype = t.oid \
WHERE \
    c.relkind='r' \
    AND n.nspname <> 'pg_catalog' \
    AND n.nspname <> 'information_schema' \
    AND n.nspname !~ '^pg_toast' \
ORDER BY c.relname"

#define PGSQL_DISCOVER_TABLE_CHILDREN   "\
SELECT \
    c.oid AS oid \
    , c.relname AS table \
    , n.nspname AS schema \
FROM pg_inherits i \
JOIN pg_class c ON i.inhrelid = c.oid \
JOIN pg_namespace n ON c.relnamespace = n.oid \
WHERE i.inhparent = $1::regclass"

#define PGSQL_GET_TABLE_STAT_SUM    "\
SELECT SUM(%s) FROM pg_stat_all_tables \
WHERE \
    schemaname <> 'pg_catalog' \
    AND schemaname <> 'information_schema' \
    AND schemaname !~ '^pg_toast'"

#define PGSQL_GET_TABLE_STAT        "SELECT %s FROM pg_stat_all_tables WHERE relname = $1"

#define PGSQL_GET_TABLE_STATIO      "SELECT %s FROM pg_statio_all_tables WHERE relname = $1"

#define PGSQL_GET_TABLE_STATIO_SUM  "\
SELECT SUM(%s) FROM pg_statio_all_tables \
WHERE \
    schemaname <> 'pg_catalog' \
    AND schemaname <> 'information_schema' \
    AND schemaname !~ '^pg_toast'"

#define PGSQL_GET_TABLE_SIZE "\
SELECT \
    (CAST(relpages AS bigint) * 8192) \
FROM pg_class \
WHERE relkind='r' AND relname = $1"

#define PGSQL_GET_TABLE_SIZE_SUM "\
SELECT (SUM(relpages) * 8192) \
FROM pg_class t \
JOIN pg_namespace n ON n.oid = t.relnamespace \
WHERE \
    t.relkind = 'r' \
    AND n.nspname <> 'pg_catalog' \
    AND n.nspname <> 'information_schema' \
    AND n.nspname !~ '^pg_toast'"

#define PGSQL_GET_TABLE_ROWS_SUM "\
SELECT SUM(reltuples::bigint) \
FROM pg_class t \
JOIN pg_namespace n ON n.oid = t.relnamespace \
WHERE \
    t.relkind = 'r' \
    AND n.nspname <> 'pg_catalog' \
    AND n.nspname <> 'information_schema' \
    AND n.nspname !~ '^pg_toast'"

#define PGSQL_GET_TABLE_ROWS "\
SELECT reltuples \
FROM pg_class \
WHERE \
    relkind='r' \
    AND relname = $1"

#define PGSQL_GET_TABLE_CHILD_COUNT "\
SELECT \
    COUNT(i.inhrelid) \
FROM pg_inherits i \
WHERE i.inhparent = $1::regclass"

#define PGSQL_GET_CHILDREN_SIZE "\
SELECT \
    (SUM(c.relpages::bigint) * 8192) \
FROM pg_inherits i \
JOIN pg_class c ON inhrelid = c.oid \
WHERE i.inhparent = $1::regclass"

#define PGSQL_GET_CHILDREN_ROWS     "\
SELECT \
    SUM(c.reltuples) \
FROM pg_inherits i \
JOIN pg_class c ON inhrelid = c.oid \
WHERE i.inhparent = $1::regclass"

/*
 * Custom key pg.table.discovery
 *
 * Returns all known Tables in a PostgreSQL database
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 * 
 * Returns:
 * {
 *        "data":[
 *                {
 *                        "{#DATABASE}":"MyDatabase",
 *                        "{#SCHEMA}":"public",
 *                        "{#TABLESPACE}":"pg_default",
 *                        "{#TABLE}":"MyTable",
 *                        "{#TYPE}":"MyTable",
 *                        "{#OWNER}":"postgres",
 *                        "{#PERSISTENCE":"permenant|temporary",
 *                        "{#ISSUBCLASS}":"0"}]}
 */
int    PG_TABLE_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;                     // Request result code
    const char  *__function_name = "PG_TABLE_DISCOVERY";    // Function name for log file
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    ret = pg_get_discovery(request, result, PGSQL_DISCOVER_TABLES, NULL);

    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom key pg.table.children.discovery
 *
 * Returns all known child tables for the specified parent table
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  parent table name
 *
 * Returns:
 * {
 *        "data":[
 *                {
 *                        "{#OID}":"12345",
 *                        "{#SCHEMA}":"public",
 *                        "{#TABLE}":"MyTable"}]}
 */
int    PG_TABLE_CHILDREN_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;                             // Request result code
    const char  *__function_name = "PG_TABLE_CHILDREN_DISCOVERY";   // Function name for log file
    char        *tablename = NULL;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // Parse parameters
    tablename = get_rparam(request, PARAM_FIRST);
    if(NULL == tablename || '\0' == *tablename) {
        zabbix_log(LOG_LEVEL_ERR, "No table name specified in %s()", __function_name);
        goto out;
    }
    
    ret = pg_get_discovery(request, result, PGSQL_DISCOVER_TABLE_CHILDREN, param_new(tablename));

out:
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom keys pg.table.* (for each field in pg_stat_all_tables)
 *
 * Returns the requested statistic for the specified data table
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  table name (default: sum for all)
 *
 * Returns: u
 */
int    PG_STAT_ALL_TABLES(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;                     // Request result code
    const char  *__function_name = "PG_STAT_ALL_TABLES";    // Function name for log file
    
    char        *tablename = NULL;
    char        *field;
    char        query[MAX_STRING_LEN];
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // Get stat field from requested key name "pb.table.<field>"
    field = &request->key[9];
    
    // Build query
    tablename = get_rparam(request, PARAM_FIRST);
    if(strisnull(tablename)) {
        zbx_snprintf(query, sizeof(query), PGSQL_GET_TABLE_STAT_SUM, field);
    } else {
        zbx_snprintf(query, sizeof(query), PGSQL_GET_TABLE_STAT, field);
    }
    
    // Set result
    if(0 == strncmp(field, "last_", 5)) {
        if(strisnull(tablename)) {
            // Can't do SUMs on text fields!
            zabbix_log(LOG_LEVEL_ERR, "No table name specified, in %s(%s)", __function_name, field);
            goto out;
        }
    
        ret = pg_get_string(request, result, query, param_new(tablename));
    }
    else {
        ret = pg_get_int(request, result, query, param_new(tablename));
    }
    
out:
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom keys pg.table.* (for each field in pg_statio_all_tables)
 *
 * Returns the requested IO statistic for the specified data table
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  table name (default: sum for all)
 *
 * Returns: u
 */
int    PG_STATIO_ALL_TABLES(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;                     // Request result code
    const char  *__function_name = "PG_STATIO_ALL_TABLES";  // Function name for log file
    
    char        *tablename = NULL;
    
    char        *field;
    char        query[MAX_STRING_LEN];
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // Get stat field from requested key name "pb.table.<field>"
    field = &request->key[9];
    
    // Build query
    tablename = get_rparam(request, PARAM_FIRST);
    if(NULL == tablename || '\0' == *tablename)
        zbx_snprintf(query, sizeof(query), PGSQL_GET_TABLE_STATIO_SUM, field);
    else
        zbx_snprintf(query, sizeof(query), PGSQL_GET_TABLE_STATIO, field);

    ret = pg_get_int(request, result, query, param_new(tablename));
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom key pg.table.size
 *
 * Returns the disk usage in bytes for the specified data table
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  table name (default: sum for all)
 *
 * TODO: implement sizing of parent/child/all for tables
 *   3:  include statistics for on of:
 *         table:    named table only (default)
 *         children: children of the named table only
 *         all:      named table and its children
 *
 * Returns: u
 */
int    PG_TABLE_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;             // Request result code
    const char  *__function_name = "PG_TABLE_SIZE"; // Function name for log file
        
    char        *query = NULL;
    char        *tablename = NULL;
            
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // Parse parameters
    tablename = get_rparam(request, PARAM_FIRST);
    
    // Build query
    if(strisnull(tablename))
        query = PGSQL_GET_TABLE_SIZE_SUM;
    else
        query = PGSQL_GET_TABLE_SIZE;

    ret = pg_get_int(request, result, query, param_new(tablename));
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom key pg.table.rows
 *
 * Returns the estimated row count for the specified class (table, index, etc.)
 *
 * See: http://www.postgresql.org/docs/9.4/static/catalog-pg-class.html
 *      https://wiki.postgresql.org/wiki/Disk_Usage
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  table name (default: sum for all)
 *
 * Returns: u
 */
int    PG_TABLE_ROWS(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;             // Request result code
    const char  *__function_name = "PG_TABLE_ROWS"; // Function name for log file
        
    char        *tablename = NULL;
    char        *query = NULL;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    tablename = get_rparam(request, PARAM_FIRST);
    if(strisnull(tablename))
        query = PGSQL_GET_TABLE_ROWS_SUM;
    else
        query = PGSQL_GET_TABLE_ROWS;

    ret = pg_get_int(request, result, query, param_new(tablename));
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom key pg.table.children
 *
 * Returns the number of tables that inherit from the specified table
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  table name
 *
 * Returns: u
 */
int    PG_TABLE_CHILDREN(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;                 // Request result code
    const char  *__function_name = "PG_TABLE_CHILDREN"; // Function name for log file
        
    char        *tablename = NULL;
    char        query[MAX_STRING_LEN];
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    tablename = get_rparam(request, PARAM_FIRST);
    if(strisnull(tablename)) {
        zabbix_log(LOG_LEVEL_ERR, "Invalid parameter count in %s(). Please specify a table name.", __function_name);
        goto out;
    }

    ret = pg_get_int(request, result, PGSQL_GET_TABLE_CHILD_COUNT, param_new(tablename));
    
out:
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom key pg.table.children.size
 *
 * Returns the sum size in bytes of all tables that inherit from the specified table
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  table name
 *
 * Returns: u
 */
int    PG_TABLE_CHILDREN_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int           ret = SYSINFO_RET_FAIL;                 // Request result code
    const char    *__function_name = "PG_TABLE_CHILDREN_SIZE";    // Function name for log file
        
    char          *tablename = NULL;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    tablename = get_rparam(request, PARAM_FIRST);
    if(NULL == tablename || '\0' == *tablename) {
        zabbix_log(LOG_LEVEL_ERR, "Invalid parameter count in %s(). Please specify a table name.", __function_name);
        goto out;
    }

    ret = pg_get_int(request, result, PGSQL_GET_CHILDREN_SIZE, param_new(tablename));
    
out:
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom key pg.table.children.rows
 *
 * Returns the sum estimated row count of all tables that inherit from the specified table
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  table name
 *
 * Returns: u
 */
int    PG_TABLE_CHILDREN_ROWS(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;                         // Request result code
    const char  *__function_name = "PG_TABLE_CHILDREN_TUPLES";  // Function name for log file    
    char        *tablename = NULL;

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    tablename = get_rparam(request, PARAM_FIRST);
    if(strisnull(tablename)) {
        zabbix_log(LOG_LEVEL_ERR, "Invalid parameter count in %s(). Please specify a table name.", __function_name);
        goto out;
    }

    ret = pg_get_int(request, result, PGSQL_GET_CHILDREN_ROWS, param_new(tablename));
    
out:
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}
