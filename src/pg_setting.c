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
    name AS setting\
    , unit AS unit \
    , category AS category \
    , short_desc AS description \
    , context AS context \
    , vartype AS vartype \
    , source AS source \
    , boot_val AS bootval \
    , reset_val AS resetval \
    , sourcefile AS sourcefile \
    , sourceline AS sourceline \
FROM pg_settings;"

#define PGSQL_GET_SETTING		     "SELECT setting,vartype FROM pg_settings WHERE name=$1;"

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
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    ret = pg_get_discovery(request, result, PGSQL_DISCOVER_SETTINGS);

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
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // parse parameters
    setting = get_rparam(request, PARAM_FIRST);
    if(NULL == setting || '\0' == *setting) {
        zabbix_log(LOG_LEVEL_ERR, "No setting name specified in %s()", __function_name);
        goto out;
    }

    // Connect to PostreSQL
    if(NULL == (conn = pg_connect(request)))
        goto out;

    // Execute the query
    res = pg_exec_params(conn, PGSQL_GET_SETTING, setting, NULL);
    if(PQresultStatus(res) != PGRES_TUPLES_OK) {
        zabbix_log(LOG_LEVEL_ERR, "Failed to execute PostgreSQL query in %s() with: %s", __function_name, PQresultErrorMessage(res));
        goto out;
    }
    
    if(0 == PQntuples(res)) {
        zabbix_log(LOG_LEVEL_DEBUG, "No results returned for query \"%s\" in %s()", PGSQL_GET_SETTING, __function_name);
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
