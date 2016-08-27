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
    {"pg.modver",                   0,              MODVER,                         NULL},
    {"pg.connect",                  CF_HAVEPARAMS,  PG_CONNECT,                     NULL},
    {"pg.version",                  CF_HAVEPARAMS,  PG_VERSION,                     NULL},
    {"pg.starttime",                CF_HAVEPARAMS,  PG_STARTTIME,                   NULL},
    {"pg.uptime",                   CF_HAVEPARAMS,  PG_UPTIME,                      NULL},
    {"pg.prepared_xacts_count",     CF_HAVEPARAMS,  PG_PREPARED_XACTS_COUNT,        NULL},
    {"pg.prepared_xacts_ratio",     CF_HAVEPARAMS,  PG_PREPARED_XACTS_RATIO,        NULL},
    {"pg.prepared_xacts_age",       CF_HAVEPARAMS,  PG_PREPARED_XACTS_AGE,          NULL},

    {"pg.setting",                  CF_HAVEPARAMS,  PG_SETTING,                     ",,data_directory"},
    {"pg.setting.discovery",        CF_HAVEPARAMS,  PG_SETTING_DISCOVERY,           NULL},

    // User queries
    {"pg.query.string",             CF_HAVEPARAMS,  PG_QUERY,                       ",,SELECT 'Lorem ipsum dolor';"},
    {"pg.query.integer",            CF_HAVEPARAMS,  PG_QUERY,                       ",,SELECT pg_backend_pid();"},
    {"pg.query.double",             CF_HAVEPARAMS,  PG_QUERY,                       ",,SELECT CAST(1234 AS double precision);"},
    {"pg.query.discovery",          CF_HAVEPARAMS,  PG_QUERY,                       ",,SELECT * FROM pg_database;"},
    
    // Client connection statistics
    {"pg.backends.count",           CF_HAVEPARAMS,  PG_BACKENDS_COUNT,              NULL},
    {"pg.backends.free",            CF_HAVEPARAMS,  PG_BACKENDS_FREE,               NULL},
    {"pg.backends.ratio",           CF_HAVEPARAMS,  PG_BACKENDS_RATIO,              NULL},
    {"pg.queries.longest",          CF_HAVEPARAMS,  PG_QUERIES_LONGEST,             NULL},
    
    // Server statistics (as per pg_stat_bgwriter)
    {"pg.checkpoints_timed",        CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    {"pg.checkpoints_req",          CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    {"pg.checkpoint_write_time",    CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    {"pg.checkpoint_sync_time",     CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    {"pg.checkpoint_avg_interval",  CF_HAVEPARAMS,  PG_BG_AVG_INTERVAL,             NULL},
    {"pg.checkpoint_time_ratio",    CF_HAVEPARAMS,  PG_BG_TIME_RATIO,               NULL},
    {"pg.buffers_checkpoint",       CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    {"pg.buffers_clean",            CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    {"pg.maxwritten_clean",         CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    {"pg.buffers_backend",          CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    {"pg.buffers_backend_fsync",    CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    {"pg.buffers_alloc",            CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    {"pg.stats_reset",              CF_HAVEPARAMS,  PG_STAT_BGWRITER,               NULL},
    {"pg.stats_reset_interval",     CF_HAVEPARAMS,  PG_BG_STATS_RESET_INTERVAL,     NULL},
    
    // Asset discovery
    {"pg.db.discovery",             CF_HAVEPARAMS,  PG_DB_DISCOVERY,                NULL},
    {"pg.namespace.discovery",      CF_HAVEPARAMS,  PG_NAMESPACE_DISCOVERY,         NULL},
    {"pg.schema.discovery",         CF_HAVEPARAMS,  PG_NAMESPACE_DISCOVERY,         NULL}, // Alias for pg.namespace.discovery
    {"pg.tablespace.discovery",     CF_HAVEPARAMS,  PG_TABLESPACE_DISCOVERY,        NULL},
    {"pg.table.discovery",          CF_HAVEPARAMS,  PG_TABLE_DISCOVERY,             NULL},
    {"pg.table.children.discovery", CF_HAVEPARAMS,  PG_TABLE_CHILDREN_DISCOVERY,    ",,pg_proc"},
    {"pg.index.discovery",          CF_HAVEPARAMS,  PG_INDEX_DISCOVERY,             NULL},
    
    // Asset class sizes
    {"pg.db.size",                  CF_HAVEPARAMS,  PG_DB_SIZE,                     NULL},
    {"pg.table.size",               CF_HAVEPARAMS,  PG_TABLE_SIZE,                  NULL},
    {"pg.table.rows",               CF_HAVEPARAMS,  PG_TABLE_ROWS,                  NULL},
    {"pg.table.children",           CF_HAVEPARAMS,  PG_TABLE_CHILDREN,              ",,pg_database"},
    {"pg.table.children.size",      CF_HAVEPARAMS,  PG_TABLE_CHILDREN_SIZE,         ",,pg_database"},
    {"pg.table.children.rows",      CF_HAVEPARAMS,  PG_TABLE_CHILDREN_ROWS,         ",,pg_database"},
    {"pg.index.size",               CF_HAVEPARAMS,  PG_INDEX_SIZE,                  NULL},
    {"pg.index.rows",               CF_HAVEPARAMS,  PG_INDEX_ROWS,                  NULL},
    {"pg.tablespace.size",          CF_HAVEPARAMS,  PG_TABLESPACE_SIZE,             ",,pg_default"},
    {"pg.namespace.size",           CF_HAVEPARAMS,  PG_NAMESPACE_SIZE,              ",,pg_catalog"},
    {"pg.schema.size",              CF_HAVEPARAMS,  PG_NAMESPACE_SIZE,              ",,pg_catalog"}, // Alias for pg.namespace.size
    
    // Database statistics (as per pg_stat_database)
    {"pg.db.numbackends",           CF_HAVEPARAMS,  PG_STAT_DATABASE,               NULL},
    {"pg.db.xact_commit",           CF_HAVEPARAMS,  PG_STAT_DATABASE,               NULL},
    {"pg.db.xact_rollback",         CF_HAVEPARAMS,  PG_STAT_DATABASE,               NULL},
    {"pg.db.blks_read",             CF_HAVEPARAMS,  PG_STAT_DATABASE,               NULL},
    {"pg.db.blks_hit",              CF_HAVEPARAMS,  PG_STAT_DATABASE,               NULL},
    {"pg.db.blks_ratio",            CF_HAVEPARAMS,  PG_DB_BLKS_RATIO,                NULL},
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
    {"pg.db.xid_age",               CF_HAVEPARAMS,  PG_DB_XID_AGE,                  NULL},
    
    // Table statistics (as per pg_stat_all_tables)
    {"pg.table.seq_scan",           CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.seq_tup_read",       CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.idx_scan",           CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.idx_scan_ratio",     CF_HAVEPARAMS,  PG_TABLE_IDX_SCAN_RATIO,        NULL},
    {"pg.table.idx_tup_fetch",      CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.n_tup_ins",          CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.n_tup_upd",          CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.n_tup_del",          CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.n_tup_hot_upd",      CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.n_live_tup",         CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.n_dead_tup",         CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.n_mod_since_analyze",CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.last_vacuum",        CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             ",,pg_database"},
    {"pg.table.last_autovacuum",    CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             ",,pg_database"},
    {"pg.table.last_analyze",       CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             ",,pg_database"},
    {"pg.table.last_autoanalyze",   CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             ",,pg_database"},
    {"pg.table.vacuum_count",       CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.autovacuum_count",   CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.analyze_count",      CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    {"pg.table.autoanalyze_count",  CF_HAVEPARAMS,  PG_STAT_ALL_TABLES,             NULL},
    
    // Table IO Statistics (as per pg_statio_all_tables)
    {"pg.table.heap_blks_read",     CF_HAVEPARAMS,  PG_STATIO_ALL_TABLES,           NULL},
    {"pg.table.heap_blks_hit",      CF_HAVEPARAMS,  PG_STATIO_ALL_TABLES,           NULL},
    {"pg.table.heap_blks_ratio",    CF_HAVEPARAMS,  PG_TABLE_HEAP_BLKS_RATIO,       NULL},
    {"pg.table.idx_blks_read",      CF_HAVEPARAMS,  PG_STATIO_ALL_TABLES,           NULL},
    {"pg.table.idx_blks_hit",       CF_HAVEPARAMS,  PG_STATIO_ALL_TABLES,           NULL},
    {"pg.table.idx_blks_ratio",     CF_HAVEPARAMS,  PG_TABLE_IDX_BLKS_RATIO,        NULL},
    {"pg.table.toast_blks_read",    CF_HAVEPARAMS,  PG_STATIO_ALL_TABLES,           NULL},
    {"pg.table.toast_blks_hit",     CF_HAVEPARAMS,  PG_STATIO_ALL_TABLES,           NULL},
    {"pg.table.toast_blks_ratio",   CF_HAVEPARAMS,  PG_TABLE_TOAST_BLKS_RATIO,      NULL},
    {"pg.table.tidx_blks_read",     CF_HAVEPARAMS,  PG_STATIO_ALL_TABLES,           NULL},
    {"pg.table.tidx_blks_hit",      CF_HAVEPARAMS,  PG_STATIO_ALL_TABLES,           NULL},
    {"pg.table.tidx_blks_ratio",    CF_HAVEPARAMS,  PG_TABLE_TIDX_BLKS_RATIO,       NULL},
    
    // Index statistics (as per pg_stat_all_indexes)
    {"pg.index.idx_scan",           CF_HAVEPARAMS,  PG_STAT_ALL_INDEXES,            NULL},
    {"pg.index.idx_tup_read",       CF_HAVEPARAMS,  PG_STAT_ALL_INDEXES,            NULL},
    {"pg.index.idx_tup_fetch",      CF_HAVEPARAMS,  PG_STAT_ALL_INDEXES,            NULL},
    
    // Index IO statistics (as per pg_statio_all_indexes)
    {"pg.index.idx_blks_read",      CF_HAVEPARAMS,  PG_STATIO_ALL_INDEXES,          NULL},
    {"pg.index.idx_blks_hit",       CF_HAVEPARAMS,  PG_STATIO_ALL_INDEXES,          NULL},
    {"pg.index.idx_blks_ratio",     CF_HAVEPARAMS,  PG_INDEX_IDX_BLKS_RATIO,        NULL},
    
    // Null terminator
    {NULL}
};

// Global Variables
const char  pgquerypath[MAX_GLOBBING_PATH_LENGTH] = DEFAULT_PG_QUERY_CONF_PATH;
char        *configPath[MAX_NUMBER_CONFIG_FILES];
int         fileCount = 0;
char        *SQLkey[MAX_NUMBER_SQL_STATEMENT_IN_RAM+1];
char        *SQLstmt[MAX_NUMBER_SQL_STATEMENT_IN_RAM+1];
int         SQLcount = 0;

// Forward function definition
const char * getPGQUERYPATH();

// Required Zabbix module functions
int         zbx_module_api_version()                { return ZBX_MODULE_API_VERSION_ONE; }
void        zbx_module_item_timeout(int timeout)    { return; }
ZBX_METRIC  *zbx_module_item_list()                 { return keys; }

int         zbx_module_uninit() {
    SQLCleanup();
    return ZBX_MODULE_OK;
}

int         zbx_module_init() { 
    char        confdir[MAX_GLOBBING_PATH_LENGTH];
    int         numfiles, i;

    // log version on startup
    zabbix_log(LOG_LEVEL_INFORMATION, "Starting agent module %s", PACKAGE_STRING);

    // init arrays
    SQLkey[0]=NULL;
    SQLstmt[0]=NULL;

    // get query config file path
    zbx_strlcpy(confdir,getPGQUERYPATH(),strlen(getPGQUERYPATH())+1);

    // append slash if needed, plus *.conf glob
    if ('/' == confdir[strlen(confdir)-1]) {
        strcat(confdir,"*.conf");
    } else {
        strcat(confdir,"/*.conf");
    }

    // get the number of files that matched the glob
    numfiles = globfilelist(confdir);

    // process all the config files
    if (numfiles < 0) {
        zabbix_log(LOG_LEVEL_CRIT, "%s: ERROR invoking globfilelist function", PACKAGE);
        return ZBX_MODULE_FAIL;
    }
    for (i = 0; i < numfiles; i++) {
        zabbix_log(LOG_LEVEL_INFORMATION, "%s: Parsing config file \"%s\"", PACKAGE, configPath[i]);
        if(readconfig(configPath[i]) == EXIT_FAILURE) {
            zbx_free(configPath[i]);
            return ZBX_MODULE_FAIL;
        }
        zbx_free(configPath[i]);
    }

    zabbix_log(LOG_LEVEL_DEBUG, "%s: End of init", PACKAGE);
    return ZBX_MODULE_OK; 
}

/*
 * Custom key: pg.modver
 *
 * Returns the version string of the libzbxpgsql module.
 *
 * Parameters:
 *
 * Returns: s
 */
 int    MODVER(AGENT_REQUEST *request, AGENT_RESULT *result)
 {
    int         ret = SYSINFO_RET_FAIL;         // Request result code
    const char  *__function_name = "MODVER";    // Function name for log file

    char        buffer[MAX_STRING_LEN];
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s", __function_name);

    zbx_snprintf(
        buffer,
        sizeof(buffer),
        "%s, compiled for Zabbix %s",
        PACKAGE_STRING,
        ZABBIX_VERSION
    );
    
    // Set result
    SET_STR_RESULT(result, strdup(buffer));
    ret = SYSINFO_RET_OK;
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s", __function_name);
    return ret;
}

/* 
 * Function: pg_exec
 *
 * Wrapper for PQexecParams. Only supports text parameters as binary parameters
 * are not possible in Zabbix item keys.
 *
 * Returns: PGresult
 */
PGresult    *pg_exec(PGconn *conn, const char *command, PGparams params) {
    PGresult *res = NULL;
    int      i = 0, nparams = 0;

    // count parameters
    nparams = param_len(params);

    // log the query
    zabbix_log(LOG_LEVEL_DEBUG, "Executing query with %i parameters: %s", nparams, command);
    for (i = 0; i < nparams; i++)
        zabbix_log(LOG_LEVEL_DEBUG, "  $%i: %s", i, params[i]);

    // execute query with escaped parameters
    res = PQexecParams(conn, command, nparams, NULL, (const char * const*) params, NULL, NULL, 0);

    // free up the params array which would have been alloc'ed for this request
    param_free(params);

    return res;
}

/*
 * Function: pg_scalar
 *
 * Executes a PostgreSQL Query using connection details from a Zabbix agent
 * request structure and fills the given buffer with the value of the first
 * column of the first row returned. The connection is closed before returning.
 *
 * Parameter [request]: Zabbix agent request structure.
 *          Passed to pg_connect_request to fetch a valid PostgreSQL server
 *          connection
 *
 * Parameter [result]:  Zabbix agent result structure used to set any errors
 *          that may occur.
 *
 * Parameter [query]:   PostgreSQL query to execute. Query should return a
 *          single scalar string value.
 *
 * Parameter [params]:  Query parameters
 *
 * Parameter [buffer]:  Buffer to the filled with the query response
 *
 * Parameter [bufferlen]:   Size in bytes of the buffer to fill
 *
 * Returns: SYSINFO_RET_OK or SYSINFO_RET_FAIL on error
 */
int pg_scalar(AGENT_REQUEST *request, AGENT_RESULT *result, const char *query, PGparams params, char *buffer, size_t bufferlen) {
    const char  *__function_name = "pg_scalar";

    PGconn      *conn = NULL;
    PGresult    *res = NULL;
    int         ret = SYSINFO_RET_FAIL;

    zabbix_log(LOG_LEVEL_DEBUG, "In %s", __function_name);

    // connect to PostgreSQL
    conn = pg_connect_request(request, result);
    if (NULL == conn)
        goto out;

    // execute scalar query
    res = pg_exec(conn, query, params);
    if(PQresultStatus(res) != PGRES_TUPLES_OK) {
        set_err_result(result, "PostgreSQL error for query \"%s\": %s", query, PQresultErrorMessage(res));
        goto out;
    }
    
    if(0 == PQntuples(res)) {
        set_err_result(result, "No results returned for query \"%s\"", query);
        goto out;
    }

    // copy result to buffer
    zbx_strlcpy(buffer, PQgetvalue(res, 0, 0), bufferlen);
    ret = SYSINFO_RET_OK;

out:
    PQclear(res);
    PQfinish(conn);
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s", __function_name);

    return ret;
}

/*
 * Function: pg_get_result
 *
 * Executes a PostgreSQL Query using connection details from a Zabbix agent
 * request structure and updates the agent result structure with the value of
 * the first column of the first row returned.
 *
 * type may be 
 *
 * Query parameters may be provided as a NULL terminated sequence of *char
 * values in the ... parameter.
 *
 * Parameter [request]: Zabbix agent request structure.
 *          Passed to pg_connect_request to fetch a valid PostgreSQL server
 *          connection
 *
 * Parameter [result]:  Zabbix agent result structure
 *
 * Parameter [type]:    Result type to set. May be one of AR_STRING, AR_UINT64
 *          or AR_DOUBLE.
 *
 * Paramater [query]:   PostgreSQL query to execute. Query should return a
 *          single scalar string value. Parameters defined using PostgreSQL's
 *          '$n' notation will be replaced with the corresponding variadic
 *          argument provided in ...
 *
 * Returns: SYSINFO_RET_OK or SYSINFO_RET_FAIL on error
 */
int    pg_get_result(AGENT_REQUEST *request, AGENT_RESULT *result, int type, const char *query, PGparams params)
{
    int         ret = SYSINFO_RET_FAIL;             // Request result code
    const char  *__function_name = "pg_get_result"; // Function name for log file
    
    char        value[MAX_STRING_LEN];

    zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __function_name, request->key);
    
    // execute scalar query
    if(SYSINFO_RET_FAIL == pg_scalar(request, result, query, params, &value[0], sizeof(value)))
        goto out;

    // Set result
    switch(type) {
        case AR_STRING:
            // string result (zabbix will clean the strdup'd buffer)
            SET_STR_RESULT(result, strdup(value));
            break;

        case AR_UINT64:
            // integer result
            // Convert E Notation
            if(1 < strlen(value) && '.' == value[1]) {
                double dbl = strtod(value, NULL);
                SET_UI64_RESULT(result, (unsigned long long) dbl);
            } else {
                SET_UI64_RESULT(result, strtoull(value, NULL, 10));
            }
            break;

        case AR_DOUBLE:
            // double result
            SET_DBL_RESULT(result, strtold(value, NULL));
            break;

        default:
            // unknown result type
            set_err_result(result, "Unsupported result type: 0x%0X in %s", type, __function_name);
            goto out;
    }

    ret = SYSINFO_RET_OK;
    
out:
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s)", __function_name, request->key);
    return ret;
}

/*
 * Function: pg_get_percentage
 *
 * Executes a PostgreSQL query on the given table using connection details from
 * a Zabbix agent request structure and calculates the quotient the given
 * columns.
 *
 * Parameter [request]: Zabbix agent request structure.
 *          Passed to pg_connect_request to fetch a valid PostgreSQL server
 *          connection
 *
 * Parameter [result]:  Zabbix agent result structure in which the quotient will
 *          will be set or an error message on failure
 *
 * Parameter [table]:   The PostgreSQL table to query
 *
 * Parameter [col1]:    The column containing the dividend to be divided
 *
 * Parameter [col2]:    The column containing the divisor
 *
 * Parameter [colFilter]:   The column to filter by if desired
 *
 * Parameter [filter]:  Value to filter by (`where [colFilter] = [filter]`)
 *
 * Parameter [type]:    Result type to set. May be one of AR_STRING, AR_UINT64
 *          or AR_DOUBLE.
 *
 * Returns: SYSINFO_RET_OK or SYSINFO_RET_FAIL on error
 */
int pg_get_percentage(AGENT_REQUEST *request, AGENT_RESULT *result, char *table, char *col1, char *col2, char *colFilter, char *filter)
{
    int         ret = SYSINFO_RET_FAIL;                 // Request result code
    const char  *__function_name = "pg_get_percentage"; // Function name for log file
    
    int         qlen = 0;
    char        query[MAX_STRING_LEN], *c = NULL;

    zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __function_name, request->key);

    zbx_snprintf(
        query,
        sizeof(query),
        "SELECT CASE WHEN (%s) = 0 THEN 1 ELSE (%s)::float / (%s) END FROM %s",
        col2,
        col1,
        col2,
        table
    );

    if (!strisnull(colFilter)) {
        qlen = strlen(query);
        c = &query[qlen];
        zbx_snprintf(c, (sizeof(query) / sizeof(char)) - qlen, " WHERE %s = $1", colFilter);
    }

    ret = pg_get_dbl(request, result, query, param_new(filter));

    zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s)", __function_name, request->key);
    return ret; 
}

/* 
 * Function: pg_version
 *
 * Returns a comparable version number (e.g 80200 or 90400) for the connected
 * PostgreSQL server version.
 *
 * Returns: int
 */
long int pg_version(AGENT_REQUEST *request, AGENT_RESULT *result) {
    const char  *__function_name = "pg_version"; // Function name for log file

    char        buffer[MAX_STRING_LEN];
    long int    version = 0;

    zabbix_log(LOG_LEVEL_DEBUG, "In %s", __function_name);

    // execute query
    if (SYSINFO_RET_OK == pg_scalar(
        request, 
        result, 
        "SELECT setting FROM pg_settings WHERE name='server_version_num'",
        NULL,
        &buffer[0],
        sizeof(buffer)
    )) {
        // convert to integer
        version = atol(buffer);
        zabbix_log(LOG_LEVEL_DEBUG, "PostgreSQL server version: %lu", version);
    }

    zabbix_log(LOG_LEVEL_DEBUG, "End of %s", __function_name);

    return version;
}

/*
 * Log an error to the agent log file and set the result message sent back to
 * the server.
 */
int set_err_result(AGENT_RESULT *result, const char *format, ...)
{
    va_list args;
    char    msg[MAX_STRING_LEN];

    // parse message string
    va_start (args, format);
    zbx_vsnprintf((char*)&msg, sizeof(msg), format, args);

    // log message
    zabbix_log(LOG_LEVEL_ERR, "PostgreSQL: %s", msg);

    if (NULL != result)
        SET_MSG_RESULT(result, strdup(msg));

    return SYSINFO_RET_FAIL;
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
char *strcat2(char *dest, const char *src)
{
    if (NULL == dest || NULL == src)
        return dest;

    // seek to the end of the dest string
    while (*dest) dest++;

    // copy one char at a time from source
    while (*dest++ = *src++);
    
    // return the last character
    return --dest;
}

/*
 * Function getPGQUERYPATH
 *
 * Returns the config directory used for SQL config files
 * for pg_query.* keys.
 * 
 * If the environment variable PGQUERYPATH is set then that is
 * used, otherwise DEFAULT_PG_QUERY_CONF_PATH is used.
 *
 * Returns: pointer to const char
 *
 */
const char * getPGQUERYPATH() {
    const char  *__function_name = "getPGQUERYPATH";
    const char  *envPGQUERYPATH = getenv("PGQUERYPATH");

    zabbix_log(LOG_LEVEL_DEBUG, "%s: In %s", PACKAGE, __function_name);
    if(NULL == &envPGQUERYPATH || '\0' == envPGQUERYPATH) {
        zabbix_log(LOG_LEVEL_TRACE, "%s: Using default config path", PACKAGE);
        return pgquerypath;
    } else {
        if (strlen(envPGQUERYPATH) > MAX_GLOBBING_PATH_LENGTH) {
            zabbix_log(LOG_LEVEL_ERR, "%s: ERROR: Env variable \"PGQUERYPATH\" value too long", PACKAGE);
            zabbix_log(LOG_LEVEL_ERR, "%s: ERROR: Length %i exceeds max length of %i",
                PACKAGE, strlen(envPGQUERYPATH), MAX_GLOBBING_PATH_LENGTH);
            return NULL;
        }
        zabbix_log(LOG_LEVEL_TRACE, "%s: Using config path from PGQUERYPATH env variable", PACKAGE);
        return envPGQUERYPATH;
    }
}

/*
 * Function globerror
 *
 * Error handler for globbing.
 *
 */
int globerror(const char *filename, int errorcode) {
    zabbix_log(LOG_LEVEL_ERR, "%s ERROR: globbing error %s: %s", PACKAGE, filename, strerror(errorcode));
    return EXIT_FAILURE;
}

/*
 * Function globfilelist
 *
 * Takes a file glob pattern and stores matching
 * filenames in configPath array.
 *
 * Returns:
 *    int number of files matched
 *    -1 if error encountered
 */
int globfilelist(const char *pattern) {
    const char  *__function_name = "globfilelist";
    glob_t   filenames;
    int      strsize;

    zabbix_log(LOG_LEVEL_DEBUG, "%s: In %s", PACKAGE, __function_name);
    zabbix_log(LOG_LEVEL_INFORMATION, "%s: Looking for config files: \"%s\"", PACKAGE, pattern);
    switch(glob(pattern, GLOB_ERR, globerror, &filenames)) {
        case 0 :
            for (fileCount = 0; fileCount < filenames.gl_pathc; fileCount++) {
                if (fileCount >= MAX_NUMBER_CONFIG_FILES) {
                    zabbix_log(LOG_LEVEL_ERR, "%s ERROR: Found more than %i config files: %i", PACKAGE, MAX_NUMBER_CONFIG_FILES, fileCount);
                    return EXIT_FAILURE;
                }
                strsize = strlen(filenames.gl_pathv[fileCount]) + 1;
                configPath[fileCount] = zbx_malloc(configPath[fileCount],strsize * sizeof(char));
                if (configPath[fileCount] == NULL) {
                    zabbix_log(LOG_LEVEL_CRIT, "%s: ERROR: zbx_malloc failed", PACKAGE);
                    return EXIT_FAILURE;
                }
                zbx_strlcpy(configPath[fileCount],filenames.gl_pathv[fileCount],strlen(filenames.gl_pathv[fileCount])+1);
            }
            globfree(&filenames);
            break;
        case GLOB_NOMATCH :
            zabbix_log(LOG_LEVEL_INFORMATION, "%s: No config files found to process... skipping", PACKAGE);
            fileCount = 0;
            break;
        default : // untrapped glob error
            fileCount = -1;
    }
    zabbix_log(LOG_LEVEL_DEBUG, "%s: End of %s (fileCount=%i)", PACKAGE, __function_name, fileCount);
    return fileCount;
}

/*
 * Function storeSQLstmt
 *
 * A simple key/value store using arrays of pointers.
 *
 * Stores a string key and a string value (SQL statement)
 * into a sorted array, maintaining sort order.
 *
 * Since it's just moving pointers it should perform well
 * even with large numbers of keys.
 *
 * Returns: 
 *   EXIT_SUCCESS = successfully inserted
 *   EXIT_FAILURE = insert failed
 *   -1           = duplicate key discarded
 */
int  storeSQLstmt(const char *key, const char *stmt) {
    const char  *__function_name = "storeSQLstmt";
    int  i;

    zabbix_log(LOG_LEVEL_DEBUG, "%s: In %s(%s,<stmt>)", PACKAGE, __function_name, key);
    // make sure we have space
    if (SQLcount >= MAX_NUMBER_SQL_STATEMENT_IN_RAM) {
        zabbix_log(LOG_LEVEL_ERR, "%s: ERROR: Keystore full: %i statements stored already", PACKAGE, SQLcount);
        return EXIT_FAILURE;
    }
    // exclude dupes
    if (SQLkeysearch((char *)key) != -1) {
        zabbix_log(LOG_LEVEL_ERR, "%s: ERROR: Duplicate key: \"%s\"", PACKAGE, key);
        return EXIT_FAILURE;
    }
    // start at the end of the index and push out
    // entries to the next spot until you find the
    // right spot to insert the new key/value pair
    i = SQLcount - 1;
    zabbix_log(LOG_LEVEL_TRACE, "%s: Starting to look for insert location (i:%i)", PACKAGE, i);
    while (i >= 0 && strcmp(key,SQLkey[i]) < 0) {
        zabbix_log(LOG_LEVEL_TRACE, "%s: moving data from slot %i to slot %i", PACKAGE, i, i+1);
        SQLkey[i+1]  = SQLkey[i];
        SQLstmt[i+1] = SQLstmt[i];
        i--;
    }
    // allocate memory for the new key
    SQLkey[i+1] = zbx_malloc(SQLkey[i+1],sizeof(char) * (strlen(key)+1));
    if (SQLkey[SQLcount] == NULL) {
        zabbix_log(LOG_LEVEL_CRIT, "%s: ERROR: zbx_malloc failed", PACKAGE);
        return EXIT_FAILURE;
    }
    // allocate memory for the new value
    SQLstmt[i+1] = zbx_malloc(SQLstmt[i+1],sizeof(char) * (strlen(stmt)+1));
    if (SQLstmt[SQLcount] == NULL) {
        zabbix_log(LOG_LEVEL_CRIT, "%s: ERROR: zbx_malloc failed", PACKAGE);
        return EXIT_FAILURE;
    }
    // store the key and value
    zabbix_log(LOG_LEVEL_TRACE, "%s: storing data in slot %i", PACKAGE, i+1);
    zbx_strlcpy(SQLkey[i+1],key,strlen(key)+1);
    zbx_strlcpy(SQLstmt[i+1],stmt,strlen(stmt)+1);
    SQLcount++;

    // Append NULL marker
    SQLkey[SQLcount+1] = NULL;
    SQLstmt[SQLcount+1] = NULL;

    zabbix_log(LOG_LEVEL_DEBUG, "%s: End of %s", PACKAGE, __function_name);
    return EXIT_SUCCESS;
}

/*
 * Function SQLkeysearch
 *
 * Searches the key array to find the
 * corresponding SQL stmt using binary
 * search.
 *
 * Returns: 
 *    If Key Found: index to key
 *    If Not Found: -1
 */
int SQLkeysearch(char *key) {
    const char  *__function_name = "SQLkeysearch";
    int  top;
    int  mid;
    int  bottom;

    zabbix_log(LOG_LEVEL_DEBUG, "%s: In %s(%s)", PACKAGE, __function_name, key);
    top = SQLcount - 1;
    bottom = 0;
    while (bottom <= top) {
        mid = (bottom + top)/2;
        zabbix_log(LOG_LEVEL_TRACE, "%s: range top:%i mid:%i bottom:%i", PACKAGE, top, mid, bottom);
        if (strcmp(SQLkey[mid], key) == 0) {
            zabbix_log(LOG_LEVEL_TRACE, "%s: found in slot:%i", PACKAGE, mid);
            return mid;
        } else if (strcmp(SQLkey[mid], key) > 0) {
            zabbix_log(LOG_LEVEL_TRACE, "%s: key between bottom and middle of range", PACKAGE);
            top    = mid - 1;
        } else if (strcmp(SQLkey[mid], key) < 0) {
            zabbix_log(LOG_LEVEL_TRACE, "%s: key between middle and top of range", PACKAGE);
            bottom = mid + 1;
        }
    }
    zabbix_log(LOG_LEVEL_DEBUG, "%s: End of %s", PACKAGE, __function_name);
    return -1;
}

/*
 * Function SQLcleanup
 *
 * Free up array memory when done.
 *
 * Returns: 
 */
int  SQLCleanup() {
    const char  *__function_name = "SQLCleanup";
    int   i;

    zabbix_log(LOG_LEVEL_DEBUG, "%s: In %s", PACKAGE, __function_name);
    zabbix_log(LOG_LEVEL_TRACE, "%s: SQLcount:%i", PACKAGE, SQLcount);
    for (i = 0; i < SQLcount; i++) {
        zabbix_log(LOG_LEVEL_TRACE, "%s: i:%i", PACKAGE, i);
        zbx_free(SQLkey[i]);
        zbx_free(SQLstmt[i]);
    }
    SQLcount = 0;
    zabbix_log(LOG_LEVEL_DEBUG, "%s: End of %s", PACKAGE, __function_name);
    return EXIT_SUCCESS;
}

/*
 * Function readconfig
 *
 * Reads the contents of a config file using libconfig.
 * 
 * Note: although libconfig supports complex parameter
 * files (such as nested parms), this function's
 * implementation only supports simple
 *    key = value
 * parameters.
 *
 * Returns:
 *   EXIT_SUCCESS = successfully parsed
 *   EXIT_FAILURE = parse failed
 */
int readconfig(const char *cfgfile) {
    const char  *__function_name = "readconfig";
    config_t          cfg;
    config_setting_t  *root, *element;
    int               i;
    const char        *key, *value;

    zabbix_log(LOG_LEVEL_DEBUG, "%s: In %s(%s)", PACKAGE, __function_name, cfgfile);
    config_init(&cfg);
    // call libconfig to parse config file into memory
    if(! config_read_file(&cfg, cfgfile)) {
        zabbix_log(LOG_LEVEL_ERR, "%s: ERROR: %s for file \"%s\"",
            PACKAGE, config_error_text(&cfg), cfgfile);
        if(CONFIG_ERR_PARSE == config_error_type(&cfg)) {
            zabbix_log(LOG_LEVEL_ERR, "%s: ERROR: Parsing error on or near line %i",
                PACKAGE, config_error_line(&cfg));
        }
        config_destroy(&cfg);
        return EXIT_FAILURE;
    }
    // start retrieving key/value pairs
    root = config_root_setting(&cfg);
    zabbix_log(LOG_LEVEL_TRACE, "%s: config_setting_length:%i", PACKAGE, config_setting_length(root));
    for (i = 0; i < config_setting_length(root); i++) {
        element = config_setting_get_elem(root, i);
        key = config_setting_name(element);
        zabbix_log(LOG_LEVEL_DEBUG, "%s: Found config key=[%s]", PACKAGE, key);
        // we only want strings
        if(CONFIG_TYPE_STRING == config_setting_type(element)) {
            value = config_setting_get_string_elem(root, i);
            zabbix_log(LOG_LEVEL_DEBUG, "%s: Found config value=[%s]", PACKAGE, value);
            zabbix_log(LOG_LEVEL_INFORMATION, "%s: ----Storing key \"%s\" and value", PACKAGE, key);
            // store it in our key/value store
            if (storeSQLstmt(key, value) == EXIT_FAILURE) {
                config_destroy(&cfg);
                return EXIT_FAILURE;
            }
        } else {
            zabbix_log(LOG_LEVEL_DEBUG, "%s: config_setting_type:%i", PACKAGE, config_setting_type(element));
            zabbix_log(LOG_LEVEL_ERR, "%s: ERROR: Element \"%s\" in \"%s\" on line %i is not a string",
                   PACKAGE, key, config_setting_source_file(element), config_setting_source_line(element));
            config_destroy(&cfg);
            return EXIT_FAILURE;
        }
    }
    config_destroy(&cfg);
    zabbix_log(LOG_LEVEL_DEBUG, "%s: End of %s", PACKAGE, __function_name);
    return EXIT_SUCCESS;
}


