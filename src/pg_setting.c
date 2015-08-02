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

/*
 * See: http://www.postgresql.org/docs/9.4/static/view-pg-settings.html
 */

#include "libzbxpgsql.h"

#define PGSQL_DISCOVER_SETTINGS     "\
SELECT \
    name \
    , unit \
    , category \
    , short_desc \
    , context \
    , vartype \
    , source \
    , boot_val \
    , reset_val \
    , sourcefile \
    , sourceline \
FROM pg_settings;"

#define PGSQL_GET_SETTING		     "SELECT setting,vartype FROM pg_settings WHERE name='%s';"

/*
 * Custom key pg.setting.discovery
 *
 * Returns all known configuration settings
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *
 * Returns:
 * {
 *        "data":[
 *                {
 *                        "{#SETTING}":"MyDatabase",
 *                        "{#UNIT}":"s|kB|etc.",
 *                        "{#CATEGORY}":"File locations|Autovacuum|etc.",
 *                        "{#DESCRIPTION}":"Sets the server's main configuration file.",
 *                        "{#CONTEXT}":"postmaster|sighup|etc.",
 *                        "{#VARTYPE}":"bool|string|integer|enum|real",
 *                        "{#SOURCE}":"default|session|override|etc.",
 *                        "{#BOOTVAL}":"default",
 *                        "{#RESETVAL}":"default",
 *                        "{#SOURCEFILE}":"/var/lib/pgsql/9.4/data/postgresql.conf",
 *                        "{#SOURCELINE}":"123"}]}
 */
int    PG_SETTING_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;                     // Request result code
    const char  *__function_name = "PG_SETTING_DISCOVERY";  // Function name for log file
    struct      zbx_json j;                                 // JSON response for discovery rule
    
    PGconn      *conn = NULL;
    PGresult    *res = NULL;
    
    char        *query = PGSQL_DISCOVER_SETTINGS;
    int         i = 0, count = 0;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // Connect to PostgreSQL
    if(NULL == (conn = pg_connect(request)))
        goto out;
    
    // Execute a query
    res = pg_exec(conn, query);
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
        zbx_json_addstring(&j, "{#SETTING}", PQgetvalue(res, i, 0), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#UNIT}", PQgetvalue(res, i, 1), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#CATEGORY}", PQgetvalue(res, i, 2), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#DESCRIPTION}", PQgetvalue(res, i, 3), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#CONTEXT}", PQgetvalue(res, i, 4), ZBX_JSON_TYPE_STRING); 
        zbx_json_addstring(&j, "{#VARTYPE}", PQgetvalue(res, i, 5), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#SOURCE}", PQgetvalue(res, i, 6), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#BOOTVAL}", PQgetvalue(res, i, 7), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#RESETVAL}", PQgetvalue(res, i, 8), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#SOURCEFILE}", PQgetvalue(res, i, 9), ZBX_JSON_TYPE_STRING);
        zbx_json_addstring(&j, "{#SOURCELINE}", PQgetvalue(res, i, 10), ZBX_JSON_TYPE_STRING);
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
 * Function: PG_SETTING
 *
 * Provides access to run-time parameters of the server such as those returned
 * by `SHOW` commands.
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  run-time configuration parameter name
 *
 * Returns: determined by parameter vartype
 */
 int    PG_SETTING(AGENT_REQUEST *request, AGENT_RESULT *result)
 {
    int         ret = SYSINFO_RET_FAIL;             // Request result code
    const char  *__function_name = "PG_SETTING"; 	// Function name for log file
    
    PGconn      *conn = NULL;
    PGresult    *res = NULL;

    char        *setting = NULL;
    char        *value = NULL;
    char        *type = NULL;
    char        query[MAX_STRING_LEN];
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // parse parameters
    setting = get_rparam(request, PARAM_FIRST);
    if(NULL == setting || '\0' == *setting) {
        zabbix_log(LOG_LEVEL_ERR, "No setting name specified in %s()", __function_name);
        goto out;
    }

    // Build the query
    zbx_snprintf(query, sizeof(query), PGSQL_GET_SETTING, setting);    
    
    // Connect to PostreSQL
    if(NULL == (conn = pg_connect(request)))
        goto out;

    // Execute the query
    res = pg_exec(conn, query);
    if(PQresultStatus(res) != PGRES_TUPLES_OK) {
        zabbix_log(LOG_LEVEL_ERR, "Failed to execute PostgreSQL query in %s() with: %s", __function_name, PQresultErrorMessage(res));
        goto out;
    }
    
    if(0 == PQntuples(res)) {
        zabbix_log(LOG_LEVEL_DEBUG, "No results returned for query \"%s\" in %s()", query, __function_name);
        goto out;
    }
    
    // Set result by type
    value = strdup(PQgetvalue(res, 0, 0));
    type = strdup(PQgetvalue(res, 0, 1));

    if(0 == strncmp("integer", type, 7))
    	SET_UI64_RESULT(result, strtoull(value, NULL, 10));
   	else if(0 == strncmp("real", type, 4))
   		SET_DBL_RESULT(result, strtold(value, NULL));
   	else
   		SET_STR_RESULT(result, value);

    ret = SYSINFO_RET_OK;
    
out:
    PQclear(res);
    PQfinish(conn);
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s)", __function_name, request->key);
    return ret;
}
