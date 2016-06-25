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

#define MAX_QUERY_LEN           4096
#define MAX_CLAUSE_LEN          4096

#define PGSQL_GET_BACKENDS      "SELECT COUNT(datid) FROM pg_stat_activity WHERE %s != pg_backend_pid()%s;"

#define PGSQL_GET_LONGEST_QUERY_92 "\
SELECT \
  COALESCE( \
    (SELECT \
      EXTRACT(EPOCH FROM NOW()) - EXTRACT(EPOCH FROM query_start) AS duration \
    FROM pg_stat_activity \
    WHERE \
      state = 'active' \
      AND pid != pg_backend_pid() %s\
    ORDER BY duration DESC \
    LIMIT 1), 0);"

#define PGSQL_GET_LONGEST_QUERY     "\
SELECT \
  COALESCE( \
    (SELECT \
      EXTRACT(EPOCH FROM NOW()) - EXTRACT(EPOCH FROM query_start) AS duration \
    FROM pg_stat_activity \
    WHERE \
      current_query NOT IN ('', '<IDLE>', '<insufficient privilege>') \
      AND procpid != pg_backend_pid() %s\
    ORDER BY duration DESC \
    LIMIT 1), 0);"

//  select * from pg_stat_activity WHERE current_query NOT IN (''::text, '<IDLE>'::text, '<insufficient privilege>'::text);
/*
 * build_activity_clause takes agent request parameters for an item key which
 * targets the pg_stat_actity table and creates an SQL clause to filter
 * results.
 *
 * This function should be reused for all items which query the pg_stat_activity
 * table.
 *
 * request is the agent request containing the user parameters.
 * buf is the character buffer to which the clause is written.
 * params is a pointer to a PGparams type which stores query parameters.
 * has_clause determines if the the clause starts with "WITH" or "AND".
 *
 * Returns non-zero on success.
 */
static int build_activity_clause(const AGENT_REQUEST *request, AGENT_RESULT *result, char *buf, PGparams *params, int has_clause) {
    const char  *__function_name = "build_activity_clause";  // Function name for log file

    int         i = 0;
    char        *param = NULL;
    char        *clause = (0 < has_clause ? PG_AND : PG_WHERE);
    int         pgi = 0;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    // iterate over the available parameters
    for(i = 0; i < 4; i++) {
        param = get_rparam(request, PARAM_FIRST + i);
        if(!strisnull(param)) {
            switch(i) {
                case 0: // <database>
                    *params = param_append(*params, param);
                    if(is_oid(param))
                        zbx_snprintf(buf, MAX_CLAUSE_LEN, " %s datid = $%i", clause, ++pgi);
                    else
                        zbx_snprintf(buf, MAX_CLAUSE_LEN, " %s datname = $%i", clause, ++pgi);
                    break;

                case 1: // <user>
                    *params = param_append(*params, param);
                    if(is_oid(param))
                        zbx_snprintf(buf, MAX_CLAUSE_LEN, " %s usesysid = $%i", clause, ++pgi);
                    else
                        zbx_snprintf(buf, MAX_CLAUSE_LEN, " %s usename = $%i", clause, ++pgi);
                    break;

                case 2: // <client>
                    *params = param_append(*params, param);
                    if(is_valid_ip(param))
                        zbx_snprintf(buf, MAX_CLAUSE_LEN, " %s client_addr = $%i::inet", clause, ++pgi);
                    else
                        // requires v9.1+
                        zbx_snprintf(buf, MAX_CLAUSE_LEN, " %s client_hostname = $%i", clause, ++pgi);
                    break;

                case 3: // <waiting>                
                    if(0 == strncmp("true\0", param, 5)) {
                        zbx_snprintf(buf, MAX_CLAUSE_LEN, " %s waiting = TRUE", clause);
                    } else if(0 == strncmp("false\0", param, 6)) {
                        zbx_snprintf(buf, MAX_CLAUSE_LEN, " %s waiting = FALSE", clause);
                    } else {
                        set_err_result(result, "Unsupported parameter value: \"%s\" in %s", param, request->key);
                        return 0;
                    }

                    break;
            }

            buf += strlen(buf);
            clause = PG_AND;
        }
    }

    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);

    return 1;
}

/*
 * Custom key pg.backends.count
 *
 * Returns statistics for connected backends (remote clients)
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  filter by database oid name
 *   3:  filter by user OID or name
 *   4:  filter by hostname or IP address of the connected host
 *   5:  return only waiting backends
 *
 * Returns: u
 */
 int    PG_BACKENDS_COUNT(AGENT_REQUEST *request, AGENT_RESULT *result)
 {
    int         ret = SYSINFO_RET_FAIL;                  // Request result code
    const char  *__function_name = "PG_BACKENDS_COUNT";  // Function name for log file

    char        query[MAX_QUERY_LEN];
    char        clause[MAX_CLAUSE_LEN];
    char        *pid = "pid";
    int         version = 0;
    PGparams    params = NULL; // freed later in pg_exec
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    // pid column is named 'procpid' in < v9.2
    if (0 == (version = pg_version(request, result)))
        goto out;
    else if (version < 90200)
        pid = "procpid";

    // build the filter clause
    memset(clause, 0, sizeof(clause));
    if (0 == build_activity_clause(request, result, clause, &params, 1))
        goto out;

    // build the full sql query
    memset(query, 0, sizeof(query));
    zbx_snprintf(query, MAX_QUERY_LEN, PGSQL_GET_BACKENDS, pid, clause);

    // get results
    ret = pg_get_int(request, result, query, params);

out:
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom key pg.backends.ratio
 *
 * Returns the ratio of used available backend connections
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *
 * Returns: d
 */
 int    PG_BACKENDS_RATIO(AGENT_REQUEST *request, AGENT_RESULT *result)
 {
    int         ret = SYSINFO_RET_FAIL;
    const char  *__function_name = "PG_BACKENDS_RATIO";

    char        query[MAX_QUERY_LEN];
    char        *pid = "pid";
    int         version = 0;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    // pid column is named 'procpid' in < v9.2
    if (0 == (version = pg_version(request, result)))
        goto out;
    else if (version < 90200)
        pid = "procpid";

    // build the full sql query
    memset(query, 0, sizeof(query));
    zbx_snprintf(
        query,
        MAX_QUERY_LEN,
        "\
SELECT \
  CASE \
    WHEN COUNT(datid) = 0 THEN 0.00 \
    ELSE (COUNT(datid)::float / current_setting('max_connections')::integer) \
  END \
FROM pg_stat_activity \
WHERE %s != pg_backend_pid();",
        pid
    );

    // get results
    ret = pg_get_dbl(request, result, query, NULL);

out:
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom key pg.backends.free
 *
 * Returns the number of available backend connections.
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *
 * Returns: d
 */
 int    PG_BACKENDS_FREE(AGENT_REQUEST *request, AGENT_RESULT *result)
 {
    int         ret = SYSINFO_RET_FAIL;
    const char  *__function_name = "PG_BACKENDS_FREE";
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    // The +1 is to account for the connection used by this query.
    ret = pg_get_dbl(
        request,
        result,
        "\
SELECT \
  current_setting('max_connections')::integer - COUNT(datid) + 1 \
FROM pg_stat_activity;",
        NULL
    );

    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom key pg.queries.longest
 *
 * Returns the duration in seconds of the longest running current query
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  filter by database oid name
 *   3:  filter by user OID or name
 *   4:  filter by hostname or IP address of the connected host
 *   5:  return only waiting backends
 *
 * Returns: d
 *
 * Support: Requires PostgreSQL v9.2 or above
 *
 * TODO: allow filtering in pg.queries.longest similar to pg.backends.count
 */
int    PG_QUERIES_LONGEST(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;                     // Request result code
    const char  *__function_name = "PG_QUERIES_LONGEST";    // Function name for log file
    
    char        query[MAX_QUERY_LEN];
    char        clause[MAX_CLAUSE_LEN];
    PGparams    params = NULL; // freed later in pg_exec
    int         version = 0;

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    // build the filter clause
    memset(clause, 0, sizeof(clause));
    if (0 == build_activity_clause(request, result, clause, &params, 1))
        goto out;

    // build the full sql query
    memset(query, 0, MAX_QUERY_LEN);

    if (0 == (version = pg_version(request, result)))
        goto out;
    else if (version < 90200)
        zbx_snprintf(query, MAX_QUERY_LEN, PGSQL_GET_LONGEST_QUERY, clause);
    else
        zbx_snprintf(query, MAX_QUERY_LEN, PGSQL_GET_LONGEST_QUERY_92, clause);
    
    ret = pg_get_dbl(request, result, query, params);
    
out:

    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}
