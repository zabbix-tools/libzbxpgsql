# SYNOPSIS
#
#   AX_PROG_GIT()
#
# DESCRIPTION
#
#   This macro provides tests of availability of git.
#
#   The --with-gitoption takes one of three possible values:
#
#   no - do not check for git
#
#   yes - do check for git in PATH
#
#   path - complete path to git binary, use this option if git can't be found
#   in the PATH
#
#   This macro calls:
#
#     AC_SUBST(GIT)
#     AC_SUBST(GIT_VERSION)
#     AM_CONDITIONAL([HAVE_GIT])
#
# LICENSE
#
#   Copyright (c) 2015 Ryan Armstrong <ryan@cavaliercoder.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

AC_DEFUN([AX_PROG_GIT],
[
    AC_ARG_WITH([git],
        AS_HELP_STRING([--with-git=@<:@ARG@:>@],
            [use Git @<:@default=yes@:>@, optionally specify path to git binary]
        ),
        [
        if test "$withval" = "no"; then
            want_git="no"
        elif test "$withval" = "yes"; then
            want_git="yes"
        else
            want_git="yes"
            GIT="$withval"
        fi
        ],
        [want_git="yes"]
    )

    GIT=""
    GIT_VERSION=""

    dnl
    dnl Check for git
    dnl

    if test "$want_git" = "yes"; then

        if test -z "$GIT" -o test; then
            AC_PATH_PROG([GIT], [git], [])
        fi

        if test ! -x "$GIT"; then
            AC_MSG_ERROR([$GIT does not exist or it is not an exectuable file])
            GIT="no"
            found_git="no"
        fi

        if test "$GIT" != "no"; then
            AC_MSG_CHECKING([for git])

            GIT_VERSION=`$GIT --version | sed -e 's/git version//'`

            #AC_DEFINE([HAVE_GIT], [1],
            #    [Define to 1 if Git is available])

            found_git="yes"
            AC_MSG_RESULT([yes])
        else
            found_git="no"
            AC_MSG_RESULT([no])
        fi
    fi

    AM_CONDITIONAL([HAVE_GIT], [test x$found_git = xyes])

    AC_SUBST([GIT])
    AC_SUBST([GIT_VERSION])
])
