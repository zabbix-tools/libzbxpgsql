#include "libzbxpgsql.h"

/*
 * Function: build_connstring
 *
 * Allocates and returns a libpq compatible connection string. This function
 * takes as input, a libpq compatible connection string with the `dbname` field
 * ommitted and the desired database name as the second parameter. This enables
 * connection strings to be built from Zabbix discovery rules where the
 * connected database may not be known when configuring Zabbix.
 *
 * Returns: libpq compatible connection string. Must be freed by the caller
 *          using zbx_free()
 */
char *build_connstring(const char *connstring, const char *dbname)
{
    char    *res = NULL, *c = NULL;
    int     bufferlen = 0;

    bufferlen =
        (NULL == connstring ? 0 : strlen(connstring))
        + (NULL == dbname ? 0 : strlen(dbname))
        + 9; // + ' dbname=\0'
   
    res = zbx_malloc(res, sizeof(char) * bufferlen);
    memset(res, 0, sizeof(char) * bufferlen);

    c = res;
    c = strcat2(c, strisnull(connstring) ? DEFAULT_CONN_STRING : connstring);
    c = strcat2(c, strisnull(connstring) ? NULL : " ");
    c = strcat2(c, "dbname=");
    c = strcat2(c, strisnull(dbname) ? DEFAULT_CONN_DBNAME : dbname);

    return res;
}

/*
 * Function: pg_connect
 *
 * Connect to PostgreSQL server
 *
 * See: http://www.postgresql.org/docs/9.4/static/libpq-connect.html#LIBPQ-PQCONNECTDB
 *
 * Parameter [connstring]:  libpq compatible connection string
 *
 * Parameter [result]:      result structure to send errors to the server
 *
 * Returns: Valid PostgreSQL connection or NULL on error
 */
PGconn    *pg_connect(const char *connstring, AGENT_RESULT *result)
 {
    const char  *__function_name = "pg_connect";

    PGconn      *conn = NULL;
    
    zabbix_log(LOG_LEVEL_DEBUG, "In %s()", __function_name);
    
    /*
     * Breaks in < v9.0
    // append application name
    if (!strisnull(connstring))
        c = strcat2(c, " ");
    c = strcat(c, "application_name='" STRVER "'");
    */

    // connect
    zabbix_log(LOG_LEVEL_DEBUG, "Connecting to PostgreSQL with: %s", connstring);
    conn = PQconnectdb(connstring);
    if(CONNECTION_OK != PQstatus(conn)) {
        set_err_result(result, PQerrorMessage(conn));
        PQfinish(conn);
        conn = NULL;
    }

    zabbix_log(LOG_LEVEL_DEBUG, "End of %s()", __function_name);
    return conn;
}

/*
 * Function: pg_connect_request
 *
 * Parses a Zabbix agent request and returns a PostgreSQL connection.
 *
 * See: http://www.postgresql.org/docs/9.4/static/libpq-connect.html#LIBPQ-PQCONNECTDB
 *
 * Parameter [request]:     Zabbix agent request structure.
 *          The following parameters may be set:
 *
 *          0: connection string (default: DEFAULT_CONN_STRING)
 *          1: connection database (default: DEFAULT_CONN_DBNAME)
 *
 * Parameter [result]:      result structure to send errors to the server
 *
 * Returns: Valid PostgreSQL connection or NULL on error
 */
 PGconn    *pg_connect_request(AGENT_REQUEST *request, AGENT_RESULT *result)
 {
    PGconn      *conn = NULL;
    char        *connstring = NULL; 

    // connect using params from agent request
    connstring = build_connstring(
        get_rparam(request, PARAM_CONN_STRING), 
        get_rparam(request, PARAM_DBNAME));

    conn = pg_connect(connstring, result);
    zbx_free(connstring);

    return conn;
}
