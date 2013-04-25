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


#include "modxslt.h"

#ifndef MXSLT_BUFFER_SIZE_START
# define MXSLT_BUFFER_SIZE_START 4096
#endif

#ifndef MXSLT_BUFFER_SIZE_MAX
# define MXSLT_BUFFER_SIZE_MAX 16384
#endif

int mxslt_ap2_close_brigade(void * ctx) {
  mxslt_ap2_ctx_t * ap2_ctx = (mxslt_ap2_ctx_t *)ctx;

  if(!ap2_ctx)
    return -1;

  if(ap2_ctx->brigade)
    apr_brigade_destroy(ap2_ctx->brigade);

  if(ap2_ctx->bucket)
    apr_bucket_destroy(ap2_ctx->bucket);

  xfree(ap2_ctx);

  return 0;
}

int mxslt_ap2_read_brigade(void * ctx, char * buffer, int len) {
  mxslt_ap2_ctx_t * ap2_ctx = (mxslt_ap2_ctx_t *)ctx;
  apr_bucket * tmp;
  apr_size_t size=0, offset=0;
  int status;

    /* Check if we have data left in the buffer first */
  if(ap2_ctx->buffer && ap2_ctx->offset < ap2_ctx->size) {
    size=(ap2_ctx->size-ap2_ctx->offset) > (len) ? (len) : (ap2_ctx->size-ap2_ctx->offset);
    memcpy(buffer, ap2_ctx->buffer+ap2_ctx->offset, size); 
    offset+=size;
    ap2_ctx->offset+=size;

        /* Check if we got all needed data */
    if(offset >= len)
      return offset;
  } 

    /* Get next bucket */ 
  if(!ap2_ctx->bucket) {
    if(!ap2_ctx->brigade)
      return offset;

    ap2_ctx->bucket=APR_BRIGADE_FIRST(ap2_ctx->brigade);
  } else {
    tmp=ap2_ctx->bucket;
    ap2_ctx->bucket=APR_BUCKET_NEXT(tmp);
    apr_bucket_delete(tmp);
  }
  
  while(1) {
      /* We got to end of brigade, so leave anyway */
    if(ap2_ctx->bucket == APR_BRIGADE_SENTINEL(ap2_ctx->brigade) || APR_BUCKET_IS_EOS(ap2_ctx->bucket)) {
      apr_brigade_destroy(ap2_ctx->brigade);

      ap2_ctx->brigade=NULL;
      ap2_ctx->bucket=NULL;
      break;
    }

      /* Try to read needed data */
    ap2_ctx->offset=0;
    status=apr_bucket_read(ap2_ctx->bucket, (const char **)(&(ap2_ctx->buffer)), &(ap2_ctx->size), APR_BLOCK_READ);
    if(status != APR_SUCCESS) {
      mxslt_error(ap2_ctx->doc, "could not read bucket\n");

      apr_bucket_delete(ap2_ctx->bucket);
      ap2_ctx->bucket=NULL;
      return 0;
    } 
      /* Copy data in result buffer */ 
    size=ap2_ctx->size > len-offset ? len-offset : ap2_ctx->size;
    memcpy(buffer+offset, ap2_ctx->buffer, size); 
    offset+=size;
    ap2_ctx->offset+=size;

      /* Check if we got all needed data */
    if(offset >= len)
      break;

    tmp=ap2_ctx->bucket;
    ap2_ctx->bucket=APR_BUCKET_NEXT(tmp);
    apr_bucket_delete(tmp);
  }

  return offset;
}

static int mxslt_ap2_writer(void * ctx, const char * buffer, int len) {
  ap_filter_t * f = (ap_filter_t *)ctx;
  apr_bucket_brigade * brigade = (apr_bucket_brigade *)f->ctx;
  apr_bucket * b;
  char * tmp;

    /* Copy stuff in output buffer */
  tmp=apr_palloc(f->r->pool, len);
  memcpy(tmp, buffer, len);

    /* Create transient bucket and append it to output
     * brigade */
  b=apr_bucket_transient_create(tmp, len, f->r->connection->bucket_alloc);
  APR_BRIGADE_INSERT_TAIL(brigade, b); 

  return len;
}

static int mxslt_ap2_closer(void * ctx) {
  ap_filter_t * f = (ap_filter_t *)ctx;
  apr_bucket_brigade * brigade = (apr_bucket_brigade *)f->ctx;
  apr_bucket * b;

    /* Append ``EOF'' at end of brigade */
  b=apr_bucket_eos_create(f->r->connection->bucket_alloc);
  APR_BRIGADE_INSERT_TAIL(brigade, b);

  return 0;
}

static mxslt_callback_t mxslt_ap2_callback = { 
  mxslt_ap2_writer,
  mxslt_ap2_closer
};

static int mxslt_ap2_doc_param_header(mxslt_doc_t * document, apr_table_t * tablep) {
  apr_array_header_t * harr = (apr_array_header_t *)apr_table_elts(tablep);
  apr_table_entry_t * entry = (apr_table_entry_t *)harr->elts;
  int i;

  for(i=0; i < harr->nelts; i++)
    mxslt_doc_param_hdr_add(document, entry[i].key, entry[i].val);

  return 0;
}

void mxslt_ap2_debug(
    void * ctx, unsigned int level, unsigned int mask, const char * msg, ...) {
  server_rec * s = (server_rec *) ctx;
  mxslt_ap2_debug_buffer_t * data;
  void * data_v;

  va_list list;
  int n;

  if(apr_threadkey_private_get(&data_v, mxslt_ap2_global_dbgbuffer) != APR_SUCCESS) {
    ap_log_error(APLOG_MARK, APLOG_CRIT, 0, s, MXSLT_NAME ": couldn't fetch area to output debugging info");
    return;
  }

  data=(mxslt_ap2_debug_buffer_t *)data_v;
  if(!data) {
    data=(mxslt_ap2_debug_buffer_t *)xmalloc(sizeof(mxslt_ap2_debug_buffer_t));
    data->buffer=(char *)xmalloc(MXSLT_BUFFER_SIZE_START);
    data->size=MXSLT_BUFFER_SIZE_START;
    data->used=0;

    apr_threadkey_private_set(data, mxslt_ap2_global_dbgbuffer);
  }

  va_start(list, msg);
  while(1) {
      /* Try to print into data->buffer */
    n=vsnprintf(data->buffer+data->used, data->size-data->used, msg, list);

      /* if n is >= 0 && < than buffer space, on all OS we know
       * about, it means that the data fit and was actually
       * written into the buffer */
    if((n >= 0 && n <= (data->size-data->used))) {
      data->used+=n;
      break;
    }

      /* If we did overflow... make sure everything is fine... */
    if(data->size == MXSLT_BUFFER_SIZE_MAX) {
      n=MXSLT_BUFFER_SIZE_MAX;
      break;
    }

      /* Now ... data->buffer was not big enough .. let's try to resize
       * it, making sure we do not overflow. */
    if(n < 0) {
      if(((data->size >> 1) > MXSLT_BUFFER_SIZE_MAX) || ((data->size >> 1) < data->size)) {
        data->size=MXSLT_BUFFER_SIZE_MAX;
      } else {
        data->size>>=1;
      }
    } else {
      if(((n + 1) > MXSLT_BUFFER_SIZE_MAX) || ((data->size + 1) < data->size)) {
        data->size=MXSLT_BUFFER_SIZE_MAX;
      } else {
        data->size=n+1;
      }
    }

    data->buffer=(char *)xrealloc(data->buffer, data->size);
  }
  va_end(list);

    /* If we actually have something to output ... */
  if(data->used && (data->buffer[data->used-1] == '\n' || data->buffer[data->used-1] == '\r')) {
    while(--data->used && (data->buffer[data->used-1] == '\n' || data->buffer[data->used-1] == '\r'))
      ;
    data->buffer[data->used]='\0';

      /* Now, data->buffer contains string to be outputted .. */
    if(mxslt_debug_compare(mask, MXSLT_DBG_FLAGS)) {
      ap_log_error(APLOG_MARK, APLOG_WARNING, 0, s, MXSLT_NAME
                     ": debug(level=%08x,mask=%08x,pid=%ld,tid=%lu): %s",
		     level, mask, (long)getpid(), mxslt_get_tid(),
		     data->buffer);
    } else {
      ap_log_error(APLOG_MARK, APLOG_WARNING, 0, s, MXSLT_NAME
                     ": debug: %s", data->buffer);
    }

    data->used=0;
  }

  return;
}

void mxslt_ap2_error(void *ctx, const char * msg, ...) {
  ap_filter_t * f = NULL;
  char * str;
  va_list args;
  int status;
  void * get;

    /* Doesn't make sense to check for errors here. */
  status=mxslt_ap2_ectxt_get(&get);

    /* But, if we didn't get the state, we are not able to parse data. */
  if(!(f=get) || status != APR_SUCCESS)
    return;

    /* Prepare string. */
  va_start(args, msg);
  str=apr_pvsprintf(f->r->pool, msg, args);
  va_end(args);

  if(str[strlen(str)-1] == '\n')
    str[strlen(str)-1]='\0';

  ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, f->r, MXSLT_NAME ": %s", str);

  return;
}

static void mxslt_ap2_hdr_set(char * uc_hdr, char * uc_value, void * uc_table) {
  ap_filter_t * f = (ap_filter_t *)uc_table;
  char * hdr, * value;

  hdr=apr_pstrdup(f->r->pool, uc_hdr);
  value=apr_pstrdup(f->r->pool, uc_value);

  apr_table_set(f->r->headers_out, hdr, value);
}

  /* Create input buffer */
xmlParserInputBufferPtr mxslt_ap2_create_input(mxslt_doc_t * doc, apr_bucket_brigade * brigade) {
  xmlParserInputBufferPtr retval;
  mxslt_ap2_ctx_t * ctx;

  retval=xmlAllocParserInputBuffer(XML_CHAR_ENCODING_NONE);
  if(!retval)
    return retval;

  ctx=(mxslt_ap2_ctx_t *)xmalloc(sizeof(mxslt_ap2_ctx_t));
  ctx->brigade=brigade;
  ctx->offset=0;
  ctx->bucket=NULL;
  ctx->buffer=NULL;
  ctx->size=0;

  retval->context=ctx;
  retval->readcallback=mxslt_ap2_read_brigade;
  retval->closecallback=mxslt_ap2_close_brigade;

  return retval;
}

int mxslt_ap2_file_parse(ap_filter_t * f, apr_bucket_brigade * brigade, 
			 const char * defaultstyle, const char * forcestyle,
			 mxslt_dir_config_t * config) {
  apr_array_header_t * harr = (apr_array_header_t *)apr_table_elts(config->params);
  apr_table_entry_t * entry = (apr_table_entry_t *)harr->elts;

  mxslt_shoot_t * state;
  mxslt_recursion_t * recursion;

  xmlParserInputBufferPtr input;
  mxslt_doc_t document;
  void * get;

  int status;
  int i;


  if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_PROTO)) {
    mxslt_ap2_debug(f->r->server, MXSLT_DBG_SAPI | MXSLT_DBG_PROTO, config->dbglevel,
		    "loading: %s\n", f->r->filename);
  }

    /* Get global state from tsd data */
  status=mxslt_ap2_state_get(&get);
  if(status != APR_SUCCESS) {
    mxslt_error(&document, "fatal - could not retrieve global state with apr_threadkey_private_get\n");

    return HTTP_INTERNAL_SERVER_ERROR;
  }

    /* Remember global state */
  state=get;
  status=mxslt_ap2_recursion_get(&get);
  if(status != APR_SUCCESS) {
    mxslt_error(&document, "fatal - could not retrieve global state with apr_threadkey_private_get\n");
    return HTTP_INTERNAL_SERVER_ERROR;
  }

    /* Remember recursion pointer */
  recursion=get;

    /* f is the context used by the mxslt_ap2_error function.  Context is
     * passed to the libxslt library and is saved in a global variable. The
     * problem here is that if apache2 uses a multithreaded model, the global
     * context is overwritten by any other executed thread. */
    /* Purpose of the following code is to save the context (f) in a tsd, and
     * use that instead */
  /* mxslt_doc_init(&document, &state, mxslt_ap2_error, f); */
  mxslt_ap2_ectxt_set(f);

    /* Initialize document parsing */
  mxslt_doc_init(&document, APOS("apache2"), state, recursion, mxslt_ap2_error, f, f);
  mxslt_doc_debug_enable(&document, config->dbglevel, mxslt_ap2_debug, f->r->server);

    /* Add headers to parameters */
  mxslt_ap2_doc_param_header(&document, f->r->headers_in); 
  mxslt_doc_param_urlparse(&document, f->r->args);

    /* Add configuration file parameters */
  for(i=0; i < harr->nelts; i++)
    mxslt_doc_param_add(&document, xstrdup(entry[i].key), xstrdup(entry[i].val));

    /* Create input parser */
  input=mxslt_ap2_create_input(&document, brigade);
  if(input == NULL) {
    mxslt_error(&document, "fatal - could not create brigade input parser!\n");

    return HTTP_INTERNAL_SERVER_ERROR;
  }

    /* Load document and adjust paths */
  status=mxslt_doc_load(
      &document, input, f->r->filename, f->r->parsed_uri.hostname, 
      f->r->parsed_uri.port, f->r->parsed_uri.path);
  if(status != MXSLT_OK) {
    mxslt_error(&document, "fatal - failed loading file: %s\n", f->r->filename);
    mxslt_doc_done(&document, state);

    return HTTP_INTERNAL_SERVER_ERROR;
  } 


  if(forcestyle)
    status=mxslt_doc_load_stylesheet(&document, (char *)forcestyle);
  else {
    status=MXSLT_NONE;

    harr=(apr_array_header_t *)apr_table_elts(config->rules);
    entry=(apr_table_entry_t *)harr->elts;

    for(i=0; status != MXSLT_OK && i < harr->nelts; i++) {
      if(mxslt_doc_parse_stylesheet(&document, entry[i].val, MSF_WITHOUT_MEDIA) == MXSLT_OK) {
	forcestyle=mxslt_yy_str_parse(&document, entry[i].key, strlen(entry[i].key));
	status=mxslt_doc_load_stylesheet(&document, (char *)forcestyle);
	xfree((char *)forcestyle);
	forcestyle=NULL;
      }
    }

    if(status != MXSLT_OK) {
      status=mxslt_doc_parse_pi(&document);
      if(status != MXSLT_OK && defaultstyle)
	status=mxslt_doc_load_stylesheet(&document, (char *)defaultstyle);
    }
  }
/*   ap_add_common_vars();
     ap_add_cgi_vars(); */

    /* Parse Processing Instructions found in document */
  switch(status) {
    case MXSLT_FAILURE:
      mxslt_error(&document, "fatal - error while parsing PI\n");
      mxslt_doc_done(&document, state);
      return HTTP_INTERNAL_SERVER_ERROR;

    case MXSLT_NONE:
      mxslt_error(&document, "fatal - no <?xml-stylesheet or <?modxslt-stylesheet found in %s\n", f->r->filename);
      mxslt_doc_done(&document, state);
      return HTTP_INTERNAL_SERVER_ERROR;

    case MXSLT_SKIP:
    case MXSLT_UNLOADABLE:
      mxslt_error(&document, "fatal - couldn't load stylesheet for: %s\n", f->r->filename);
      mxslt_doc_done(&document, state);
      return HTTP_INTERNAL_SERVER_ERROR;
  }

    /* parse */
  if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE0)) {
    mxslt_ap2_debug(f->r->server, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE0,
		    config->dbglevel, "parsing document: %s\n", f->r->filename);
  }

  status=mxslt_doc_parse(&document, mxslt_ap2_hdr_set, f);
  if(status == MXSLT_FAILURE) {
    mxslt_error(&document, "fatal - couldn't apply stylesheet to: %s\n", f->r->filename);
    mxslt_doc_done(&document, state);

    return HTTP_INTERNAL_SERVER_ERROR;
  }

    /* We need to strdup content_type since request will go on
     * after deallocating memory! */
  apr_table_unset(f->r->headers_out, "Content-Length");

    /* Send Document */
  f->r->content_type=apr_pstrdup(f->r->pool, (char *)document.content_type);
  f->ctx=apr_brigade_create(f->r->pool, NULL);
  mxslt_doc_send(&document, &mxslt_ap2_callback, f);

    /* free up memory */
  mxslt_doc_done(&document, state);

    /* pass brigade over */
  return HTTP_OK;
}
