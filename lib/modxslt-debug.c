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
#include <ctype.h>
#include <assert.h>

#ifndef MXSLT_DBGLEVEL_ENV
# define MXSLT_DBGLEVEL_ENV "MXSLT_DBGLEVEL"
#endif

#ifndef MXSLT_DBGLEVEL_DEFAULT
# define MXSLT_DBGLEVEL_DEFAULT 0
#endif

unsigned int mxslt_debug_firstlevel(void) {
  unsigned int retval=MXSLT_DBGLEVEL_DEFAULT;
  char * var;

  var=getenv(MXSLT_DBGLEVEL_ENV);
  if(!var || !*var)
    return retval;

  retval=(unsigned int)atoi(var);
  return retval;
}

# define MXSLT_DBG_STR_LONGEST mxslt_sizeof_str("variables")
# define MXSLT_DBG_STR_SHORTEST mxslt_sizeof_str("all")

struct mxslt_debug_string_t {
  char * name;

  unsigned int nsize;
  unsigned int bit;
} mxslt_debug_strings[] = { 
    /* IMPORTANT: This list needs to be alphabetically sorted */
  { "all", mxslt_sizeof_str("all"), MXSLT_DBG_ALL },
  { "config", mxslt_sizeof_str("config"), MXSLT_DBG_CONFIG  },
  { "debug", mxslt_sizeof_str("debug"), MXSLT_DBG_DEBUG },
  { "flags", mxslt_sizeof_str("flags"), MXSLT_DBG_FLAGS },
  { "internal", mxslt_sizeof_str("internal"), MXSLT_DBG_INTERNAL },
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


unsigned int mxslt_debug_parse(char * str) {
  unsigned int flags=0;
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

    flags|=mxslt_debug_flag(str, (unsigned int)(cursor-str));
    str=cursor;
  }

  return flags;
}

unsigned int mxslt_debug_flag(const char * str, unsigned int len) {
  register int min=0, max=mxslt_sizeof_array(mxslt_debug_strings)-1, i;
  int diff;

    /* Make sure null strings do not cause troubles */
  if(!str || !*str) 
    return 0;

    /* Invalid strings should be discarded immediately */
  if (len > MXSLT_DBG_STR_LONGEST || len < MXSLT_DBG_STR_SHORTEST)
    return MXSLT_DBG_ERROR;

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

unsigned int mxslt_debug_enable(
    mxslt_shoot_t * shoot, unsigned int level,
    mxslt_debug_hdlr_f dbghdlr, void * dbgctx) {
  unsigned int old;

  assert(shoot);

  if (!shoot->mxslt_state)
    mxslt_shoot_init(shoot);

  old=shoot->mxslt_state->dbglevel;
  shoot->mxslt_state->dbglevel=level;
  if(dbghdlr) {
    shoot->mxslt_state->dbghdlr=(mxslt_debug_hdlr_f)dbghdlr;
    shoot->mxslt_state->dbgctx=dbgctx;
  }

  return old;
}

unsigned long int mxslt_get_tid(void) {
#ifdef HAVE_PTHREADS
  pthread_t self = pthread_self();
  return *(unsigned long int*)(&self);
#else
  return 0;
#endif
}
