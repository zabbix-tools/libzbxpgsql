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

#define PGSQL_DISCOVER_DBS  "\
SELECT \
    d.oid as oid, \
    d.datname as database, \
    pg_catalog.pg_encoding_to_char(d.encoding) as encoding, \
    d.datcollate as lc_collate, \
    d.datctype as lc_ctype, \
    pg_catalog.pg_get_userbyid(d.datdba) as owner, \
    t.spcname as tablespace, \
    pg_catalog.shobj_description(d.oid, 'pg_database') as description \
FROM pg_catalog.pg_database d \
    JOIN pg_catalog.pg_tablespace t on d.dattablespace = t.oid \
WHERE \
    d.datallowconn = 't' \
    AND d.datistemplate = 'n' \
ORDER BY 1;"

#define PGSQL_GET_DB_STAT   "SELECT %s FROM pg_stat_database WHERE datname = $1"

#define PGSQL_GET_DB_STAT_SUM   "SELECT SUM(%s::bigint) FROM pg_stat_database"

#define PGSQL_GET_DB_SIZE   "SELECT pg_catalog.pg_database_size(d.datname) FROM pg_catalog.pg_database d WHERE d.datname = $1"

#define PGSQL_GET_DB_SIZE_SUM   "SELECT SUM(pg_catalog.pg_database_size(d.datname)::bigint) FROM pg_catalog.pg_database d"

/*
 * Custom key pg.db.discovery
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *
 * Returns all known Databases in a PostgreSQL instances
 *
 * Returns:
 * {
 *        "data":[
 *                {
 *                        "{#OID}":"1234",
 *                        "{#DATABASE}":"MyDatabase",
 *                        "{#ENCODING}":"UTF8",
 *                        "{#LC_COLLATE}":"en_US.UTF-8",
 *                        "{#LC_CTYPE}":"en_US.UTF-8",
 *                        "{#TEMPLATE}":"1|0",
 *                        "{#TABLESPACE":"pg_default",
 *                        "{#DESCRIPTION}":"something or otheer"}]}
 */
int    PG_DB_DISCOVERY(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;                 // Request result code
    const char  *__function_name = "PG_DB_DISCOVERY";   // Function name for log file
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
 
    ret = pg_get_discovery(request, result, PGSQL_DISCOVER_DBS, NULL);
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom keys pg.* (for each field in pg_stat_database)
 *
 * Returns the requested statistic for the specified database
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  filter by database name (default: sum of all databases)
 *
 * Returns: u
 */
int    PG_STAT_DATABASE(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;                // Request result code
    const char  *__function_name = "PG_STAT_DATABASE"; // Function name for log file
    
    char        *datname = NULL;
    char        *field;
    char        query[MAX_STRING_LEN];
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // Get stat field from requested key name "pb.db.<field>"
    // No escaping needed as the fields are hard coded
    field = &request->key[6];
    
    // Build query
    datname = get_rparam(request, PARAM_FIRST);
    if(strisnull(datname)) {
        // no database name provided. get sum of all
        zbx_snprintf(query, sizeof(query), PGSQL_GET_DB_STAT_SUM, field);
        datname = NULL; // ensure datname is NULL, not '\0'
    } else {
        zbx_snprintf(query, sizeof(query),  PGSQL_GET_DB_STAT, field);
    }

    // Get results based on type
    if (0 == strncmp(field, "stats_reset", 11)) {
        // stats_reset is a string
        if(strisnull(datname)) {
            // Can't do SUMs on text fields!
            set_err_result(result, "No database specified");
            goto out;
        }

        ret = pg_get_string(request, result, query, param_new(datname));
    }
    else if(0 == strncmp(field, "blk_", 4))
        // all blk_* fields are doubles
        ret = pg_get_dbl(request, result, query, param_new(datname));
    else 
        // all other fields are integers
        ret = pg_get_int(request, result, query, param_new(datname));
    
out:
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

int     PG_DB_BLKS_PERC(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;                 // Request result code
    const char  *__function_name = "PG_DB_BLKS_PERC";   // Function name for log file
    
    char        *dbname = NULL;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    dbname = get_rparam(request, PARAM_FIRST);

    if(strisnull(dbname)) {
        ret = pg_get_percentage(
            request,
            result,
            "pg_stat_database",
            "sum(blks_hit)",
            "sum(blks_hit) + sum(blks_read)",
            NULL,
            NULL);

    } else {
        ret = pg_get_percentage(
            request,
            result,
            "pg_stat_database",
            "blks_hit",
            "blks_hit + blks_read",
            "datname",
            dbname);
    }

    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret; 
}

/*
 * Custom key pg.db.size
 *
 * Returns the size of the specified database in bytes
 *
 * See: https://wiki.postgresql.org/wiki/Disk_Usage
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  filter by database name (default: sum of all databases)
 *
 * Returns: u
 */
int    PG_DB_SIZE(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;             // Request result code
    const char  *__function_name = "PG_DB_SIZE";    // Function name for log file
    
    char        *datname = NULL;
    char        *query = NULL;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // Build query
    datname = get_rparam(request, PARAM_FIRST);
    if (strisnull(datname)) {
        query = PGSQL_GET_DB_SIZE_SUM;
        datname = NULL; // ensure datname is NULL, not '\0'
    } else {
        query = PGSQL_GET_DB_SIZE;
    }

    ret = pg_get_int(request, result, query, param_new(datname));
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}

/*
 * Custom key pg.db.xid_age
 *
 * Returns the age (in transaction count) of the current transaction ID for the
 * given database. If no database is given, the oldest transaction ID is returned.
 *
 * See:
 * https://www.postgresql.org/docs/9.5/static/routine-vacuuming.html#VACUUM-FOR-WRAPAROUND
 * https://github.com/postgres/postgres/blob/master/src/backend/access/transam/varsup.c#L267
 *
 * Parameters:
 *   0:  connection string
 *   1:  connection database
 *   2:  filter by database name (default: max value for all databases)
 *
 * Returns: f
 */
int    PG_DB_XID_AGE(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    int         ret = SYSINFO_RET_FAIL;
    const char  *__function_name = "PG_DB_XID_AGE";
    
    char        *datname = NULL;
    char        *query = NULL;
    PGparams    params = NULL; // freed later in pg_exec

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);

    datname = get_rparam(request, PARAM_FIRST);
    if (strisnull(datname)) {
        query = "SELECT MAX(AGE(datfrozenxid)) FROM pg_database;";
        datname = NULL; // ensure datname is NULL, not '\0'
    } else {
        query = "SELECT AGE(datfrozenxid) FROM pg_database WHERE datname = $1;";
    }

    ret = pg_get_int(request, result, query, param_new(datname));
    
out:

    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return ret;
}   

