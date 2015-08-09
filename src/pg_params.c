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

// TODO: Add clean up func
 
int param_len(PGparams params)
{
    int  len = 0;
    char **c = NULL;

    if (NULL == params)
        return 0;

    for (c = params; *c; c++) 
        len++;

    return len;
}

char **param_append(PGparams dest, char *s)
{
    int len = 0;

    // never append nulls
    if(NULL == s)
        return dest;

    // allocate new array
    if(NULL == dest) {
        dest = zbx_malloc(dest, sizeof(PGparams) * 2);
        dest[0] = strdup(s);
        dest[1] = NULL;
        return dest;
    } 

    // extend array and append
    len = param_len(dest);
    dest = zbx_realloc(dest, len + 2);
    dest[len] = strdup(s); // dup so we can free everything later
    dest[len + 1] = NULL;

    return dest;
}

void param_free(PGparams params)
{
    PGparams p = NULL;

    if (NULL == params)
        return;
    
    for (p = params; *p; p++)
        zbx_free(*p);

    zbx_free(params);

    zabbix_log(LOG_LEVEL_DEBUG, "Free param list %p", params);
}