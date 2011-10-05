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


#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "modxslt1/modxslt.h"

#ifdef HAVE_SYSLOG_H
# include <syslog.h>
#endif

void xraise(char * str, ...) {
  va_list list;

  va_start(list, str);
    /* Ok, try to be as loud as possible */
  vfprintf(stderr, str, list);
  va_end(list);

#ifdef HAVE_SYSLOG_H
  openlog(MXSLT_STD_HANDLER, LOG_NDELAY | LOG_CONS | LOG_PID, LOG_DAEMON);
  vsyslog(LOG_ERR, str, list);
  closelog();
#endif

  abort();
}

#ifdef MXSLT_DEBUG_MEMORY

# define MXSLT_XMEM_MAGIC 0xCACCACCA
# define MXSLT_XMEM_SCREWED 0xABCDDBCA 

  /* I consider allocating 0 bytes an error */
void * xmalloc(size_t size) {
  void * retval = NULL;

  if(size)
    retval=malloc(size+sizeof(int));

  if(retval) {
    *(int *)retval=MXSLT_XMEM_MAGIC;
    return (void *)((char *)retval+sizeof(int));
  }

  xraise("malloc call error - size: %ld, pointer: %08x\n", size, retval);

  return NULL;
}

  /* I consider reallocating a null ptr an error 
   * I consider resizing to 0 an error */
void * xrealloc(const void * ptr, size_t size) {
  const void * orig = ptr;
  void * retval = NULL;

  if(ptr != NULL) { 
    ptr=(void *)((char *)ptr-sizeof(int));
    if(*(int *)ptr != MXSLT_XMEM_MAGIC) {
      xraise("realloc - pointer %08x does not start with magic value\n", orig);
    }

    if(size) {
        /* Force memory to be allocated somewhere else */
      retval=malloc(size+sizeof(int));
      memcpy(retval, (void *)ptr, size+sizeof(int));
    }

    if(retval) {
      xfree((void *)((char *)ptr+sizeof(int)));

      return (void *)((char *)retval+sizeof(int));
    }
  }

  xraise("realloc call error - size: %ld, pointer: %08x\n", size, ptr);
  return NULL;
}

void xfree(void * ptr) {
  if(ptr == NULL) 
    xraise("free - called with NULL pointer\n");

  ptr=(void *)((char *)ptr-sizeof(int));

  if(*(int *)ptr != MXSLT_XMEM_MAGIC) 
    xraise("free - pointer %08x does not start with magic value\n", ptr);

    /* Screw up magic value */
  *(int *)ptr=MXSLT_XMEM_SCREWED;

  free(ptr);
}

#else
  /* I consider allocating 0 bytes an error */
void * xmalloc(size_t size) {
  void * retval = NULL;

  if(size)
    retval=malloc(size);

  if(retval)
    return retval;

  xraise("malloc call error - size: %ld, pointer: %08x\n", size, retval);

    /* never reached */
  return 0;
}

  /* I consider reallocating a null ptr an error 
   * I consider resizing to 0 an error */
void * xrealloc(const void * ptr, size_t size) {
  void * retval = NULL;

  if(ptr != NULL && size) {
    retval=realloc((void *)ptr, size);

    if(retval)
      return retval;
  }

  xraise("realloc call error - size: %ld, pointer: %08x\n", size, ptr);
    /* never reached */
  return 0;
}

#endif

  /* I consider a NULL string an error */
char * xstrdup(const char * str) {
  char * retval = NULL;

  if(str != NULL) {
    retval=(char *)(xmalloc(strlen(str)+1));
    strcpy(retval, str);

    if(retval)
      return retval;
  }

  xraise("strdup - called with null string\n");

    /* never reached */
  return 0;
}

void * xstrndup(const char * str, size_t size) {
  char * retval = NULL;
  
  if(str != NULL) {
    if(size) {
      retval=(char *)(xmalloc(size+1));
      memcpy(retval, str, size);
      retval[size]='\0';
    } else {
      retval=xstrdup("");
    }
  }

  return retval;
}
