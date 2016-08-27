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

/*
 * Custom key pg.query.*
 *
 * Returns the value of the first column of the first row returned by the
 * specified SQL query.
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  scalar SQL query to execute
 *   n:  query parameters
 *
 * Returns: u
 */
int     PG_QUERY(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;         // Request result code
    const char  *__function_name = "PG_QUERY";  // Function name for log file
    char        *query = NULL;
    int         i = 0;
    PGparams    params = NULL;

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    // Get the user SQL query parameter
    query = get_rparam(request, PARAM_FIRST);
    if(NULL == query || '\0' == *query) {
        set_err_result(result, "No query specified");
        goto out;
    }

    // Check if query comes from configs
    if(SQLkeysearch(query) >= 0) {
        zabbix_log(LOG_LEVEL_INFORMATION, "Found key \"%i\" in memory", *query);
        query = SQLstmt[SQLkeysearch(query)];
        zabbix_log(LOG_LEVEL_INFORMATION, "Now SQL query = \"%i\"", *query);
    }

    // parse user params
    zabbix_log(LOG_LEVEL_DEBUG, "Appending %i params to query", request->nparam - 3);
    for (i = 3; i < request->nparam; i++) {
        params = param_append(params, get_rparam(request, i));
    }

    // Return the appropriate result type for this key
    // as per `pg.query.{type}`
    if(0 == strncmp(&request->key[9], "string", 5))
        ret = pg_get_string(request, result, query, params);

    else if(0 == strncmp(&request->key[9], "integer", 7))
        ret = pg_get_int(request, result, query, params);

    else if(0 == strncmp(&request->key[9], "double", 6))
        ret = pg_get_dbl(request, result, query, params);

    else if(0 == strncmp(&request->key[9], "discovery", 9))
        ret = pg_get_discovery(request, result, query, params);

    else
        set_err_result(result, "Unsupported query type: %s", request->key[9]);

out:
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}
