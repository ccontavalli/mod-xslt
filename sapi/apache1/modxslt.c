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

#ifndef HTTP_RECURSION_ERROR
# define HTTP_RECURSION_ERROR HTTP_INTERNAL_SERVER_ERROR
#endif

module MODULE_VAR_EXPORT mxslt_module;

  /* Handlers:
   *   dyn handler -> "mod-xslt-dynamic" -> dynamic documents,
   *   		only if the MIME type is text/xml
   *   frc handler -> "mod-xslt-force" -> dynamic documents, 
   *   		independently from the MIME type
   *   std handler -> MXSLT_NAME -> static .xml documents
   */

  /* This is global since it is initialized in childe_init
   * and used in the various handlers */
mxslt_shoot_t ap1_mxslt_global_state = MXSLT_SHOOT_INIT;
mxslt_recursion_t mxslt_global_recursion = MXSLT_RECURSION_INIT;
mxslt_table_t mxslt_global_ips = mxslt_table_init_static();
int mxslt_global_dbglevel = 0;

  /* Set when fixup phase should not be run */
int mxslt_global_skip_fixup = 0;
# define mxslt_ap1_fixup_enable() mxslt_global_skip_fixup=0
# define mxslt_ap1_fixup_disable() mxslt_global_skip_fixup=1
# define mxslt_ap1_fixup_isdisabled() mxslt_global_skip_fixup

typedef struct mxslt_ap1_http_store_t {
  struct in_addr ip;
  uri_components URI;
} mxslt_ap1_http_store_t;

static void mxslt_ap1_outputter(void * data, char * msg, ...) {
  request_rec * r=(request_rec *)data; 
  va_list args;
  char * str;

  va_start(args, msg);
  str=ap_pvsprintf(r->pool, msg, args); 
  va_end(args);

  ap_log_rerror(APLOG_MARK, APLOG_ERR | APLOG_NOERRNO, r, 
      	    MXSLT_NAME ": %s", str);

  return;
}

static int mxslt_ap1_http_handle(mxslt_doc_t * doc, void ** store, void * context, const char * uri) {
  mxslt_ap1_http_store_t * data=(mxslt_ap1_http_store_t *)(*store);
  request_rec * r = (request_rec *)context;
  struct hostent * hp;
  struct in_addr ** address;
  uri_components URI;
  int status;

    /* Slice up URI in pieces */
  status=ap_parse_uri_components(r->pool, uri, &URI);
  if(!status) {
    mxslt_debug_print(doc, MXSLT_DBG_SAPI, "couldn't parse uri |%s| with ap_parse_uri_components\n", mxslt_debug_string(uri));
      /* Let's see what the standard handler is able to do... */
    return MXSLT_FALSE;
  }
  
    /* Get hostname 
     * gethostbyname is not reentrant, but:
     *   apache1 is not multithreaded
     *   gethostbyname is not called as long
     *     as I need hp */
  hp=gethostbyname(URI.hostname);

  if(!hp) {
    mxslt_debug_print(doc, MXSLT_DBG_SAPI, "couldn't resolve hostname |%s|, status: %d\n", mxslt_debug_string(URI.hostname), h_errno);
    return MXSLT_SKIP;
  }

  for(address=(struct in_addr **)(hp->h_addr_list); address && *address; address++) {
    mxslt_debug_print(doc, MXSLT_DBG_SAPI | MXSLT_DBG_VERBOSE0 | MXSLT_DBG_DEBUG,
		      "looking up address: %s for |%s|, aka: %s\n", inet_ntoa(**address), uri, URI.hostname);

      /* XXX: how do we deal with ipv6 addresses? from gethostbyname(3): ``the only
       *      valid address type is currently AF_INET'' */
    status=mxslt_table_search(&mxslt_global_ips, NULL, (void *)((*address)->s_addr), NULL);
    if(status == MXSLT_TABLE_FOUND) {
      mxslt_debug_print(doc, MXSLT_DBG_SAPI | MXSLT_DBG_VERBOSE0, "handling request for |%s| as an internal"
			" request, using apache internals\n", uri);

        /* If a store pointer was provided */ 
      if(store) {
        /* data=(mxslt_ap1_http_store_t *)ap_palloc(r->pool, sizeof(mxslt_ap1_http_store_t));*/

          /* Allocate data to pass over to the open handler if needed */
	  /* Watch out: in other parts of the code, we assume that if *store != NULL,
	   * its address won't change - mxslt_ap1_http_open */
        if(*store == NULL) {
          data=(mxslt_ap1_http_store_t *)ap_palloc(r->pool, sizeof(mxslt_ap1_http_store_t));
          *store=data; 
        }

          /* Copy data needed by the open handler */
        memcpy(&(data->URI), &URI, sizeof(uri_components));
        memcpy(&(data->ip), *address, sizeof(struct in_addr));
      }

      return MXSLT_TRUE;
    }
  }

  return MXSLT_FALSE;
}

static void mxslt_ap1_dump_table(mxslt_dir_config_t * config, int level, request_rec * r, const char * str, table * table) {
  array_header * harr = ap_table_elts(table);
  table_entry * entry = (table_entry *)harr->elts; 
  int i;
   
  mxslt_ap1_debug(r->server, level, config->dbglevel, "%s", str);
  for(i=0; i < harr->nelts; i++) {
    mxslt_ap1_debug(r->server, level, config->dbglevel, "  %s -> %s\n", 
		    mxslt_debug_string(entry[i].key), mxslt_debug_string(entry[i].val));
  }
}

static void mxslt_ap1_dump_config(mxslt_dir_config_t * config, int level, request_rec * r) {
  mxslt_ap1_debug(r->server, level, config->dbglevel, "fetched config (%08x):\n", level);
  mxslt_ap1_debug(r->server, level, config->dbglevel, "* config: %08x\n", (int)config); 
  mxslt_ap1_debug(r->server, level, config->dbglevel, "* config->state: %d\n", config->state); 
  mxslt_ap1_debug(r->server, level, config->dbglevel, "* config->tmpdir: %s\n", config->tmpdir);
  mxslt_ap1_debug(r->server, level, config->dbglevel, "* config->unlink: %d\n", config->unlink);
  mxslt_ap1_debug(r->server, level, config->dbglevel, "* config->dbglevel: %08x\n", config->dbglevel);

  mxslt_ap1_dump_table(config, level, r, "* config->mime_styles\n", config->mime_styles);
  mxslt_ap1_dump_table(config, level, r, "* config->default_styles\n", config->default_styles);
  mxslt_ap1_dump_table(config, level, r, "* config->filter_dynamic\n", config->filter_dynamic);
  mxslt_ap1_dump_table(config, level, r, "* config->filter_force\n", config->filter_force);
  mxslt_ap1_dump_table(config, level, r, "* config->params\n", config->params);
  mxslt_ap1_dump_table(config, level, r, "* config->rules\n", config->rules);

  return;
}

static int mxslt_ap1_http_open(mxslt_doc_t * doc, void * store, void * context, const char ** uri, void ** retval) {
  mxslt_ap1_http_store_t * data=(mxslt_ap1_http_store_t *)store;
  uri_components URI;
  char * tmpfile, * realuri;
  mxslt_dir_config_t * config;
  request_rec * r=(request_rec *)context;
  request_rec * subr=r, * tmpreq=r;
  int status;
  int fd, reclevel=0;

    /* Get apache 1 config */
  config=(mxslt_dir_config_t *)ap_get_module_config(r->per_dir_config, &mxslt_module);
  if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_VERBOSE1 | MXSLT_DBG_DEBUG)) 
    mxslt_ap1_dump_config(config, MXSLT_DBG_SAPI | MXSLT_DBG_VERBOSE1 | MXSLT_DBG_DEBUG, r);

    /* Try to open temporary file */
  fd=mxslt_ap1_mktemp_file(config, r, config->tmpdir, &tmpfile); 
  if(fd < 0)
    return MXSLT_FAILURE;

    /* If we have no specific data about the host we're going
     * to subprocess, get it */
  if(data) {
      /* Make sub request, and listen to redirects */
    tmpreq=mxslt_ap1_sub_request(config, subr, fd, &(data->ip), &(data->URI), &status);
  } else {
      /* Slice up URI in pieces */
    status=ap_parse_uri_components(r->pool, *uri, &URI);
    if(!status) {
      mxslt_debug_print(doc, MXSLT_DBG_SAPI, "couldn't parse uri |%s| with ap_parse_uri_components\n", mxslt_debug_string(uri));
      return MXSLT_FAILURE;
    }

      /* Perform sub request */
    tmpreq=mxslt_ap1_sub_request(config, subr, fd, NULL, &(URI), &status);
  }

  realuri=(char *)*uri;
  reclevel=mxslt_url_recurse_level(doc->state->recursion);
  while(1) {
    if(!tmpreq) {
      mxslt_url_recurse_pop(doc->state->recursion, (mxslt_url_recurse_level(doc->state->recursion))-(reclevel)); 
      return MXSLT_FAILURE;
    }

      /* Check if status is a redirect */
    if(tmpreq->status < 300 || tmpreq->status >= 400)
      break;

      /* It is. Get the real uri of the next request */
    realuri=(char *)ap_table_get(subr->headers_in, "Location");

      /* Check if we already walked the new uri */
    if(mxslt_url_recurse_allowed(doc->state->recursion, realuri) != MXSLT_OK) {
        /* Output error */
      mxslt_error(doc, "warning - maximum recursion level reached" 
		       "or url already walked: %s (%s)\n", realuri, *uri);
      ap_destroy_sub_req(tmpreq);

        /* Dump stack trace */
      mxslt_url_recurse_dump(doc->state->recursion, &mxslt_ap1_outputter, r); 

        /* Pop last walked urls */
      mxslt_url_recurse_pop(doc->state->recursion, (mxslt_url_recurse_level(doc->state->recursion))-(reclevel)); 

      return MXSLT_FAILURE;
    }

      /* Check if we can handle the uri */
    if(mxslt_ap1_http_handle(doc, &store, context, realuri) == MXSLT_FALSE) {
      mxslt_url_recurse_pop(doc->state->recursion, (mxslt_url_recurse_level(doc->state->recursion))-(reclevel)); 

        /* Uhm... can we do this? eg, if we destroy the sub request, are
	 * we sure location will survive? make sure by copying the uri in
	 * the request pool... */
      mxslt_error(doc, "warning - '%s' redirects to '%s', which is not local"
		       " - loop checks disabled - good luck\n", *uri, realuri);
      (*uri)=ap_pstrdup(r->pool, realuri);
      ap_destroy_sub_req(tmpreq);

      return MXSLT_SKIP; 
    }

      /* Open file for subrequest (it is closed after each sub request) */
      /* XXX: is this dangerous? What happens if, in the mean time, the file
       *      has been replaced by a symbolic link to something else? */
    fd=open(tmpfile, O_RDWR | O_TRUNC);
    if(fd < 0) {
      ap_destroy_sub_req(tmpreq);
      mxslt_url_recurse_pop(doc->state->recursion, (mxslt_url_recurse_level(doc->state->recursion))-(reclevel)); 

      return MXSLT_FAILURE;
    }

    ap_destroy_sub_req(tmpreq);
    tmpreq=mxslt_ap1_sub_request(config, r, fd, &(data->ip), &(data->URI), &status);

      /* Remember we already walked this uri */
    mxslt_url_recurse_push(doc->state->recursion, realuri);
  } 
    /* Forget about those urls */
  mxslt_url_recurse_pop(doc->state->recursion, (mxslt_url_recurse_level(doc->state->recursion))-(reclevel)); 

    /* Check request was succesful */
  if(tmpreq->status != HTTP_OK) {
    ap_destroy_sub_req(tmpreq);
    return MXSLT_FAILURE;
  }

    /* Try to open newly created file */
  fd=open(tmpfile, O_RDONLY); 
  if(fd == -1) {
    mxslt_error(doc, "couldn't open: %s (%s, %s, %s) - errno: %d\n", tmpfile, uri, realuri, subr->filename, errno);
    ap_destroy_sub_req(tmpreq);

    return MXSLT_FAILURE;
  }
  ap_destroy_sub_req(tmpreq);

  *retval=((void *)fd);
  return MXSLT_OK;
}

static int mxslt_ap1_http_read(mxslt_doc_t * doc, void * context, const char * buffer, int len) {
  int fd = (int)context;

  return read(fd, (char *)buffer, len);
}

static int mxslt_ap1_http_close(mxslt_doc_t * doc, void * context) {
  int fd = (int)context;

  return close(fd);
}

  /* This function is called right before the handler 
   * for the file and after deciding whetever to allow or not access
   * to it (http_request.c:process_request_internal) 
   * Decides if we need to take care of the file */
static int mxslt_ap1_fixup(request_rec * r) {
  mxslt_recursion_t * recursion=&mxslt_global_recursion;
  mxslt_dir_config_t * config;
  char * type = NULL;
  char * handler = NULL;
  int status;

  config=(mxslt_dir_config_t *)ap_get_module_config(r->per_dir_config, &mxslt_module);
  if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_VERBOSE1 | MXSLT_DBG_DEBUG)) 
    mxslt_ap1_dump_config(config, MXSLT_DBG_SAPI | MXSLT_DBG_VERBOSE1 | MXSLT_DBG_DEBUG, r);

  assert(mxslt_global_skip_fixup == 0 || mxslt_global_skip_fixup == 1);

    /* If this is not something we want to handle, return and give up */
  if(!mxslt_engine_on(config->state) || r->header_only || mxslt_ap1_fixup_isdisabled()) {
    mxslt_ap1_fixup_enable();
    return DECLINED;
  }

    /* Get type of the stuff we are going to process */
  if(r->handler) 
    type=(char *)r->handler;
  else
    type=(char *)r->content_type;

  if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE1)) {
    mxslt_ap1_debug(r->server, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE1, 
		    config->dbglevel, "fixup is deciding over type: %s\n", type);
  }

    /* Check if we need to intercept the request
     * by setting a custom handler */
  if(ap_table_get(config->filter_dynamic, type))
    handler=MXSLT_DYN_HANDLER;
  else
    if(ap_table_get(config->filter_force, type))
      handler=MXSLT_FRC_HANDLER;

    /* Ok, if we decided to use a custom handler,
     * we need to check we are allowed to recurse */
  if(handler) {
    if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE0)) {
      mxslt_ap1_debug(r->server, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE0, 
		    config->dbglevel, "handler %s is being called to parse: %s(%s)\n", handler,
		    mxslt_debug_string(r->uri), mxslt_debug_string(type));
    }

    status=mxslt_url_recurse_allowed(recursion, r->uri);
    if(status != MXSLT_OK) {
      if(status == MXSLT_MAX_LEVEL)
        ap_log_rerror(APLOG_MARK, APLOG_ERR | APLOG_NOERRNO, r, 
            MXSLT_NAME ": maximum recursion level of " STR(MXSLT_MAX_RECURSION_LEVEL) " reached, skipping %s", r->uri);
      else
        ap_log_rerror(APLOG_MARK, APLOG_ERR | APLOG_NOERRNO, r, 
            MXSLT_NAME ": loop detected while processing %s", r->uri);
      mxslt_url_recurse_dump(recursion, &mxslt_ap1_outputter, r);

      return HTTP_RECURSION_ERROR;
    }

       /* Remember original mime type */
    ap_table_set(r->notes, MXSLT_NOTE_HANDLER, type);
    r->handler=handler;
  }

  return DECLINED;
}

static int mxslt_ap1_std_handler(request_rec * r) {
  mxslt_recursion_t * recursion=&mxslt_global_recursion;
  mxslt_dir_config_t * config;
  const char * type;
  const char * forcestyle, * defaultstyle;
  int status;

      /* If this is not something we want to handle, 
       * return and give up */
  if(r->header_only) 
    return DECLINED;

      /* If the method is not get, discard the request */
  if(r->method_number != M_GET)
    return DECLINED;

    /* Check if we already opened this file */
  status=mxslt_url_recurse_allowed(recursion, r->filename);
  if(status != MXSLT_OK) {
    if(status == MXSLT_MAX_LEVEL)
      ap_log_rerror(APLOG_MARK, APLOG_ERR | APLOG_NOERRNO, r, 
            MXSLT_NAME ": maximum recursion level of " STR(MXSLT_MAX_RECURSION_LEVEL) " reached, skipping %s", r->uri);
    else
      ap_log_rerror(APLOG_MARK, APLOG_ERR | APLOG_NOERRNO, r, 
            MXSLT_NAME ": loop detected while processing %s", r->uri);
    mxslt_url_recurse_dump(recursion, &mxslt_ap1_outputter, r);

    return HTTP_RECURSION_ERROR;
  }

  mxslt_url_recurse_push(recursion, r->filename);

    /* Get mime type of the stuff we are going to process */
  if(r->content_type) 
    type=r->content_type;
  else
    type=r->handler;

    /* Ok, get configuration and stuff we have stored in the request header */
  config=(mxslt_dir_config_t *)ap_get_module_config(r->per_dir_config, &mxslt_module);
  if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_VERBOSE1 | MXSLT_DBG_DEBUG)) 
    mxslt_ap1_dump_config(config, MXSLT_DBG_SAPI | MXSLT_DBG_VERBOSE1 | MXSLT_DBG_DEBUG, r);
  
    /* Get default style for given type */
  forcestyle=ap_table_get(config->mime_styles, type); 
  defaultstyle=ap_table_get(config->default_styles, type); 

  if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI)) {
    mxslt_ap1_debug(r->server, MXSLT_DBG_SAPI, 
		    config->dbglevel, "statically parsing %s, type: %s, handler: %s, content-type: %s\n",
		    r->filename, type, r->handler, r->content_type);
  }

  status=mxslt_ap1_file_parse(config, r, r->filename, defaultstyle, forcestyle);

  mxslt_url_recurse_pop(recursion, 1);

  return status;
}

#if 0
  /* If it is a static file, no problem... but, if it is dynamic, how do we handle it?
  ap_update_mtime(r, r->finfo.st_mtime);
  ap_table_setn(r->headers_out, "Last-Modified", ap_gm_timestr_822(r->pool, mod_time));
  ap_meets_conditions(r);
  */
#endif 

static inline void mxslt_ap1_import_headers(request_rec * where, request_rec * what) {
  array_header * harr = ap_table_elts(what->headers_out);
  table_entry * entry = (table_entry *)harr->elts;
  int i;

  ap_clear_table(where->headers_out);

  for(i=0; i < harr->nelts; i++)
    ap_table_add(where->headers_out, entry[i].key, entry[i].val);

  return;
}

static inline int mxslt_ap1_passover(request_rec * r, char * filename) {
  FILE * fin;

    /* If we return DECLINED here, the original file is
     * sent back to the client, not the parsed one! */
  fin=fopen(filename, "r");
  if(fin == NULL) {
    ap_log_rerror(APLOG_MARK, APLOG_ERR | APLOG_NOERRNO, r, 
      	    MXSLT_NAME ": couldn't re-open file `%s`", filename);
    return HTTP_INTERNAL_SERVER_ERROR;
  }
  ap_send_http_header(r);
  ap_send_fd(fin, r);
  ap_rflush(r);
  fclose(fin);

  return OK;
}

static int mxslt_ap1_dyn_handler(request_rec * r) {
  mxslt_recursion_t * recursion=&mxslt_global_recursion;
  mxslt_dir_config_t * config;
  request_rec * subr;
  char * tmpfile;
  const char * defaultstyle, * type, * forcestyle;
  int status;

    /* Ok, get configuration and stuff we have stored in the request header */
  config=(mxslt_dir_config_t *)ap_get_module_config(r->per_dir_config, &mxslt_module);
  type=ap_table_get(r->notes, MXSLT_NOTE_HANDLER);

  if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_VERBOSE1 | MXSLT_DBG_DEBUG)) 
    mxslt_ap1_dump_config(config, MXSLT_DBG_SAPI | MXSLT_DBG_VERBOSE1 | MXSLT_DBG_DEBUG, r);

    /* Check that we really got here because of the 
     * fixup handler */
  if(!type) {
    ap_log_rerror(APLOG_MARK, APLOG_ERR | APLOG_NOERRNO, r, 
		  MXSLT_NAME ": what made you think you could use " MXSLT_FRC_HANDLER "?");
    return HTTP_INTERNAL_SERVER_ERROR;
  }

    /* Get default style for given type */
  forcestyle=ap_table_get(config->mime_styles, type); 
  defaultstyle=ap_table_get(config->default_styles, type); 

    /* Make sub request */
  mxslt_ap1_fixup_disable();
  subr=mxslt_ap1_sub_request_pass(config, r, r->uri, &tmpfile, &status);
  mxslt_ap1_fixup_enable();
  if(!subr) {
      /* Otherwise, apache believes it had an error
       * while fetching error document */
    r->status=HTTP_OK;

    return status;
  }

  if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE0)) {
    mxslt_ap1_debug(r->server, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE0, 
		    config->dbglevel, "uri: %s, subrequest handler: %s, content-type: %s\n",
		    mxslt_debug_string(r->uri), subr->handler, subr->content_type);
  }

    /* Get content type of sub request */
  if(subr->handler) 
    type=subr->handler;
  else
    type=subr->content_type;

    /* Copy subrequest headers in new headers */
  mxslt_ap1_import_headers(r, subr);

    /* Check content type of subrequest */
  if(!type || subr->status != HTTP_OK || (
     (strncmp(type, "text/xml", mxslt_sizeof_str("text/xml")) || 
         (*(type+mxslt_sizeof_str("text/xml")) != ';' && *(type+mxslt_sizeof_str("text/xml")) != '\0')) && 
     (strncmp(type, "application/xml", mxslt_sizeof_str("application/xml")) || 
         (*(type+mxslt_sizeof_str("application/xml")) != ';' && *(type+mxslt_sizeof_str("application/xml")) != '\0')))) {

    if(subr->status == HTTP_OK) {
      if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI)) {
        mxslt_ap1_debug(r->server, MXSLT_DBG_SAPI, config->dbglevel, 
		      "uri: %s -- not parsed! Content-Type is not text/xml (%s)!\n", mxslt_debug_string(r->uri), type);
      }
      status=DONE;
    } else {
      status=subr->status;
      if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI)) {
        mxslt_ap1_debug(r->server, MXSLT_DBG_SAPI,
		    config->dbglevel, "uri: %s -- not parsed! subrequest status %d != 200, returning raw!\n", mxslt_debug_string(r->uri), status);
      }
    }

    r->content_type=subr->content_type;
    mxslt_ap1_passover(r, tmpfile);
    ap_destroy_sub_req(subr);

    return OK;
  }

  ap_destroy_sub_req(subr);

    /* Request is not pushed from fixup handler because it
     * is sometimes called just to check the kind of output
     * that would be generated by the handler (directory index) */
  mxslt_url_recurse_push(recursion, r->uri);

    /* Parse document */ 
  status=mxslt_ap1_file_parse(config, r, tmpfile, defaultstyle, forcestyle);
  mxslt_url_recurse_pop(recursion, 1);

  return status;
}

static int mxslt_ap1_frc_handler(request_rec * r) {
  mxslt_recursion_t * recursion=&mxslt_global_recursion;
  mxslt_dir_config_t * config;
  request_rec * subr;
  char * tmpfile;
  const char * forcestyle, * type, * defaultstyle;
  int status;

    /* Ok, get configuration and stuff we have stored in the request header */
  config=(mxslt_dir_config_t *)ap_get_module_config(r->per_dir_config, &mxslt_module);
  type=(char *)ap_table_get(r->notes, MXSLT_NOTE_HANDLER);

  if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_VERBOSE1 | MXSLT_DBG_DEBUG)) 
    mxslt_ap1_dump_config(config, MXSLT_DBG_SAPI | MXSLT_DBG_VERBOSE1 | MXSLT_DBG_DEBUG, r);

    /* If type was not specifyed, the user used MXSLT_FRC_HANDLER in 
     * the configuration file! */
  if(!type) {
    ap_log_rerror(APLOG_MARK, APLOG_ERR | APLOG_NOERRNO, r, 
		  MXSLT_NAME ": what made you think you could use " MXSLT_FRC_HANDLER "?");
    return HTTP_INTERNAL_SERVER_ERROR;
  }

    /* Get default style for given type */
  forcestyle=(char *)ap_table_get(config->mime_styles, type); 
  defaultstyle=(char *)ap_table_get(config->default_styles, type); 

    /* This subrequest is not protected against loops because
     * it is used just to load the current document being parsed,
     * and afaik cannot lead to loops until this document is parsed */

    /* Make sub request */
  mxslt_ap1_fixup_disable();
  subr=mxslt_ap1_sub_request_pass(config, r, r->uri, &tmpfile, &status);
  mxslt_ap1_fixup_enable();

  if(!subr) {
      /* See other handlers */
    r->status=HTTP_OK;
    return status;
  }

    /* Check request status */
  if(subr->status != HTTP_OK) {
    if(mxslt_debug_compare(config->dbglevel, MXSLT_DBG_SAPI)) {
      mxslt_ap1_debug(r->server, MXSLT_DBG_SAPI,
	    config->dbglevel, "uri: %s -- not parsed! subrequest status %d != 200, returning raw!\n", mxslt_debug_string(r->uri), status);
    }

    r->content_type=subr->content_type;
    status=subr->status;
    mxslt_ap1_passover(r, tmpfile);
    ap_destroy_sub_req(subr);

    return status;
  }

    /* Import subrequest headers */
  mxslt_ap1_import_headers(r, subr);
  ap_destroy_sub_req(subr);

    /* Request is not pushed from fixup handler because fixup
     * is sometimes called just to check the kind of output
     * that would be generated by the handler (directory index) */
  mxslt_url_recurse_push(recursion, r->uri);

    /* Parse document */ 
  status=mxslt_ap1_file_parse(config, r, tmpfile, defaultstyle, forcestyle);
  mxslt_url_recurse_pop(recursion, 1);

  return status;
}

static const handler_rec mxslt_ap1_handlers[] = {
  { MXSLT_STD_HANDLER, mxslt_ap1_std_handler },
  { MXSLT_DYN_HANDLER, mxslt_ap1_dyn_handler },
  { MXSLT_FRC_HANDLER, mxslt_ap1_frc_handler },
  { NULL, NULL }
};

static const char * mxslt_ap1_default_mime_style(cmd_parms * cmd, void * pcfg, const char * mime, const char * style) {
  mxslt_dir_config_t * conf = (mxslt_dir_config_t *) pcfg;

  ap_table_set(conf->default_styles, mime, style);

  return NULL;
}

static const char * mxslt_ap1_set_mime_style(cmd_parms * cmd, void * pcfg, const char * mime, const char * style) {
  mxslt_dir_config_t * conf = (mxslt_dir_config_t *) pcfg;

  ap_table_set(conf->mime_styles, mime, style);

  return NULL;
}

static const char * mxslt_ap1_param(cmd_parms * cmd, void * pcfg, const char * param, const char * value) {
  mxslt_dir_config_t * conf = (mxslt_dir_config_t *) pcfg;

  if(!param || !value)
    return "both param and value must be specifyed";

  if(mxslt_debug_compare(conf->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_CONFIG | MXSLT_DBG_VARIABLES)) {
    mxslt_ap1_debug(cmd->server, MXSLT_DBG_SAPI | MXSLT_DBG_CONFIG | MXSLT_DBG_VARIABLES,
	    conf->dbglevel, "config - setting to value: %s = \"%s\"\n", mxslt_debug_string(param), mxslt_debug_string(value));
  }
  ap_table_set(conf->params, param, value);

  return NULL;
}

static const char * mxslt_ap1_nodefault_mime_style(cmd_parms * cmd, void * pcfg, const char * mime) {
  mxslt_dir_config_t * conf = (mxslt_dir_config_t *) pcfg;

  ap_table_unset(conf->default_styles, mime);

  return NULL;
}

static const char * mxslt_ap1_set_debug(cmd_parms * cmd, void * pcfg, const char * l) {
  mxslt_dir_config_t * conf = (mxslt_dir_config_t *) pcfg;

  unsigned char * ch=(unsigned char *)l;
  int mask;

    /* If this is made only of digits, consider this the 
     * debugging mask directly, as with XSLTDebugMask */
  if(isdigit(*ch) || *ch == '-') {
    for(ch++; isdigit(*ch); ch++)
      ;

    for(; isspace(*ch); ch++)
      ;

    if(!*ch) {
      mask=atoi(l);
      conf->dbglevel=mask;
      return NULL;
    }
  }

  mask=mxslt_debug_parse((char *)l);
  if(mask & MXSLT_DBG_ERROR)
    return "invalid string for XSLTDebug, or unknown debug option";

  conf->dbglevel=mask;
  return NULL;
}

static const char * mxslt_ap1_set_int_slot(cmd_parms * cmd, void * pcfg, const char * value) {
  int offset = (int)(cmd->info);
  int number;
  
  assert(value);

    /* Ok, verify the string is not empty */ 
  if(!*value)
    return "invalid number supplied .. 0?";

    /* Try to convert string to number */ 
  if(sscanf(value, "%i", &number) != 1)
    return "invalid number supplied .. invalid character found (must be [0-9]+)";

    /* Ok, store number in original struct */
  *(unsigned int *)(pcfg + offset)=(unsigned int)number;
  return NULL;
}

static const char * mxslt_ap1_unset_mime_style(cmd_parms * cmd, void * pcfg, const char * mime) {
  mxslt_dir_config_t * conf = (mxslt_dir_config_t *) pcfg;

  ap_table_unset(conf->mime_styles, mime);

  return NULL;
}

static const char * mxslt_ap1_add_filter_dynamic(cmd_parms * cmd, void * pcfg, const char * mime) {
  mxslt_dir_config_t * conf = (mxslt_dir_config_t *) pcfg;

  ap_table_set(conf->filter_dynamic, mime, "");

  return NULL;
}

static const char * mxslt_ap1_del_filter_dynamic(cmd_parms * cmd, void * pcfg, const char * mime) {
  mxslt_dir_config_t * conf = (mxslt_dir_config_t *) pcfg;

  ap_table_unset(conf->filter_dynamic, mime);

  return NULL;
}

static const char * mxslt_ap1_add_filter_force(cmd_parms * cmd, void * pcfg, const char * mime) {
  mxslt_dir_config_t * conf = (mxslt_dir_config_t *) pcfg;

  ap_table_set(conf->filter_force, mime, "");

  return NULL;
}

static const char * mxslt_ap1_add_rule(cmd_parms * cmd, void * pcfg, char * stylesheet, char * condition) {
  mxslt_dir_config_t * conf = (mxslt_dir_config_t *) pcfg;
  int len;

  if(mxslt_debug_compare(conf->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_CONFIG)) {
    mxslt_ap1_debug(cmd->server, MXSLT_DBG_SAPI | MXSLT_DBG_CONFIG,
	    conf->dbglevel, "config - stylesheet %s will be used when %s\n", mxslt_debug_string(stylesheet), mxslt_debug_string(condition));
  }
  if(*stylesheet == '"' && *(stylesheet+(len=strlen(stylesheet))-1) == '"') {
    *(stylesheet+len-1)='\0';
    stylesheet+=1;
  }

  if(*condition == '"' && *(condition+(len=strlen(condition))-1) == '"') {
    *(condition+len-1)='\0';
    condition+=1;
  }

  ap_table_set(conf->rules, stylesheet, condition);

  return NULL;
}

static const char * mxslt_ap1_del_rule(cmd_parms * cmd, void * pcfg, const char * stylesheet) {
  mxslt_dir_config_t * conf = (mxslt_dir_config_t *) pcfg;

  ap_table_unset(conf->rules, stylesheet);

  return NULL;
}


static const char * mxslt_ap1_del_filter_force(cmd_parms * cmd, void * pcfg, const char * mime) {
  mxslt_dir_config_t * conf = (mxslt_dir_config_t *) pcfg;

  ap_table_unset(conf->filter_force, mime);

  return NULL;
}


static const char * mxslt_ap1_set_tmp_dir(cmd_parms * cmd, void * pcfg, const char * dir) {
  mxslt_dir_config_t * conf = (mxslt_dir_config_t *) pcfg;
  struct stat fstats;

    /* Check if parameter is valid */
    /* XXX: does this work under windows? Does anybody care? */
  if(!dir || !*dir || *dir != '/')
    return ap_pstrcat(cmd->pool, cmd->cmd->name, " directory is not absolute: ", dir, NULL);

    /* Check if directory exists */
  if(stat(dir, &fstats))
    return ap_pstrcat(cmd->pool, cmd->cmd->name, " couldn't stat directory: ", dir, NULL);

  if(!S_ISDIR(fstats.st_mode))
    return ap_pstrcat(cmd->pool, cmd->cmd->name, " are you joking? This is not a directory: ", dir, NULL);

    /* Strip any eventual slash at the end of the path name (avoid //) */
  if(mxslt_debug_compare(conf->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_CONFIG)) {
    mxslt_ap1_debug(cmd->server, MXSLT_DBG_SAPI | MXSLT_DBG_CONFIG,
	    conf->dbglevel, "config - temporary directory set to: %s\n", dir);
  }

  conf->tmpdir=ap_pstrdup(cmd->pool, dir);
  if(conf->tmpdir[strlen(conf->tmpdir)-1] == '/')
    conf->tmpdir[strlen(conf->tmpdir)-1]='\0';

  return NULL;
}

static void * mxslt_ap1_create_dir_config(pool *p, char * dir) {
  mxslt_dir_config_t * nconf;

  nconf=(mxslt_dir_config_t *)ap_palloc(p, sizeof(mxslt_dir_config_t));
  if(!nconf)
    return NULL;

  nconf->params=ap_make_table(p, 0);
  nconf->mime_styles=ap_make_table(p, 0);
  nconf->default_styles=ap_make_table(p, 0);
  nconf->filter_dynamic=ap_make_table(p, 0);
  nconf->filter_force=ap_make_table(p, 0);
  nconf->rules=ap_make_table(p, 0);
  nconf->state=MXSLT_STATE_UNSET;
  nconf->tmpdir=NULL;
  nconf->unlink=1;
  nconf->signature=0;

  nconf->dbglevel=mxslt_global_dbglevel;

  if(mxslt_debug_compare(nconf->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE1)) {
    mxslt_ap1_debug(NULL, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE1,
	    nconf->dbglevel, "created config for %s, address: %08x\n", dir, (int)nconf);
  }

  return nconf;
}

static void * mxslt_ap1_merge_dir_config(pool * p, void * basev, void * overridev) {
  mxslt_dir_config_t * base = (mxslt_dir_config_t *)basev;
  mxslt_dir_config_t * override = (mxslt_dir_config_t *)overridev;
  mxslt_dir_config_t * nconf;

  if(mxslt_debug_compare(base->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE1)) {
    mxslt_ap1_debug(NULL, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE1,
	    base->dbglevel, "merging config %08x with %08x\n", (int)basev, (int)overridev);
  }

    /* XXX Note: ap_overlay_tables just appends one table at the end of
     *   the second table. Using the apache API, this is ok anyway because
     *   a fetch returns the _first_ matching type, and making values uniq
     *   here would require a quadratic algorithm (foreach key, foreach 
     *   inserted key). However, the params table is parsed with old
     *   values first and new value then. This is because params are then
     *   walked and inserted in a hashing table, where the last values
     *   overwrite first values... yes, this is sick. */
  nconf=(mxslt_dir_config_t *)ap_palloc(p, sizeof(mxslt_dir_config_t));
  nconf->mime_styles=ap_overlay_tables(p, override->mime_styles, base->mime_styles);
  nconf->default_styles=ap_overlay_tables(p, override->default_styles, base->default_styles);
  nconf->params=ap_overlay_tables(p, base->params, override->params);
  nconf->filter_dynamic=ap_overlay_tables(p, override->filter_dynamic, base->filter_dynamic);
  nconf->filter_force=ap_overlay_tables(p, override->filter_force, base->filter_force);
  nconf->rules=ap_overlay_tables(p, override->rules, base->rules);
  nconf->state=(override->state == MXSLT_STATE_UNSET) ? base->state : override->state;
  nconf->tmpdir=override->tmpdir ? override->tmpdir : base->tmpdir;
  nconf->unlink=override->unlink;
  nconf->signature=0;
  nconf->dbglevel=base->dbglevel|override->dbglevel;

  if(mxslt_debug_compare(nconf->dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE2)) {
    mxslt_ap1_debug(NULL, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE2,
	    nconf->dbglevel, "created config: %08x\n", (int)nconf);
  }

  return nconf;
}

#ifdef HAVE_NETWORK_IOCTLS
# include <net/if.h>

static int mxslt_ap1_setanyiplist(server_rec * s, int level, mxslt_table_t * ips_table) {
  struct ifconf ifc;
  int fd, status;
  struct ifreq * ifrp, ifr;

    /* Zero up structures */
  memset(&ifc, 0, sizeof(struct ifconf));
  memset(&ifr, 0, sizeof(struct ifreq));

    /* open socket */
  fd=socket(AF_INET, SOCK_DGRAM, 0);
  if(fd < 0)
    return MXSLT_FAILURE;

    /* Perform ioctl */
  status=ioctl(fd, SIOCGIFCONF, (char *)&ifc);
  if(status < 0)
    return MXSLT_FAILURE;

    /* Allocate buffer */
  ifc.ifc_buf=(char *)xmalloc(ifc.ifc_len+1);

    /* Perform ioctl */
  status=ioctl(fd, SIOCGIFCONF, (char *)&ifc);
  if(status < 0) {
    xfree(ifc.ifc_buf);
    return MXSLT_FAILURE;
  }

    /* For each interface we found */
  for(ifrp=(struct ifreq *)(ifc.ifc_buf); (char *)ifrp < (((char *)ifc.ifc_buf)+ifc.ifc_len); ifrp++) {
    strcpy(ifr.ifr_name, ifrp->ifr_name);
    ifr.ifr_addr.sa_family=AF_INET;
    status=ioctl(fd, SIOCGIFADDR, &ifr);
    if(status < 0) {
      xfree(ifc.ifc_buf);
      return MXSLT_FAILURE;
    }

    if(mxslt_debug_compare(level, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG)) {
      mxslt_ap1_debug(s, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG, level, 
	"adding local address: %s\n", inet_ntoa((struct in_addr)(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr)));
    }

      /* XXX: void * is supposed to be as big as an int, and I assume an int is at least 32 bits here */
    mxslt_table_insert(ips_table, NULL, (void *)((((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr).s_addr), NULL);
  }

  xfree(ifc.ifc_buf);

  return MXSLT_OK;
}
#endif

  /* Calculate an ip address hash */
static mxslt_table_size_t mxslt_ap1_table_hash(const char * tohash, const mxslt_table_size_t wrap) {
  return (((int)(tohash))%wrap);
}

  /* Compare two ip addresses */
static int mxslt_ap1_table_cmp(void * key1, void * key2) {
  return (int)key1 - (int)key2;
}

static void mxslt_ap1_init(server_rec * s, pool * p) {
  listen_rec * listen;
  static int inaddr_any;
  int status;

    /* Add version name to apache id */
  ap_add_version_component(MXSLT_NAME "/" MXSLT_VERSION);

    /* Initialize modxslt-library */
  mxslt_xml_load();

    /* Get debugging level from environment */
  mxslt_global_dbglevel=mxslt_debug_firstlevel();
  if(mxslt_ap1_fixup_isdisabled()) {
    ap_log_error(MXSLT_NAME, 0, APLOG_WARNING | APLOG_NOERRNO, s, 
		   "fixup disabled during init (%d), enabling!", mxslt_ap1_fixup_isdisabled());
    mxslt_ap1_fixup_enable();
  }

    /* Precalculate value for inaddr_any */
  inaddr_any=htonl(INADDR_ANY);

    /* Setup ip hashing table */
  mxslt_table_set_hash(&mxslt_global_ips, mxslt_ap1_table_hash);
  mxslt_table_set_cmp(&mxslt_global_ips, mxslt_ap1_table_cmp);

    /* Walk all listeners */
  if(mxslt_debug_compare(mxslt_global_dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE1)) {
    mxslt_ap1_debug(s, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE1, mxslt_global_dbglevel, "ap_listeners: %08x", (unsigned int)ap_listeners);
  }

  listen=ap_listeners;
  do {
    if(listen->local_addr.sin_addr.s_addr == inaddr_any) {
      if(mxslt_debug_compare(mxslt_global_dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE1)) {
        mxslt_ap1_debug(s, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE1, mxslt_global_dbglevel, 
        	"adding address (ANY): %s - %08x, %08x\n", inet_ntoa((listen->local_addr).sin_addr), (int)listen, (int)listen->next);
      }

#ifdef HAVE_NETWORK_IOCTLS
      status=mxslt_ap1_setanyiplist(s, mxslt_global_dbglevel, &mxslt_global_ips); 
      if(status != MXSLT_OK) {
	  /* Warn the user, but try to go on anyway */
	ap_log_error(MXSLT_NAME, 0, APLOG_WARNING, s, 
		     "failed to fetch the ips corresponding to INADDR_ANY - please read the README!");
      }
#else 
      ap_log_error(MXSLT_NAME, 0, APLOG_WARNING | APLOG_NOERRNO, s, 
		   "INADDR_ANY is being used without ioctl support - read mod-xslt README!");
#endif
    } else {

      if(mxslt_debug_compare(mxslt_global_dbglevel, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE1)) {
        mxslt_ap1_debug(s, MXSLT_DBG_SAPI | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE1, mxslt_global_dbglevel, 
       		"adding address: %s - %08x\n", inet_ntoa((listen->local_addr).sin_addr), (int)listen);
      }

        /* XXX: void * is supposed to be as big as an int, and I assume an int is at least 32 bits here */
      mxslt_table_insert(&mxslt_global_ips, NULL, (void *)((listen->local_addr).sin_addr.s_addr), NULL);
    }

    listen=listen->next;
  } while(listen && listen != ap_listeners);
}

static void mxslt_ap1_child_init(server_rec * s, pool *p) {
  ap_log_error(MXSLT_NAME, 0, APLOG_DEBUG | APLOG_NOERRNO, s, "starting child");
  mxslt_xml_init(&ap1_mxslt_global_state, mxslt_ap1_http_handle, mxslt_ap1_http_open, 
		 mxslt_ap1_http_close, mxslt_ap1_http_read);
}

static void mxslt_ap1_child_exit(server_rec * s, pool *p) {
  ap_log_error(MXSLT_NAME, 0, APLOG_DEBUG | APLOG_NOERRNO, s, "stopping child");
  mxslt_xml_done(&ap1_mxslt_global_state);
}

static const command_rec mxslt_ap1_cmds[] = {
  {"XSLTEngine", ap_set_flag_slot, (void *)XtOffsetOf(mxslt_dir_config_t, state), OR_OPTIONS, TAKE1, 
    	"XSLTEngine <on|off> - Set this to On to enable xslt parsing" },
  {"XSLTDisableSignature", ap_set_flag_slot, (void *)XtOffsetOf(mxslt_dir_config_t, signature), OR_OPTIONS, TAKE1, 
    	"XSLTDisableSignature <on|off> - Ignored, for backward compatibility only"},
  {"XSLTTmpDir", mxslt_ap1_set_tmp_dir, NULL, OR_OPTIONS, TAKE1,
    	"XSLTTmpDir <Directory> - Takes the name of a directory we can use to store temporary files"},
  {"XSLTDebug", mxslt_ap1_set_debug, NULL, OR_OPTIONS, RAW_ARGS,
        "XSLTDebug <level> - Set debugging to the specified level"},
  {"XSLTDebugMask", mxslt_ap1_set_int_slot, (void *)XtOffsetOf(mxslt_dir_config_t, dbglevel), OR_OPTIONS, TAKE1, 
    	"XSLTDebugMask <number> - Set the debugging output mask"},

  {"XSLTAddFilter", mxslt_ap1_add_filter_dynamic, NULL, OR_OPTIONS, TAKE1, 
    	"XSLTAddFilter <MimeType> - Takes the mime type/handler of the files you want to filter with mod_xslt" },
  {"XSLTDelFilter", mxslt_ap1_del_filter_dynamic, NULL, OR_OPTIONS, TAKE1, 
    	"XSLTDelFilter <MimeType> - Takes the mime type/handler of the files you don't want to filter anymore with mod_xslt" },
  {"XSLTAddForce", mxslt_ap1_add_filter_force, NULL, OR_OPTIONS, TAKE1, 
    	"XSLTAddFilter <MimeType> - Takes the mime type/handler of the files you want to force filter with mod_xslt" },
  {"XSLTDelForce", mxslt_ap1_del_filter_force, NULL, OR_OPTIONS, TAKE1, 
    	"XSLTDelFilter <MimeType> - Takes the mime type/handler of the files you don't "
	"want to force filter anymore with mod_xslt" },

  {"XSLTParam", mxslt_ap1_param, NULL, OR_OPTIONS, TAKE2, 
    	"XSLTParam <Param> <Value> - Pass over this parameter to the xml parser"},

  {"XSLTAddRule", mxslt_ap1_add_rule, NULL, OR_OPTIONS, TAKE2,
        "XSLTAddRule <Stylesheet> <Rule> - Uses a given stylesheet for xml pages matching rule"},
  {"XSLTDelRule", mxslt_ap1_del_rule, NULL, OR_OPTIONS, TAKE1,
        "XSLTDelRule <Stylesheet> - Removes a previously set rule"},

  {"XSLTDefaultStylesheet", mxslt_ap1_default_mime_style, NULL, OR_OPTIONS, TAKE2, 
        "XSLTDefaultStylesheet <MimeType> <Stylesheet> - Use the provided stylesheet if no other stylesheet can be used"}, 
  {"XSLTNoDefaultStylesheet", mxslt_ap1_nodefault_mime_style, NULL, OR_OPTIONS, TAKE1, 
        "XSLTDefaultStylesheet <MimeType> - Use the provided stylesheet if no other stylesheet can be used"}, 

  {"XSLTSetStylesheet", mxslt_ap1_set_mime_style, NULL, OR_OPTIONS, TAKE2, 
    	"XSLTSetStylesheet <MimeType> <Stylesheet> - Force the usage of a particular stylesheet for a particular mime type"},
  {"XSLTUnSetStylesheet", mxslt_ap1_unset_mime_style, NULL, OR_OPTIONS, TAKE1, 
    	"XSLTUnSetStylesheet <MimeType> - Don't force the usage of a particular stylesheet anymore"},

  {"XSLTUnlink", ap_set_flag_slot, (void *)XtOffsetOf(mxslt_dir_config_t, unlink), OR_OPTIONS, TAKE1,
    	"XSLTUnlink <on|off> - Set this to off to disable deletion of temporary files"},
  {NULL}
};

module MODULE_VAR_EXPORT mxslt_module = {
  STANDARD_MODULE_STUFF,
  mxslt_ap1_init,     	     /* module initializer                  */
  mxslt_ap1_create_dir_config,    /* create per-dir    config structures */
  mxslt_ap1_merge_dir_config,     /* merge  per-dir    config structures */
  NULL,                      /* create per-server config structures */
  NULL,                      /* merge  per-server config structures */
  mxslt_ap1_cmds,                 /* table of config file commands       */
  mxslt_ap1_handlers,             /* [#8] MIME-typed-dispatched handlers */
  NULL,                      /* [#1] URI to filename translation    */
  NULL,                      /* [#4] validate user id from request  */
  NULL,                      /* [#5] check if the user is ok _here_ */
  NULL,                      /* [#3] check access by host address   */
  NULL,                      /* [#6] determine MIME type            */
  mxslt_ap1_fixup,                /* [#7] pre-run fixups                 */
  NULL,                      /* [#9] log a transaction              */
  NULL,                      /* [#2] header parser                  */
  mxslt_ap1_child_init,           /* child_init                          */
  mxslt_ap1_child_exit,           /* child_exit                          */
  NULL                       /* [#0] post read-request              */
};

