#
# SYNOPSIS
#
#   AX_SAGA_CHECK_LIBPNG([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   Test for the PNG library of a particular version (or newer)
#
#   If no path to the installed libpng library is given,
#   the macro searchs under /usr, /usr/local, /opt and
#   /usr/local/package/libpng-*
#
#   This macro calls:
#
#     AC_SUBST(HAVE_LIBPNG)
#     AC_SUBST(LIBPNG_LOCATION)
#     AC_SUBST(LIBPNG_CPPFLAGS) 
#     AC_SUBST(LIBPNG_LDFLAGS)
#     AC_SUBST(LIBPNG_S_LIBS)
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

AC_DEFUN([AX_SAGA_CHECK_LIBPNG],
[
  AC_ARG_VAR([LIBPNG_LOCATION],[LIBPNG installation directory])

  HAVE_LIBPNG="no"
  tmp_location="external"



  AC_ARG_WITH([libpng-location],
              AS_HELP_STRING([--with-libpng-location=DIR],
              [use libpng (default is 'external') at DIR (optional)]),
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

  if ! test "x$LIBPNG_LOCATION" = "x"; then
    tmp_location=$LIBPNG_LOCATION
  fi

  if test "x$tmp_location" = "xexternal"; then

    AC_MSG_CHECKING([for libpng])
    HAVE_LIBPNG=yes
    LIBPNG_SOURCE="external"
    LIBPNG_LOCATION="\$(SAGA_MB_ROOT)/external/libpng/"
    LIBPNG_CPPFLAGS="-I$LIBPNG_LOCATION"
    LIBPNG_LDFLAGS="$LIBPNG_LOCATION/libpng.a"
    LIBPNG_S_LIBS="$LIBPNG_LOCATION/libpng.a"
    AC_MSG_RESULT([no location specified, using external])

  else
    
    packages=`ls /usr/local/package/libpng-* 2>>/dev/null`
    
    for tmp_path in $tmp_location $LIBPNG_LOCATION /usr /usr/local /opt /opt/local $packages; do
      
      AC_MSG_CHECKING(for libpng in $tmp_path)

      have_something=`ls $tmp_path/lib/libpng.*   2>/dev/null`

      saved_cppflags=$CPPFLAGS
      saved_ldflags=$LDFLAGS

      LIBPNG_PATH=$tmp_path
      LIBPNG_LDFLAGS="-L$tmp_path/lib/ -lpng -lz"
      LIBPNG_CPPFLAGS="-I$tmp_path/include/"

      CPPFLAGS="$CPPFLAGS $LIBPNG_CPPFLAGS"
      export CPPFLAGS

      LDFLAGS="$LDFLAGS $LIBPNG_LDFLAGS"
      export LDFLAGS

      AC_LINK_IFELSE([AC_LANG_PROGRAM([[@%:@include <png.h>]],
                                      [[
                                        (void) png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
                                        return (0);
                                      ]])],
                                      link_libpng="yes",
                                      link_libpng="no")
            
      if test "x$link_libpng" = "xyes"; then

        AC_MSG_RESULT(yes)

        AC_MSG_CHECKING(for static lib libpng)
        if test -e "$tmp_path/lib/libpng.a"; then
          LIBPNG_S_LIBS="$tmp_path/lib/libpng.a"
          AC_MSG_RESULT([$LIBPNG_S_LIBS])
        else
          AC_MSG_RESULT([no])
        fi

        LIBPNG_LOCATION=$tmp_path
        LIBPNG_SOURCE="system"
        HAVE_LIBPNG=yes

        export HAVE_LIBPNG

        break;
        
      else # link ok

        AC_MSG_RESULT(no)
        LIBPNG_LDFLAGS=""
        LIBPNG_CPPFLAGS=""

      fi # link ok

    done # foreach path

  fi # tmp_location == 'external


  # fall back to external again
  if test "x$HAVE_LIBPNG" = "xno"; then
    HAVE_LIBPNG=yes
    LIBPNG_SOURCE="external"
    LIBPNG_LOCATION="\$(SAGA_MB_ROOT)/external/libpng/"
    LIBPNG_CPPFLAGS="-I$LIBPNG_LOCATION"
    LIBPNG_LDFLAGS="$LIBPNG_LOCATION/libpng.a"
    LIBPNG_S_LIBS="$LIBPNG_LOCATION/libpng.a"
  fi


  AC_SUBST(HAVE_LIBPNG)
  AC_SUBST(LIBPNG_LOCATION)
  AC_SUBST(LIBPNG_SOURCE)
  AC_SUBST(LIBPNG_CPPFLAGS)
  AC_SUBST(LIBPNG_LDFLAGS)
  AC_SUBST(LIBPNG_S_LIBS)
  AC_SUBST(LIBPNG_NEEDS_BOOL)

])

