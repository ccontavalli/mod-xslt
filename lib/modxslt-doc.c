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
#include "modxslt1/modxslt-internal.h"

#ifdef USE_EXSLT
# include <libexslt/exslt.h>
#endif

#ifndef MXSLT_PARAM_HINT_SIZE
# define MXSLT_PARAM_HINT_SIZE 53
#endif
#ifndef MXSLT_RECUR_HINT_SIZE
# define MXSLT_RECUR_HINT_SIZE 17
#endif

#include <libxml/xmlversion.h>

#ifdef LIBXML_XINCLUDE_ENABLED
# include <libxml/xinclude.h>

# define XINCLUDE_MAX_DEPTH 40
#endif

#include <assert.h>
#include <stddef.h>


  /* This table is filled at preinit time and contains all 
   * the `static' variables, like those holding compilation parameters */
static mxslt_table_t mxslt_global_param_table = mxslt_table_inith_static(MXSLT_PARAM_HINT_SIZE);
static volatile int mxslt_global_count = 0;

  /* Holds the global callbacks used to fetch http urls.
   * I really do hate using globals here, but couldn't find
   * a better way to interface with libxml */
#ifdef HAVE_PTHREADS
static pthread_key_t mxslt_global_state; 
#else
  /* Why don't I use a simple pointer? */
mxslt_state_t * mxslt_global_state = NULL;
#endif


  /* Used as a simple yaslt http handler function */
int mxslt_url_handle(mxslt_doc_t * doc, void ** fuffa, void * foo, const char * bar) { 
  return MXSLT_FALSE;
}

#ifdef HAVE_PTHREADS
mxslt_state_t * mxslt_get_state(void) {
  return (mxslt_state_t *)pthread_getspecific(mxslt_global_state);
}
#endif

#ifdef HAVE_PTHREADS
inline void mxslt_set_state(mxslt_state_t * next, mxslt_state_t ** prev) {
  mxslt_state_t * handler;

  handler=(mxslt_state_t *)pthread_getspecific(mxslt_global_state);

    /* Save previous handler */
  *prev=handler;

    /* Set new handler */
  pthread_setspecific(mxslt_global_state, next);

  return;
}
#else
inline void mxslt_set_state(mxslt_state_t * next, mxslt_state_t ** prev) {
    /* If we have a prev pointer, save current state */
  if(prev)
    *prev=mxslt_global_state;

    /* If we have a next pointer, set current state to next */
  mxslt_global_state=next;

  return;
}
#endif

void mxslt_doc_param_free(const char * key, const void * value) {
  xfree((void *)key);
  if(value)
    xfree((void *)value);
}

  /* Add an header to hashing table */
void mxslt_doc_param_hdr_add(mxslt_doc_t * doc, char * uc_key, char * uc_value) {
  char * key, * value;
  size_t uc_key_size;
  
  uc_key_size=strlen(uc_key);
  key=(char *)xmalloc(sizeof("HEADER[]")+uc_key_size);
  memcpy(key, "HEADER[", mxslt_sizeof_str("HEADER["));
  memcpy(key+mxslt_sizeof_str("HEADER["), uc_key, uc_key_size); 
  *(key+mxslt_sizeof_str("HEADER[")+uc_key_size)=']';
  *(key+mxslt_sizeof_str("HEADER[")+uc_key_size+1)='\0';
  value=xstrdup(uc_value);

  mxslt_doc_param_add(doc, key, value);
 
  return;
}

mxslt_table_status_e mxslt_doc_param_get(mxslt_doc_t * doc, char * key, char ** value) {
  mxslt_table_status_e retval;

  if(!key)
    return MXSLT_TABLE_FAILURE;

    /* If key starts with 'M', do an additional lookup in 
     * the static global table */
  if(*key == 'M') {
    retval=mxslt_table_search(&mxslt_global_param_table, NULL, key, (void **)value);
    if(retval == MXSLT_TABLE_FOUND)
      return retval;
  }

  retval=mxslt_table_search(&((doc)->table), NULL, key, (void **)value);

  return retval;
}

mxslt_table_status_e mxslt_doc_param_add(mxslt_doc_t * doc, char * key, char * value) {
  mxslt_table_record_t * record;
  mxslt_table_status_e retval;
 
    /* Check if the record is already in */
  retval=mxslt_table_search(&(doc->table), &record, key, NULL);
  if(retval == MXSLT_TABLE_FOUND) {
    if(record->data)
      xfree(record->data);
    if(value)
      record->data=value;
    else
      record->data=NULL;

    return retval;
  }

  return mxslt_table_insert(&(doc->table), NULL, key, value);
}

#define SKIP_BLANKS for(; MXSLT_IS_BLANK(*cur); cur++)
int mxslt_get_static_attr(const char * content, const mxslt_attr_search_t * attr, char ** store, int nelems) {
  char * cur, * start;
  register int min, max, i=0;
  int diff=0;
  size_t len;

  for(cur=(char *)content, nelems-=1; *cur; cur++) {
      /* Skip blanks we may have before start of attribute */
    SKIP_BLANKS;

    if(!*cur) {
      /* We got to the end of buffer in the right place - no more attributes to
       * scan or no attributes at all */
      return MXSLT_OK;
    }

      /* Go to end of attribute name (terminated either by a space or by the
       * equal sign - or by the end of buffer, of course) */
    for(start=cur; *cur && !MXSLT_IS_BLANK(*cur) && *cur != '='; cur++)
      ;

      /* Binary search the attribute in the array */
    for(min=0, max=nelems, len=(size_t)(cur-start); max >= min;) {
      i=(max+min)>>1;

        /* get the middle set record */
      diff=memcmp(start, attr[i].name, attr[i].size > len ? len : attr[i].size );
      if(!diff && attr[i].size == len) 
	break;

      if(diff < 0)
        max=i-1;
      else
	min=i+1;
    }

      /* Skip blanks we may have after attribute name but before = sign */
    SKIP_BLANKS;

    if(*cur++ != '=') {
	/* Well, we were looking for the equal sign but no equal... not good. */
      return MXSLT_FAILURE;
    }
    
      /* Skip blanks between = and start of attribute value */
    SKIP_BLANKS;

      /* We couldn't find the correct terminator */
    if(*cur != '\'' && *cur != '\"')
      return MXSLT_FAILURE;

      /* Go to end of attribute value */
    for(start=cur++; *cur && *cur != *start; cur++)
      ;

    if(!*cur) {
      /* We got to the end of buffer while we were looking for the closing ' or
       * " of an attribute value */
      return MXSLT_FAILURE;
    }

      /* We have an attribute and it's value if we are interested in them, just
       * store them */
    if(!diff && attr[i].size == len) {
      if(store[i])
	xfree(store[i]);

      store[i]=xstrndup(start+1, (size_t)(cur-start)-1);
    }

    /* cur++ in the for statement makes cur point to the character after the
     * closing ' or " */
  }

    /* If we got here, there were no trailing spaces after end of an attribute
     * value */
  return MXSLT_OK;
}

  /* XXX: Are variables like &amp; replaced by the library? */
# define HINT_GROWTH 31
int mxslt_set_static_attr(const char * content, const mxslt_attr_search_t * attr, char ** set, int nelems, char ** toret) {
  char * cur, * attrib, * value, * preb;
  register int min, max, i=0;
  int diff=0;
  char * retval, * store;
  size_t size, len, gain;

  *toret=NULL;
  gain=HINT_GROWTH;
  size=strlen(content)+HINT_GROWTH;
  store=retval=(char *)xmalloc(size+1);

  for(cur=(char *)content, nelems-=1; *cur; cur++, store++) {
      /* Skip blanks we may have before start of attribute */
    for(preb=cur; MXSLT_IS_BLANK(*cur); cur++, store++)
      *store=*cur;

    if(!*cur) {
      /* We got to the end of buffer in the right place - no more attributes to
       * scan or no attributes at all */
      *store='\0';
      *toret=retval;
      return MXSLT_OK;
    }

      /* Go to end of attribute name (terminated either by a space or by the
       * equal sign - or by the end of buffer, of course) */
    for(attrib=cur; *cur && !MXSLT_IS_BLANK(*cur) && *cur != '='; cur++, store++)
      *store=*cur;

      /* Binary search the attribute in the array */
    for(min=0, max=nelems, len=(size_t)(cur-attrib); max >= min;) {
      i=(max+min)>>1;

        /* get the middle set record */
      diff=memcmp(attrib, attr[i].name, attr[i].size > len ? len : attr[i].size);
      if(!diff && attr[i].size == len) 
	break;

      if(diff < 0)
        max=i-1;
      else
	min=i+1;
    }

      /* Skip blanks we may have after attribute name but before = sign */
    for(; MXSLT_IS_BLANK(*cur); cur++, store++)
      *store=*cur;

    if(*cur != '=') {
	/* Well, we were looking for the equal sign but no equal... not good. */
      xfree(retval);
      return MXSLT_FAILURE;
    }
    *store++=*cur++;
    
      /* Skip blanks between = and start of attribute value */
    for(; MXSLT_IS_BLANK(*cur); cur++, store++)
      *store=*cur;

      /* We couldn't find the correct terminator */
    if(*cur != '\'' && *cur != '\"') {
      xfree(retval);
      return MXSLT_FAILURE;
    }

      /* Go to end of attribute value */
    for(*store++=*cur, value=cur++; *cur && *cur != *value; cur++, store++)
      *store=*cur;

    if(!*cur) {
      /* We got to the end of buffer while we were looking for the closing ' or
       * " of an attribute value */
      xfree(retval);
      return MXSLT_FAILURE;
    }
    *store=*cur;

      /* We have an attribute and its value if we are interested in them, just
       * replace them */
    if(!diff && attr[i].size == len) {
      if(set[i]) {
	  /* Allocate memory for new value */
	len=strlen(set[i]);
	if(len > (size_t)(cur-value-1)+gain) {
	  size+=len-(size_t)(cur-value-1)+gain;
	  store=(char *)(store-retval);
	  retval=xrealloc(retval, size+1);
	  store=(char *)(retval+(ptrdiff_t)store);
	  gain=0;
        } else {
	  if(len > (size_t)(cur-value-1))
	    gain-=len-(size_t)(cur-value-1);
	}
	  /* Move store back to where we need to store the cursor */
	store=(store-(cur-value));
	*store++='"';
	memcpy(store, set[i], len);
	store+=len;
	*store='"';
      } else {
	  /* Ok, no attribute value, get rid of it */
	gain+=(size_t)(cur-preb+1);
	store=(store-(cur-preb+1));
      }
    }

    /* cur++ in the for statement makes cur point to the character after the
     * closing ' or " */
  }

    /* If we got here, there were no trailing spaces after end of an attribute
     * value */
  *store='\0';
  *toret=retval;
  return MXSLT_OK;
}

#ifdef MXSLT_LIBXSLT_HACK
void mxslt_doc_error_null(void * f, const char * fu, ...) {
  return;
}
#endif

void mxslt_shoot_init(mxslt_shoot_t * shoot) {
  mxslt_state_t state = MXSLT_STATE_INIT();

  memset(shoot, 0, sizeof(mxslt_shoot_t));
  shoot->mxslt_state=xmalloc(sizeof(mxslt_state_t));
  (*shoot->mxslt_state)=state;
}

void mxslt_recursion_init(mxslt_recursion_t * rec) {
  rec->rec_level=0;
  rec->rec_lastrecord=0;

  mxslt_table_init(&(rec->rec_table));
}

void mxslt_doc_null(void) {
}

unsigned int mxslt_doc_debug_enable(
    mxslt_doc_t * document, unsigned int level, mxslt_debug_hdlr_f dbghdlr,
    void * dbgctx) {
  unsigned int old;

  assert(document);

  old=document->dbglevel;
  document->dbglevel=level;
  if(dbghdlr) {
    document->dbghdlr=(mxslt_debug_hdlr_f)dbghdlr;
    document->dbgctx=dbgctx;
  }

  return old;
}

void mxslt_doc_init(
    mxslt_doc_t * document, char * sapit, mxslt_shoot_t * shoot,
    mxslt_recursion_t * recursion, mxslt_error_hdlr_f mxslt_doc_error, 
    void * errctx, void * ctx) {
  char * value;
  size_t len;

  /* mxslt_debug_mem(); */

    /* Zero up document */
  memset(document, 0, sizeof(mxslt_doc_t));

    /* Setup libxml state */
#ifdef HAVE_LIBXML_THREADS
    /* Initialize state */
  xmlSetGlobalState((xmlGlobalStatePtr)(shoot->xml_state), (xmlGlobalStatePtr *)&((document->shoot).xml_state));
#else
  (document->shoot).xml_state=NULL;
#endif

    /* Setup modxslt state */
  shoot->mxslt_state->document=document;
  shoot->mxslt_state->ctx=ctx;
  shoot->mxslt_state->recursion=recursion;
  mxslt_set_state(shoot->mxslt_state, &((document->shoot).mxslt_state));
  document->state=shoot->mxslt_state;

    /* Watch out! XXX: while for apache1 we should
     * be ok (as long as nobody else uses libxslt)
     * libxslt does not have a ``global state'' handling 
     * functions and there's thus a race condition in 
     * error context handling 
     * we should probably either: use a lock, or make
     * context a pointer to a tsd and use that instead... */

    /* Setup generic error handling */
  xmlSetGenericErrorFunc(errctx, mxslt_doc_error);
  xsltSetGenericErrorFunc(errctx, mxslt_doc_error);

    /* Work around xslt bug */
    /** Sometimes, xslt library calls xsltGenericError
     ** passing xsltGenericDebugContext as context instead
     ** of the correct xsltGenericErrorContext */
#ifdef MXSLT_LIBXSLT_DEBUG
  xsltSetGenericDebugFunc(errctx, mxslt_doc_error);
#else
# ifdef MXSLT_LIBXSLT_HACK 
  xsltSetGenericDebugFunc(errctx, mxslt_doc_error_null);
# endif
#endif

  document->errhdlr=mxslt_doc_error;
  document->errctx=errctx;
  document->parser=sapit;

  document->dbghdlr=(mxslt_debug_hdlr_f)mxslt_doc_null;
  document->dbgctx=NULL;

    /* Initialize tables */
  mxslt_table_init(&document->table);

    /* Strip `s from sapi name */
  len=strlen(sapit);
  value=(char *)xmalloc(len-1);
  memcpy(value, sapit+1, len-2); 
  value[len-2]='\0';

    /* Insert it into table */
  mxslt_table_insert(&document->table, NULL, xstrdup("MODXSLT[sapi]"), value);

  return;
}

int mxslt_doc_load(mxslt_doc_t * document, xmlParserInputBufferPtr buf, char * filename, 
		   char * hostname, int port, char * path) {
  if(!document)
    return MXSLT_FAILURE;

  mxslt_doc_debug_print(document, MXSLT_DBG_LIBXML | MXSLT_DBG_VERBOSE0, 
	"parsing document: %s aka http://%s:%d%s\n", mxslt_debug_string(filename), 
	mxslt_debug_string(hostname), port ? port : 80, mxslt_debug_string(path));

    /* URL of the current document */
  if(hostname) {
    if(port != 80 && port > 0 && port <= 65535) {
        /* 10 is the maximum length of a 32 bits int (althought this may be at most 6 characters, I'm trying
	 * to stay on the safe side ... */
      document->localurl=(char *)(xmalloc(sizeof("http://:")+strlen(hostname)+10+strlen(path)));
      sprintf(document->localurl, "http://%s:%d%s", hostname, port, path);
    } else {
      document->localurl=(char *)(xmalloc(sizeof("http://")+strlen(hostname)+strlen(path)));
      sprintf(document->localurl, "http://%s%s", hostname, path);
    }
  } else {
    document->localurl=xstrdup(filename);
  }

  document->localfile=xstrdup(filename);

  mxslt_doc_param_add(document, xstrdup("MODXSLT[fileurl]"), document->localfile);
  mxslt_doc_param_add(document, xstrdup("MODXSLT[httpurl]"), document->localurl);

  document->file=mxslt_doc_xml_parse(document, buf, document->localfile); 
  if(document->file == NULL)
    return MXSLT_FAILURE;

  return MXSLT_OK;
}

  /* XXX set state to standard value */
int mxslt_doc_done(mxslt_doc_t * document, mxslt_shoot_t * shoot) {
  mxslt_pi_style_t * style_pi;
  mxslt_pi_param_t * param_pi;
  mxslt_pi_base_t * base_pi;
  mxslt_pi_t * pi;

    /* Free document */
  if(document->file) {
    xmlFreeDoc(document->file);
    document->file=NULL;
  }

  if(document->stylesheet) {
    xsltFreeStylesheet(document->stylesheet);
    document->stylesheet=NULL;
  }

    /* Free up list of possible stylesheets to apply */
  for(pi=document->pi_first; pi != NULL;) {
    switch(pi->type) {
      case MXSLT_PI_PARAM:
	  /* Cast and move on to next pi */
	param_pi=(mxslt_pi_param_t *)pi;
	pi=pi->next;
	
	  /* Free up parameter */
        if(param_pi->param)
          xmlFree(param_pi->param);
        if(param_pi->value)
          xmlFree(param_pi->value);
 
        xfree(param_pi);
	break;

      case MXSLT_PI_UBASE:
	  /* Cast and move on to next pi */
	base_pi=(mxslt_pi_base_t *)pi;
	pi=pi->next;

	if(base_pi->file)
	  xmlFree(base_pi->file);
	if(base_pi->directory)
	  xmlFree(base_pi->directory);

	xfree(base_pi);
	break;

      case MXSLT_PI_STDST:
      case MXSLT_PI_MODST:
	  /* Cast and move to next pi */
	style_pi=(mxslt_pi_style_t *)pi;
	pi=pi->next;

	  /* Free up parameters */
        if(style_pi->href)
          xmlFree(style_pi->href);
        if(style_pi->ctype)
          xmlFree(style_pi->ctype);
        if(style_pi->media)
          xmlFree(style_pi->media);

        xfree(style_pi);
	break;
    }
  }

    /* Free table */
  mxslt_table_done(&(document->table));

  if(document->content_type)
    xfree(document->content_type);
    /* Free up other document parameters */

    /* Forget about this document */
  shoot->mxslt_state->document = NULL;

    /* Restore global state, discarding current state */
#ifdef HAVE_LIBXML_THREADS
  xmlSetGlobalState((xmlGlobalStatePtr)((document->shoot).xml_state), (xmlGlobalStatePtr *)(&(shoot->xml_state)));
#endif
  mxslt_set_state((document->shoot).mxslt_state, &(shoot->mxslt_state));
  /* mxslt_undebug_mem(); */

  return MXSLT_OK;
}

char * mxslt_doc_findmedia(xsltStylesheetPtr style) {
  xsltStylesheetPtr walk;
  char * type;

  for(walk=style->imports; walk; walk=walk->next) {
    if(walk->mediaType)
      return (char *)walk->mediaType;

    if(walk->imports) {
      type=mxslt_doc_findmedia(walk);
      if(type)
	return type;
    }
  }

  return NULL;
}

int mxslt_doc_parse(mxslt_doc_t * document, mxslt_header_set_f header_set, void * header_data) {
  char * type="text/xml", * rt, * encoding="UTF-8";
  size_t type_size=mxslt_sizeof_str("text/xml"), enco_size=mxslt_sizeof_str("UTF-8");

    /* Note: params is _not_ declared as a global static variable
     *   since it needs to be modified, depending on the sapit specified, and,
     *   as you know, modifying a global variable in a multithreaded
     *   application is BAD (without locking) - also note that in this function
     *   it is not declared static, since it needs be modifyed by the code -
     *   and static variables, when not const, are as bad as globals if
     *   modifyed */ 
  const char * params[] = {
    "modxslt-sapi", NULL,
    "modxslt-interface", APOS("2"),
    "modxslt-name", APOS(MXSLT_NAME),
    "modxslt-handler", APOS(MXSLT_STD_HANDLER),
    "modxslt-namespace", APOS(MXSLT_NS_URI),
#ifdef HAS_LIBPCRE
    "modxslt-conf-libpcre", APOS("true"),
#else
    "modxslt-conf-libpcre", APOS("false"),
#endif
#ifdef USE_EXSLT
    "modxslt-conf-exslt", APOS("true"),
#else
    "modxslt-conf-exslt", APOS("false"),
#endif
#ifdef MXSLT_DISABLE_EXTENSIONS
    "modxslt-conf-extensions", APOS("true"),
#else
    "modxslt-conf-extensions", APOS("false"),
#endif
#ifdef HAVE_LIBXML_THREADS
    "modxslt-conf-libxmlthreads", APOS("true"),
#else
    "modxslt-conf-libxmlthreads", APOS("false"),
#endif
#ifdef MXSLT_LIBXSLT_HACK
    "modxslt-conf-libxslthack", APOS("true"),
#else
    "modxslt-conf-libxslthack", APOS("false"),
#endif
#ifdef MXSLT_FALLBACK_WRAPAROUND
    "modxslt-conf-fallbackwrap", APOS("true"),
#else
    "modxslt-conf-fallbackwrap", APOS("false"),
#endif
#ifdef HAVE_PTHREADS
    "modxslt-conf-thread", APOS("pthread"),
#else
    "modxslt-conf-thread", APOS("none"),
#endif
#ifdef MXSLT_DISABLE_XINCLUDE
    "modxslt-conf-xinclude", APOS("false"),
#else
    "modxslt-conf-xinclude", APOS("true"),
#endif
    "modxslt-version", APOS(MXSLT_VERSION), 
    "modxslt-version-major", APOS(MXSLT_MAJOR),
    "modxslt-version-minor", APOS(MXSLT_MINOR),
    "modxslt-version-patchlevel", APOS(MXSLT_PATCHLEVEL),
    "modxslt-copyright", APOS("Copyright (C) Carlo Contavalli 2002-2008"),
    "modxslt-license", APOS("GPL - GNU General Public License"),
    NULL, NULL 
  };
  params[1]=document->parser;

    /* IFF we must be raw, there's nothing 
     * we have to do, beside setting up some 
     * headers */
  if(document->flags & MXSLT_BERAW) {
      /* We don't need the table anymore ... FREE */ 
    /* mxslt_table_done(&(document->table)); */

      /* Ok find out the encoding of the document */
    if(document->file->encoding) {
      document->encoder=xmlFindCharEncodingHandler((const char *)document->file->encoding);
      if(document->encoder->input && document->encoder->output) {
        encoding=(char *)document->file->encoding;
        enco_size=strlen(encoding);
      } else {
        xmlCharEncCloseFunc(document->encoder);
        document->encoder=NULL;
      }
    }

      /* prepare Content-Type */
    rt=(char *)xmalloc(type_size+enco_size+sizeof("; charset="));
    memcpy(rt, type, type_size);
    memcpy(rt+type_size, "; charset=", mxslt_sizeof_str("; charset="));
    memcpy(rt+type_size+mxslt_sizeof_str("; charset="), encoding, enco_size+1);

    header_set("Content-Type", rt, header_data);

      /* Remember the encoding used */
    document->encoding=(xmlChar *)encoding;
    document->content_type=(xmlChar *)rt;

    return MXSLT_OK;
  }
    
    /* Remember the functions we can
     * use to setup headers */
  document->header_set=header_set;
  document->header_data=header_data;

#ifndef MXSLT_DISABLE_XINCLUDE
# if LIBXML_VERSION >= 20603
  xmlXIncludeProcessFlags(document->file, MXSLT_XSLT_OPTIONS);
# else
  xmlXIncludeProcess(document->file);
# endif
#endif

    /* Try to apply stylesheet */
  document->parsed=mxslt_doc_xml_apply_stylesheet(document, document->stylesheet, document->file, (const char **)params);
  if((document->parsed) == NULL) {
    mxslt_error(document, "libxslt: couldn't apply stylesheet.\n"); 
    return MXSLT_FAILURE;
  }

    /* Select the best known encoding */
  document->encoder=NULL;
  if(document->stylesheet->encoding) {
    document->encoder=xmlFindCharEncodingHandler((const char *)document->stylesheet->encoding);
    if(document->encoder->input && document->encoder->output) {
      encoding=(char *)document->stylesheet->encoding;
      enco_size=strlen(encoding);
    } else {
      xmlCharEncCloseFunc(document->encoder);
      document->encoder=NULL;
    }
  } 

    /* Now, correctly setup headers */
  if(document->stylesheet->mediaType) {
    type=(char *)document->stylesheet->mediaType;
    type_size=strlen(type);
  } else {
      /* Try to find mediaType in imported stylesheets */
    rt=(document->stylesheet->imports ? mxslt_doc_findmedia(document->stylesheet) : NULL);
    if(rt) {
      type=rt;
      type_size=strlen(rt);
    } else {
      if(document->stylesheet->method) {
	switch(document->stylesheet->method[0]) {
	  case 'x':
	    if(strcmp((char *)document->stylesheet->method, "xml"))
	      break;
	    type=(char *)"text/xml";
	    type_size=mxslt_sizeof_str("text/xml");
	    break;

	  case 'h':
	    if(strcmp((char *)document->stylesheet->method, "html"))
	      break;
	    type=(char *)"text/html";
	    type_size=mxslt_sizeof_str("text/html");
	    break;

	  case 't':
	    if(strcmp((char *)document->stylesheet->method, "text"))
	      break;
	    type=(char *)"text/plain";
	    type_size=mxslt_sizeof_str("text/plain");
	    break;
	} 
      }
    }
  }

    /* prepare Content-Type */
  rt=(char *)xmalloc(type_size+enco_size+sizeof("; charset="));
  memcpy(rt, type, type_size);
  memcpy(rt+type_size, "; charset=", mxslt_sizeof_str("; charset="));
  memcpy(rt+type_size+mxslt_sizeof_str("; charset="), encoding, enco_size+1);

    /* Set headers */
  /* header_set(header_data, "Content-Encoding", encoding); */
  header_set("Content-Type", rt, header_data);

    /* Remember the encoding used */
  document->encoding=(xmlChar *)encoding;
  document->content_type=(xmlChar *)rt;

    /* XXX! Do we want to free up parameters now? FREE */
  /* mxslt_table_done(&(document->table));*/

  return MXSLT_OK;
}

int mxslt_doc_send(mxslt_doc_t * document, mxslt_callback_t * callback, void * ctx) {
  xmlOutputBufferPtr output;
  mxslt_pi_t * pi;
  char * media, * href, * type, * result;
  mxslt_pi_style_t * pi_style;
  static const mxslt_attr_search_t xml_stylesheet[] = {
    { (xmlChar *)"href", mxslt_sizeof_str("href") },
    { (xmlChar *)"media", mxslt_sizeof_str("media") },
    { (xmlChar *)"type", mxslt_sizeof_str("type") }
  };
  char * xml_stylesheet_values[] = { NULL, NULL, NULL };
  enum xml_stylesheet_types {
    t_href=0,
    t_media,
    t_type
  };

  output=xmlAllocOutputBuffer(document->encoder);
  if(!output) {
    if(document->parsed)
      xmlFreeDoc(document->parsed);

    if(document->stylesheet && !(document->flags & MXSLT_BERAW))
      xsltFreeStylesheet(document->stylesheet);

    xmlFreeDoc(document->file);
    mxslt_table_done(&(document->table));

    mxslt_error(document, "libxml: couldn't create output buffer.\n"); 
    return MXSLT_FAILURE;
  }

  output->written=0;
  output->context=ctx;
  output->writecallback=callback->writer;
  output->closecallback=callback->closer;

    /* Send output document */
  if(document->flags & MXSLT_BERAW) {
      /* Replace variables in known processing instructions */
    for(pi=document->pi_first; pi; pi=pi->next) {
      switch(pi->type) {
	case MXSLT_PI_STDST:
  	  pi_style=(mxslt_pi_style_t *)pi;

	    /* Parse strings */
	  if(pi_style->media) {
	    media=mxslt_yy_str_parse(document, pi_style->media, strlen(pi_style->media));
	    xml_stylesheet_values[t_media]=(char *)xmlEncodeEntitiesReentrant(document->file, (xmlChar *)media);
	    xfree(media);
	  }

	  if(pi_style->href) {
	    href=mxslt_yy_str_parse(document, pi_style->href, strlen(pi_style->href));
	      /* Get rid of any illegal character for an url */
	    xml_stylesheet_values[t_href]=(char *)xmlURIEscapeStr((xmlChar *)href, (xmlChar *)"@/:=?;#%&,+");
	    xfree(href);

	      /* Escape known and dangerous entities */
            href=(char *)xmlEncodeEntitiesReentrant(document->file, (xmlChar *)xml_stylesheet_values[t_href]);
	    xmlFree(xml_stylesheet_values[t_href]);

	      /* Adjust href if it starts with local:// */
	    if(!xmlStrncmp((xmlChar *)href, (xmlChar *)"local://", mxslt_sizeof_str("local://"))) {
	      xml_stylesheet_values[t_href]=(char *)xmlBuildURI((xmlChar *)(href+mxslt_sizeof_str("local://")), 
							        (xmlChar *)document->localurl);
	      xmlFree(href);
	    } else {
	      xml_stylesheet_values[t_href]=href;
	    }
	  }

	  if(pi_style->type) {
	    type=mxslt_yy_str_parse(document, pi_style->ctype, strlen(pi_style->ctype));
	    xml_stylesheet_values[t_type]=(char *)xmlEncodeEntitiesReentrant(document->file, (xmlChar *)type);
	    xfree(type);
	  }

	    /* Replace attributes */
	  mxslt_set_static_attr((char *)pi_style->node->content, xml_stylesheet, xml_stylesheet_values, 
				       mxslt_sizeof_array(xml_stylesheet), &result);

          mxslt_doc_debug_print(document, MXSLT_DBG_LIBXML | MXSLT_DBG_VERBOSE0, 
 	           "while returning raw document -- original: '%s', replacement: '%s'\n", 
		   mxslt_debug_string(pi_style->node->content), result);

	    /* This is necessary since different allocation
	     * strategies are (may be) used in mod-xslt and libxml */
	  xmlFree(pi_style->node->content);
	  pi_style->node->content=xmlStrdup((xmlChar *)result);
	  xfree(result);

	    /* Free up encoded entities */
	  if(xml_stylesheet_values[t_type]) {
	    xmlFree(xml_stylesheet_values[t_type]);
	    xml_stylesheet_values[t_type]=NULL;
	  }
	  if(xml_stylesheet_values[t_media]) {
	    xmlFree(xml_stylesheet_values[t_media]);
	    xml_stylesheet_values[t_media]=NULL;
	  }
	  if(xml_stylesheet_values[t_href]) {
	    xmlFree(xml_stylesheet_values[t_href]);
	    xml_stylesheet_values[t_href]=NULL;
	  }
	  break;

	case MXSLT_PI_MODST:
  	  pi_style=(mxslt_pi_style_t *)pi;

	    /* Parse strings */
	  if(pi_style->media)
	    xml_stylesheet_values[t_media]=(char *)xmlEncodeEntitiesReentrant(document->file, (xmlChar *)pi_style->media);

	  if(pi_style->href) {
	      /* Get rid of any illegal character for an url */
	    xml_stylesheet_values[t_href]=(char *)xmlURIEscapeStr((xmlChar *)pi_style->href, (xmlChar *)"@/:=?;#%&,+");
            href=(char *)xmlEncodeEntitiesReentrant(document->file, (xmlChar *)xml_stylesheet_values[t_href]);
	    xmlFree(xml_stylesheet_values[t_href]);

	      /* Adjust href if it starts with local:// */
	    if(!xmlStrncmp((xmlChar *)href, (xmlChar *)"local://", mxslt_sizeof_str("local://"))) {
	      xml_stylesheet_values[t_href]=(char *)xmlBuildURI((xmlChar *)(href+mxslt_sizeof_str("local://")), 
							        (xmlChar *)document->localurl);
	      xmlFree(href);
	    } else {
	      xml_stylesheet_values[t_href]=href;
	    }
	  }

	  if(pi_style->type)
	    xml_stylesheet_values[t_type]=(char *)xmlEncodeEntitiesReentrant(document->file, (xmlChar *)pi_style->ctype);

	    /* Replace attributes */
	  mxslt_set_static_attr((char *)pi_style->node->content, xml_stylesheet, xml_stylesheet_values, 
				       mxslt_sizeof_array(xml_stylesheet), &result);

          mxslt_doc_debug_print(document, MXSLT_DBG_LIBXML | MXSLT_DBG_VERBOSE0, 
 	           "while returning raw document -- original: '%s', replacement: '%s'\n", 
		   mxslt_debug_string(pi_style->node->content), result);

	    /* This is necessary since different allocation
	     * strategies are (may be) used in mod-xslt and libxml */
	  xmlFree(pi_style->node->content);
	  pi_style->node->content=xmlStrdup((xmlChar *)result);
	  xfree(result);

	    /* Free up encoded entities */
	  if(xml_stylesheet_values[t_type]) {
	    xmlFree(xml_stylesheet_values[t_type]);
	    xml_stylesheet_values[t_type]=NULL;
	  }
	  if(xml_stylesheet_values[t_media]) {
	    xmlFree(xml_stylesheet_values[t_media]);
	    xml_stylesheet_values[t_media]=NULL;
	  }
	  if(xml_stylesheet_values[t_href]) {
	    xmlFree(xml_stylesheet_values[t_href]);
	    xml_stylesheet_values[t_href]=NULL;
	  }
	  break;
      }
    }
    xmlSaveFormatFileTo(output, document->file, (char *)document->encoding, 1); 
  } else {
    if(!document->parsed) {
      xsltFreeStylesheet(document->stylesheet);
      xmlFreeDoc(document->file);
      mxslt_table_done(&(document->table));

      return MXSLT_FAILURE;
    }

    xsltSaveResultTo(output, document->parsed, document->stylesheet);
    xmlOutputBufferClose(output);
    xmlFreeDoc(document->parsed);

      /* XXX -> do we really want to put them here?? FREE */
    xsltFreeStylesheet(document->stylesheet);
  }

  xmlFreeDoc(document->file);
  mxslt_table_done(&(document->table));

  document->stylesheet=NULL;
  document->file=NULL;
  
  return MXSLT_OK;
}

int mxslt_doc_parse_stylesheet(mxslt_doc_t * document, char * media, int flags) {
  char * tmp;
  int status;

    /* type is ok, if we have no media
     * accept the stylesheet 
     *   www.w3.org/1999/06/REC-xml-stylesheet-19990629 */
  if(!media)
    return MXSLT_OK;

      /* Skip any blank */
  for(tmp=media; MXSLT_IS_BLANK(*tmp); tmp++)
    ;

  status=mxslt_doc_screen_check(document, &tmp, flags);
    /* If this media can be used, stop looking for a suitable media */
  if(status == MXSLT_TRUE)
    return MXSLT_OK;
  if(status == MXSLT_ERROR)
    return MXSLT_FAILURE; /* XXX: On parsing error, we probably should just skip
			      to the next comma and go on - maybe we should report
			      the problem */

    /* If we got here, we reached end 
     * of string without finding any 
     * suitable media */
  return MXSLT_SKIP;
}

xsltStylesheetPtr mxslt_doc_load_stylesheet_file(mxslt_doc_t * document, char * file) {
  xmlURIPtr URI;
  xmlChar *URL;
  const xmlChar *base;
  xsltStylesheetPtr ret = NULL;
  xmlAttrPtr ID;
  xmlDocPtr tmp;
  xmlNodePtr subtree;

  base=document->file->doc->URL;
  URI=xmlParseURI((const char *)file);
  if(URI == NULL) {
    mxslt_error(document, "xmlParseURI failed for %s\n", file);
    return(NULL);
  }

  mxslt_doc_debug_print(document, MXSLT_DBG_LIBXML, "loading stylesheet: %s\n", mxslt_debug_string(file));
  mxslt_doc_debug_print(document, MXSLT_DBG_LIBXML | MXSLT_DBG_DEBUG, 
		    	"stylesheet - base: %s, name (server): %s, path: %s\n", mxslt_debug_string((char *)base), 
			mxslt_debug_string((char *)URI->server), mxslt_debug_string((char *)URI->path));

  if((URI->fragment != NULL) && (URI->scheme == NULL) &&
     (URI->opaque == NULL) && (URI->authority == NULL) &&
     (URI->server == NULL) && (URI->user == NULL) &&
     (URI->path == NULL) && (URI->query == NULL)) {

      /* Handle stylesheet inserted directly in the document */
    if(URI->fragment[0] == '#')
      ID = xmlGetID(document->file, (const xmlChar *) &(URI->fragment[1]));
    else
      ID = xmlGetID(document->file, (const xmlChar *) URI->fragment);

      /* Return immediately if the id was not found */
    if(!ID) {
      xmlFreeURI(URI);
      return NULL;
    }

    subtree=ID->parent;
    tmp=xmlNewDoc(NULL);

      /* Dict where added at a certain point
       * in libxml... */
    tmp->dict=document->file->dict;

    if (tmp != NULL) {
      xmlUnlinkNode(subtree);
      xmlAddChild((xmlNodePtr)tmp, subtree);
    }
  } else {
    URL = xmlBuildURI((xmlChar *)file, base);
    if (URL != NULL) {
      mxslt_doc_debug_print(document, MXSLT_DBG_LIBXML | MXSLT_DBG_VERBOSE0, 
		"parsing stylesheet: %s\n", mxslt_debug_string((char *)URL));
      tmp=mxslt_doc_xml_load_entity(document, (char *)URL); 
      xmlFree(URL);
    } else {
      mxslt_doc_debug_print(document, MXSLT_DBG_LIBXML | MXSLT_DBG_VERBOSE0,
		"parsing stylesheet: %s\n", mxslt_debug_string((char *)file));
      tmp=mxslt_doc_xml_load_entity(document, (char *)file);
    }
  }

  xmlFreeURI(URI);

  if(tmp == NULL)
    return NULL;

  ret=xsltParseStylesheetDoc(tmp);

  if(ret == NULL)
    xmlFreeDoc(tmp);

  return ret;
}

int mxslt_doc_load_stylesheet(mxslt_doc_t * document, char * href) {
  mxslt_doc_debug_print(document, MXSLT_DBG_LIBXML | MXSLT_DBG_VERBOSE0,
		"adjusting url for stylesheet: %s\n", mxslt_debug_string((char *)href));

  document->stylesheet=mxslt_doc_load_stylesheet_file(document, href);

  if(document->stylesheet)
    return MXSLT_OK;

  return MXSLT_SKIP;
}

  /* Returns:
   *   MXSLT_OK on succesfull load of a stylesheet & succesfull interpretation of pi
   *   MXSLT_FAILURE in case of failure
   *   MXSLT_NONE if no stylesheet was found
   *   MXSLT_UNLOADABLE if no loadable stylesheet was found */
int mxslt_doc_parse_pi(mxslt_doc_t * document) {
  mxslt_pi_t * pi;
  mxslt_pi_style_t * pi_style;
  mxslt_pi_param_t * pi_param;
  mxslt_pi_base_t * pi_base;
  char * parsed_href;
  int status=MXSLT_NONE;

    /* We're parsing the stylesheet */
  document->flags |= MXSLT_STYLE;

    /* Check each pi in turn */
  for(pi=document->pi_first; pi != NULL; pi=pi->next) {
    switch(pi->type) {
      case MXSLT_PI_PARAM:
	pi_param=(mxslt_pi_param_t *)pi;

	  /* Add parameter to hashing table */
	if(pi_param->param)
          mxslt_doc_param_add(document, xstrdup(pi_param->param), 
			      pi_param->value ? xstrdup(pi_param->value) : pi_param->value);

	break;

      case MXSLT_PI_UBASE:
	pi_base=(mxslt_pi_base_t *)pi;

	if(document->file->URL)
	  xmlFree((char *)document->file->URL);
	document->file->URL=xmlStrdup((xmlChar *)pi_base->directory);
	break;

      case MXSLT_PI_MODST:
      case MXSLT_PI_STDST:
	  /* Cast style */
	pi_style=(mxslt_pi_style_t *)pi;

	  /* If we already found a stylesheet
	   * go on with parameters */
	if(status == MXSLT_OK || document->stylesheet)
	  continue;

	  /* If no media, the stylesheet is good */
        if(pi_style->media)
          status=mxslt_doc_parse_stylesheet(document, pi_style->media, 0);
        else
          status=MXSLT_OK;

        switch(status) {
          case MXSLT_OK:
            if(pi_style->href) {
	        /* Substitute variables in href */
    	      parsed_href=mxslt_yy_str_parse(document, pi_style->href, strlen(pi_style->href));

	        /* Try to load stylesheet */
	        /* XXX I'm not encoding the url here since it looks 
	         *     to me that libxml is already taking care of escaping urls */
  	      status=mxslt_doc_load_stylesheet(document, parsed_href);
              if(status != MXSLT_OK) {
                mxslt_error(document, "couldn't load: %s (%s), errno: %d\n", parsed_href, pi_style->href, errno);
		status=MXSLT_UNLOADABLE;
	      }
	      xfree(parsed_href);
            } else {
              document->flags|=MXSLT_BERAW;
            }
	    break;

	  case MXSLT_SKIP:
	    status=MXSLT_UNLOADABLE;
	    break;

          case MXSLT_FAILURE:
	    return MXSLT_FAILURE;
        }
	break;
    }
  }

  return status;
}

  /* Gosh... Do I have to specify an init and done? */
static void * mxslt_xslt_module_init(xsltTransformContextPtr ctxt, const xmlChar * URI) {
  return NULL;
}

static void mxslt_xslt_module_done(xsltTransformContextPtr ctxt, const xmlChar * URI, void * data) {
  return;
}


#ifdef HAVE_PTHREADS
static void mxslt_pthread_destroy(void * ptr) {
  if(!ptr)
    return;

  xfree(ptr);
}
#endif

  /* This piece of code is assumed to be executed only by the main thread - the
   * counter is not used to count the number of threads/users of the library,
   * but the number of modules using this library, the number of ``softwares''
   * that initialized the library from the main thread */
void mxslt_xml_unload(void) {
  if(--mxslt_global_count)
    return;

  mxslt_table_free(&mxslt_global_param_table, NULL);
#ifdef HAVE_PTHREADS
  pthread_key_delete(mxslt_global_state);
#endif
}

  /* Precalculate static tables and sets up globals.
   * This should be called before any other mxslt_ functions is invoked,
   * possibly from the main thread of execution. */
void mxslt_xml_load(void) {
  static const char * const params[] = {
    "MODXSLT[interface]", "2",
    "MODXSLT[name]", MXSLT_NAME,
    "MODXSLT[handler]", MXSLT_STD_HANDLER,
    "MODXSLT[namespace]", MXSLT_NS_URI,
#ifdef HAS_LIBPCRE
    "MODXSLT[conf-libpcre]", "true",
#else
    "MODXSLT[conf-libpcre]", "false",
#endif
#ifdef USE_EXSLT
    "MODXSLT[conf-exslt]", "true",
#else
    "MODXSLT[conf-exslt]", "false",
#endif
#ifdef MXSLT_DISABLE_EXTENSIONS
    "MODXSLT[conf-extensions]", "true",
#else
    "MODXSLT[conf-extensions]", "false",
#endif
#ifdef HAVE_LIBXML_THREADS
    "MODXSLT[conf-libxmlthreads]", "true",
#else
    "MODXSLT[conf-libxmlthreads]", "false",
#endif
#ifdef MXSLT_LIBXSLT_HACK
    "MODXSLT[conf-libxslthack]", "true",
#else
    "MODXSLT[conf-libxslthack]", "false",
#endif
#ifdef MXSLT_FALLBACK_WRAPAROUND
    "MODXSLT[conf-fallbackwrap]", "true",
#else
    "MODXSLT[conf-fallbackwrap]", "false",
#endif
#ifdef HAVE_PTHREADS
    "MODXSLT[conf-thread]", "pthread",
#else
    "MODXSLT[conf-thread]", "none",
#endif
#ifdef MXSLT_DISABLE_XINCLUDE
    "MODXSLT[conf-xinclude]", APOS("false"),
#else
    "MODXSLT[conf-xinclude]", APOS("true"),
#endif
    "MODXSLT[version]", MXSLT_VERSION, 
    "MODXSLT[version-major]", MXSLT_MAJOR,
    "MODXSLT[version-minor]", MXSLT_MINOR,
    "MODXSLT[version-patchlevel]", MXSLT_PATCHLEVEL,
    "MODXSLT[copyright]", "Copyright (C) Carlo Contavalli 2002-2008",
    "MODXSLT[license]", "GPL - GNU General Public License",
    NULL, NULL 
  };

  char ** ptr = (char **)params;

    /* Check if library was already initialized */
  if(mxslt_global_count++)
    return;

    /* Add all static fields */
  for(; *ptr != NULL; ptr+=2)
    mxslt_table_insert(&mxslt_global_param_table, NULL, *ptr, *(ptr+1));
  
    /* If we have pthreads, initialize
     * task specific data - I hate this :|
     * just could not find any better way with libxml */
#ifdef HAVE_PTHREADS
  pthread_key_create(&mxslt_global_state, &mxslt_pthread_destroy);
#endif

  return;
}

void mxslt_xml_init(
    mxslt_shoot_t * shoot, mxslt_url_handler_t* http_handler,
    mxslt_url_handler_t* file_handler) {
  if (!shoot->mxslt_state)
    mxslt_shoot_init(shoot);

#ifdef HAVE_LIBXML_THREADS
  xmlGlobalStatePtr prev_xml_state=NULL;

    /* Get current state */
  shoot->xml_state=xmlNewGlobalState();
  xmlSetGlobalState(shoot->xml_state, &prev_xml_state);

  mxslt_debug_print(
      shoot->mxslt_state, MXSLT_DBG_INTERNAL,
      "threading enabled in libxml - setup global state\n");
#endif /* HAVE_LIBXML_THREADS */

    /* Initialize xml parser */
  xmlInitParser();
#ifdef USE_EXSLT
  exsltRegisterAll();
  mxslt_debug_print(
      shoot->mxslt_state, MXSLT_DBG_INTERNAL,
      "initialized libxml - registered EXSLT extensions\n");
#else /* USE_EXSLT */
  mxslt_debug_print(
      shoot->mxslt_state, MXSLT_DBG_INTERNAL,
      "initialized libxml - *NO* EXSLT extensions\n");
#endif /* USE_EXSLT */

  xmlKeepBlanksDefault(0);
  xmlLineNumbersDefault(1);
  xmlSubstituteEntitiesDefault(1);
  xmlLoadExtDtdDefaultValue=MXSLT_XSLT_OPTIONS;

#ifndef MXSLT_DISABLE_XINCLUDE 
  xsltSetXIncludeDefault(1);
  mxslt_debug_print(
      shoot->mxslt_state, MXSLT_DBG_INTERNAL, "libxml - xinclude enabled\n");
#else /* MXSLT_DISABLE_XINCLUDE */
  mxslt_debug_print(
      shoot->mxslt_state, MXSLT_DBG_INTERNAL, "libxml - xinclude disabled\n");
#endif

#ifndef MXSLT_DISABLE_EXTENSIONS
    /* Register extension module */
  xsltRegisterExtModule(
      (xmlChar *)MXSLT_NS_URI, mxslt_xslt_module_init, mxslt_xslt_module_done);
  mxslt_debug_print(
      shoot->mxslt_state, MXSLT_DBG_INTERNAL,
      "libxml - registered yaslt extensions as "MXSLT_NS_URI" namespace.");
#else /* MXSLT_DISABLE_EXTENSIONS */
  mxslt_debug_print(
      shoot->mxslt_state, MXSLT_DBG_INTERNAL, "libxml - yaslt extensions not registered\n");
#endif /* MXSLT_DISABLE_EXTENSIONS */

    /* IF an handler is provided */
  if(http_handler)
    shoot->mxslt_state->http_handler=*http_handler;

  xmlRegisterInputCallbacks(mxslt_url_match, mxslt_url_open, mxslt_url_read,  mxslt_url_close);
  xmlRegisterInputCallbacks(mxslt_local_match, mxslt_local_open, mxslt_url_read,  mxslt_url_close);

#ifdef HAVE_LIBXML_THREADS
  xmlSetGlobalState(prev_xml_state, (xmlGlobalStatePtr *)&(shoot->xml_state));
#endif /* HAVE_LIBXML_THREADS */

  mxslt_debug_print(shoot->mxslt_state, MXSLT_DBG_INTERNAL|MXSLT_DBG_VERBOSE1,
		    "initialization completed\n");
  return;
}

void mxslt_xml_done(mxslt_shoot_t * shoot) { 
  mxslt_state_t * state;

#ifdef HAVE_LIBXML_THREADS
  xmlGlobalStatePtr new_state=(xmlGlobalStatePtr)(shoot->xml_state);
  xmlGlobalStatePtr prev_state;
  
  /* prev_state=xmlGetGlobalState(); */
  xmlSetGlobalState(new_state, &prev_state);
#endif

    /* set global state to NULL & Free it up */
  mxslt_set_state(NULL, &state);
  if(state)
    xfree(state);

    /* Cleanup global parser */
  xmlCleanupParser();
  xmlMemoryDump();
  
#ifdef HAVE_LIBXML_THREADS
  xmlSetGlobalState(prev_state, &new_state);
  xmlFreeGlobalState(new_state);
#endif

  return;
}

#ifdef MXSLT_LIBXML_HTTP_STATUS
  /* Ok, http functions of libxml reads the document
   * even when a 400 or 500 status is returned from
   * the server. This is not what I want */
void * mxslt_url_real_open(mxslt_doc_t * doc, char * uri) {
  xmlNanoHTTPCtxt * retval;

    /* Open up http connection */
  retval=(xmlNanoHTTPCtxt *)xmlNanoHTTPMethodRedir(uri, NULL, NULL, NULL, NULL, NULL, 0);

    /* Check web server return status */
  if(retval->returnValue != 200) {
    mxslt_error(doc, "skipping document: %s - web server status: %d\n", uri, retval->returnValue);
    xmlNanoHTTPClose(retval);

    return NULL;
  }

  return retval;
}
#endif

  /* Check if an url uses the local scheme */
int mxslt_local_match(const char * filename) {
  if(!xmlStrncasecmp((xmlChar *)filename, (xmlChar *)"local://", mxslt_sizeof_str("local://")))
    return 1;
  return 0;
}

  /* open a local:// url */
void * mxslt_local_open(const char * uri) {
  mxslt_state_t * state=mxslt_get_state();
  mxslt_url_t * retval;
  char * uri_new;
  char * uri_mine=NULL;
  char * uri_try=NULL;
  
  mxslt_doc_t * doc;
  mxslt_recursion_t * recursion;
  void * context;

    /* Set up pointers */
  doc=state->document;
  recursion=state->recursion;
  context=state->ctx;

    /* Open up URL */
  retval=xmalloc(sizeof(mxslt_url_t)); 

    /* Check we didn't reach maximum recursion level */
  if(mxslt_url_recurse_allowed(recursion, uri) != MXSLT_OK) {
    mxslt_error(doc, "maximum recursion level reached or uri already walked for: %s\n", uri);
    retval->handled=MXSLT_FAILURE;

    return retval;
  }

    /* Check if we have an http handler */
  if((state->http_handler).open) {
    uri_mine=uri_new=(char *)xmlBuildURI((xmlChar *)uri+mxslt_sizeof_str("local://"), (xmlChar *)doc->localurl);
    retval->handled=mxslt_sapi_http_open(doc, NULL, context, (const char **)&uri_new, &(retval->data));

    switch(retval->handled) {
      case MXSLT_OK:
	  /* If handler was really succesful */
	if(retval->data)
	  return retval;

	  /* Otherwise, let libxml2 try with other handlers */
	mxslt_error(doc, "failed opening url: %s (%s) - falling back", uri_new, uri);
	/* No break here, it's a real failure - mark it as such */
	retval->handled=MXSLT_FAILURE;

      case MXSLT_FAILURE:
	  /* Here, I don't think any other libxml2 Input handler
	   * will be catching local:// urls */
	xmlFree(uri_mine);
	return retval;

      case MXSLT_DECLINE:
	uri_try=uri_new;
	break;
	
      case MXSLT_SKIP:
	break;
    }
  } 

    /* If uri_try was set by someone, parse it. 
     * Otherwise, build an uri starting from base */
  if(uri_try == NULL) {
    uri_try=(char *)xmlBuildURI((xmlChar *)uri+mxslt_sizeof_str("local://"), (xmlChar *)doc->localurl);
    retval->data=mxslt_url_real_open(doc, (char *)uri_try);
    xmlFree(uri_try);
  } else {
    retval->data=mxslt_url_real_open(doc, (char *)uri_try);
  }

    /* free up local uri, if any */
  if(uri_mine)
    xmlFree(uri_mine);

    /* if no data, even the http_real_open failed. Otherwise, remember
     * that we skipped the sapi handler */
  if(retval->data)
    retval->handled=MXSLT_SKIP;
  else
    retval->handled=MXSLT_FAILURE;

  return retval;
}

  /* This is to avoid ugly libxml hacks */
int mxslt_url_match(const char * filename) {
  return xmlIOHTTPMatch(filename);
}

int mxslt_url_recurse_allowed(mxslt_recursion_t * recursion, const char * uri) {
  mxslt_table_record_t * cur_record;

    /* Check we didn't pass maximum recursion level */
  if(recursion->rec_level >= MXSLT_MAX_RECURSION_LEVEL)
    return MXSLT_MAX_LEVEL; 

    /* Check url was not already walked */
  if(mxslt_table_search(&(recursion->rec_table), &cur_record, uri, NULL) == MXSLT_TABLE_FOUND)
    return MXSLT_LOOP;

  return MXSLT_OK;
}

void mxslt_url_recurse_dump(mxslt_recursion_t * recursion, void (*outputter)(void *, char *, ...), void * data) {
  mxslt_table_record_t * cur_record=(recursion->rec_lastrecord);
  int i;

    /* dump stack! */
  outputter(data, "stack trace follows:");
  for(i=recursion->rec_level; cur_record != NULL; cur_record=(mxslt_table_record_t *)cur_record->data, i--)
    outputter(data, " * %02d - %s", i, cur_record->key);
}

void mxslt_url_recurse_push(mxslt_recursion_t * recurse, const char * uri) {
    /* Update loop avoiding variables */
  mxslt_table_insert(&(recurse->rec_table), &(recurse->rec_lastrecord), xstrdup(uri), (recurse->rec_lastrecord));
  (recurse->rec_level)++;
}

void mxslt_url_recurse_pop(mxslt_recursion_t * recurse, int n) {
  mxslt_table_record_t * cur_record;

    /* update loop avoiding variables */
  for(; n && recurse->rec_level; n--, (recurse->rec_level)--) {
    cur_record=(recurse->rec_lastrecord);
    (recurse->rec_lastrecord)=(mxslt_table_record_t *)(cur_record->data);
    xfree(cur_record->key);
    mxslt_table_record_remove(&(recurse->rec_table), cur_record, NULL);
  }
}

  /* XXX: we have _no way_ to tell libxml an error verifyed.
   * 	  eg, if we return NULL, the standard http handler is
   * 	  executed, doubling the time needed to return the error
   * 	  and performing an outgoing connection anyway.
   * 	  HTTP handlers in libxml rely on the fact that they will
   * 	  be the last executed, where a returned NULL won't lead
   *	  to other handlers being run. 
   *
   *	  We use a weird approach here: we tell libxml everything
   *	  is all right, and then fail the first read... it really
   *	  is a libxml upstream problem... */
void * mxslt_url_open(const char * uri) {
  mxslt_state_t * state=mxslt_get_state();
  mxslt_url_t * retval;
  mxslt_doc_t * doc;
  mxslt_recursion_t * recursion;
  char * uri_new=(char *)uri;
  void * context, * store = NULL;
  int status;

    /* Get needed globals */
  doc=state->document;
  recursion=state->recursion;
  context=state->ctx;

     /* Open up URL */
  retval=xmalloc(sizeof(mxslt_url_t));
    
    /* Check we didn't reach maximum recursion level */
  if(mxslt_url_recurse_allowed(recursion, uri) != MXSLT_OK) {
    mxslt_error(doc, "maximum recursion level reached or url already walked for: %s\n", uri); 
    retval->handled=MXSLT_FAILURE;

    return retval;
  }
 
    /* Check if we can handle the uri */
  status=mxslt_sapi_http_handle(doc, &store, context, uri);
  if(status == MXSLT_TRUE) {
    retval->handled=mxslt_sapi_http_open(doc, store, context, (const char **)&uri_new, &(retval->data));
    switch(retval->handled) {
      case MXSLT_OK:
	break;

      case MXSLT_FAILURE:
	return retval;

      case MXSLT_SKIP:
        retval->data=mxslt_url_real_open(doc, (char *)uri_new);
	break;
    }
  } else {
    retval->handled=MXSLT_SKIP;
    retval->data=mxslt_url_real_open(doc, (char *)uri);
  }

    /* Check if open was succesfull */
  if(!retval->data) {
    mxslt_error(doc, "failed opening url: %s (%s) - falling back\n", uri_new, uri);
    retval->handled=MXSLT_FAILURE;

    return retval;
  }

    /* Update variables to avoid loops */
  mxslt_url_recurse_push(recursion, uri_new);
  return (void *)retval;
}

int mxslt_url_read(void * ptr, char * buffer, int len) {
  mxslt_url_t * ctx=(mxslt_url_t *)ptr;
  mxslt_doc_t * doc=mxslt_get_state()->document;

  if(ctx == NULL) 
    return 0;

    /* XXX: read the comment about mxslt_url_open */
  if(ctx->handled == MXSLT_FAILURE) {
    errno=EIO;
    return -1;
  }

  if(ctx->handled == MXSLT_OK && doc->state->http_handler.read)
    return mxslt_sapi_http_read(doc, ctx->data, buffer, len);
  
  return xmlIOHTTPRead(ctx->data, (char *)buffer, len);
}

int mxslt_url_close(void * ptr) {
  mxslt_state_t * state=mxslt_get_state();
  mxslt_url_t * ctx=(mxslt_url_t *)ptr;
  mxslt_recursion_t * recursion;
  int retval=0;
  mxslt_doc_t * doc;
  
  if(!ptr)
    return 0;

    /* Get needed globals */
  doc=(state->document);
  recursion=state->recursion;

    /* Close the connection up */
  if(ctx->data) {
    switch(ctx->handled) {
      case MXSLT_OK: /* means the sapi handler was called */
        retval=mxslt_sapi_http_close(doc, ctx->data);
	break;

      case MXSLT_SKIP: /* means the standard xml http client was called */
        retval=xmlIOHTTPClose(ctx->data);
	break;

      /* In case of failures, we don't have anything
       * to free up / get rid of. */
    }
  }

    /* Free up context */
  xfree(ctx);

    /* Done... good, un recurse from site */
  mxslt_url_recurse_pop(recursion, 1);

  return retval;
}
