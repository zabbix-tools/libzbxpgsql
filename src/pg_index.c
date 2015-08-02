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

#define PGSQL_DISCOVER_INDEXES      "\
SELECT \
    ic.oid AS oid \
    , ic.relname AS index \
    , current_database() AS database \
    , n.nspname AS schema \
    , t.relname AS table \
    , a.rolname AS owner \
    , m.amname AS access \
FROM pg_index i \
JOIN pg_class ic ON ic.oid = i.indexrelid \
JOIN pg_namespace n ON n.oid = ic.relnamespace \
JOIN pg_roles a ON a.oid = ic.relowner \
JOIN pg_class t ON t.oid = i.indrelid \
JOIN pg_am m ON m.oid = ic.relam \
WHERE \
    n.nspname <> 'pg_catalog' \
    AND n.nspname <> 'information_schema' \
    AND n.nspname !~ '^pg_toast'"


#define PGSQL_GET_INDEX_SIZE        "SELECT (CAST(relpages AS bigint) * 8192) FROM pg_class WHERE (relkind='i' AND relname = '%s')"

#define PGSQL_GET_INDEX_SIZE_SUM    "SELECT (SUM(relpages) * 8192) FROM pg_class WHERE relkind='i'"

#define PGSQL_GET_INDEX_ROWS        "SELECT reltuples FROM pg_class WHERE (relkind='i' AND relname = '%s')"

#define PGSQL_GET_INDEX_ROWS_SUM    "SELECT SUM(reltuples) FROM pg_class WHERE relkind='i'"

/*
 * Custom key pg.index.discovery
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *
 * Returns all known indexes in a PostgreSQL database
 *
 * Returns:
 * {
 *        "data":[
 *                {
 *                        "{#OID}":"12345",
 *                        "{#INDEX}":"MyIndex",
 *                        "{#DATABASE}":"MyDatabase",
 *                        "{#SCHEMA}":"public",
 *                        "{#TABLE}":"MyTable",
 *                        "{#OWNER}":"postgres",
 *                        "{#ACCESS}":"btree|hash"}]}
 */
int    PG_INDEX_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;                 // Request result code
    const char  *__function_name = "PG_DB_DISCOVERY";   // Function name for log file
    struct      zbx_json j;                             // JSON response for discovery rule
    
    PGconn      *conn = NULL;
    PGresult    *res = NULL;
    
    char        query[MAX_STRING_LEN], buffer[MAX_STRING_LEN];
    char        *c = NULL;
    int         i = 0, count = 0;

    char        *param_table = NULL, *param_schema = NULL;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    // build the query
    c = strcat2(query, PGSQL_DISCOVER_INDEXES);

    // filter by schema name
    param_schema = get_rparam(request, PARAM_FIRST);
    if(!strisnull(param_schema)) {
        zbx_snprintf(buffer, sizeof(buffer), " AND n.nspname = '%s'", param_schema);
        c = strcat2(c, buffer);
    }

    // filter by table name
    param_table = get_rparam(request, PARAM_FIRST + 1);
    if(!strisnull(param_table)) {
        zbx_snprintf(buffer, sizeof(buffer), " AND t.relname = '%s'", param_table);
        c = strcat2(c, buffer);
    }
    
    // Connect to PostreSQL
    if(NULL == (conn = pg_connect(request)))
        goto out;
    
    // Execute a query
    res = pg_exec(conn, query);
    if(PQresultStatus(res) != PGRES_TUPLES_OK) {
        zabbix_log(LOG_LEVEL_ERR, "Failed to execute PostgreSQL query in %s() with: %s", __function_name, PQresultErrorMessage(res));
        goto out;
    }

    count = PQntuples(res);

    // Create JSON array of discovered objects
    zbx_json_init(&j, ZBX_JSON_STAT_BUF_LEN);
    zbx_json_addarray(&j, ZBX_PROTO_TAG_DATA);
    
    for(i = 0; i < count; i++) {
        zbx_json_addobject(&j, NULL);        
        zbx_json_addstring(&j, "{#OID}", PQgetvalue(res, i, 0), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#INDEX}", PQgetvalue(res, i, 1), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#DATABASE}", PQgetvalue(res, i, 2), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#SCHEMA}", PQgetvalue(res, i, 3), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#TABLE}", PQgetvalue(res, i, 4), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#OWNER}", PQgetvalue(res, i, 5), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#ACCESS}", PQgetvalue(res, i, 6), ZBX_JSON_TYPE_STRING);
        zbx_json_close(&j);         
    }
    
    // Finalize JSON response
    zbx_json_close(&j);
    SET_STR_RESULT(result, strdup(j.buffer));
    zbx_json_free(&j);
    ret = SYSINFO_RET_OK;
        
out:
    PQclear(res);
    PQfinish(conn);
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom keys pg.index.* (for each field in pg_stat_all_indexes)
 *
 * Returns the requested statistic for the specified index
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  filter by index name (default: sum of all indexes)
 *
 * Returns: u
 */
int    PG_STAT_ALL_INDEXES(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;                     // Request result code
    const char  *__function_name = "PG_STAT_ALL_INDEXES";   // Function name for log file
    
    char        *index = NULL;
    
    PGconn      *conn = NULL;
    PGresult    *res = NULL;
    
    char        *field;
    char        query[MAX_STRING_LEN];
    char        *buffer = NULL;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // Get stat field from requested key name "pb.table.<field>"
    field = &request->key[9];
    
    // Build query
    index = get_rparam(request, PARAM_FIRST);
    if(NULL == index || '\0' == *index)
        zbx_snprintf(query, sizeof(query), "SELECT SUM(%s) FROM pg_stat_all_indexes", field);
    else
        zbx_snprintf(query, sizeof(query),  "SELECT %s FROM pg_stat_all_indexes WHERE indexrelname = '%s'", field, index);

    // Connect to PostreSQL
    if(NULL == (conn = pg_connect(request)))
        goto out;
    
    // Execute a query
    res = pg_exec(conn, query);
    if(PQresultStatus(res) != PGRES_TUPLES_OK) {
        zabbix_log(LOG_LEVEL_ERR, "Failed to execute PostgreSQL query in %s() with: %s", __function_name, PQresultErrorMessage(res));
        goto out;
    }
    
    if(0 == PQntuples(res)) {
        zabbix_log(LOG_LEVEL_ERR, "No results returned for query \"%s\" in %s()", query, __function_name);
        goto out;
    }
    
    // Set result
    buffer = strdup(PQgetvalue(res, 0, 0));
    SET_UI64_RESULT(result, atoi(buffer));
    ret = SYSINFO_RET_OK;
        
out:
    PQclear(res);
    PQfinish(conn);
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom keys pg.index.* (for each field in pg_statio_all_indexes)
 *
 * Returns the requested IO statistic for the specified index
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  filter by index name (default: sum of all indexes)
 *
 * Returns: u
 */
int    PG_STATIO_ALL_INDEXES(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int             ret = SYSINFO_RET_FAIL;                 // Request result code
    const char          *__function_name = "PG_STAT_ALL_INDEXES";   // Function name for log file
    
    char        *index = NULL;
    
    PGconn      *conn = NULL;
    PGresult        *res = NULL;
    
    char        *field;
    char        query[MAX_STRING_LEN];
    char        *buffer = NULL;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // Get stat field from requested key name "pb.table.<field>"
    field = &request->key[9];
    
    // Build query
    index = get_rparam(request, PARAM_FIRST);
    if(NULL == index || '\0' == *index)
        zbx_snprintf(query, sizeof(query), "SELECT SUM(%s) FROM pg_statio_all_indexes", field);
    else
        zbx_snprintf(query, sizeof(query),  "SELECT %s FROM pg_statio_all_indexes WHERE indexrelname = '%s'", field, index);

    // Connect to PostreSQL
    if(NULL == (conn = pg_connect(request)))
        goto out;
    
    // Execute a query
    res = pg_exec(conn, query);
    if(PQresultStatus(res) != PGRES_TUPLES_OK) {
        zabbix_log(LOG_LEVEL_ERR, "Failed to execute PostgreSQL query in %s() with: %s", __function_name, PQresultErrorMessage(res));
        goto out;
    }
    
    if(0 == PQntuples(res)) {
        zabbix_log(LOG_LEVEL_ERR, "No results returned for query \"%s\" in %s()", query, __function_name);
        goto out;
    }
    
    // Set result
    buffer = strdup(PQgetvalue(res, 0, 0));
    SET_UI64_RESULT(result, atoi(buffer));
    ret = SYSINFO_RET_OK;
        
out:
    PQclear(res);
    PQfinish(conn);
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom key pg.index.size
 *
 * Returns the disk usage in bytes for the specified index
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  filter by index name (default: sum of all indexes)
 *
 * Returns: u
 */
int    PG_INDEX_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;             // Request result code
    const char  *__function_name = "PG_INDEX_SIZE"; // Function name for log file
        
    char        query[MAX_STRING_LEN];
    char        *index = NULL; //, *include = NULL;
            
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // Parse parameters
    index = get_rparam(request, PARAM_FIRST);
    
    // Build query  
    if(NULL == index || '\0' == *index)
        zbx_snprintf(query, sizeof(query), PGSQL_GET_INDEX_SIZE_SUM);
    else
        zbx_snprintf(query, sizeof(query), PGSQL_GET_INDEX_SIZE, index);

    ret = pg_get_int(request, result, query);
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom key pg.index.rows
 *
 * Returns the estimated row count for the specified index
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  filter by index name (default: sum of all indexes)
 *
 * Returns: u
 */
int    PG_INDEX_ROWS(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;             // Request result code
    const char  *__function_name = "PG_INDEX_ROWS"; // Function name for log file
        
    char        query[MAX_STRING_LEN];
    char        *index = NULL; //, *include = NULL;
            
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // Parse parameters
    index = get_rparam(request, PARAM_FIRST);
    
    // Build query  
    if(NULL == index || '\0' == *index)
        zbx_snprintf(query, sizeof(query), PGSQL_GET_INDEX_ROWS_SUM);
    else
        zbx_snprintf(query, sizeof(query), PGSQL_GET_INDEX_ROWS, index);

    ret = pg_get_int(request, result, query);
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}
