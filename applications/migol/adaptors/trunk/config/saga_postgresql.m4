#
# SYNOPSIS
#
#   AX_SAGA_CHECK_POSTGRESQL([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   Test for the POSTGRESQL libraries of a particular version (or newer)
#
#   If no path to the installed postgresql library is given,
#   the macro searchs under /usr, /usr/local, /opt, /usr/local/pgsql
#   and /usr/local/package/postgresql-*
#
#   This macro calls:
#
#     AC_SUBST(HAVE_POSTGRESQL)
#     AC_SUBST(POSTGRESQL_LOCATION)
#     AC_SUBST(POSTGRESQL_CPPFLAGS) 
#     AC_SUBST(POSTGRESQL_LDFLAGS)
#     AC_SUBST(POSTGRESQL_S_LIBS)
#
# LAST MODIFICATION
#
#   2007-03-04
#
# COPYLEFT
#
#   Copyright (c) 2007 Andre Merzky      <andre@merzky.net>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_SAGA_CHECK_POSTGRESQL],
[
  AC_ARG_VAR([POSTGRESQL_LOCATION],[POSTGRESQL installation directory])

  HAVE_POSTGRESQL=no

  tmp_location=""
  AC_ARG_WITH([postgresql],
              AS_HELP_STRING([--with-postgresql=DIR],
              [use postgresql (default is YES) at DIR (optional)]),
              [
              if test "$withval" = "no"; then
                want_postgresql="no"
              elif test "$withval" = "yes"; then
                want_postgresql="yes"
                tmp_location=""
              else
                want_postgresql="yes"
                tmp_location="$withval"
              fi
              ],
              [want_postgresql="yes"])

  # use POSTGRESQL_LOCATION if avaialble, and if not 
  # overwritten by --with-postgresql=<dir>

  if test "x$want_postgresql" = "xyes"; then
    
    packages=`ls /usr/local/package/postgresql-* 2>>/dev/null`
    
    for tmp_path in $tmp_location $POSTGRESQL_LOCATION /usr /usr/local /usr/local/pgsql /opt $packages; do
      
      AC_MSG_CHECKING(for postgresql in $tmp_path)

      have_something=`ls $tmp_path/lib/libpq* 2>/dev/null`

      if test "x$have_something" = "x"; then
        AC_MSG_RESULT(no)
        continue
      fi

      saved_cppflags=$CPPFLAGS
      saved_ldflags=$LDFLAGS

      POSTGRESQL_PATH=$tmp_path
      POSTGRESQL_LDFLAGS="-L$tmp_path/lib/ -lpq"
      POSTGRESQL_CPPFLAGS="-I$tmp_path/include -I$tmp_path/include/pgsql -I$tmp_path/include/postgresql"

      CPPFLAGS="$CPPFLAGS $POSTGRESQL_CPPFLAGS"
      export CPPFLAGS

      LDFLAGS="$LDFLAGS $POSTGRESQL_LDFLAGS"
      export LDFLAGS

      AC_LINK_IFELSE([AC_LANG_PROGRAM([[@%:@include <libpq-fe.h>]],
                                      [[
                                        PGconn* connection = PQconnectdb ("testdb");
                                        return (0);
                                      ]])],
                                      link_postgresql="yes",
                                      link_postgresql="no")
            
      if test "x$link_postgresql" = "xno"; then
      
        AC_MSG_RESULT(no)

        LDFLAGS=$saved_ldflags
        CPPFLAGS=$saved_cppflags
        POSTGRESQL_LOCATION=""
      
      else

        AC_MSG_RESULT(yes)

        POSTGRESQL_LOCATION=$tmp_path
        HAVE_POSTGRESQL=yes

        export HAVE_POSTGRESQL

        AC_MSG_CHECKING(for postgresql version)
        # check for version > 8.x
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[@%:@include <pg_config.h>]],
                                           [[#if PG_VERSION_NUM < 80000
                                             # error "using postgres version 7.x
                                             #endif]])],
                                           old_postgres="no",
                                           old_postgres="yes")

        if test "x$old_postgres" = "xyes"; then
          AC_MSG_RESULT([older than 8.0])
          AC_DEFINE(SOCI_PGSQL_NOPARAMS,  1)
          AC_DEFINE(SOCI_PGSQL_NOPREPARE, 1)
          AGE_POSTGRESQL="older than 8.0"
        else
          AC_MSG_RESULT([newer than 8.0])
          AGE_POSTGRESQL="newer than 8.0"
        fi

        AC_MSG_CHECKING(for static lib postgresql)
        if test -e "$tmp_path/lib/libpq.a"; then
          POSTGRESQL_S_LIBS="$tmp_path/lib/libpq.a"
          AC_MSG_RESULT([$POSTGRESQL_S_LIBS])
        else
          AC_MSG_RESULT([no])
        fi

        AC_SUBST(HAVE_POSTGRESQL)
        AC_SUBST(AGE_POSTGRESQL)
        AC_SUBST(POSTGRESQL_LOCATION)
        AC_SUBST(POSTGRESQL_CPPFLAGS)
        AC_SUBST(POSTGRESQL_LDFLAGS)
        AC_SUBST(POSTGRESQL_S_LIBS)
        
        #if test "$tmp_location" != "" && \
        #   test "$tmp_location" != "$tmp_path" ; then
        #  AC_MSG_WARN([POSTGRESQL found:
        #               not in $tmp_location
        #               but in $tmp_path])
        #fi

        break;

      fi # link ok

    done # foreach path


    #if test "$HAVE_POSTGRESQL" == "no" ; then
    #  POSTGRESQL_LOCATION=""
    #  AC_MSG_WARN(POSTGRESQL not found)
    #fi

  fi # want_postgresql

])

