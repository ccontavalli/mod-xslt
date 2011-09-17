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

void mxslt_ap1_debug(void *ctx_v, int level, int mask, const char * msg, ...) {
  server_rec * s = (server_rec *)ctx_v;
  static char * buffer = NULL;
  static int size = 0;

  va_list list;
  int n;

    /* Prepare buffer, in case it is not ready */
  if(!buffer) {
    buffer=(char *)xmalloc(MXSLT_BUFFER_SIZE_START);
    size=MXSLT_BUFFER_SIZE_START;
  }

  va_start(list, msg);
  while(1) {
      /* Try to print into buffer */
    n=vsnprintf(buffer, size, msg, list);
    if((n >= 0 && n <= size)) 
      break;

    if(size == MXSLT_BUFFER_SIZE_MAX) {
      n=MXSLT_BUFFER_SIZE_MAX;
      break;
    }
      /* Now ... buffer was not big enough ..
       * increase size of buffer -- making sure we do not overflow */
    if(n < 0) {
      if(((size >> 1) > MXSLT_BUFFER_SIZE_MAX) || ((size >> 1) < size)) {
	size=MXSLT_BUFFER_SIZE_MAX;
      } else {
        size>>=1;
      }
    } else {
      if(((n + 1) > MXSLT_BUFFER_SIZE_MAX) || ((size + 1) < size)) {
	size=MXSLT_BUFFER_SIZE_MAX;
      } else {
        size=n+1;
      }
    }

    buffer=(char *)xrealloc(buffer, size);
  }
  va_end(list);

    /* Remove trailing new lines and cr */
  if(n && (buffer[n-1] == '\n' || buffer[n-1] == '\r')) {
    while(--n && (buffer[n-1] == '\n' || buffer[n-1] == '\r'))
      ;
    buffer[n]='\0';
  }
    /* Now, buffer contains string to be outputted .. */
  if(mxslt_debug_compare(mask, MXSLT_DBG_FLAGS)) {
    ap_log_error(MXSLT_NAME, 0, APLOG_WARNING | APLOG_NOERRNO, s, 
		   "debug(%08x/%08x): %s", level, mask, buffer);
  } else {
    ap_log_error(MXSLT_NAME, 0, APLOG_WARNING | APLOG_NOERRNO, s, 
		   "debug: %s", buffer);
  }

  return;
}


static int mxslt_ap1_writer(void * ctx, const char * buffer, int blen) {
  request_rec * r = (request_rec *)ctx;

  return ap_rwrite(buffer, blen, r);
}

static int mxslt_ap1_closer(void * ctx) {
  return 0;
}

static mxslt_callback_t mxslt_ap1_callback = { 
  mxslt_ap1_writer,
  mxslt_ap1_closer
};

table * mxslt_ap1_merge_tables(pool * p, table * table_new, table * table_old) {
  table * retval;
  array_header * a=ap_table_elts(table_old);
  table_entry * elts=(table_entry *)a->elts;
  int i;

  retval=ap_copy_table(p, table_new);
  for(i=0; i < a->nelts; i++) 
    ap_table_mergen(retval, elts[i].key, elts[i].val);

  return retval;
}

typedef struct mxslt_ap1_hdr_state_t {
  request_rec * r;
  mxslt_dir_config_t * config;
} mxslt_ap1_hdr_state_t;

static void mxslt_ap1_hdr_set(char * uc_hdr, char * uc_value, void * uc_table) {
  mxslt_ap1_hdr_state_t * hstate = (mxslt_ap1_hdr_state_t *)uc_table;
  char * hdr, * value;

  hdr=ap_pstrdup(hstate->r->pool, uc_hdr);
  value=ap_pstrdup(hstate->r->pool, uc_value);

  if(mxslt_debug_compare(hstate->config->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_PROTO)) {
    mxslt_ap1_debug(hstate->r->server, MXSLT_DBG_SAPI | MXSLT_DBG_PROTO, hstate->config->dbglevel, 
	"setting header '%s' to '%s'\n", mxslt_debug_string(hdr), mxslt_debug_string(value));
  }

  ap_table_set(hstate->r->headers_out, hdr, value);
}

static int mxslt_ap1_doc_param_header(mxslt_doc_t * document, table * tablep) {
  array_header * harr = ap_table_elts(tablep);
  table_entry * entry = (table_entry *)harr->elts;
  int i;

  for(i=0; i < harr->nelts; i++)
    mxslt_doc_param_hdr_add(document, entry[i].key, entry[i].val);

  return 0;
}

void mxslt_ap1_error(void *ctx, const char * msg, ...) {
  request_rec * r = (request_rec *)ctx;
  char * str;
  va_list args;	

    /* Work around libxslt bug... */
  if(ctx == NULL)
    return;

    /* Prepare string */
  va_start(args, msg);
  str=ap_pvsprintf(r->pool, (char *)msg, args);
  va_end(args);

  if(str[strlen(str)-1] == '\n')
    str[strlen(str)-1]='\0';

  ap_log_rerror(APLOG_MARK, APLOG_ERR | APLOG_NOERRNO, r, "%s: %s", MXSLT_NAME, str);

  return;
}

typedef struct mxslt_remove_file_t {
  char * filename;
  request_rec * r;

  mxslt_dir_config_t * config;
} mxslt_remove_file_t;

void mxslt_remove_file(mxslt_remove_file_t * data) {
  assert(data);

  if(mxslt_debug_compare(data->config->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG)) {
    mxslt_ap1_debug(data->r->server, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG, data->config->dbglevel,
		  "removing temporary file: %s\n", data->filename);
  }

  unlink(data->filename);
}

int mxslt_ap1_mktemp_file(mxslt_dir_config_t * config, request_rec * r, char * dir, char ** file) {
  mxslt_remove_file_t * data;
  int tmpfd;

  if(dir)
    (*file)=ap_pstrcat(r->pool, dir, "/", MXSLT_TMP_NAME, NULL);
  else
    (*file)=ap_pstrcat(r->pool, MXSLT_DEFAULT_TMP_DIR, "/", MXSLT_TMP_NAME, NULL);

  if((tmpfd=mkstemp(*file)) == -1)  {
    ap_log_rerror(APLOG_MARK, APLOG_ERR |APLOG_NOERRNO, r, MXSLT_NAME ": couldn't open temp file '%s'", *file);
    return tmpfd;
  }

  if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG)) {
    mxslt_ap1_debug(r->server, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG, config->dbglevel, 
		    "created temporary file: %s (%s)\n", *file, (config->unlink ? "will be removed" : "steady"));
  }

    /* Remember to remove this file */
  if(config->unlink) {
    if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE0)) {
      mxslt_ap1_debug(r->server, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE0, config->dbglevel,
     		"registering cleanup: %s - %s\n", mxslt_debug_string(r->uri), *file);
    }

    data=(mxslt_remove_file_t *)ap_palloc(r->pool, sizeof(mxslt_remove_file_t));
    data->config=config;
    data->filename=*file;
    data->r=r;

    ap_register_cleanup(r->pool, data, (void (*)(void *))mxslt_remove_file, ap_null_cleanup);
  }
 
  return tmpfd;
}

request_rec * mxslt_ap1_sub_request(mxslt_dir_config_t * config, request_rec * r, int tmpfd, 
				    struct in_addr * ip, uri_components * uri, int * toret) {
  request_rec * subr;
  request_rec tmp;
  conn_rec tmp_conn; 

  BUFF * buff_orig;
  BUFF * buff_new;
  int status=HTTP_INTERNAL_SERVER_ERROR;

  assert(config && r && toret);

    /* Setup a new buffer for the subrequest */
  buff_orig=r->connection->client;
  buff_new=ap_bcreate(r->pool, B_RDWR);
  buff_new->fd_in=-1; /* buff_orig->fd_in; -1 should be invalid*/
  buff_new->incnt=0; /* buff_orig->incnt; */
  buff_new->inptr=NULL; /* buff_orig->inptr; */
  buff_new->fd=tmpfd;

    /* we should change, in the original request:
     *    conn_rec connection
     * 	  hostname
     * 	  server 
     * 	  headers_in */

    /* Save original values before changing them */
  tmp.hostname=r->hostname;
  tmp.headers_in=r->headers_in;
  tmp.connection=r->connection;

    /* Forge up values so to build the request we want */
    /* XXX: this is a sort of hack to overcome the fact
     *      that many needed functions are private in 
     *      apache sources */
  r->hostname=uri->hostname;
  r->headers_in=ap_make_table(r->pool, 5);
  if(ip) {
    	/* Original code - let's try to simply use a memcpy instead of an unitialized struct */
    r->connection=&tmp_conn;
    memcpy(&(r->connection->local_addr.sin_addr), ip, sizeof(struct in_addr));
    r->connection->server=r->server;
    r->connection->local_addr.sin_port=htons(uri->port);
    r->connection->pool=r->pool;
    ap_update_vhost_given_ip(r->connection);

      /* Setup virtual host data */
    r->connection->base_server=r->connection->server;
    ap_update_vhost_from_headers(r);
  }
 
    /* Setup new buffer to be used */
  r->connection->client=buff_new;

    /* Prepare subrequest */
  if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_PROTO)) {
    mxslt_ap1_debug(r->server, MXSLT_DBG_SAPI | MXSLT_DBG_PROTO, config->dbglevel,
		  "performing subrequest for uri: path - %s, query - %s, args - %s\n", 
		  mxslt_debug_string(uri->path), mxslt_debug_string(uri->query), mxslt_debug_string(r->args));
  }

  subr=(request_rec *)ap_sub_req_method_uri("GET", uri->path, r);
  subr->args=uri->query; /* r->args;*/
 
  /* we shouldn't need those, since we are making 
   * a simple sub request 
  subr->protocol=r->protocol;
  subr->proto_num=r->proto_num;

  subr->chunked=r->chunked;
  subr->byterange=r->byterange;
  subr->boundary=r->boundary;
  subr->range=r->range;
  subr->clength=r->clength; 

  ap_bsetflag(subr->connection->client, B_CHUNK, 0);  */
     
    /* If there was an error in fixup handler (called
     * by sub_req_method_uri), do not run request */
  if(subr->status == OK || subr->status == HTTP_OK)
    status=ap_run_sub_req(subr);
    
    /* Cleanup after subrequest */
  ap_bflush(subr->connection->client);

    /* In the reply, setup the correct code */
  r->status_line=ap_pstrdup(r->pool, subr->status_line);
  r->status=subr->status;

  ap_rflush(subr);

    /* Destroy buffer and restore previous values */
  buff_new->fd_in=-1;
  ap_bclose(buff_new);

    /* Save original values before changing them */
  r->connection->client=buff_orig;
  r->hostname=tmp.hostname;
  r->headers_in=tmp.headers_in;
  r->connection=tmp.connection;

  if(status != OK && status != DONE) {
    *toret=status;

    ap_log_rerror(APLOG_MARK, APLOG_ERR | APLOG_NOERRNO, r, MXSLT_NAME ": couldn't fetch '%s'", uri->path);
    ap_destroy_sub_req(subr);
    return NULL; 
  }

  return subr;
}

request_rec * mxslt_ap1_sub_request_pass(mxslt_dir_config_t * config, request_rec * r, 
			char * uri, char ** file, int * toret) {

  request_rec * subr;
  BUFF * pbuff;
  BUFF * buff;
  int status=HTTP_INTERNAL_SERVER_ERROR;
  int tmpfd;

  assert(toret != NULL && file != NULL && r != NULL && config != NULL);
  *toret=HTTP_INTERNAL_SERVER_ERROR;

    /* Try to create a temporary file */
  if(mxslt_ap1_mktemp_file(config, r, config->tmpdir, file) < 0)
    return NULL;

   
    /* Setup a new buffer for a subrequest */
  pbuff=r->connection->client;
  buff=ap_bcreate(r->pool, B_WR);
  /*buff->flags=pbuff->flags&(~B_CHUNK); */
  buff->fd_in=pbuff->fd_in;
  buff->incnt=pbuff->incnt;
  buff->inptr=pbuff->inptr;
  buff->inbase=pbuff->inbase;
  buff->fd=tmpfd;
  r->connection->client=buff;

    /* Prepare subrequest */
  if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_PROTO)) {
    mxslt_ap1_debug(r->server, MXSLT_DBG_SAPI | MXSLT_DBG_PROTO, config->dbglevel,
		  "performing subrequest for uri: %s\n", uri);
  }
  subr=(request_rec *)ap_sub_req_method_uri((char *)r->method, uri, r);

  /* subr->the_request=r->the_request; */
  /* subr->assbackwards=1;*/
  subr->protocol=r->protocol;
  subr->proto_num=r->proto_num;

  /**/
  subr->chunked=r->chunked;
  subr->remaining=r->remaining;
  subr->byterange=r->byterange;
  subr->boundary=r->boundary;
  subr->range=r->range;
  subr->clength=r->clength; /**/

    /* Setup request headers and arguments */
  /* subr->headers_in=r->headers_in; */
  subr->args=r->args;
  ap_bsetflag(subr->connection->client, B_CHUNK, 0); 
      
  /* TODO: there is a bug here .. need to 
     make sure the Host header is set correctly... */

    /* If there was an error in fixup handler (called
     * by sub_req_method_uri), do not run request */
  if(!subr->status || subr->status == HTTP_OK)
    status=ap_run_sub_req(subr);
    
    /* Cleanup after subrequest */
  ap_bflush(subr->connection->client);
  r->status_line=ap_pstrdup(r->pool, subr->status_line);
  r->status=subr->status;

    /* Flush request buffer */
  ap_rflush(r);

    /* Restore old connection buffer */
  r->connection->client=pbuff;
  pbuff->inptr=buff->inptr;
  pbuff->incnt=buff->incnt;

    /* Destroy buffer and restore previous values */
  buff->fd_in=-1;
  ap_bclose(buff);

  if(status != OK && status != DONE) {
    *toret=status;

    ap_log_rerror(APLOG_MARK, APLOG_ERR | APLOG_NOERRNO, r, MXSLT_NAME ": subrequest failed, with status %d!", status);
    ap_destroy_sub_req(subr);
    return NULL; 
  }

  return subr;
}

int mxslt_ap1_file_parse(mxslt_dir_config_t * config, request_rec * r, 
	const char * filename, const char * defaultstyle, const char * forcestyle) {
  mxslt_ap1_hdr_state_t hstate = { r, config };
  array_header * harr = ap_table_elts(config->params);
  table_entry * entry = (table_entry *)harr->elts;
  xmlParserInputBufferPtr input;
  static mxslt_doc_t document;
  int status;
  int i;

  if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_PROTO)) {
    mxslt_ap1_debug(r->server, MXSLT_DBG_SAPI | MXSLT_DBG_PROTO, config->dbglevel,
		  "loading: %s\n", filename);
  }

    /* Remember the name of the temporary file */
  ap_table_setn(r->notes, MXSLT_NOTE_LOG, filename);

    /* Initialize document parsing */
  mxslt_doc_init(&document, APOS("apache1"), &ap1_mxslt_global_state, 
		 &mxslt_global_recursion, mxslt_ap1_error, r, r);
  mxslt_debug_enable(&document, config->dbglevel, mxslt_ap1_debug, r->server);
  if(config->signature == MXSLT_DCS_TRUE)
    mxslt_doc_flags_disable_signature(&document, MXSLT_TRUE);

    /* Parse url & header arguments */
  mxslt_ap1_doc_param_header(&document, r->headers_in);
  mxslt_doc_param_urlparse(&document, r->args);

    /* Add parameters specified in httpd.conf */
  for(i=0; i < harr->nelts; i++)
    mxslt_doc_param_add(&document, xstrdup(entry[i].key), xstrdup(entry[i].val));

    /* Create input parser */
  input=mxslt_create_input_file(&document, (char *)filename);
  if(input == NULL) {
      /* yaslt create input file set errno accordingly */
    switch(errno) {
      case EACCES:
        status=HTTP_FORBIDDEN;
	break;
        
      case EISDIR:
      case ENOTDIR:
      case ENOENT:
        status=HTTP_NOT_FOUND;
	break;
	
      default: 
        status=HTTP_INTERNAL_SERVER_ERROR;
        break;
    }

      /* Ok, print the error out & free up memory */
    mxslt_error(&document, "fatal - could not create parser for: %s (%s)\n", filename, r->filename);
    mxslt_doc_done(&document, &ap1_mxslt_global_state);

    return status;
  }

    /* Load document and adjust paths */
  status=mxslt_doc_load(&document, input, r->filename, (char *)r->hostname,
		       r->server->port, r->parsed_uri.path);
  if(status != MXSLT_OK) {
    mxslt_error(&document, "fatal - failed loading file: %s (%s)\n", filename, r->filename);
    mxslt_doc_done(&document, &ap1_mxslt_global_state);

    return HTTP_INTERNAL_SERVER_ERROR;
  } 

    /* Ok, if we have a default stylesheet to use, use it */
  if(forcestyle)
    status=mxslt_doc_load_stylesheet(&document, (char *)forcestyle);
  else {
    status=MXSLT_NONE;

      /* Start walking all stylesheet config->rules */
    harr=ap_table_elts(config->rules);
    entry=(table_entry *)harr->elts;

      /* Verify each rule in turn */
    for(i=0; status != MXSLT_OK && i < harr->nelts; i++) {

      if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_RULES)) {
        mxslt_ap1_debug(r->server, MXSLT_DBG_SAPI | MXSLT_DBG_RULES, config->dbglevel,
		  "trying to check: %s, %s\n", mxslt_debug_string(entry[i].val), mxslt_debug_string(entry[i].key));
      }
      if(mxslt_doc_parse_stylesheet(&document, entry[i].val, MSF_WITHOUT_MEDIA) == MXSLT_OK) {
	forcestyle=mxslt_yy_str_parse(&document, entry[i].key, strlen(entry[i].key));

        if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_RULES)) {
          mxslt_ap1_debug(r->server, MXSLT_DBG_SAPI | MXSLT_DBG_RULES, 
		  config->dbglevel, "applying: %s\n", mxslt_debug_string(forcestyle));
	}

        status=mxslt_doc_load_stylesheet(&document, (char *)forcestyle);
	xfree((char *)forcestyle);
	forcestyle=NULL;
      }
    }

      /* Try to load the stylesheet looking at PI */
    if(status != MXSLT_OK) {
      status=mxslt_doc_parse_pi(&document);
      if(status != MXSLT_OK && defaultstyle)
          /* Try to load default stylesheet */ 	 
	status=mxslt_doc_load_stylesheet(&document, (char *)defaultstyle);
    }
  }

  switch(status) {
    case MXSLT_FAILURE:
      mxslt_error(&document, "fatal - error while parsing PI\n");
      mxslt_doc_done(&document, &ap1_mxslt_global_state);
      return HTTP_INTERNAL_SERVER_ERROR;

    case MXSLT_SKIP:
    case MXSLT_UNLOADABLE:
      mxslt_error(&document, "fatal - couldn't load stylesheet for: %s (%s)\n", filename, r->filename);
      mxslt_doc_done(&document, &ap1_mxslt_global_state);
      return HTTP_INTERNAL_SERVER_ERROR;

    case MXSLT_NONE:
      mxslt_error(&document, "fatal - no <?xml-stylesheet or <?modxslt-stylesheet found in %s (%s)\n", filename, r->filename);
      mxslt_doc_done(&document, &ap1_mxslt_global_state);
      return HTTP_INTERNAL_SERVER_ERROR;
  }

    /* parse */
  if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE0)) {
    mxslt_ap1_debug(r->server, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE0, 
		  config->dbglevel, "parsing document: %s\n", filename);
  }
  status=mxslt_doc_parse(&document, mxslt_ap1_hdr_set, &hstate);
  if(status == MXSLT_FAILURE) {
    mxslt_error(&document, "fatal - couldn't apply stylesheet to: %s (%s)\n", filename, r->filename);
    mxslt_doc_done(&document, &ap1_mxslt_global_state);

    return HTTP_INTERNAL_SERVER_ERROR;
  }

    /* Unset content length */
  ap_table_unset(r->headers_out, "Content-Length");

   /* Send Document */
  r->content_type=(char *)document.content_type;
  ap_send_http_header(r);
  mxslt_doc_send(&document, &mxslt_ap1_callback, r);

    /* free up memory */
  mxslt_doc_done(&document, &ap1_mxslt_global_state);

  return DONE;
}

#if 0
  /* Reserved for future usage ... */
static int mxslt_ap1_hdr_get(void * uc_table, char * hdr, char ** value) {
  request_rec * r = (request_rec *)uc_table;
  char * retval;

  retval=(char *)ap_table_get(r->headers_in, hdr);
  if(value)
    *value=retval;

  if(retval)
    return 1;

  return 0;
}
#endif


