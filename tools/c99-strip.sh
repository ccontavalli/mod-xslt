#! /bin/sh

#C 2002-2008 Carlo Contavalli
#M <ccontavalli at inscatolati.net>
#W mod-xslt
#D Removes any C99 construct from mod-xslt code
#A 'yes' - in order to avoid accidental use of 
#. this script by developers
#R sed, test, find, xargs, dirname, echo
#L mod-xslt README
#N Works by evaluating a list of regular expressions,
#. which is not a very reliable method to make sure
#. all C99 constructs have been removed

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

  # Add here regular expressions that will be evaluated by the script
reg_exp01='s/MXSLT_DEBUG("\(.*"\)\([,)].*\)/fprintf(stderr, "%s \1, __FUNCTION__\2/'
reg_exp02='s/MXSLT_DEBUG(\.\.\.)/MXSLT_DEBUG/'
reg_exp03='s/MXSLT_DEBUG(\([^,)]*\))/fprintf(stderr, "%s" \1, __FUNCTION__)/'
reg_exp04='s/MXSLT_DEBUG(str, \.\.\.).*/MXSLT_DEBUG/'
reg_exp05='s/mxslt_error(\([^,]*\), \(.*\));/((\1)->errhdlr((\1)->errctx, \2));/'
reg_exp06='s/mxslt_expr_yy_error(msg) mxslt_error(mxslt_yy_document, msg)/mxslt_expr_yy_error(msg) ((mxslt_yy_document)->errhdlr((mxslt_yy_document)->errctx, msg))/'
reg_exp07='s/mxslt_error(doc, \.\.\.) ((doc)->errhdlr((doc)->errctx, __VA_ARGS__))/mxslt_error/'

  # Go to the right directory
dir=`dirname $0`
cwd=`cd $dir && cd .. && pwd`
cd "$cwd"

if test ! -d ./lib || test ! -d ./utils; then
  echo 1>&2 "Error: couldn't find ./lib nor ./utils"
  exit 1
fi


if test "x$1" != "xyes"; then
  echo 2>&1 "If you really want to strip away C99 macros"
  echo 2>&1 "type '$0 yes'"
  echo 2>&1 "(this is not dangerous for you, but it is for me)"
  exit 1
fi

value=''
for var in `set | grep 'reg_' |cut -d'=' -f1`; do
  value="$value -e '`eval echo \\$$var`' "
done

find ./lib ./sapi ./utils -name '*.c' -o -name '*.h' \
	-o -name '*.lex' -o -name '*.y'|xargs -i -- echo sed -i "$value" {} |/bin/sh
