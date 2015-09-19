#include "libzbxpgsql.h"

/* 
 * Function: pg_get_databases
 *
 * Returns a null delimited list of database names which the connected
 * PostgreSQL user is allowed to connect to (i.e. has been granted 'CONNECT').
 */
static char *pg_get_databases(AGENT_REQUEST *request) {
    const char  *__function_name = "pg_get_databases"; // Function name for log file

    PGconn      *conn = NULL;
    PGresult    *res = NULL;

    char        *databases = NULL, *c = NULL;
    int         rows = 0, i = 0, bufferlen = 0;

    zabbix_log(LOG_LEVEL_DEBUG, "In %s", __function_name);

    // connect to PostgreSQL
    conn = pg_connect_request(request);
    if (NULL == conn)
        goto out;

    // get connectable databases
    res = pg_exec(conn, "SELECT datname FROM pg_database WHERE datallowconn = 't' AND pg_catalog.has_database_privilege(current_user, oid, 'CONNECT');", NULL);
    if(0 == PQntuples(res)) {
        zabbix_log(LOG_LEVEL_ERR, "Failed to get connectable PostgreSQL databases");
        goto out;
    }

    rows = PQntuples(res);

    // iterate over each row to calculate buffer size
    bufferlen = 1; // 1 for null terminator
    for(i = 0; i < rows; i++) {
        bufferlen += strlen(PQgetvalue(res, i, 0)) + 1;
    }

    // allocate databases multi-string
    databases = zbx_malloc(databases, sizeof(char) * bufferlen);
    memset(databases, '\0', sizeof(char) * bufferlen);

    // iterate over each row and copy the results
    c = databases;
    for(i = 0; i < rows; i++) {
        c = strcat2(c, PQgetvalue(res, i, 0)) + 1;
    }

out:
    PQclear(res);
    PQfinish(conn);
    
    zabbix_log(LOG_LEVEL_DEBUG, "End of %s", __function_name);

    return databases;
}

/*
 * Function: pg_get_discovery
 *
 * Executes a PostgreSQL Query using connection details from a Zabbix agent
 * request structure and updates the agent result structure with the JSON
 * discovery data for each returned row.
 *
 * Query parameters may be provided as a NULL terminated sequence of *char
 * values in the ... parameter.
 *
 * Parameter [request]: Zabbix agent request structure.
 *          Passed to pg_connect_request to fetch as valid PostgreSQL
 *          server connection
 *
 * Parameter [result]:  Zabbix agent result structure
 *
 * Paramater [query]:   PostgreSQL query to execute. Query should column names
 *           that match the desired discovery fields.
 *
 * Parameter [deep]:    Execute against all connectable databases
 *
 * Returns: SYSINFO_RET_OK or SYSINFO_RET_FAIL on error
 */
 int    pg_get_discovery(AGENT_REQUEST *request, AGENT_RESULT *result, const char *query, PGparams params)
 {
    int         ret = SYSINFO_RET_FAIL;                 // Request result code
    const char  *__function_name = "pg_get_discovery";  // Function name for log file
    
    struct      zbx_json j;                             // JSON response for discovery rule
    
    int         i = 0, x = 0, columns = 0, rows = 0;
    char        *c = NULL;
    char        buffer[MAX_STRING_LEN];

    PGconn      *conn = NULL;
    PGresult    *res = NULL;

    zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __function_name, request->key);

    // Connect to PostreSQL
    if(NULL == (conn = pg_connect_request(request)))
        goto out;
    
    // Execute a query
    res = pg_exec(conn, query, params);
    if(PQresultStatus(res) != PGRES_TUPLES_OK) {
        zabbix_log(LOG_LEVEL_ERR, "Failed to execute PostgreSQL query in %s(%s) with: %s", __function_name, request->key, PQresultErrorMessage(res));
        goto out;
    }

    // count rows and columns
    rows = PQntuples(res);
    columns = PQnfields(res);

    // Create JSON array of discovered objects
    zbx_json_init(&j, ZBX_JSON_STAT_BUF_LEN);
    zbx_json_addarray(&j, ZBX_PROTO_TAG_DATA);
    
    // create discovery instance for each row
    for(i = 0; i < rows; i++) {
        zbx_json_addobject(&j, NULL);
        
        // add each row field as a discovery field
        for(x = 0; x < columns; x++) {
            // set discovery key name to uppercase column name
            zbx_snprintf(buffer, sizeof(buffer), "{#%s}", PQfname(res, x));
            for(c = &buffer[0]; *c; c++)
                *c = toupper(*c);

            zbx_json_addstring(&j, buffer, PQgetvalue(res, i, x), ZBX_JSON_TYPE_STRING);
        }

        zbx_json_close(&j);         
    }

    // Finalize JSON response
    zbx_json_close(&j);
    SET_STR_RESULT(result, strdup(j.buffer));
    zbx_json_free(&j);

    ret = SYSINFO_RET_OK;

out:

    PQclear(res);
    PQfinish(conn);

    zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s)", __function_name, request->key);
    return ret;
}

/*
 * Function: pg_get_discovery_wide
 *
 * Executes a PostgreSQL Query on all accessible databases, using connection
 * details from a Zabbix agent request structure and updates the agent result
 * structure with the JSON discovery data for each returned row.
 *
 * Query parameters may be provided as a NULL terminated sequence of *char
 * values in the ... parameter.
 *
 * Parameter [request]: Zabbix agent request structure.
 *          Passed to pg_connect_request to fetch as valid PostgreSQL
 *          server connection
 *
 * Parameter [result]:  Zabbix agent result structure
 *
 * Paramater [query]:   PostgreSQL query to execute. Query should column names
 *           that match the desired discovery fields.
 *
 * Parameter [deep]:    Execute against all connectable databases
 *
 * Returns: SYSINFO_RET_OK or SYSINFO_RET_FAIL on error
 */
 int    pg_get_discovery_wide(AGENT_REQUEST *request, AGENT_RESULT *result, const char *query, PGparams params)
 {
    int         ret = SYSINFO_RET_FAIL;                     // Request result code
    const char  *__function_name = "pg_get_discovery_wide"; // Function name for log file
    
    struct      zbx_json j;                                 // JSON response for discovery rule
    
    int         i = 0, x = 0, columns = 0, rows = 0;
    char        *databases = NULL, *db = NULL, *c = NULL;
    char        *connstring = NULL;
    char        buffer[MAX_STRING_LEN];

    PGconn      *conn = NULL;
    PGresult    *res = NULL;

    zabbix_log(LOG_LEVEL_DEBUG, "In %s(%s)", __function_name, request->key);

    // get a list of databases
    databases = pg_get_databases(request);
    if (NULL == databases)
        goto out;

    // Create JSON array of discovered objects
    zbx_json_init(&j, ZBX_JSON_STAT_BUF_LEN);
    zbx_json_addarray(&j, ZBX_PROTO_TAG_DATA);

    // query each accessible database
    for (db = databases; *db; db += strlen(db) + 1) {
        // build connection string
        zbx_free(connstring);
        connstring = build_connstring(get_rparam(request, PARAM_CONN_STRING), db);

        // Connect to PostreSQL
        if(NULL == (conn = pg_connect(connstring)))
            goto out;
        
        // Execute a query
        res = pg_exec(conn, query, params);
        if(PQresultStatus(res) != PGRES_TUPLES_OK) {
            zabbix_log(LOG_LEVEL_ERR, "Failed to execute PostgreSQL query in %s(%s) with: %s", __function_name, request->key, PQresultErrorMessage(res));
            goto out;
        }

        // count rows and columns
        rows = PQntuples(res);
        columns = PQnfields(res);

        // create discovery instance for each row
        for(i = 0; i < rows; i++) {
            zbx_json_addobject(&j, NULL);
            
            // add each row field as a discovery field
            for(x = 0; x < columns; x++) {
                // set discovery key name to uppercase column name
                zbx_snprintf(buffer, sizeof(buffer), "{#%s}", PQfname(res, x));
                for(c = &buffer[0]; *c; c++)
                    *c = toupper(*c);

                zbx_json_addstring(&j, buffer, PQgetvalue(res, i, x), ZBX_JSON_TYPE_STRING);
            }

            zbx_json_close(&j);         
        }
    }

    // Finalize JSON response
    zbx_json_close(&j);
    SET_STR_RESULT(result, strdup(j.buffer));
    zbx_json_free(&j);

    ret = SYSINFO_RET_OK;

out:
    zbx_free(connstring);
    zbx_free(databases);

    PQclear(res);
    PQfinish(conn);

    zabbix_log(LOG_LEVEL_DEBUG, "End of %s(%s)", __function_name, request->key);
    return ret;
}

