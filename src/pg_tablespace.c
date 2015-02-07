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

#define PGSQL_DISCOVER_TABLESPACES  "SELECT t.oid, t.spcname, a.rolname from pg_tablespace t JOIN pg_authid a ON a.oid = t.spcowner"

#define PGSQL_GET_TS_SIZE       "SELECT pg_tablespace_size('%s')"
/*
 * Custom key pg.tablespace.discovery
 *
 * Returns all known tablespaces in a PostgreSQL instance
 *
 * Parameter [0-4]:     <host,port,db,user,passwd>
 *
 * Returns:
 * {
 *        "data":[
 *                {
 *                        "{#OID}":"12345",
 *                        "{#TABLESPACE}":"MyTableSpace",
 *                        "{#OWNER}":"postgres"}]}
 */
int    PG_TABLESPACE_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;                         // Request result code
    const char  *__function_name = "PG_TABLESPACE_DISCOVERY";   // Function name for log file
    struct      zbx_json j;                                     // JSON response for discovery rule
    
    PGconn      *conn = NULL;
    PGresult    *res = NULL;
    
    char        query[MAX_STRING_LEN] = PGSQL_DISCOVER_TABLESPACES;
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
    }
             
    // Create JSON array of discovered objects
    zbx_json_init(&j, ZBX_JSON_STAT_BUF_LEN);
    zbx_json_addarray(&j, ZBX_PROTO_TAG_DATA);
    
    for(i = 0; i < count; i++) {
        zbx_json_addobject(&j, NULL);        
        zbx_json_addstring(&j, "{#OID}", PQgetvalue(res, i, 0), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#TABLESPACE}", PQgetvalue(res, i, 1), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#OWNER}", PQgetvalue(res, i, 2), ZBX_JSON_TYPE_STRING);
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
 * Custom key pg.tablespace.size
 *
 * Returns the size of the specified tablespace in bytes
 *
 * Parameter [0-4]:         <host,port,db,user,passwd>
 *
 * Parameter[tablespace]:   <tablespace>
 *
 * Returns: u
 */
int    PG_TABLESPACE_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;                     // Request result code
    const char  *__function_name = "PG_TABLESPACE_SIZE";    // Function name for log file

    char        *tablespace = NULL;
    char        query[MAX_STRING_LEN];
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // Build query
    tablespace = get_rparam(request, PARAM_FIRST);
    if(NULL == tablespace || '\0' == *tablespace) {
        zabbix_log(LOG_LEVEL_ERR, "No tablespace specified in %s()", __function_name);
        goto out;
    }
    else
        zbx_snprintf(query, sizeof(query), PGSQL_GET_TS_SIZE, tablespace);

    ret = pg_get_int(request, result, query);
    
out:
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}
