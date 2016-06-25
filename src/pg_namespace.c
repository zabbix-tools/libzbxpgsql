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
  n.oid AS oid, \
  current_database() || '.' || n.nspname AS path, \
  n.nspname AS schema, \
  n.nspname AS namespace, \
  current_database() AS database, \
  pg_catalog.pg_get_userbyid(n.nspowner) AS owner, \
  pg_catalog.obj_description(n.oid, 'pg_namespace') AS description \
FROM pg_catalog.pg_namespace n \
WHERE  \
  n.nspname !~ '^pg_'  \
  AND n.nspname <> 'information_schema' \
ORDER BY namespace;"

#define PGSQL_GET_NS_SIZE           "\
SELECT \
  SUM(pg_relation_size(quote_ident(schemaname) || '.' || quote_ident(tablename))::bigint) \
FROM pg_tables \
WHERE schemaname = $1"

/*
 * Custom key pg.namespace.discovery
 *
 * Returns all known schemas/namespaces in a PostgreSQL database
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  search mode: deep (default) | shallow
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
    char        *mode = NULL;

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    mode = get_rparam(request, PARAM_FIRST);

    if (strisnull(mode) || 0 == strcmp(mode, "deep")) {
      // search all connectable databases
      ret = pg_get_discovery_wide(request, result, PGSQL_DISCOVER_NAMESPACES, NULL);
    } else if (0 == strcmp(mode, "shallow")) {
      // search only connected database
      ret = pg_get_discovery(request, result, PGSQL_DISCOVER_NAMESPACES, NULL);
    } else {
      set_err_result(result, "Invalid search mode parameter: %s", mode);
    }
    
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
    int         ret = SYSINFO_RET_FAIL;                 // Request result code
    const char  *__function_name = "PG_NAMESPACE_SIZE"; // Function name for log file
    char        *schema = NULL;
            
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // Parse parameters
    schema = get_rparam(request, PARAM_FIRST);
    
    // Build query  
    if(strisnull(schema)) {
      set_err_result(result, "No schema name specified");
      goto out;
    }

    ret = pg_get_int(request, result, PGSQL_GET_NS_SIZE, param_new(schema));

out:

    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}
