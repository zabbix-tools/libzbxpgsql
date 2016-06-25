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

#define PGSQL_GET_VERSION           "SELECT version();"

#define PGSQL_GET_STARTTIME         "SELECT pg_postmaster_start_time();"

#define PGSQL_GET_UPTIME            "SELECT EXTRACT(EPOCH FROM NOW()) - EXTRACT(EPOCH FROM pg_postmaster_start_time());"

/*
 * Custom key pg.connect
 *
 * Returns 1 if the Zabbix Agent can connect to PostgreSQL instance
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *
 * Returns: b
 */
int    PG_CONNECT(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int             ret = SYSINFO_RET_FAIL;
    const char      *__function_name = "PG_CONNECT";
    PGconn          *conn = NULL;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // connect without setting an error message on failure
    conn = pg_connect_request(request, NULL);
            
    if(NULL != conn && CONNECTION_OK == PQstatus(conn)) {
        SET_UI64_RESULT(result, 1);
        PQfinish(conn);
    }
    else {
        SET_UI64_RESULT(result, 0);
    }
    
    // Set result    
    ret = SYSINFO_RET_OK;
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom key pg.version
 *
 * Returns the version string of the connection PostgreSQL Server, E.g.:
 *  PostgreSQL 9.4.4 on x86_64-unknown-linux-gnu, compiled by gcc 
 *  (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3, 64-bit
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *
 * Returns: s
 */
int    PG_VERSION(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int             ret = SYSINFO_RET_FAIL;             // Request result code
    const char      *__function_name = "PG_VERSION";    // Function name for log file
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    ret = pg_get_string(request, result, PGSQL_GET_VERSION, NULL);
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom key pg.starttime
 *
 * Returns the start time of the postmaster daemon. E.g.
 *  2015-08-08 08:00:17.894706+00
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *
 * Returns: s
 */
int    PG_STARTTIME(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int             ret = SYSINFO_RET_FAIL;            // Request result code
    const char      *__function_name = "PG_STARTTIME"; // Function name for log file
    
    char            *query = PGSQL_GET_STARTTIME;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    ret = pg_get_string(request, result, query, NULL);
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom key pg.uptime
 *
 * Returns the uptime of the postmaster daemon in second. E.g.
 *  86400
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *
 * Returns: u
 */
int    PG_UPTIME(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int             ret = SYSINFO_RET_FAIL;         // Request result code
    const char      *__function_name = "PG_UPTIME"; // Function name for log file
    
    char            *query = PGSQL_GET_UPTIME;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    ret = pg_get_int(request, result, query, NULL);
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom key pg.prepared_xacts_count
 *
 * Returns the number of transactions that are currently prepared for two phase
 * commit.
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  filter by database
 *
 * Returns: u
 */
int    PG_PREPARED_XACTS_COUNT(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int             ret = SYSINFO_RET_FAIL;
    const char      *__function_name = "PG_PREPARED_XACTS_COUNT";
    
    char            query[MAX_STRING_LEN];
    char            *datname = NULL;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    // Build query
    datname = get_rparam(request, PARAM_FIRST);
    if(strisnull(datname)) {
        ret = pg_get_int(request, result, "SELECT COUNT (transaction) FROM pg_prepared_xacts;", NULL);
    } else {
        ret = pg_get_int(request, result, "SELECT COUNT (transaction) FROM pg_prepared_xacts WHERE database = $1;", param_new(datname));
    }
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom key pg.prepared_xacts_ratio
 *
 * Returns the number of transactions that are currently prepared for two phase
 * commit as ratio of the maximum allowed prepared transactions.
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *
 * Returns: d
 */
int     PG_PREPARED_XACTS_RATIO(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int             ret = SYSINFO_RET_FAIL;
    const char      *__function_name = "PG_PREPARED_XACTS_RATIO";
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    ret = pg_get_dbl(
        request,
        result,
        "\
SELECT \
  CASE \
    WHEN setting::integer = 0 THEN 0.00 \
    ELSE ((SELECT COUNT (transaction) FROM pg_prepared_xacts)::float / setting::integer) \
  END \
FROM pg_settings \
WHERE name = 'max_prepared_transactions';",
        NULL
    );
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}