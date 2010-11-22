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

  #AC_MSG_NOTICE([LIBPNG_LOCATION: $LIBPNG_LOCATION])

  HAVE_LIBPNG=no

  tmp_location=""
  AC_ARG_WITH([libpng],
              AS_HELP_STRING([--with-libpng=DIR],
              [use libpng (default is YES) at DIR (optional)]),
              [
              if test "$withval" = "no"; then
                want_libpng="no"
              elif test "$withval" = "yes"; then
                want_libpng="yes"
                tmp_location=""
              else
                want_libpng="yes"
                tmp_location="$withval"
              fi
              ],
              [want_libpng="yes"])

  # use LIBPNG_LOCATION if avaialble, and if not 
  # overwritten by --with-libpng=<dir>

  if test "x$want_libpng" = "xyes"; then
    
    packages=`ls /usr/local/package/libpng-* 2>>/dev/null`
    
    for tmp_path in $tmp_location $LIBPNG_LOCATION /usr /usr/local /opt /opt/local $packages; do
      
      AC_MSG_CHECKING(for libpng in $tmp_path)

      have_something=`ls $tmp_path/lib/liblibpng.*   2>/dev/null`

      saved_cppflags=$CPPFLAGS
      saved_ldflags=$LDFLAGS

      LIBPNG_PATH=$tmp_path
      LIBPNG_LDFLAGS="-L$tmp_path/lib/ -lpng"
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
        HAVE_LIBPNG=yes

        export HAVE_LIBPNG

        AC_SUBST(HAVE_LIBPNG)
        AC_SUBST(LIBPNG_LOCATION)
        AC_SUBST(LIBPNG_CPPFLAGS)
        AC_SUBST(LIBPNG_LDFLAGS)
        AC_SUBST(LIBPNG_S_LIBS)
        AC_SUBST(LIBPNG_NEEDS_BOOL)

        break;
        
      else # link ok

        AC_MSG_RESULT(no)

      fi # link ok

    done # foreach path

  fi # want_libpng

])

