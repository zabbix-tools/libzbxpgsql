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

int     PG_QUERY_STRING(AGENT_REQUEST *request, AGENT_RESULT *result)
 {
    int         ret = SYSINFO_RET_FAIL;                 // Request result code
    const char  *__function_name = "PG_QUERY_STRING";   // Function name for log file
    char        *query = NULL;

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    query = get_rparam(request, PARAM_FIRST);
    if(NULL == query || '\0' == *query) {
        zabbix_log(LOG_LEVEL_ERR, "No query specified in %s()", __function_name);
        goto out;
    }

    ret = pg_get_string(request, result, query);

out:
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

int     PG_QUERY_INTEGER(AGENT_REQUEST *request, AGENT_RESULT *result)
 {
    int         ret = SYSINFO_RET_FAIL;                 // Request result code
    const char  *__function_name = "PG_QUERY_INTEGER";   // Function name for log file
    char        *query = NULL;

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    query = get_rparam(request, PARAM_FIRST);
    if(NULL == query || '\0' == *query) {
        zabbix_log(LOG_LEVEL_ERR, "No query specified in %s()", __function_name);
        goto out;
    }

    ret = pg_get_int(request, result, query);

out:
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

int     PG_QUERY_DOUBLE(AGENT_REQUEST *request, AGENT_RESULT *result)
 {
    int         ret = SYSINFO_RET_FAIL;                 // Request result code
    const char  *__function_name = "PG_QUERY_DOUBLE";   // Function name for log file
    char        *query = NULL;

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    query = get_rparam(request, PARAM_FIRST);
    if(NULL == query || '\0' == *query) {
        zabbix_log(LOG_LEVEL_ERR, "No query specified in %s()", __function_name);
        goto out;
    }

    ret = pg_get_dbl(request, result, query);

out:
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}
