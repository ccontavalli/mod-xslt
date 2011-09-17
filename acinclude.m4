#
#   mod-xslt -- Copyright (C) 2002-2008 
# 		 Carlo Contavalli 
# 		 <ccontavalli at inscatolati.net>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

# AC_DEFINED(VARIABLE, [ACTION_IF_DEFINED], [ACTION_IF_NOT_DEFINED])
AC_DEFUN([AC_DEFINED], [{ 
  
  # If value is not defined
m4_ifvaln([$3], [
  if test "x$$1" = "x"; then
    $3
  fi
])dnl

  # If value is defined
m4_ifvaln([$2], [
  if test "x$$1" != "x"; then
    $2
  fi
])dnl
}] ) 

# AC_ISCMD(VARIABLE, PARAMETER, [ACTION_IF_IT_IS], [ACTION_IF_IT_IS_NOT])
AC_DEFUN([AC_ISCMD], [ { 
  $$1 $2 >/dev/null 2>/dev/null; 
  __TMP_STATUS="$?"

m4_ifvaln([$3], [ 
  if test "$__TMP_STATUS" = "0"; then 
    $3 
  fi])dnl

m4_ifvaln([$4], [
  if test "$__TMP_STATUS" != "0"; then 
    $4 
  fi])dnl
} ])

# AC_APACHE_APXS([ACTION_IF_FOUND], [ACTION_IF_NOT_FOUND])
AC_DEFUN([AC_APACHE_APXS], [ { 
  if test "x$APXS" = "x"; then
    AC_ARG_WITH(apxs, AC_HELP_STRING([[--with-apxs=FILE]], [Path to apache apxs program]), 
                [ APXS="$withval" ], [AC_PATH_PROGS(APXS, [$1], 
		[$PATH:/usr/sbin/:/usr/local/apache/bin/:/usr/local/apache2/bin])])
 
    AC_DEFINED(APXS,,[$2])
    AC_ISCMD(APXS, [-q CC],,[$3])
  fi
} ])

# AC_SAPI_STEP(CURRENT_STEP, NEXT_STEP, MESSAGE, VAR)
AC_DEFUN([AC_SAPI_STEP], [ { 
  __current_step="$1"
  __possible_steps="$2"
  __message="$3"

  if test "x$$4" = "xtrue"; then
    for __next_step in $__possible_steps; do
      if test "x$__next_step" != "x$sapi_previous"; then
        if test "x$__next_step" = "x$__current_step"; then
          m4_pattern_allow([AC_SAPI_STEP])
          AC_MSG_ERROR([Detected loop in [AC_SAPI_STEP]. Please report this as a bug!])
          m4_pattern_forbid([AC_SAPI_STEP])
        fi

        AC_MSG_NOTICE([$__current_step: $3])
        AC_MSG_NOTICE([$__current_step: Falling back to '$__next_step'])
 
        sapi_previous="$__current_step" 
        SAPI="$__next_step"
        continue 2
      fi
    done
  fi

  AC_MSG_ERROR([$1: $3])
} ])
