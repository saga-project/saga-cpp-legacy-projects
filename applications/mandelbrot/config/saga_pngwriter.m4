
# SYNOPSIS
#
#   AX_SAGA_CHECK_LIBPNGWRITER([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   Test for the PNGWRITER library of a particular version (or newer)
#
#   If no path to the installed libpngwriter library is given,
#   the macro searchs under /usr, /usr/local, /opt and
#   /usr/local/package/libpngwriter-*
#
#   This macro calls:
#
#     AC_SUBST(HAVE_LIBPNGWRITER)
#     AC_SUBST(LIBPNGWRITER_LOCATION)
#     AC_SUBST(LIBPNGWRITER_CPPFLAGS) 
#     AC_SUBST(LIBPNGWRITER_LDFLAGS)
#     AC_SUBST(LIBPNGWRITER_S_LIBS)
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

AC_DEFUN([AX_SAGA_CHECK_LIBPNGWRITER],
[
  AC_ARG_VAR([LIBPNGWRITER_LOCATION],[LIBPNGWRITER installation directory])

  HAVE_LIBPNGWRITER="no"
  tmp_location="external"

  AC_ARG_WITH([libpngwriter-location],
              AS_HELP_STRING([--with-libpngwriter-location=DIR],
              [use libpngwriter (default is 'external') at DIR (optional)]),
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

  # use LIBPNGWRITER_LOCATION if avaialble, and if not 
  # overwritten by --with-libpngwriter=<dir>

  echo "tmp: $tmp_location"

  if test "x$tmp_location" = "xexternal"; then

    HAVE_LIBPNGWRITER=yes
    LIBPNGWRITER_SOURCE="external"
    LIBPNGWRITER_LOCATION="\$(SAGA_MB_ROOT)/external/libpngwriter/"
    LIBPNGWRITER_CPPFLAGS="-I$LIBPNGWRITER_LOCATION"
    LIBPNGWRITER_LDFLAGS="$LIBPNGWRITER_LOCATION/libpngwriter.a"
    LIBPNGWRITER_S_LIBS="$LIBPNGWRITER_LOCATION/libpngwriter.a"

  else
    
    packages=`ls /usr/local/package/libpngwriter-* 2>>/dev/null`
    
    for tmp_path in $tmp_location $LIBPNGWRITER_LOCATION /usr /usr/local /opt /opt/local $packages; do
      
      AC_MSG_CHECKING(for libpngwriter in $tmp_path)

      have_something=`ls $tmp_path/lib/libpngwriter.*   2>/dev/null`

      saved_cppflags=$CPPFLAGS
      saved_ldflags=$LDFLAGS

      LIBPNGWRITER_PATH=$tmp_path
      LIBPNGWRITER_LDFLAGS="-L$tmp_path/lib/ -lpngwriter"
      LIBPNGWRITER_CPPFLAGS="-I$tmp_path/include/"

      CPPFLAGS="$CPPFLAGS $LIBPNGWRITER_CPPFLAGS"
      export CPPFLAGS

      LDFLAGS="$LDFLAGS $LIBPNGWRITER_LDFLAGS"
      export LDFLAGS

      AC_LINK_IFELSE([AC_LANG_PROGRAM([[@%:@include <pngwriter.h>]],
                                      [[
                                        pngwriter png (10, 10, 256, "/tmp/test.$$.png");
                                        return (0);
                                      ]])],
                                      link_libpngwriter="yes",
                                      link_libpngwriter="no")
            
      if test "x$link_libpngwriter" = "xyes"; then

        AC_MSG_RESULT(yes)

        AC_MSG_CHECKING(for static lib libpngwriter)
        if test -e "$tmp_path/lib/libpngwriter.a"; then
          LIBPNGWRITER_S_LIBS="$tmp_path/lib/libpngwriter.a"
          AC_MSG_RESULT([$LIBPNGWRITER_S_LIBS])
        else
          AC_MSG_RESULT([no])
        fi

        LIBPNGWRITER_LOCATION=$tmp_path
        LIBPNGWRITER_SOURCE="system"
        HAVE_LIBPNGWRITER=yes

        export HAVE_LIBPNGWRITER

        break;
        
      else # link ok

        AC_MSG_RESULT(no)
        LIBPNGWRITER_LDFLAGS=""
        LIBPNGWRITER_CPPFLAGS=""

      fi # link ok

    done # foreach path

  fi # tmp_location == 'external


  # fall back to external again
  if test "x$HAVE_LIBPNGWRITER" = "xno"; then
    HAVE_LIBPNGWRITER=yes
    LIBPNGWRITER_SOURCE="external"
    LIBPNGWRITER_LOCATION="\$(SAGA_MB_ROOT)/external/libpngwriter/"
    LIBPNGWRITER_CPPFLAGS="-I$LIBPNGWRITER_LOCATION"
    LIBPNGWRITER_LDFLAGS="$LIBPNGWRITER_LOCATION/libpngwriter.a"
    LIBPNGWRITER_S_LIBS="$LIBPNGWRITER_LOCATION/libpngwriter.a"
  fi


  AC_SUBST(HAVE_LIBPNGWRITER)
  AC_SUBST(LIBPNGWRITER_LOCATION)
  AC_SUBST(LIBPNGWRITER_SOURCE)
  AC_SUBST(LIBPNGWRITER_CPPFLAGS)
  AC_SUBST(LIBPNGWRITER_LDFLAGS)
  AC_SUBST(LIBPNGWRITER_S_LIBS)
  AC_SUBST(LIBPNGWRITER_NEEDS_BOOL)

])

