AC_INIT([libzbxpgsql], [1.1.0], [ryan@cavaliercoder.com])
AM_INIT_AUTOMAKE([-Wall -Werror -Wno-portability foreign])
m4_pattern_allow([AM_PROG_AR])
AM_PROG_AR
AC_PROG_LIBTOOL # seem to need this on CentOS 5 (libtool v1.5)
LT_INIT
AC_PROG_CC
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

# check for stdlib header files
AC_HEADER_STDC

# check for libconfig
AC_CHECK_HEADERS(libconfig.h, [], [ AC_MSG_ERROR("Unable to locate libconfig development headers.")])
AC_CHECK_LIB([config],[config_init],[],AC_MSG_ERROR(["Unable to locate libconfig library."]))

# Checking for PostgreSQL support
AX_LIB_POSTGRESQL("8.1")
if test "x$found_postgresql" = "xyes"; then
	if test "$postgresql_version_check" != "1"; then
		AC_MSG_ERROR([PostgreSQL version mismatch])
	fi
else
	AC_MSG_ERROR([PostgreSQL library not found])
fi

# Checking for Zabbix headers
AX_LIB_ZABBIX
if test ! "x$found_zabbix" = "xyes"; then
	AC_MSG_ERROR([Zabbix headers not found])
fi

# output
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_OUTPUT
