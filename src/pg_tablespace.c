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

#define PGSQL_DISCOVER_TABLESPACES  "\
SELECT  \
  t.oid AS oid \
  , t.spcname AS tablespace \
  , pg_catalog.pg_get_userbyid(spcowner) AS owner \
  , pg_catalog.shobj_description(oid, 'pg_tablespace') AS description \
FROM pg_catalog.pg_tablespace t \
ORDER BY t.spcname;"

#define PGSQL_DISCOVER_TABLESPACES_92  "\
SELECT  \
  t.oid AS oid \
  , t.spcname AS tablespace \
  , pg_catalog.pg_get_userbyid(spcowner) AS owner \
  , pg_catalog.pg_tablespace_location(oid) AS location \
  , pg_catalog.shobj_description(oid, 'pg_tablespace') AS description \
FROM pg_catalog.pg_tablespace t \
ORDER BY t.spcname;"

#define PGSQL_GET_TS_SIZE       "SELECT pg_tablespace_size($1)"

/*
 * Custom key pg.tablespace.discovery
 *
 * Returns all known tablespaces in a PostgreSQL instance
 *
 * Parameters:
 *   0:  connection string
 *
 * Returns:
 * {
 *        "data":[
 *                {
 *                        "{#OID}":"12345",
 *                        "{#TABLESPACE}":"MyTableSpace",
 *                        "{#OWNER}":"postgres",
 *                        "{#LOCATION}":"/var/lib/pgsql/9.4/data",
 *                        "{#DESCRIPTION}":"Default tablespace"}]}
 */
int    PG_TABLESPACE_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;                         // Request result code
    const char  *__function_name = "PG_TABLESPACE_DISCOVERY";   // Function name for log file
    
    int         version = 0;

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    if (0 == (version = pg_version(request, result)))
      goto out;
    else if (version >= 90200)
        // tablespace location introduced in v9.2
        ret = pg_get_discovery(request, result, PGSQL_DISCOVER_TABLESPACES_92, NULL);
    else
        ret = pg_get_discovery(request, result, PGSQL_DISCOVER_TABLESPACES, NULL);

out:

    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom key pg.tablespace.size
 *
 * Returns the size of the specified tablespace in bytes
 *
 * Parameters:
 *   0:  connection string
 *   1:  tablespace name
 *
 * Returns: u
 */
int    PG_TABLESPACE_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;                     // Request result code
    const char  *__function_name = "PG_TABLESPACE_SIZE";    // Function name for log file
    char        *tablespace = NULL;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // Build query
    tablespace = get_rparam(request, PARAM_FIRST);
    if(strisnull(tablespace)) {
        set_err_result(result, "No tablespace specified");
        goto out;
    }
    
    // execute query
    ret = pg_get_int(request, result, PGSQL_GET_TS_SIZE, param_new(tablespace));
    
out:
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}
