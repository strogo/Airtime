dnl-----------------------------------------------------------------------------
dnl Copyright (c) 2004 Media Development Loan Fund
dnl
dnl This file is part of the LiveSupport project.
dnl http://livesupport.campware.org/
dnl To report bugs, send an e-mail to bugs@campware.org
dnl
dnl LiveSupport is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl LiveSupport is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with LiveSupport; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
dnl
dnl
dnl Author   : $Author$
dnl Version  : $Revision$
dnl Location : $URL$
dnl-----------------------------------------------------------------------------

dnl-----------------------------------------------------------------------------
dnl Macro to check for available modules using pkg-conf
dnl
dnl usage:
dnl PKG_CHECK_MODULES(GSTUFF,[gtk+-2.0 >= 1.3], action-if, action-not)
dnl
dnl defines GSTUFF_LIBS, GSTUFF_CFLAGS, see pkg-config man page
dnl also defines GSTUFF_PKG_ERRORS on error
dnl
dnl This function was taken from the glade-- project
dnl-----------------------------------------------------------------------------
AC_DEFUN([PKG_CHECK_MODULES], [
  succeeded=no

  if test -z "$PKG_CONFIG"; then
    AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  fi

  if test "$PKG_CONFIG" = "no" ; then
     echo "*** The pkg-config script could not be found. Make sure it is"
     echo "*** in your path, or set the PKG_CONFIG environment variable"
     echo "*** to the full path to pkg-config."
     echo "*** Or see http://www.freedesktop.org/software/pkgconfig to get pkg-config."
  else
     PKG_CONFIG_MIN_VERSION=0.9.0
     if $PKG_CONFIG --atleast-pkgconfig-version $PKG_CONFIG_MIN_VERSION; then
        AC_MSG_CHECKING(for $2)

        if $PKG_CONFIG --exists "$2" ; then
            AC_MSG_RESULT(yes)
            succeeded=yes

            AC_MSG_CHECKING($1_CFLAGS)
            $1_CFLAGS=`$PKG_CONFIG --cflags "$2"`
            AC_MSG_RESULT($$1_CFLAGS)

            AC_MSG_CHECKING($1_LIBS)
            $1_LIBS=`$PKG_CONFIG --libs "$2"`
            AC_MSG_RESULT($$1_LIBS)
        else
            $1_CFLAGS=""
            $1_LIBS=""
            ## If we have a custom action on failure, don't print errors, but 
            ## do set a variable so people can do so.
            $1_PKG_ERRORS=`$PKG_CONFIG --errors-to-stdout --print-errors "$2"`
            ifelse([$4], ,echo $$1_PKG_ERRORS,)
        fi

        AC_SUBST($1_CFLAGS)
        AC_SUBST($1_LIBS)
     else
        echo "*** Your version of pkg-config is too old. You need version $PKG_CONFIG_MIN_VERSION or newer."
        echo "*** See http://www.freedesktop.org/software/pkgconfig"
     fi
  fi

  if test $succeeded = yes; then
     ifelse([$3], , :, [$3])
  else
     ifelse([$4], , AC_MSG_ERROR([Library requirements ($2) not met; consider adjusting the PKG_CONFIG_PATH environment variable if your libraries are in a nonstandard prefix so pkg-config can find them.]), [$4])
  fi
])



dnl-----------------------------------------------------------------------------
dnl Macro to check for C++ namespaces
dnl for more information on this macro, see
dnl http://autoconf-archive.cryp.to/ac_cxx_namespaces.html
dnl
dnl usage:
dnl  If the compiler can prevent names clashes using namespaces,
dnl  define HAVE_NAMESPACES.
dnl-----------------------------------------------------------------------------
AC_DEFUN([AC_CXX_NAMESPACES],
[AC_CACHE_CHECK(whether the compiler implements namespaces,
ac_cv_cxx_namespaces,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([namespace Outer { namespace Inner { int i = 0; }}],
                [using namespace Outer::Inner; return i;],
 ac_cv_cxx_namespaces=yes, ac_cv_cxx_namespaces=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_namespaces" = yes; then
  AC_DEFINE(HAVE_NAMESPACES,,[define if the compiler implements namespaces])
fi
])


dnl-----------------------------------------------------------------------------
dnl Macro to check for the boost datetime library.
dnl for more information on boost, see http://www.boost.org/
dnl for more information on this macro, see
dnl http://autoconf-archive.cryp.to/ax_boost_date-time.html
dnl
dnl usage:
dnl This macro checks to see if the Boost.DateTime library is installed.
dnl It also attempts to guess the currect library name using several attempts.
dnl It tries to build the library name using a user supplied name or suffix
dnl and then just the raw library.
dnl 
dnl If the library is found, HAVE_BOOST_DATE_TIME is defined and
dnl BOOST_DATE_TIME_LIB is set to the name of the library.
dnl 
dnl This macro calls AC_SUBST(BOOST_DATE_TIME_LIB).
dnl-----------------------------------------------------------------------------
AC_DEFUN([AX_BOOST_DATE_TIME],
[AC_REQUIRE([AC_CXX_NAMESPACES])dnl
AC_CACHE_CHECK(whether the Boost::DateTime library is available,
ax_cv_boost_date_time,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[#include <boost/date_time/gregorian/gregorian_types.hpp>]],
                                   [[using namespace boost::gregorian; date d(2002,Jan,10); return 0;]]),
                   ax_cv_boost_date_time=yes, ax_cv_boost_date_time=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_boost_date_time" = yes; then
  AC_DEFINE(HAVE_BOOST_DATE_TIME,,[define if the Boost::DateTime library is available])
  dnl Now determine the appropriate file names
  AC_ARG_WITH([boost-date-time],AS_HELP_STRING([--with-boost-date-time],
  [specify the boost date-time library or suffix to use]),
  [if test "x$with_boost_date_time" != "xno"; then
    ax_date_time_lib=$with_boost_date_time
    ax_boost_date_time_lib=boost_date_time-$with_boost_date_time
  fi])
  for ax_lib in $ax_date_time_lib $ax_boost_date_time_lib boost_date_time; do
    AC_CHECK_LIB($ax_lib, main, [BOOST_DATE_TIME_LIB=$ax_lib break])
  done
  AC_SUBST(BOOST_DATE_TIME_LIB)
fi
])dnl

