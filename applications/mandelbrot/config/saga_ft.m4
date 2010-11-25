#
# SYNOPSIS
#
#   AX_SAGA_CHECK_LIBFT([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   Test for the FT library of a particular version (or newer)
#
#   If no path to the installed libfreetype library is given,
#   the macro searchs under /usr, /usr/local, /opt and
#   /usr/local/package/libfreetype-*
#
#   This macro calls:
#
#     AC_SUBST(HAVE_LIBFT)
#     AC_SUBST(LIBFT_LOCATION)
#     AC_SUBST(LIBFT_CPPFLAGS) 
#     AC_SUBST(LIBFT_LDFLAGS)
#     AC_SUBST(LIBFT_S_LIBS)
#
# LAST MODIFICATION
#
#   2010-11-22
#
# COPYLEFT
#
#   Copyright (c) 2007 Andre Merzky      <andre@merzky.net>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty
#   provided the copyright notice and this notice are preserved.

AC_DEFUN([AX_SAGA_CHECK_LIBFT],
[
  AC_ARG_VAR([LIBFT_LOCATION],[LIBFT installation directory])

  HAVE_LIBFT="no"
  tmp_location="external"

  AC_ARG_WITH([libfreetype-location],
              AS_HELP_STRING([--with-libfreetype-location=DIR],
              [use libfreetype (default is 'external') at DIR (optional)]),
              [
              if test "$withval" = "no"; then
                tmp_location="external"
              elif test "$withval" = "yes"; then
                tmp_location=""
              else
                tmp_location="$withval"
              fi
              ],
              [tmp_location="external"])

  if ! test "x$LIBFT_LOCATION" = "x"; then
    tmp_location=$LIBFT_LOCATION
  fi

  if test "x$tmp_location" = "xexternal"; then

    AC_MSG_CHECKING([for libfreetype])
    HAVE_LIBFT=yes
    LIBFT_SOURCE="external"
    LIBFT_LOCATION="\$(SAGA_MB_ROOT)/external/libfreetype/"
    LIBFT_CPPFLAGS="-I$LIBFT_LOCATION/include"
    LIBFT_LDFLAGS="$LIBFT_LOCATION/libfreetype.a"
    LIBFT_S_LIBS="$LIBFT_LOCATION/libfreetype.a"
    AC_MSG_RESULT([no location specified, using external])

  else
    
    packages=`ls /usr/local/package/libfreetype-* 2>>/dev/null`
    
    for tmp_path in $tmp_location $LIBFT_LOCATION /usr /usr/local /opt /opt/local $packages; do
      
      AC_MSG_CHECKING(for libfreetype in $tmp_path)

      have_something=`ls $tmp_path/lib/libfreetype.* 2>/dev/null`

      saved_cppflags=$CPPFLAGS
      saved_ldflags=$LDFLAGS

      LIBFT_PATH=$tmp_path
      LIBFT_LDFLAGS="-L$tmp_path/lib/ -lfreetype"
      LIBFT_CPPFLAGS="-I$tmp_path/include/"

      CPPFLAGS="$CPPFLAGS $LIBFT_CPPFLAGS"
      export CPPFLAGS

      LDFLAGS="$LDFLAGS $LIBFT_LDFLAGS"
      export LDFLAGS

      AC_LINK_IFELSE([AC_LANG_PROGRAM([[@%:@include <freetype/freetype.h>]],
                                      [[
                                        FT_Library  library;
                                        (void) Init_FreeType( &library );
                                        return (0);
                                      ]])],
                                      link_libfreetype="yes",
                                      link_libfreetype="no")
            
      if test "x$link_libfreetype" = "xyes"; then

        AC_MSG_RESULT(yes)

        AC_MSG_CHECKING(for static lib libfreetype)
        if test -e "$tmp_path/lib/libfreetype.a"; then
          LIBFT_S_LIBS="$tmp_path/lib/libfreetype.a"
          AC_MSG_RESULT([$LIBFT_S_LIBS])
        else
          AC_MSG_RESULT([no])
        fi

        LIBFT_LOCATION=$tmp_path
        LIBFT_SOURCE="system"
        HAVE_LIBFT=yes

        export HAVE_LIBFT

        break;
        
      else # link ok

        AC_MSG_RESULT(no)
        LIBFT_LDFLAGS=""
        LIBFT_CPPFLAGS=""

      fi # link ok

    done # foreach path

  fi # tmp_location == 'external

  # fall back to external again
  if test "x$HAVE_LIBFT" = "xno"; then
    HAVE_LIBFT=yes
    LIBFT_SOURCE="external"
    LIBFT_LOCATION="\$(SAGA_MB_ROOT)/external/libfreetype/"
    LIBFT_CPPFLAGS="-I$LIBFT_LOCATION/include"
    LIBFT_LDFLAGS="$LIBFT_LOCATION/libfreetype.a"
    LIBFT_S_LIBS="$LIBFT_LOCATION/libfreetype.a"
  fi

  AC_SUBST(HAVE_LIBFT)
  AC_SUBST(LIBFT_LOCATION)
  AC_SUBST(LIBFT_SOURCE)
  AC_SUBST(LIBFT_CPPFLAGS)
  AC_SUBST(LIBFT_LDFLAGS)
  AC_SUBST(LIBFT_S_LIBS)
  AC_SUBST(LIBFT_NEEDS_BOOL)

])

