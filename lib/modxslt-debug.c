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


#include "modxslt0/modxslt.h"
#include <ctype.h>

#ifndef MXSLT_DBGLEVEL_ENV
# define MXSLT_DBGLEVEL_ENV "MXSLT_DBGLEVEL"
#endif

#ifndef MXSLT_DBGLEVEL_DEFAULT
# define MXSLT_DBGLEVEL_DEFAULT 0
#endif

int mxslt_debug_firstlevel(void) {
  char * var;
  int retval=MXSLT_DBGLEVEL_DEFAULT;

  var=getenv(MXSLT_DBGLEVEL_ENV);
  if(!var || !*var)
    return retval;

  retval=atoi(var);
  return retval;
}

# define MXSLT_DBG_STR_LONGEST mxslt_sizeof_str("variables")
# define MXSLT_DBG_STR_SHORTEST mxslt_sizeof_str("sapi")

struct mxslt_debug_string_t {
  char * name;
  int nsize;
  int bit;
} mxslt_debug_strings[] = { 
  { "config", mxslt_sizeof_str("config"), MXSLT_DBG_CONFIG  },
  { "debug", mxslt_sizeof_str("debug"), MXSLT_DBG_DEBUG },
  { "flags", mxslt_sizeof_str("flags"), MXSLT_DBG_FLAGS },
  { "libxml", mxslt_sizeof_str("libxml"), MXSLT_DBG_LIBXML },
  { "parser", mxslt_sizeof_str("parser"), MXSLT_DBG_PARSER },
  { "proto", mxslt_sizeof_str("proto"), MXSLT_DBG_PROTO },
  { "rules", mxslt_sizeof_str("rules"), MXSLT_DBG_RULES },
  { "sapi", mxslt_sizeof_str("sapi"), MXSLT_DBG_SAPI },
  { "variables", mxslt_sizeof_str("variables"), MXSLT_DBG_VARIABLES },
  { "verbose0", mxslt_sizeof_str("verbose0"), MXSLT_DBG_VERBOSE0 },
  { "verbose1", mxslt_sizeof_str("verbose1"), MXSLT_DBG_VERBOSE1 },
  { "verbose2", mxslt_sizeof_str("verbose2"), MXSLT_DBG_VERBOSE2 }
};


int mxslt_debug_parse(char * str) {
  int flags=0;
  char * cursor;

    /* return immediately if we have no string */
  if(!str || !*str)
    return MXSLT_DBG_ERROR;

  while(!(flags & MXSLT_DBG_ERROR)) {
      /* Skip whites and commas... */
    for(; isspace((unsigned char)*str) || *str == ','; str++)
      ;
      
      /* Go to the end of the option */
    for(cursor=str; *cursor && *cursor != ',' && !isspace((unsigned char)*cursor); cursor++)
      ;

      /* If we couldn't find any option, return now */
    if(!(cursor-str))
      break;

    flags|=mxslt_debug_flag(str, cursor-str);
    str=cursor;
  }

  return flags;
}

int mxslt_debug_flag(const char * str, int len) {
  register int min=0, max=mxslt_sizeof_array(mxslt_debug_strings)-1, i;
  int diff;

    /* Make sure null strings do not cause troubles */
  if(!str || !*str || len > MXSLT_DBG_STR_LONGEST || len < MXSLT_DBG_STR_SHORTEST)
    return 0;

    /* Simple binary search in flag list */
  while(max >= min) {
    i=(max+min)>>1;

      /* We add 1 to compare the '\0', in order to avoid matching substrings */
    diff=memcmp(str, mxslt_debug_strings[i].name, 
	(mxslt_debug_strings[i].nsize > len ? len : mxslt_debug_strings[i].nsize));
    if(!diff)
      return mxslt_debug_strings[i].bit;

    if(diff < 0)
      max=i-1;
    else
      min=i+1;
  }

  return MXSLT_DBG_ERROR;
}
