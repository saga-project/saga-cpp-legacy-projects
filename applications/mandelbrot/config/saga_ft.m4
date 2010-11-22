#
# SYNOPSIS
#
#   AX_SAGA_CHECK_LIBFREETYPE([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   Test for the FREETYPE library of a particular version (or newer)
#
#   If no path to the installed libfreetype library is given,
#   the macro searchs under /usr, /usr/local, /opt and
#   /usr/local/package/libfreetype-*
#
#   This macro calls:
#
#     AC_SUBST(HAVE_LIBFREETYPE)
#     AC_SUBST(LIBFREETYPE_LOCATION)
#     AC_SUBST(LIBFREETYPE_CPPFLAGS) 
#     AC_SUBST(LIBFREETYPE_LDFLAGS)
#     AC_SUBST(LIBFREETYPE_S_LIBS)
#
# LAST MODIFICATION
#
#   2010-11-22
#
# COPYLEFREETYPE
#
#   Copyright (c) 2007 Andre Merzky      <andre@merzky.net>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty
#   provided the copyright notice and this notice are preserved.

AC_DEFUN([AX_SAGA_CHECK_LIBFT],
[
  AC_ARG_VAR([LIBFREETYPE_LOCATION],[LIBFREETYPE installation directory])

  #AC_MSG_NOTICE([LIBFREETYPE_LOCATION: $LIBFREETYPE_LOCATION])

  HAVE_LIBFREETYPE=no

  tmp_location=""
  AC_ARG_WITH([libfreetype],
              AS_HELP_STRING([--with-libfreetype=DIR],
              [use libfreetype (default is YES) at DIR (optional)]),
              [
              if test "$withval" = "no"; then
                want_libfreetype="no"
              elif test "$withval" = "yes"; then
                want_libfreetype="yes"
                tmp_location=""
              else
                want_libfreetype="yes"
                tmp_location="$withval"
              fi
              ],
              [want_libfreetype="yes"])

  # use LIBFREETYPE_LOCATION if avaialble, and if not 
  # overwritten by --with-libfreetype=<dir>

  if test "x$want_libfreetype" = "xyes"; then
    
    packages=`ls /usr/local/package/libfreetype-* 2>>/dev/null`
    
    for tmp_path in $tmp_location $LIBFREETYPE_LOCATION /usr /usr/local /opt /opt/local $packages; do
      
      AC_MSG_CHECKING(for libfreetype in $tmp_path)

      have_something=`ls $tmp_path/lib/liblibfreetype.*   2>/dev/null`

      saved_cppflags=$CPPFLAGS
      saved_ldflags=$LDFLAGS

      LIBFREETYPE_PATH=$tmp_path
      LIBFREETYPE_LDFLAGS="-L$tmp_path/lib/ -lfreetype"
      LIBFREETYPE_CPPFLAGS="-I$tmp_path/include/"

      CPPFLAGS="$CPPFLAGS $LIBFREETYPE_CPPFLAGS"
      export CPPFLAGS

      LDFLAGS="$LDFLAGS $LIBFREETYPE_LDFLAGS"
      export LDFLAGS

      AC_LINK_IFELSE([AC_LANG_PROGRAM([[@%:@include <freetype/freetype.h>]],
                                      [[
                                        FREETYPE_Library  library;
                                        (void) Init_FreeType( &library );
                                        return (0);
                                      ]])],
                                      link_libfreetype="yes",
                                      link_libfreetype="no")
            
      if test "x$link_libfreetype" = "xyes"; then

        AC_MSG_RESULT(yes)

        AC_MSG_CHECKING(for static lib libfreetype)
        if test -e "$tmp_path/lib/libfreetype.a"; then
          LIBFREETYPE_S_LIBS="$tmp_path/lib/libfreetype.a"
          AC_MSG_RESULT([$LIBFREETYPE_S_LIBS])
        else
          AC_MSG_RESULT([no])
        fi

        LIBFREETYPE_LOCATION=$tmp_path
        HAVE_LIBFREETYPE=yes

        export HAVE_LIBFREETYPE

        AC_SUBST(HAVE_LIBFREETYPE)
        AC_SUBST(LIBFREETYPE_LOCATION)
        AC_SUBST(LIBFREETYPE_CPPFLAGS)
        AC_SUBST(LIBFREETYPE_LDFLAGS)
        AC_SUBST(LIBFREETYPE_S_LIBS)
        AC_SUBST(LIBFREETYPE_NEEDS_BOOL)

        break;
        
      else # link ok

        AC_MSG_RESULT(no)

      fi # link ok

    done # foreach path

  fi # want_libfreetype

])

