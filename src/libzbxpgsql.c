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

/*
 * See:
 *     LibPQ:       http://www.postgresql.org/docs/9.4/static/libpq.html
 *     Statistics:  http://www.postgresql.org/docs/9.4/static/monitoring-stats.html
 */

#include "libzbxpgsql.h"

// Define custom keys
static ZBX_METRIC keys[] =
/*      KEY                         FLAG            FUNCTION                        TEST PARAMETERS */
{
    {"pg.connect",                  CF_HAVEPARAMS,  PG_CONNECT,                     NULL},
    {"pg.version",                  CF_HAVEPARAMS,  PG_VERSION,                     NULL},

    {"pg.setting",                  CF_HAVEPARAMS,  PG_SETTING,                     ",,,,,data_directory"},
    {"pg.setting.discovery",        CF_HAVEPARAMS,  PG_SETTING_DISCOVERY,           NULL},

    // User queries
    {"pg.query.string",             CF_HAVEPARAMS,  PG_QUERY,                       ",,,,,SELECT 'Lorem ipsum dolor';"},
    {"pg.query.integer",            CF_HAVEPARAMS,  PG_QUERY,                       ",,,,,SELECT pg_backend_pid();"},
    {"pg.query.double",             CF_HAVEPARAMS,  PG_QUERY,                       ",,,,,SELECT CAST(1234 AS double precision);"},
    
    // Client connection statistics
    {"pg.backends.count",           CF_HAVEPARAMS,  PG_BACKENDS_COUNT,              NULL},
    {"pg.queries.longest",          CF_HAVEPARAMS,  PG_QUERIES_LONGEST,             NULL},

    // Server statistics (as per pg_stat_bgwriter)
    {"pg.checkpoints_timed",        CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    {"pg.checkpoints_req",          CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    {"pg.checkpoint_write_time",    CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    {"pg.checkpoint_sync_time",     CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    {"pg.buffers_checkpoint",       CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    {"pg.buffers_clean",            CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    {"pg.maxwritten_clean",         CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    {"pg.buffers_backend",          CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    {"pg.buffers_backend_fsync",    CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    {"pg.buffers_alloc",            CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    {"pg.stats_reset",              CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    
    // Asset discovery
    {"pg.db.discovery",             CF_HAVEPARAMS,  PG_DB_DISCOVERY,                NULL},
    {"pg.namespace.discovery",      CF_HAVEPARAMS,  PG_NAMESPACE_DISCOVERY,         NULL},
    {"pg.schema.discovery",         CF_HAVEPARAMS,  PG_NAMESPACE_DISCOVERY,         NULL}, // Alias for pg.namespace.discovery
    {"pg.tablespace.discovery",     CF_HAVEPARAMS,  PG_TABLESPACE_DISCOVERY,        NULL},
    {"pg.table.discovery",          CF_HAVEPARAMS,  PG_TABLE_DISCOVERY,             NULL},
    {"pg.table.children.discovery", CF_HAVEPARAMS,  PG_TABLE_CHILDREN_DISCOVERY,    ",,,,,pg_proc"},
    {"pg.index.discovery",          CF_HAVEPARAMS,  PG_INDEX_DISCOVERY,             NULL},
    
    // Asset class sizes
    {"pg.db.size",                  CF_HAVEPARAMS,  PG_DB_SIZE,                     NULL},
    {"pg.table.size",               CF_HAVEPARAMS,  PG_TABLE_SIZE,                  NULL},
    {"pg.table.rows",               CF_HAVEPARAMS,  PG_TABLE_ROWS,                  NULL},
    {"pg.index.size",               CF_HAVEPARAMS,  PG_INDEX_SIZE,                  NULL},
    {"pg.index.rows",               CF_HAVEPARAMS,  PG_INDEX_ROWS,                  NULL},
    {"pg.tablespace.size",          CF_HAVEPARAMS,  PG_TABLESPACE_SIZE,             ",,,,,pg_default"},
    {"pg.namespace.size",           CF_HAVEPARAMS,  PG_NAMESPACE_SIZE,              ",,,,,pg_catalog"},
    {"pg.schema.size",              CF_HAVEPARAMS,  PG_NAMESPACE_SIZE,              ",,,,,pg_catalog"}, // Alias for pg.namespace.size
    
    // Database statistics (as per pg_stat_database)
    {"pg.db.numbackends",           CF_HAVEPARAMS,  PG_STAT_DATABASE,               NULL},
    {"pg.db.xact_commit",           CF_HAVEPARAMS,  PG_STAT_DATABASE,               NULL},
    {"pg.db.xact_rollback",         CF_HAVEPARAMS,  PG_STAT_DATABASE,               NULL},
    {"pg.db.blks_read",             CF_HAVEPARAMS,  PG_STAT_DATABASE,               NULL},
    {"pg.db.blks_hit",              CF_HAVEPARAMS,  PG_STAT_DATABASE,               NULL},
    {"pg.db.tup_returned",          CF_HAVEPARAMS,  PG_STAT_DATABASE,               NULL},
    {"pg.db.tup_fetched",           CF_HAVEPARAMS,  PG_STAT_DATABASE,               NULL},
    {"pg.db.tup_inserted",          CF_HAVEPARAMS,  PG_STAT_DATABASE,               NULL},
    {"pg.db.tup_updated",           CF_HAVEPARAMS,  PG_STAT_DATABASE,               NULL},
    {"pg.db.tup_deleted",           CF_HAVEPARAMS,  PG_STAT_DATABASE,               NULL},
    {"pg.db.conflicts",             CF_HAVEPARAMS,  PG_STAT_DATABASE,               NULL},
    {"pg.db.temp_files",            CF_HAVEPARAMS,  PG_STAT_DATABASE,               NULL},
    {"pg.db.temp_bytes",            CF_HAVEPARAMS,  PG_STAT_DATABASE,               NULL},
    {"pg.db.deadlocks",             CF_HAVEPARAMS,  PG_STAT_DATABASE,               NULL},
    {"pg.db.blk_read_time",         CF_HAVEPARAMS,  PG_STAT_DATABASE,               NULL},
    {"pg.db.blk_write_time",        CF_HAVEPARAMS,  PG_STAT_DATABASE,               NULL},
    {"pg.db.stats_reset",           CF_HAVEPARAMS,  PG_STAT_DATABASE,               ",,postgres,,,"},
    
    // Table partition info
    {"pg.table.children",           CF_HAVEPARAMS,  PG_TABLE_CHILDREN,              ",,,,,pg_database"},
    {"pg.table.children.size",      CF_HAVEPARAMS,  PG_TABLE_CHILDREN_SIZE,         ",,,,,pg_database"},
    {"pg.table.children.tuples",    CF_HAVEPARAMS,  PG_TABLE_CHILDREN_TUPLES,       ",,,,,pg_database"},
    
    // Table statistics (as per pg_stat_all_tables)
    {"pg.table.seq_scan",           CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.seq_tup_read",       CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.idx_scan",           CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.idx_tup_fetch",      CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.n_tup_ins",          CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.n_tup_upd",          CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.n_tup_del",          CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.n_tup_hot_upd",      CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.n_live_tup",         CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.n_dead_tup",         CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.last_vacuum",        CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             ",,,,,pg_database"},
    {"pg.table.last_autovacuum",    CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             ",,,,,pg_database"},
    {"pg.table.last_analyze",       CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             ",,,,,pg_database"},
    {"pg.table.last_autoanalyze",   CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             ",,,,,pg_database"},
    {"pg.table.vacuum_count",       CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.autovacuum_count",   CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.analyze_count",      CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.autoanalyze_count",  CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    
    // Table IO Statistics (as per pg_statio_all_tables)
    {"pg.table.heap_blks_read",     CF_HAVEPARAMS,  PG_STATIO_ALL_TABLES,           NULL},
    {"pg.table.heap_blks_hit",      CF_HAVEPARAMS,  PG_STATIO_ALL_TABLES,           NULL},
    {"pg.table.idx_blks_read",      CF_HAVEPARAMS,  PG_STATIO_ALL_TABLES,           NULL},
    {"pg.table.idx_blks_hit",       CF_HAVEPARAMS,  PG_STATIO_ALL_TABLES,           NULL},
    {"pg.table.toast_blks_read",    CF_HAVEPARAMS,  PG_STATIO_ALL_TABLES,           NULL},
    {"pg.table.toast_blks_hit",     CF_HAVEPARAMS,  PG_STATIO_ALL_TABLES,           NULL},
    {"pg.table.tidx_blks_read",     CF_HAVEPARAMS,  PG_STATIO_ALL_TABLES,           NULL},
    {"pg.table.tidx_blks_hit",      CF_HAVEPARAMS,  PG_STATIO_ALL_TABLES,           NULL},
    
    // Index statistics (as per pg_stat_all_indexes)
    {"pg.index.idx_scan",           CF_HAVEPARAMS,  PG_STAT_ALL_INDEXES,            NULL},
    {"pg.index.idx_tup_read",       CF_HAVEPARAMS,  PG_STAT_ALL_INDEXES,            NULL},
    {"pg.index.idx_tup_fetch",      CF_HAVEPARAMS,  PG_STAT_ALL_INDEXES,            NULL},
    
    // Index IO statistics (as per pg_statio_all_indexes)
    {"pg.index.idx_blks_read",      CF_HAVEPARAMS,  PG_STATIO_ALL_INDEXES,          NULL},
    {"pg.index.idx_blks_hit",       CF_HAVEPARAMS,  PG_STATIO_ALL_INDEXES,          NULL},
    
    // Null terminator
    {NULL}
};

// Required Zabbix module functions
int         zbx_module_api_version()                { return ZBX_MODULE_API_VERSION_ONE; }
void        zbx_module_item_timeout(int timeout)    { return; }
ZBX_METRIC  *zbx_module_item_list()                 { return keys; }
int         zbx_module_uninit()                     { return ZBX_MODULE_OK; }

int         zbx_module_init() { 
    zabbix_log(LOG_LEVEL_INFORMATION, "Starting agent module %s", STRVER);

    return ZBX_MODULE_OK; 
}

/*
 * Function: pg_connect
 *
 * Parses a Zabbix agent request and returns a PostgreSQL connection.
 *
 * See: http://www.postgresql.org/docs/9.4/static/libpq-connect.html#LIBPQ-PQCONNECTDB
 *
 * Parameter [request]: Zabbix agent request structure.
 *          The following parameters may be set:
 *
 *          0: connection string (default: DEFAULT_CONN_STRING)
 *          1: connection database (default: DEFAULT_CONN_DBNAME)
 *
 * Returns: Valid PostgreSQL connection or NULL on error
 */
 PGconn    *pg_connect(AGENT_REQUEST *request)
 {
    const char  *__function_name = "pg_connect";
    PGconn      *conn = NULL;
    char        *param_connstring = NULL, *param_dbname = NULL, *connstring = NULL;
    int         param_connstring_len = 0, param_dbname_len = 0, connstring_len = 0;;

    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    // get connection string from first parameter
    param_connstring = get_rparam(request, PARAM_CONN_STRING);
    param_connstring = (NULL == param_connstring) ? DEFAULT_CONN_STRING : param_connstring;
    param_connstring_len = strlen(param_connstring);

    // get database name from second parameter
    param_dbname = get_rparam(request, PARAM_DBNAME);
    param_dbname = (NULL == param_dbname) ? DEFAULT_CONN_DBNAME : param_dbname;
    param_dbname_len = strisnull(param_dbname) ? 0 : strlen(param_dbname);

    // create buffer to concat connection string and database name
    connstring_len = param_connstring_len + param_dbname_len + 9; // +9 to allow for ' dbname=' + '\0'
    connstring = zbx_malloc(NULL, connstring_len);
    zbx_strlcpy(connstring, param_connstring, param_connstring_len + 1);

    // append dbname= key
    if (!strisnull(param_dbname)) {
        if (!strisnull(connstring)) {
            zbx_strlcat(connstring, " ", connstring_len);
        }
        zbx_strlcat(connstring, "dbname=", connstring_len);
        zbx_strlcat(connstring, param_dbname, connstring_len);
    }

    // connect
    conn = PQconnectdb(connstring);
    if(CONNECTION_OK != PQstatus(conn)) {
        zabbix_log(LOG_LEVEL_ERR, "Failed to connect to PostgreSQL in %s():\n%s", __function_name, PQerrorMessage(conn));
        zabbix_log(LOG_LEVEL_DEBUG, "Connection string: %s", connstring);
        PQfinish(conn);
        conn = NULL;
    }

    // clean up
    zbx_free(connstring);

    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return conn;
 }

/* Function: pg_exec
 *
 * Wrapper for PQexec to include logging
 *
 * Returns: PGresult
 */
PGresult    *pg_exec(PGconn *conn, const char *command) {
    zabbix_log(LOG_LEVEL_DEBUG, "Executing query: %s", command);
    return PQexec(conn, command);
}

/*
 * Function: pg_get_string
 *
 * Executes a PostgreSQL Query using connection details from a Zabbix agent
 * request structure and updates the agent result structure with the string
 * value of the first column of the first row returned.
 *
 * Parameter [request]: Zabbix agent request structure.
 *          Passed to pg_connect to fetch as valid PostgreSQL
 *          server connection
 *
 * Parameter [result]:  Zabbix agent result structure
 *
 * Paramater [query]:   PostgreSQL query to execute. Query should return a
 *          single scalar string value
 *
 * Returns: SYSINFO_RET_OK or SYSINFO_RET_FAIL on error
 */
 int    pg_get_string(AGENT_REQUEST *request, AGENT_RESULT *result, const char *query)
 {
    int         ret = SYSINFO_RET_FAIL;             // Request result code
    const char  *__function_name = "pg_get_string"; // Function name for log file
    
    PGconn      *conn = NULL;
    PGresult    *res = NULL;
    char        *buffer = NULL;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __function_name, request->key);
    
    // Connect to PostreSQL
    if(NULL == (conn = pg_connect(request)))
        goto out;
    
    // Execute a query
    res = pg_exec(conn, query);
    if(PQresultStatus(res) != PGRES_TUPLES_OK) {
        zabbix_log(LOG_LEVEL_ERR, "Failed to execute PostgreSQL query in %s(%s) with: %s", __function_name, request->key, PQresultErrorMessage(res));
        goto out;
    }
    
    if(0 == PQntuples(res)) {
        zabbix_log(LOG_LEVEL_DEBUG, "No results returned for query \"%s\" in %s(%s)", query, __function_name, request->key);
        goto out;
    }
    
    // Set result
    buffer = strdup(PQgetvalue(res, 0, 0));
    SET_STR_RESULT(result, buffer);    
    ret = SYSINFO_RET_OK;
    
out:
    PQclear(res);
    PQfinish(conn);
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s)", __function_name, request->key);
    return ret;
}

/*
 * Function: pg_get_int
 *
 * Executes a PostgreSQL Query using connection details from a Zabbix agent
 * request structure and updates the agent result structure with the integer
 * value of the first column of the first row returned.
 *
 * Parameter [request]: Zabbix agent request structure.
 *          Passed to pg_connect to fetch as valid PostgreSQL
 *          server connection
 *
 * Parameter [result]:  Zabbix agent result structure
 *
 * Paramater [query]:   PostgreSQL query to execute. Query should return a
 *          single scalar integer value
 *
 * Returns: SYSINFO_RET_OK or SYSINFO_RET_FAIL on error
 */
 int    pg_get_int(AGENT_REQUEST *request, AGENT_RESULT *result, const char *query)
 {
    int         ret = SYSINFO_RET_FAIL;             // Request result code
    const char  *__function_name = "pg_get_int";    // Function name for log file
    
    PGconn      *conn = NULL;
    PGresult    *res = NULL;
    char        *buffer = NULL;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __function_name, request->key);
    
    // Connect to PostreSQL
    if(NULL == (conn = pg_connect(request)))
        goto out;
    
    // Execute a query
    res = pg_exec(conn, query);
    if(PQresultStatus(res) != PGRES_TUPLES_OK) {
        zabbix_log(LOG_LEVEL_ERR, "Failed to execute PostgreSQL query in %s(%s) with: %s", __function_name, request->key, PQresultErrorMessage(res));
        goto out;
    }
    
    if(0 == PQntuples(res)) {
        zabbix_log(LOG_LEVEL_DEBUG, "No results returned for query \"%s\" in %s(%s)", query, __function_name, request->key);
        goto out;
    }
    
    // Set result
    buffer = strdup(PQgetvalue(res, 0, 0));
    
    // Convert E Notation
    if(1 < strlen(buffer) && '.' == buffer[1]) {
        double dbl = strtod(buffer, NULL);
        SET_UI64_RESULT(result, (unsigned long long) dbl);
    }
    
    else {
        SET_UI64_RESULT(result, strtoull(buffer, NULL, 10));
    }
    
    ret = SYSINFO_RET_OK;
    
out:
    PQclear(res);
    PQfinish(conn);
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s)", __function_name, request->key);
    return ret;
}

/*
 * Function: pg_get_dbl
 *
 * Executes a PostgreSQL Query using connection details from a Zabbix agent
 * request structure and updates the agent result structure with the floating
 * point integer value of the first column of the first row returned.
 *
 * Parameter [request]: Zabbix agent request structure.
 *          Passed to pg_connect to fetch as valid PostgreSQL
 *          server connection
 *
 * Parameter [result]:  Zabbix agent result structure
 *
 * Paramater [query]:   PostgreSQL query to execute. Query should return a
 *          single scalar floating point number value
 *
 * Returns: SYSINFO_RET_OK or SYSINFO_RET_FAIL on error
 */
 int    pg_get_dbl(AGENT_REQUEST *request, AGENT_RESULT *result, const char *query)
 {
    int         ret = SYSINFO_RET_FAIL;             // Request result code
    const char  *__function_name = "pg_get_dbl";    // Function name for log file
    
    PGconn      *conn = NULL;
    PGresult    *res = NULL;
    char        *buffer = NULL;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __function_name, request->key);
    
    // Connect to PostreSQL
    if(NULL == (conn = pg_connect(request)))
        goto out;
    
    // Execute a query
    res = pg_exec(conn, query);
    if(PQresultStatus(res) != PGRES_TUPLES_OK) {
        zabbix_log(LOG_LEVEL_ERR, "Failed to execute PostgreSQL query in %s(%s) with: %s", __function_name, request->key, PQresultErrorMessage(res));
        goto out;
    }
    
    if(0 == PQntuples(res)) {
        zabbix_log(LOG_LEVEL_DEBUG, "No results returned for query \"%s\" in %s(%s)", query, __function_name, request->key);
        goto out;
    }
    
    // Set result
    buffer = strdup(PQgetvalue(res, 0, 0));
    SET_DBL_RESULT(result, strtold(buffer, NULL));
    ret = SYSINFO_RET_OK;
    
out:
    PQclear(res);
    PQfinish(conn);
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s)", __function_name, request->key);
    return ret;
}

/*
 * Function: is_oid
 * 
 * Returns: 1 if the specified string is a valid PostgreSQL OID
 *
 * See also: http://www.postgresql.org/docs/9.4/static/datatype-oid.html
 */
int is_oid(char *str)
{
    char *p = NULL;
    int res = 0;

    for(p = str; '\0' != *p; p++) {
        if (0 == isdigit(*p))
            return 0;
        res = 1;
    }

    return res;
}

/* 
 * Function: is_valid_ip
 * 
 * Returns: 1 if the specified string is a valid IPv4 or IPv6 address
 */
int is_valid_ip(char *str)
{
    struct in6_addr in;
    int res = 0;

    // test for valid IPv4 address
    if(1 == inet_pton(AF_INET, str, &(in)))
        res = 1;

    // test for valid IPv6 address
    if(1 == inet_pton(AF_INET6, str, &(in)))
        res = 1;

    return res;
}

/*
 * Function: strcat2
 *
 * An attempt to improve the performance and usability of strcat.
 * Buffer sizing is the responsibility of the caller.
 *
 * Returns: pointer to the last character of the updated destination string
 */
char *strcat2(char *destination, const char *source)
{
    // seek to the end of the destination string
    while (*destination) destination++;

    // copy one char at a time from source
    while (*destination++ = *source++);
    
    // return the last character
    return --destination;
}
