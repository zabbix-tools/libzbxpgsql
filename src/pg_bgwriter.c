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

#define PGSQL_GET_BGWRITER_STAT     "SELECT %s FROM pg_stat_bgwriter;"

#define PGSQL_BG_AVG_INTERVAL	"\
SELECT \
	EXTRACT(EPOCH FROM (NOW() - stats_reset)) / (checkpoints_timed + checkpoints_req) \
FROM pg_stat_bgwriter;"

/*
 * Custom keys pg.* (for each field in pg_stat_bgwriter)
 *
 * Returns the requested global statistic for the PostgreSQL server
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
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
        ret = pg_get_dbl(request, result, query, NULL);

    else if(0 == strncmp(field, "stats_reset", 11))
        ret = pg_get_string(request, result, query, NULL);
    
    else
        ret = pg_get_int(request, result, query, NULL);
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

int     PG_BG_AVG_INTERVAL(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	return pg_get_dbl(request, result, PGSQL_BG_AVG_INTERVAL, NULL);
}
