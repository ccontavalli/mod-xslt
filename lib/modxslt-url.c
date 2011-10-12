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

int mxslt_doc_url_decode(char * url) {
  char * store;

    /* Maps an ascii character to its decimal value */
  static const char decode[] = {
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1, 
-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

  for(store=url; *url != '\0'; url++, store++) {
    switch(*url) {
      case '+':
	*store=' ';
	break;

      case '%':
	if(*(url+1) == '\0' || *(url+2) == '\0' || 
	   decode[(unsigned short int)*(url+1)] < 0 || decode[(unsigned short int)*(url+2)] < 0) {
	  *store='\0'; /* XXX was 0 */
	  return MXSLT_FAILURE;
	}

	(*store)=(decode[(unsigned short int)*(url+1)])<<4;
	(*store)+=decode[(unsigned short int)*(url+2)];
	  /* Do not allow to add a \0 */
	if(*store == '\0')
	  return MXSLT_FAILURE;
	url+=2;
	break;

      default:
	*store=*url;
	break;
    }
  }

  *store='\0';

  return MXSLT_OK;
}

int mxslt_doc_param_urlparse(mxslt_doc_t * document, const char * query) {
  int status;
  char * key, * value;
  const char * ch=query, * start;

  mxslt_doc_debug_print(document, MXSLT_DBG_DEBUG | MXSLT_DBG_PROTO, "decoding URL arguments: %s\n", query);

  if(!query)
    return MXSLT_OK;

  while(1) {
      /* Uhm... what happens if url is like
       *   value=param&&&value2=param2?
       * and if it is
       *   value=param&=param2&value3=param3?
       */
    while(1) {
      for(start=ch; *ch && *ch != '=' && *ch != '&'; ch++)
        ;

      if(start != ch || (*ch != '&' && *ch != '=')) 
	break;

      ch++;
    }

      /* If we haven't found any key, return OK */
    if(!*ch && ch == start)
      return MXSLT_OK;

      /* In any other case, decode key and
       * format it correctly -- XXX: are we sure we 
       * should decode key? Doesn't this lead to sometimes
       * weird results? */ 
    key=(char *)xmalloc(sizeof("GET[]")+ch-start);
    sprintf(key, "GET[%.*s]", ch-start, start); 
    status=mxslt_doc_url_decode(key+4);
    if(status != MXSLT_OK) {
      xfree(key);
      return status;
    }

      /* Skip any = we may have found */
    if(*ch == '=')
      ch++;

      /* Look for the end of the value */
    for(start=ch; *ch && *ch != '&'; ch++)
      ;
    
      /* If we found some value, store it, 
       * otherwise remember we found the key anyway */
    if(start == ch) {
      mxslt_doc_param_add(document, key, NULL);
      mxslt_doc_debug_print(document, MXSLT_DBG_DEBUG | MXSLT_DBG_VARIABLES, "url parser - setting as seen: %s\n", key);
    } else {
      value=xstrndup(start, ch-start);
      status=mxslt_doc_url_decode(value);
      if(status != MXSLT_OK) {
        xfree(value);
	xfree(key);
        return status;
      }
      mxslt_doc_param_add(document, key, value);
      mxslt_doc_debug_print(document, MXSLT_DBG_DEBUG | MXSLT_DBG_VARIABLES, "url parser - setting to value: %s = \"%s\"\n", key, value);
    }

      /* If we found an '&', move on */
    /* 
    if(*ch == '&') 
      ch++; */
  }
}
