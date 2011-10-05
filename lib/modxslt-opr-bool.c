/*
 *  mod-xslt -- Copyright (C) 2002-2008 
 *   		 Carlo Contavalli 
 *   		 <ccontavalli at inscatolati.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include "modxslt1/modxslt.h"

static int mxslt_opr_bool_and(mxslt_doc_t * doc, int val1, int val2) {
  return val1 && val2;
}

static int mxslt_opr_bool_or(mxslt_doc_t * doc, int val1, int val2) {
  return val1 || val2;
}

const struct mxslt_opr_t * mxslt_opr_bool_lookup (char * str) {
  static const struct mxslt_opr_t mxslt_bool_opr[] = {
      { "and", (mxslt_opr_call_f)mxslt_opr_bool_and },
      { "or", (mxslt_opr_call_f)mxslt_opr_bool_or }
  };

    /* Check is weakened (we don't use a full string compare)
     * since validation was already performed by flex */
  switch(*str) {
    case 'a':
      return &mxslt_bool_opr[0];

    case 'o':
      return &mxslt_bool_opr[1];
  }

  return NULL;
}
