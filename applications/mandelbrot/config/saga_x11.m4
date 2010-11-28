#
# SYNOPSIS
#
#   AX_SAGA_CHECK_LIBX11([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   Test for the X11 library of a particular version (or newer)
#
#   If no path to the installed libx11 library is given,
#   the macro searchs under /usr, /usr/local, /opt and
#   /usr/local/package/libx11-*
#
#   This macro calls:
#
#     AC_SUBST(HAVE_LIBX11)
#     AC_SUBST(LIBX11_LOCATION)
#     AC_SUBST(LIBX11_CPPFLAGS) 
#     AC_SUBST(LIBX11_LDFLAGS)
#     AC_SUBST(LIBX11_S_LIBS)
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

AC_DEFUN([AX_SAGA_CHECK_LIBX11],
[
  AC_ARG_VAR([LIBX11_LOCATION],[LIBX11 installation directory])

  HAVE_LIBX11="no"
  tmp_location=""

  AC_ARG_WITH([libx11-location],
              AS_HELP_STRING([--with-libx11-location=DIR],
              [use libx11 (default is 'autodetect') at DIR (optional)]),
              [
              if test "$withval" = "no"; then
                tmp_location=""
              elif test "$withval" = "yes"; then
                tmp_location=""
              else
                tmp_location="$withval"
              fi
              ],
              [tmp_location=""])

  # use LIBX11_LOCATION if avaialble, and if not 
  # overwritten by --with-libx11=<dir>

  packages=`ls /usr/local/package/X11* 2>>/dev/null`
  
  for tmp_path in $tmp_location $LIBX11_LOCATION /usr /usr/X11 /usr/local /usr/local/X11 /opt /opt/local $packages; do
    
    AC_MSG_CHECKING(for libx11 in $tmp_path)

    have_something=`ls $tmp_path/lib/libX11.*   2>/dev/null`

    saved_cppflags=$CPPFLAGS
    saved_ldflags=$LDFLAGS

    LIBX11_PATH=$tmp_path
    LIBX11_LDFLAGS="-L$tmp_path/lib/ -lX11"
    LIBX11_CPPFLAGS="-I$tmp_path/include/"

    CPPFLAGS="$CPPFLAGS $LIBX11_CPPFLAGS"
    export CPPFLAGS

    LDFLAGS="$LDFLAGS $LIBX11_LDFLAGS"
    export LDFLAGS

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[@%:@include <X11/Xlib.h>]],
                                    [[
                                      (void) XOpenDisplay (NULL);
                                      return (0);
                                    ]])],
                                    link_libx11="yes",
                                    link_libx11="no")
          
    if test "x$link_libx11" = "xyes"; then

      AC_MSG_RESULT(yes)

      AC_MSG_CHECKING(for static lib libx11)
      if test -e "$tmp_path/lib/libX11.a"; then
        LIBX11_S_LIBS="$tmp_path/lib/libX11.a"
        AC_MSG_RESULT([$LIBX11_S_LIBS])
      else
        AC_MSG_RESULT([no])
      fi

      LIBX11_LOCATION=$tmp_path
      LIBX11_SOURCE="system"
      HAVE_LIBX11=yes

      export HAVE_LIBX11

      break;
      
    else # link ok

      AC_MSG_RESULT(no)
      LIBX11_LDFLAGS=""
      LIBX11_CPPFLAGS=""

    fi # link ok

  done # foreach path

  AC_SUBST(HAVE_LIBX11)
  AC_SUBST(LIBX11_LOCATION)
  AC_SUBST(LIBX11_SOURCE)
  AC_SUBST(LIBX11_CPPFLAGS)
  AC_SUBST(LIBX11_LDFLAGS)
  AC_SUBST(LIBX11_S_LIBS)
  AC_SUBST(LIBX11_NEEDS_BOOL)

])

