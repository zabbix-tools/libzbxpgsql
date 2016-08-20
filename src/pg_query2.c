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
 * Custom key pg.query2.*
 *
 * Returns the value of the first column of the first row returned by the
 * specified SQL query.
 *
 * This is a clone of pg_query.c, but it reads SQL from the config file:
 *   "/etc/libzbxpgsql/query.conf"
 * (using the C library libconfig) instead of reading SQL from the zabbix key.
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  name of query to retrieve from config file
 *   n:  query parameters
 *
 * Returns: u
 */
int     PG_QUERY2(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;         // Request result code
    const char  *__function_name = "PG_QUERY2";  // Function name for log file
    char        *query = NULL, *queryname = NULL;
    int         i = 0;
    PGparams    params = NULL;
    const char  *config_file = "/etc/libzbxpgsql.d/query.conf";  // Config file containing queries
    const char  *queryconst;
    config_t    cfg;

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    config_init(&cfg);

    // Get the user SQL query name parameter
    queryname = get_rparam(request, PARAM_FIRST);
    if(NULL == queryname || '\0' == *queryname) {
        set_err_result(result, "Query name missing from key");
        goto out;
    }
    zabbix_log(LOG_LEVEL_DEBUG, "Query name: %s", queryname);

    // read the config file into memory
    if(! config_read_file(&cfg, config_file)) {
        set_err_result(result, "%s \"%s\"", config_error_text(&cfg), config_file);
        goto out;
    }

    // get the query
    if(! config_lookup_string(&cfg, queryname, &queryconst)) {
        set_err_result(result, "Query \"%s\" not found in %s", queryname, config_file);
        goto out;
    }

    // copy to non-constant
    query = zbx_malloc(query, sizeof(char) * strlen(queryconst));
    zbx_strlcpy(query, queryconst, strlen(queryconst));
    zabbix_log(LOG_LEVEL_DEBUG, "Query retrieved: %s", query);

    // parse user params
    zabbix_log(LOG_LEVEL_DEBUG, "Appending %i params to query", request->nparam - 3);
    for (i = 3; i < request->nparam; i++) {
        params = param_append(params, get_rparam(request, i));
    }

    // Return the appropriate result type for this key
    // as per `pg.query2.{type}`
    if(0 == strncmp(&request->key[10], "string", 5))
        ret = pg_get_string(request, result, query, params);

    else if(0 == strncmp(&request->key[10], "integer", 7))
        ret = pg_get_int(request, result, query, params);

    else if(0 == strncmp(&request->key[10], "double", 6))
        ret = pg_get_dbl(request, result, query, params);

    else if(0 == strncmp(&request->key[10], "discovery", 9))
        ret = pg_get_discovery(request, result, query, params);

    else
        set_err_result(result, "Unsupported query type: %s", request->key[10]);

    zbx_free(query);

out:
    config_destroy(&cfg);
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}
