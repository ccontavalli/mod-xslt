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


#ifndef MXSLT_H
# define MXSLT_H

# include "modxslt-info.h"
# include "modxslt-macro.h"

  /* XML include files */
# include <libxml/tree.h>
# include <libxml/uri.h>
# include <libxml/parserInternals.h>
# include <libxml/threads.h>
# include <libxml/nanohttp.h>

  /* XSLT include files */
# include <libxslt/xslt.h>
# include <libxslt/xsltInternals.h>
# include <libxslt/transform.h>
# include <libxslt/xsltutils.h>
# include <libxslt/extensions.h>
# include <libxslt/templates.h>
# include <libxslt/imports.h>

# undef PACKAGE_BUGREPORT
# undef PACKAGE_NAME
# undef PACKAGE_STRING
# undef PACKAGE_TARNAME
# undef PACKAGE_VERSION

# include <modxslt1/modxslt-system.h>
# include <modxslt1/modxslt-integers.h>

# include "modxslt-version.h"
# include "modxslt-table.h"

# include <stdio.h>
# include <stdlib.h>
# include <stdarg.h>
# include <string.h>
# include <errno.h>


# ifdef HAVE_PTHREADS
#  include <pthread.h>
# endif

# ifndef MXSLT_XSLT_OPTIONS
#  ifndef MXSLT_DISABLE_XINCLUDE
#   define MXSLT_XSLT_OPTIONS XML_PARSE_NOENT|XML_PARSE_DTDLOAD|XML_PARSE_DTDATTR| \
 	   XML_PARSE_NSCLEAN|XML_PARSE_NOCDATA|XML_PARSE_XINCLUDE
#  else
#   define MXSLT_XSLT_OPTIONS XML_PARSE_NOENT|XML_PARSE_DTDLOAD|XML_PARSE_DTDATTR| \
 	   XML_PARSE_NSCLEAN|XML_PARSE_NOCDATA
#  endif
# endif

  /* Initial size of params table.
   * Should be a power of 2 minus 1.
   * Default 2^3 - 1 = 7 */
# define MXSLT_PARAM_START 7 
# define APOS(str) "'" str "'"
# define STR(macro) #macro

typedef enum mxslt_status_t {
  MXSLT_OK=0,
  MXSLT_FAILURE,
  MXSLT_NONE,
  MXSLT_UNLOADABLE,
  MXSLT_DECLINE,
  MXSLT_SKIP,
  MXSLT_PARAM,
  MXSLT_PARAM_BADKEY,
  MXSLT_PARAM_OVERFLOW,
  MXSLT_LOOP,
  MXSLT_MAX_LEVEL
} mxslt_status_t;

typedef enum mxslt_op_t {
  MXSLT_ERROR=-1,
  MXSLT_FALSE=0,
  MXSLT_TRUE=1
} mxslt_op_t;

typedef enum mxslt_doc_flags_e {
  MXSLT_BERAW=BIT(0),  /* Return raw result */
  MXSLT_ISWRG=BIT(1),  /* Document contains errors */
  MXSLT_STYLE=BIT(3),  /* We're loading stylesheet */
} mxslt_doc_flags_e;

  /* Holds call backs to be called to output data */
typedef struct mxslt_callback_t {
  int (*writer)(void *, const char *, int);
  int (*closer)(void *);
} mxslt_callback_t;

#ifndef MXSLT_MAX_RECURSION_LEVEL
# define MXSLT_MAX_RECURSION_LEVEL 15
#endif

  /* Holds information about the maximum
   * recursion level reached by the server */
typedef struct mxslt_recursion_t {
  int rec_level;
  mxslt_table_t rec_table;
  mxslt_table_record_t * rec_lastrecord;
} mxslt_recursion_t;

# define MXSLT_RECURSION_INIT { 0, mxslt_table_init_static(), NULL }

typedef struct mxslt_shoot_t mxslt_shoot_t; 
typedef struct mxslt_doc_t mxslt_doc_t;
typedef struct mxslt_state_t mxslt_state_t;

typedef struct mxslt_url_handler_t mxslt_url_handler_t;

  /* Holds current yaslt state. Created at xml_init time, restored on each
   * request */
# define MXSLT_SHOOT_INIT  { NULL, NULL }
struct mxslt_shoot_t {
  mxslt_state_t * mxslt_state;
  void * xml_state;
};

typedef struct mxslt_pi_t mxslt_pi_t;

typedef void (*mxslt_error_hdlr_f)(void * ctx, const char * msg, ...);
typedef void (*mxslt_debug_hdlr_f)(
    void * ctx, unsigned int level, unsigned int mask, const char * msg, ...);
typedef void (*mxslt_header_set_f)(char * name, char * value, void * data);

  /* Shortcut some boring casts */
typedef int (*mxslt_opr_call_f)(mxslt_doc_t *, void *, void *);


  /* Holds informations about a 
   * single document */
struct mxslt_doc_t {
  int flags; 	/* Document state flags */
  void * ctx; 	/* Document context, used by sapi handlers */

    /* URL of the document */
  char * localurl;
  char * localfile;

    /* Header handling callbacks */
  mxslt_header_set_f header_set;
  void * header_data;

    /* Xml translation handling */
  xmlDocPtr file;
  xmlDocPtr parsed;

  xsltStylesheetPtr stylesheet;
  char * parser; /* 'apache1', 'apache2... */

    /* Xml pi pointers */
  mxslt_pi_t * pi_first;
  mxslt_pi_t * pi_last;

    /* Xml document info */
  xmlChar * encoding;
  xmlChar * content_type;
  xmlCharEncodingHandlerPtr encoder;

    /* Error handling */
  mxslt_error_hdlr_f errhdlr;
  void * errctx; 

    /* Parameters parsing */
  mxslt_table_t table;

    /* Context used by http handler */

    /* State being currently used */
  mxslt_state_t * state;
  mxslt_shoot_t shoot;

  mxslt_debug_hdlr_f dbghdlr;
  void * dbgctx;

  unsigned int dbglevel;
};

# include "modxslt-memory.h"
# include "modxslt-debug.h"

typedef int (*mxslt_url_handle_f)(struct mxslt_doc_t *, void **, void *, const char *);
typedef int (*mxslt_url_open_f)(struct mxslt_doc_t *, void *, void *, const char **, void **);
typedef int (*mxslt_url_read_f)(struct mxslt_doc_t *, void *, const char *, int);
typedef int (*mxslt_url_close_f)(struct mxslt_doc_t *, void *);

struct mxslt_url_handler_t {
  mxslt_url_handle_f handle;
  mxslt_url_close_f close;
  mxslt_url_read_f read;
  mxslt_url_open_f open;
};

  /* Holds the current global state of the library */
# define MXSLT_STATE_INIT() { MXSLT_HTTP_HANDLER_INIT, NULL, NULL, NULL, \
      (mxslt_debug_hdlr_f)mxslt_doc_null, NULL, mxslt_debug_firstlevel() } 
struct mxslt_state_t {
  mxslt_url_handler_t http_handler;

  struct mxslt_recursion_t * recursion;
  struct mxslt_doc_t * document;
  void * ctx;

  mxslt_debug_hdlr_f dbghdlr;
  void * dbgctx;

  unsigned int dbglevel;
};

  /* Keeps the function pointers used by sapi to override http handling */
# define MXSLT_HTTP_HANDLER_INIT { mxslt_url_handle, NULL, NULL, NULL }

typedef enum mxslt_scan_flag_e {
  MSF_WITHOUT_MEDIA=BIT(0)
} mxslt_scan_flag_e;

  /* Holds information about the current
   * scanning state */
typedef struct mxslt_scan_t {
  int flags;

  int status;
  mxslt_doc_t * document;
} mxslt_scan_t;

  /* Holds informations abouta an
   * operator */
typedef struct mxslt_opr_t {
  char * opr;
  mxslt_opr_call_f call;
} mxslt_opr_t;

  /* Holds a list of static attributes
   * to be looked for in an array */
typedef struct mxslt_attr_search_t {
  xmlChar * name;
  size_t size;
} mxslt_attr_search_t;

  /* Holds a variable for the yaslt
   * parser */
typedef struct mxslt_var_t {
  char * string;  /* Variable value. NULL is valid */
  int found;	/* If the variable was found */
} mxslt_var_t;

  /* Include parsing prototypes */
/* # include "../parser/modxslt-screen-expr.parser.h" */

  /* Used internally to call operator handlers */
# define mxslt_opr_bool_call(op, doc, val1, val2) ((op)->call)(doc, (void *)val1, (void *)val2)
# define mxslt_opr_cmp_call(op, doc, val1, val2) ((op)->call)(doc, (void *)val1, (void *)val2)

  /* Used to call http handlers */
# ifdef HAVE_PTHREADS
#  define mxslt_global_http_state (*(mxslt_url_handler_t *)(pthread_getspecific(mxslt_global_http_handler)))
# else /* HAVE_PTHREADS */
#  define mxslt_global_http_state mxslt_global_http_handler
# endif

  /* Used to call http handling globals */
# define mxslt_sapi_http_data_get(doc, store, ctx, rec) ((mxslt_global_http_state.get)(doc, store, ctx, rec))
# define mxslt_sapi_http_open(doc, store, ctx, uri, ret) (((doc->state->http_handler).open)(doc, store, ctx, uri, ret))
# define mxslt_sapi_http_read(doc, ctx, buffer, len) (((doc->state->http_handler).read)(doc, ctx, buffer, len))
# define mxslt_sapi_http_close(doc, ctx) (((doc->state->http_handler).close)(doc, ctx))
# define mxslt_sapi_http_handle(doc, store, ctx, uri) (((doc->state->http_handler).handle)(doc, store, ctx, uri))


  /* Error reporting function */
# define mxslt_error(doc, ...) ((doc)->errhdlr((doc)->errctx, __VA_ARGS__))

  /* Ok, I'm lazy... */
# ifndef MXSLT_IS_BLANK
#  define MXSLT_IS_BLANK(val) (((val)== ' ') || ((val)== '\n') || ((val)=='\r') || ((val)=='\t'))
# endif

# include "modxslt-proto.h"

#endif /* MXSLT_H */
