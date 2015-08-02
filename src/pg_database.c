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

#define PGSQL_DISCOVER_DBS  "\
SELECT  \
    d.oid as oid, \
    d.datname as database, \
    pg_catalog.pg_encoding_to_char(d.encoding) as encoding, \
    d.datcollate as lc_collate, \
    d.datctype as lc_ctype, \
    pg_catalog.pg_get_userbyid(d.datdba) as owner, \
    t.spcname as tablespace, \
    pg_catalog.shobj_description(d.oid, 'pg_database') as description \
FROM pg_catalog.pg_database d \
    JOIN pg_catalog.pg_tablespace t on d.dattablespace = t.oid \
WHERE \
    d.datallowconn = 't' \
    AND d.datistemplate = 'n' \
ORDER BY 1;"

#define PGSQL_GET_DB_STAT   "SELECT %s FROM pg_stat_database WHERE datname = '%s'"

#define PGSQL_GET_DB_STAT_SUM   "SELECT SUM(%s) FROM pg_stat_database"

#define PGSQL_GET_DB_SIZE   "SELECT pg_catalog.pg_database_size(d.datname) FROM pg_catalog.pg_database d WHERE d.datname = '%s'"

#define PGSQL_GET_DB_SIZE_SUM   "SELECT SUM(pg_catalog.pg_database_size(d.datname)) FROM pg_catalog.pg_database d"

/*
 * Custom key pg.db.discovery
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *
 * Returns all known Databases in a PostgreSQL instances
 *
 * Returns:
 * {
 *        "data":[
 *                {
 *                        "{#DATNAME}":"MyDatabase",
 *                        "{#LC_COLLATE}":"en_US.UTF-8",
 *                        "{#LC_CTYPE}":"en_US.UTF-8",
 *                        "{#TEMPLATE}":"1|0",
 *                        "{#TABLESPACE":"pg_default"}]}
 */
int    PG_DB_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;                 // Request result code
    const char  *__function_name = "PG_DB_DISCOVERY";   // Function name for log file
    struct      zbx_json j;                             // JSON response for discovery rule
    
    PGconn      *conn = NULL;
    PGresult    *res = NULL;
    
    // Ignore tables that don't allow connections as we can't monitor them anyway!
    char        *query = PGSQL_DISCOVER_DBS;
    int         i = 0, count = 0;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // Connect to PostreSQL
    if(NULL == (conn = pg_connect(request)))
        goto out;
    
    // Execute a query
    res = PQexec(conn, query);
    if(PQresultStatus(res) != PGRES_TUPLES_OK) {
        zabbix_log(LOG_LEVEL_ERR, "Failed to execute PostgreSQL query in %s() with: %s", __function_name, PQresultErrorMessage(res));
        goto out;
    }
    
    if(0 == (count = PQntuples(res))) {
        zabbix_log(LOG_LEVEL_DEBUG, "No results returned for query \"%s\" in %s()", query, __function_name);
        goto out;
    }
             
    // Create JSON array of discovered objects
    zbx_json_init(&j, ZBX_JSON_STAT_BUF_LEN);
    zbx_json_addarray(&j, ZBX_PROTO_TAG_DATA);
    
    for(i = 0; i < count; i++) {
        zbx_json_addobject(&j, NULL);
        zbx_json_addstring(&j, "{#OID}", PQgetvalue(res, i, 0), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#DATABASE}", PQgetvalue(res, i, 1), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#ENCODING}", PQgetvalue(res, i, 2), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#LC_COLLATE}", PQgetvalue(res, i, 3), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#LC_CTYPE}", PQgetvalue(res, i, 4), ZBX_JSON_TYPE_STRING); 
        zbx_json_addstring(&j, "{#OWNER}", PQgetvalue(res, i, 5), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#TABLESPACE}", PQgetvalue(res, i, 6), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#DESCRIPTION}", PQgetvalue(res, i, 7), ZBX_JSON_TYPE_STRING);
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
 * Custom keys pg.* (for each field in pg_stat_database)
 *
 * Returns the requested statistic for the specified database
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  filter by database name (default: sum of all databases)
 *
 * Returns: u
 */
int    PG_STAT_DATABASE(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;                 // Request result code
    const char  *__function_name = "PG_STAT_DATABASE";  // Function name for log file
    
    char        *datname = NULL;
    char        *field;
    char        query[MAX_STRING_LEN];
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // Get stat field from requested key name "pb.db.<field>"
    field = &request->key[6];
    
    // Build query
    datname = get_rparam(request, PARAM_FIRST);
    if(NULL == datname || '\0' == *datname)
        zbx_snprintf(query, sizeof(query), PGSQL_GET_DB_STAT_SUM, field);
    else
        zbx_snprintf(query, sizeof(query),  PGSQL_GET_DB_STAT, field, datname);

    // Get results based on type
    if (0 == strncmp(field, "stats_reset", 11)) {
        if(NULL == datname || '\0' == *datname) {
            // Can't do SUMs on text fields!
            zabbix_log(LOG_LEVEL_ERR, "No database specified bro, in %s", __function_name);
            goto out;
        }
        ret = pg_get_string(request, result, query);
    }
    else if(0 == strncmp(field, "blk_", 4))
        ret = pg_get_dbl(request, result, query);
    else 
        ret = pg_get_int(request, result, query);
    
out:
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom key pg.db.size
 *
 * Returns the size of the specified database in bytes
 *
 * See: https://wiki.postgresql.org/wiki/Disk_Usage
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  filter by database name (default: sum of all databases)
 *
 * Returns: u
 */
int    PG_DB_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;             // Request result code
    const char  *__function_name = "PG_DB_SIZE";    // Function name for log file
    
    char        *datname = NULL;
    char        query[MAX_STRING_LEN];
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // Build query
    datname = get_rparam(request, PARAM_FIRST);
    if(NULL == datname || '\0' == *datname)
        zbx_snprintf(query, sizeof(query), PGSQL_GET_DB_SIZE_SUM);
    else
        zbx_snprintf(query, sizeof(query), PGSQL_GET_DB_SIZE, datname);

    ret = pg_get_int(request, result, query);
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}
