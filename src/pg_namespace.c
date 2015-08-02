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

#define PGSQL_DISCOVER_NAMESPACES   "\
SELECT  \
  n.oid as oid, \
  n.nspname AS namespace, \
  current_database() as database, \
  pg_catalog.pg_get_userbyid(n.nspowner) AS owner, \
  pg_catalog.obj_description(n.oid, 'pg_namespace') AS description \
FROM pg_catalog.pg_namespace n \
WHERE  \
  n.nspname !~ '^pg_'  \
  AND n.nspname <> 'information_schema' \
ORDER BY namespace;"

#define PGSQL_GET_NS_SIZE           "SELECT sum(pg_relation_size(quote_ident(schemaname) || '.' || quote_ident(tablename)))::bigint FROM pg_tables WHERE schemaname = '%s'"

/*
 * Custom key pg.namespace.discovery
 *
 * Returns all known schemas/namespaces in a PostgreSQL database
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *
 * Returns:
 * {
 *        "data":[
 *                {
 *                        "{#OID}":"12345",
 *                        "{#SCHEMA}":"public",
 *                        "{#DATABASE}:"MyDb",
 *                        "{#OWNER}":"postgres"}]}
 */
int    PG_NAMESPACE_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;                         // Request result code
    const char  *__function_name = "PG_NAMESPACE_DISCOVERY";    // Function name for log file

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    ret = pg_get_discovery(request, result, PGSQL_DISCOVER_NAMESPACES);

    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom key pg.namespace.size
 *
 * Returns the disk usage in bytes for the specified namespace/schema
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  filter by schema name (default: sum of all schema)
 *
 * Returns: u
 */
int    PG_NAMESPACE_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int             ret = SYSINFO_RET_FAIL;             // Request result code
    const char          *__function_name = "PG_NAMESPACE_SIZE"; // Function name for log file
        
    char        query[MAX_STRING_LEN];
    char        *schema = NULL;
            
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // Parse parameters
    schema = get_rparam(request, PARAM_FIRST);
    
    // Build query  
    if(NULL == schema || '\0' == *schema) {
    zabbix_log(LOG_LEVEL_ERR, "No schema name specified in %s()", __function_name);
    goto out;
    }
    else
    zbx_snprintf(query, sizeof(query), PGSQL_GET_NS_SIZE, schema);

    ret = pg_get_int(request, result, query);

out:

    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}
