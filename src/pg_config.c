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

// Default query config file location
#ifdef __FreeBSD__
#define DEFAULT_PGCONFIGFILE    "/usr/local/etc/zabbix/libzbxpgsql.conf"
#else
#define DEFAULT_PGCONFIGFILE    "/etc/zabbix/libzbxpgsql.conf"
#endif

char **query_keys = NULL;
char **query_values = NULL;
int query_count = 0;

/*
 * Function getPGCONFIGFILE
 *
 * Returns the config file path.
 * 
 * If the environment variable PGCONFIGFILE is set then that is
 * used, otherwise DEFAULT_PGCONFIGFILE is used.
 *
 * Returns: pointer to const char
 */
static inline const char * getPGCONFIGFILE() {
    char *path = NULL;
    if('\0' == (path = getenv("PGCONFIGFILE"))) {
        path = DEFAULT_PGCONFIGFILE;
    } else if (strlen(path) > MAX_GLOBBING_PATH_LENGTH) {
        zabbix_log(LOG_LEVEL_ERR, "PGCONFIGFILE exceeds maximum length of %i", MAX_GLOBBING_PATH_LENGTH);
        return NULL;
    }
    
    return path;
}

static inline int add_named_query(const char *name, const char *query)
{
    int i = query_count - 1;
    while(i >= 0 && (NULL == query_keys[i] || 0 > strcmp(name, query_keys[i]))) {
        query_keys[i+1] = query_keys[i];
        query_values[i+1] = query_values[i];
        query_keys[i] = NULL;
        query_values[i] = NULL;
        i--;
    }

    query_keys[i+1] = strdup(name);
    query_values[i+1] = strdup(query);

    return EXIT_SUCCESS;
}

/*
 * Function get_query_by_name
 *
 * Searches the key array to find the
 * corresponding SQL stmt using binary
 * search.
 *
 * Returns: 
 *    If Key Found: pointer to query string
 *    If Not Found: NULL
 */
const char *get_query_by_name(const char *key) {
    int top = query_count - 1;
    int mid = 0;
    int bottom = 0;
    int cmp = -1;

    while (bottom <= top) {
        mid = (bottom + top)/2;
        cmp = strcmp(query_keys[mid], key);
        if (cmp == 0) {
            return query_values[mid];
        } else if (cmp > 0) {
            top = mid - 1;
        } else if (cmp < 0) {
            bottom = mid + 1;
        }
    }

    return NULL;
}

static int read_config_queries(const config_setting_t *root)
{
    int                 i = 0;
    const char          *key = NULL, *value = NULL;
    config_setting_t    *node = NULL;

    if (CONFIG_TYPE_GROUP != config_setting_type(root)) {
        zabbix_log(LOG_LEVEL_ERR, "queries is not a valid configuration group");
        return EXIT_FAILURE;
    }

    query_count = config_setting_length(root);
    query_keys = (char**) zbx_calloc(query_keys, query_count + 1, sizeof(char*));
    query_values = (char**) zbx_calloc(query_values, query_count + 1, sizeof(char*));

    for (i = 0; i < query_count; i++) {
        node = config_setting_get_elem(root, i);
        key = config_setting_name(node);
        if (CONFIG_TYPE_STRING != config_setting_type(node)) {
            zabbix_log(LOG_LEVEL_ERR, "query '%s' is not a valid string", key);
            return EXIT_FAILURE;
        }

        value = config_setting_get_string_elem(root, i);
        if (EXIT_SUCCESS != (add_named_query(key, value)))
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static int read_config(const char *cfgfile)
{
    int                 i = 0;
    int                 res = EXIT_FAILURE;
    int                 cfglen = 0;
    const char          *key = NULL;
    config_t            cfg;
    config_setting_t    *root, *node;

    config_init(&cfg);
    if (CONFIG_TRUE != (config_read_file(&cfg, cfgfile))) {
        zabbix_log(LOG_LEVEL_ERR, "%s in %s:%i",
            config_error_text(&cfg), cfgfile, config_error_line(&cfg));
        goto out;
    }

    root = config_root_setting(&cfg);
    cfglen = config_setting_length(root);
    for (i = 0; i < cfglen; i++) {
        node = config_setting_get_elem(root, i);
        key = config_setting_name(node);

        if (0 == strncmp(key, "queries", 8)) {
            if (EXIT_SUCCESS != (read_config_queries(node)))
                goto out;
        } else {
            zabbix_log(LOG_LEVEL_ERR, "unrecognised configuration parameter: %s", key);
            goto out;
        }
    }

    res = EXIT_SUCCESS;

out:
    config_destroy(&cfg);
    return res;
}

int init_config()
{
    const char *cfgfile = getPGCONFIGFILE();
    zabbix_log(LOG_LEVEL_INFORMATION, "using module configuration file: %s", cfgfile);
    if (EXIT_SUCCESS != (read_config(cfgfile)))
        return ZBX_MODULE_FAIL;

    return ZBX_MODULE_OK;
}

int uninit_config()
{
    return EXIT_SUCCESS;
}
