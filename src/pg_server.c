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

#define PGSQL_GET_BGWRITER_STAT     "SELECT %s FROM pg_stat_bgwriter"

#define PGSQL_GET_VERSION           "SELECT version();"

/*
 * Custom key pg.connect
 *
 * Returns 1 if the Zabbix Agent can connect to PostgreSQL instance
 *
 * Parameters:
 *   0:  connection string
 *
 * Returns: b
 */
int    PG_CONNECT(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int             ret = SYSINFO_RET_FAIL;             // Request result code
    const char          *__function_name = "PG_CONNECT";        // Function name for log file
    PGconn      *conn = NULL;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    conn = pg_connect(request);
            
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
 *
 * Returns: s
 */
int    PG_VERSION(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int             ret = SYSINFO_RET_FAIL;             // Request result code
    const char      *__function_name = "PG_VERSION";    // Function name for log file
    
    // Ignore tables that don't allow connections as we can't monitor them anyway!
    char            *query = PGSQL_GET_VERSION;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    ret = pg_get_string(request, result, query);
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom keys pg.* (for each field in pg_stat_bgwriter)
 *
 * Returns the requested global statistic for the PostgreSQL server
 *
 * Parameters:
 *   0:  connection string
 *
 * Returns: u
 */
int    PG_STAT_BGWRITER(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;                 // Request result code
    const char  *__function_name = "PG_STAT_BGWRITER";  // Function name for log file
    
    char        *field;
    char        query[MAX_STRING_LEN];
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // Get stat field from requested key name "pb.table.<field>"
    field = &request->key[3];
    
    // Build query
    zbx_snprintf(query, sizeof(query), PGSQL_GET_BGWRITER_STAT, field);
    
    // Get field value
    if(0 == strncmp(field, "checkpoint_", 11))
        ret = pg_get_dbl(request, result, query);
    else if(0 == strncmp(field, "stats_reset", 11))
        ret = pg_get_string(request, result, query);
    else
        ret = pg_get_int(request, result, query);
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}