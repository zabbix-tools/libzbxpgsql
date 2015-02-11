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

int     PG_QUERY(AGENT_REQUEST *request, AGENT_RESULT *result)
 {
    int         ret = SYSINFO_RET_FAIL;                 // Request result code
    const char  *__function_name = "PG_QUERY_STRING";   // Function name for log file
    char        *query = NULL;

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    // Get the user SQL query parameter
    query = get_rparam(request, PARAM_FIRST);
    if(NULL == query || '\0' == *query) {
        zabbix_log(LOG_LEVEL_ERR, "No query specified in %s()", __function_name);
        goto out;
    }

    // Return the appropriate result type for this key
    // as per `pg.query.{type}`
    if(0 == strncmp(&request->key[9], "string", 5))
        ret = pg_get_string(request, result, query);
    else if(0 == strncmp(&request->key[9], "integer", 7))
        ret = pg_get_int(request, result, query);
    else if(0 == strncmp(&request->key[9], "double", 6))
        ret = pg_get_dbl(request, result, query);
    else
        zabbix_log(LOG_LEVEL_ERR, "Unsupported query return type: %s in %s()", request->key[9], __function_name);

out:
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}
