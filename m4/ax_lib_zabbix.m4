#
# SYNOPSIS
#
#   AX_LIB_ZABBIX
#
# DESCRIPTION
#
#   This macro provides tests of availability of Zabbix source headers.
#
#   AX_LIB_ZABBIX macro takes no arguments.
#
#   The --with-zabbix option takes one of three possible values:
#
#   no - do not check for Zabbix headers
#
#   yes - do check for Zabbix headers in the default location
#
#   path - complete path to Zabbix source headers
#
#   This macro calls:
#
#     AC_SUBST(ZABBIX_CPPFLAGS)
#     AC_SUBST(ZABBIX_HEADERS)
#
#   And sets:
#
#     HAVE_ZABBIX
#
# LICENSE
#
#   Copyright (c) 2016 Ryan Armstrong <ryan@cavaliercoder.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 1

AC_DEFUN([AX_LIB_ZABBIX],
[
    ZABBIX_HEADERS="/usr/src/zabbix/include"

    AC_ARG_WITH([zabbix],
        AS_HELP_STRING([--with-zabbix=@<:@ARG@:>@],
            [use Zabbix headers @<:@default=/usr/src/zabbix/include@:>@, optionally specify path to Zabbix source headers]
        ),
        [
        if test "$withval" = "no"; then
            want_zabbix="no"
        elif test "$withval" = "yes"; then
            want_zabbix="yes"
        else
            want_zabbix="yes"
            ZABBIX_HEADERS="$withval"
        fi
        ],
        [want_zabbix="yes"]
    )

    ZABBIX_CPPFLAGS=""

    dnl
    dnl Check Zabbix headers
    dnl

    if test "$want_zabbix" = "yes"; then        
        AC_MSG_CHECKING([for Zabbix header files])

        if test ! -f "$ZABBIX_HEADERS/module.h"; then
            dnl test in .../include
            if test -f "$ZABBIX_HEADERS/include/module.h"; then
                ZABBIX_HEADERS="$ZABBIX_HEADERS/include"
            else
                found_zabbix="no"
                AC_MSG_RESULT([no])

                AC_MSG_ERROR([$ZABBIX_HEADERS/module.h does not exist])
                ZABBIX_HEADERS="no"
            fi
        fi

        if test "$ZABBIX_HEADERS" != "no"; then
            ZABBIX_CPPFLAGS="-I$ZABBIX_HEADERS"
            AC_DEFINE([HAVE_ZABBIX], [1],
                [Define to 1 if Zabbix headers are available])

            found_zabbix="yes"
            AC_MSG_RESULT([yes])
        fi
    fi

    AC_SUBST([ZABBIX_CPPFLAGS])
    AC_SUBST([ZABBIX_HEADERS])
])
