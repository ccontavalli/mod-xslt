#! /bin/sh

#C 2002-2008 Carlo Contavalli 
#M <ccontavalli at inscatolati.net>
#W mod-xslt
#D Lists all unresolved symbols in an object
#. file. It is mainly useful to verify all 
#. needed functions of a given library/module 
#. have been written, and that linking was all 
#. succesful
#A file - Name of the obj file to verify 
#L objdump(1)
#R objdump, test, tr, grep, echo
#P tr -s
#N Works only on GNU Systems using GLIBC

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

test -z "$1" && { 
  echo 1>&2 "You must supply the name of the library or .so to check!"
  exit 1;
}

test ! -f "$1" && {
  echo 1>&2 "File does not exist: $1"
  exit 1;
}

if test `which objdump` = ''; then
  echo 1>&2 "Could not find 'objdump' in path"
  exit 1
fi

objdump -t "$1" |grep UND | grep -v GLIBC | tr -s ' ' | tr '\t' ' ' |grep ' 00000000'
