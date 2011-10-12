/*
 *  mod-xslt -- Copyright (C) 2002-2008 
 *   		 Carlo Contavalli 
 *   		 <ccontavalli at inscatolati.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2, as 
 *  published by the Free Software Foundation.
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

  /* TSD Being used by mod-xslt */
apr_threadkey_t * mxslt_ap2_global_dbgbuffer;

apr_threadkey_t * mxslt_ap2_global_state;
apr_threadkey_t * mxslt_ap2_global_ectxt;
apr_threadkey_t * mxslt_ap2_global_recursion;
unsigned int mxslt_ap2_global_dbglevel=0;

  /* Declare apache 2 module */
module AP_MODULE_DECLARE_DATA mxslt_module;

  /* XXX Assumes eos can only be the last element of a brigade! 
   * This is not that wrong if we think that after eos no more data is read
   * and no more data can be stored in the brigade. However, APR_BRIGADE_LAST
   * is implemented by taking the previous brigade of the first element. What
   * happens if the element I've been given is not the first one? */
#define mxslt_ap2_brigade_isfinal(brigade) APR_BUCKET_IS_EOS(APR_BRIGADE_LAST(brigade))

static void mxslt_ectxt_destroy(void * state) {
  /* Empty, nothing to be done */
}

static void mxslt_recursion_destroy(void * state) {
  mxslt_url_recurse_pop((mxslt_recursion_t *)state, 
			 mxslt_url_recurse_level((mxslt_recursion_t *)state));
  xfree(state);
}

static void mxslt_dbgbuffer_destroy(void * state) {
  mxslt_ap2_debug_buffer_t * buff=(mxslt_ap2_debug_buffer_t *)state;

  xfree(buff->buffer);
  xfree(buff);
}

static void mxslt_ap2_outputter(void * data, char * msg, ...) {
  request_rec * r=(request_rec *)data;
  va_list args;
  char * str;

  va_start(args, msg);
  str=apr_pvsprintf(r->pool, msg, args);
  va_end(args);

  ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, MXSLT_NAME ": %s", str);
}

void mxslt_ap2_brigade_dump(apr_bucket_brigade * brigade) {
  apr_bucket * bucket;

  for (bucket = APR_BRIGADE_FIRST(brigade); bucket !=APR_BRIGADE_SENTINEL(brigade);
       bucket = APR_BUCKET_NEXT(bucket)) {
    printf("bucket: %08x, type: %08x, length: %d, start: %d, data: %08x\n", (int)bucket,
	   (int)bucket->type, (int)bucket->length, (int)bucket->start, (int)bucket->data);
  }
}

static void mxslt_ap2_child_init(apr_pool_t *p, server_rec *s) {
  mxslt_recursion_t * recursion;
  mxslt_shoot_t * state;

  ap_log_error(APLOG_MARK, APLOG_STARTUP | APLOG_DEBUG, 0, NULL,
	 	 MXSLT_NAME ": starting child");

    /* Ok, seems like server pool is really for the whole process,
     * not just the thread... so, use our own memory allocation routines,
     * just to make sure they are correctly freed when the thread dies */
  state=(mxslt_shoot_t *)xmalloc(sizeof(mxslt_shoot_t));
  recursion=(mxslt_recursion_t *)xmalloc(sizeof(mxslt_recursion_t));

  mxslt_state_init(state);
  mxslt_recursion_init(recursion);

  mxslt_ap2_state_set(state);
  mxslt_ap2_recursion_set(recursion);

  mxslt_xml_init(state, NULL, NULL);

  return;
}

static apr_status_t mxslt_ap2_in_filter(ap_filter_t *f, apr_bucket_brigade *brigade) {
  f->r->no_local_copy=1;
  return APR_SUCCESS;
}

static apr_status_t mxslt_ap2_out_filter(ap_filter_t *f, apr_bucket_brigade *brigade) {
  mxslt_dir_config_t * conf=mxslt_ap2_get_config(f->r->per_dir_config, &mxslt_module);
  apr_bucket_brigade * data;
  mxslt_recursion_t * recursion;
  apr_status_t status;
  const char * forcestyle, * defaultstyle;
  void * get;

    /* ignore headers only requests, redirects (!200 status) or requests when
     * the environment has "no-xslt" set */
  if(f->r->header_only || f->r->status != HTTP_OK ||
     apr_table_get(f->r->subprocess_env, "no-xslt"))
    return ap_pass_brigade(f->next, brigade);

    /* drop the previously calculated length, which is no longer correct */
  apr_table_unset(f->r->headers_out, "Content-Length");

  data=(apr_bucket_brigade *)f->ctx;
  ap_save_brigade(f, &data, &brigade, f->r->pool);
  f->ctx=data;

  if(!mxslt_ap2_brigade_isfinal(data)) 
    return APR_SUCCESS;

    /* Get any mime style that may have been specifyed with httpd.conf
     * parameters */
  forcestyle=(char *)apr_table_get(conf->mime_styles, f->r->content_type);
  defaultstyle=(char *)apr_table_get(conf->default_styles, f->r->content_type);

    /* Ok, prepare to push request uri, so we avoid dangerous loops */
  status=mxslt_ap2_recursion_get(&get);
  if(status != APR_SUCCESS) {
    ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, f->r, MXSLT_NAME ": failed to fetch thread recursion data. Killing request.");
    status=HTTP_INTERNAL_SERVER_ERROR;
    goto error;
  }

    /* Ok, maybe this is the worker model... which calls child init only once
     * per child, but where every child has many threads ...  so, this is the
     * first time we got called for a thread... prepare global variables ... */
  if(!get) {
    mxslt_ap2_child_init(f->r->server->process->pool, f->r->server);

    status=mxslt_ap2_recursion_get(&get);
    if(status != APR_SUCCESS) {
      ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, f->r, MXSLT_NAME ": failed to fetch thread recursion data. Killing request.");
      status=HTTP_INTERNAL_SERVER_ERROR;
      goto error;
    }
  }

    /* Remember recursion pointer */
  recursion=get;

    /* Verify we are allowed to walk this uri */
  status=mxslt_url_recurse_allowed(recursion, f->r->uri);
  if(status != MXSLT_OK) {
    if(status == MXSLT_MAX_LEVEL) 
      ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, f->r, 
  	   MXSLT_NAME ": maximum recursion level of " STR(MXSLT_MAX_RECURSION_LEVEL) " reached, skipping %s", f->r->uri);
    else
      ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, f->r, 
	   MXSLT_NAME ": loop detected while processing %s", f->r->uri);
    mxslt_url_recurse_dump(recursion, &mxslt_ap2_outputter, f->r);
      
    status=HTTP_INTERNAL_SERVER_ERROR;
    goto error;
  }

    /* Parse data, avoiding loops */
  mxslt_url_recurse_push(recursion, f->r->uri);
  status=mxslt_ap2_file_parse(f, data, defaultstyle, forcestyle, conf);
  mxslt_url_recurse_pop(recursion, 1);

  if(status == HTTP_OK)
    return ap_pass_brigade(f->next, f->ctx);

error:
  return status;
}

static void mxslt_ap2_child_exit(void * data) {
  int status;

  status=mxslt_ap2_state_get(&data);
  if(status == APR_SUCCESS) {
    mxslt_xml_done(data);
    ap_log_error(APLOG_MARK, APLOG_STARTUP | APLOG_DEBUG, 0, NULL,
	 	 MXSLT_NAME ": stopping child -- memory cleanup succeded");
  } else {
    ap_log_error(APLOG_MARK, APLOG_STARTUP | APLOG_DEBUG, 0, NULL,
	 	 MXSLT_NAME ": stopping child -- memory cleanup failed");
  }

  xfree(data);
  return;
}

static int mxslt_ap2_init_handler(apr_pool_t * p, apr_pool_t * plog, apr_pool_t * ptemp, server_rec * s) {
    /* Add version name to apache id */
  ap_add_version_component(p, "mod-xslt/" MXSLT_VERSION);
  return OK;
}

  /* mxslt_register_hooks is called only once, when 
   * the is started */
static void mxslt_register_hooks(apr_pool_t *p) {
    /* Initialize modxslt-library once */
  mxslt_xml_load();

  ap_log_error(APLOG_MARK, APLOG_STARTUP | APLOG_DEBUG, 0, NULL,
	 	 MXSLT_NAME ": registering hooks");

    /* Prepare TSDs */
  if(apr_threadkey_private_create(&mxslt_ap2_global_state, mxslt_ap2_child_exit, p) ||
     apr_threadkey_private_create(&mxslt_ap2_global_ectxt, mxslt_ectxt_destroy, p) ||
     apr_threadkey_private_create(&mxslt_ap2_global_recursion, mxslt_recursion_destroy, p) ||
     apr_threadkey_private_create(&mxslt_ap2_global_dbgbuffer, mxslt_dbgbuffer_destroy, p)
     /* apr_threadkey_private_create(&mxslt_ap2_global_ips, mxslt_ips_destroy, p) */) {
    ap_log_error(APLOG_MARK, APLOG_STARTUP | APLOG_WARNING, 0, NULL,
	 	 MXSLT_NAME ": failed allocating thread specific data. Killing myself.");
    return;
  }

    /* Register real hooks */
  /* ap_hook_child_init(mxslt_ap2_child_init, NULL, NULL, APR_HOOK_MIDDLE); */
  ap_hook_post_config(mxslt_ap2_init_handler, NULL, NULL, APR_HOOK_MIDDLE);
  ap_register_output_filter(MXSLT_STD_HANDLER, mxslt_ap2_out_filter, NULL, AP_FTYPE_RESOURCE);
  ap_register_input_filter(MXSLT_STD_HANDLER, (ap_in_filter_func)mxslt_ap2_in_filter, NULL, AP_FTYPE_RESOURCE);

  mxslt_ap2_global_dbglevel=mxslt_debug_firstlevel();
  return;
}

static void *mxslt_create_dir_config(apr_pool_t *p, char *dir) {
  mxslt_dir_config_t * nconf=(mxslt_dir_config_t *)apr_palloc(p, sizeof(mxslt_dir_config_t));

  nconf->mime_styles=apr_table_make(p, 0);
  nconf->default_styles=apr_table_make(p, 0);
  nconf->params=apr_table_make(p, 0);
  nconf->rules=apr_table_make(p, 0);
  nconf->dbglevel=mxslt_ap2_global_dbglevel;
  nconf->signature=0;

  return nconf;
}

static void *mxslt_merge_dir_config(apr_pool_t *p, void *basev, void *overridesv) {
  mxslt_dir_config_t * base = (mxslt_dir_config_t *)basev;
  mxslt_dir_config_t * override = (mxslt_dir_config_t *)overridesv;
  mxslt_dir_config_t * nconf = (mxslt_dir_config_t *)apr_palloc(p, sizeof(mxslt_dir_config_t));

    /* XXX if you are wondering why params are merged in the wrong order,
     *     take a look to the comment in  this same function in the apache API */
  nconf->mime_styles=apr_table_overlay(p, override->mime_styles, base->mime_styles);
  nconf->default_styles=apr_table_overlay(p, override->default_styles, base->default_styles);
  nconf->params=apr_table_overlay(p, base->params, override->params);
  nconf->rules=apr_table_overlay(p, override->rules, base->rules);

  nconf->dbglevel=base->dbglevel | override->dbglevel;
  nconf->signature=0;

  return nconf;
}

static const char * mxslt_ap2_param(cmd_parms *cmd, void * pcfg, const char * parameter, const char * value) {
  mxslt_dir_config_t * config;

  if(!parameter || !value)
    return "both parameter and value must be set";
  
  config=(mxslt_dir_config_t *)pcfg;
  apr_table_set(config->params, parameter, value);

  return NULL;
}

static const char * mxslt_ap2_add_rule(cmd_parms *cmd, void * pcfg, const char * url, const char * rule) {
  mxslt_dir_config_t * config;

  if(!url || !rule)
    return "both parameter and value must be set";
  
  config=(mxslt_dir_config_t *)pcfg;
  apr_table_set(config->rules, url, rule);

  return NULL;
}

static const char * mxslt_ap2_del_rule(cmd_parms *cmd, void *pcfg, const char *stylesheet) {
  mxslt_dir_config_t * config; 
  
  config=(mxslt_dir_config_t *)pcfg;
  apr_table_unset(config->rules, stylesheet);

  return NULL;
}


static const char * mxslt_ap2_default_stylesheet(cmd_parms *cmd, void *pcfg, const char *mime, const char *style) {
  mxslt_dir_config_t * config;

  config=(mxslt_dir_config_t *)pcfg;
  apr_table_set(config->default_styles, mime, style);

  return NULL;
}

static const char * mxslt_ap2_set_stylesheet(cmd_parms *cmd, void *pcfg, const char *mime, const char *style) {
  mxslt_dir_config_t * config;

  config=(mxslt_dir_config_t *)pcfg;
  apr_table_set(config->mime_styles, mime, style);

  return NULL;
}

static const char * mxslt_ap2_unset_stylesheet(cmd_parms *cmd, void *pcfg, const char *mime) {
  mxslt_dir_config_t * config; 
  
  config=(mxslt_dir_config_t *)pcfg;
  apr_table_unset(config->mime_styles, mime);

  return NULL;
}

static const char * mxslt_ap2_nodefault_stylesheet(cmd_parms *cmd, void *pcfg, const char *mime) {
  mxslt_dir_config_t * config; 
  
  config=(mxslt_dir_config_t *)pcfg;
  apr_table_unset(config->default_styles, mime);

  return NULL;
}

static const char * mxslt_ap2_set_debug(cmd_parms * cmd, void * pcfg, const char * l) {
  mxslt_dir_config_t * conf=(mxslt_dir_config_t *) pcfg;

  unsigned char * ch=(unsigned char *)l;
  int mask;

    /* If this is made only of digits, consider this the debugging mask
     * directly, as with XSLTDebugMask */
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


static const command_rec mxslt_cmds[] = {
  AP_INIT_TAKE2("XSLTParam", mxslt_ap2_param, NULL, 
		OR_OPTIONS, "XSLTParam <Param> <Value>"),
  AP_INIT_TAKE2("XSLTAddRule", mxslt_ap2_add_rule, NULL, 
		OR_OPTIONS, "XSLTAddRule <Stylesheet> <Condition>"),
  AP_INIT_TAKE1("XSLTDelRule", mxslt_ap2_del_rule, NULL, 
		OR_OPTIONS, "XSLTDelRule <Stylesheet>"),
  AP_INIT_TAKE2("XSLTSetStylesheet", mxslt_ap2_set_stylesheet, NULL, 
		OR_OPTIONS, "XSLTSetStylesheet <MimeType> <Stylesheet>"),
  AP_INIT_TAKE2("XSLTDefaultStylesheet", mxslt_ap2_default_stylesheet, NULL, 
		OR_OPTIONS, "XSLTDefaultStylesheet <MimeType> <Stylesheet>"),
  AP_INIT_TAKE1("XSLTUnSetStylesheet", mxslt_ap2_unset_stylesheet, NULL, 
		OR_OPTIONS, "XSLTUnSetStylesheet <MimeType>"),
  AP_INIT_TAKE1("XSLTNoDefaultStylesheet", mxslt_ap2_nodefault_stylesheet, NULL, 
		OR_OPTIONS, "XSLTNoDefaultStylesheet <MimeType>"),
  AP_INIT_FLAG("XSLTDisableSignature", ap_set_flag_slot, 
 	       (void *)APR_OFFSETOF(mxslt_dir_config_t, signature), OR_OPTIONS,
	       "XSLTDisableSignature <on|off> - ignored, "
               "provided for backward compatibility only"),
  AP_INIT_TAKE1("XSLTDebugMask", ap_set_int_slot, 
		(void *)APR_OFFSETOF(mxslt_dir_config_t, dbglevel), OR_OPTIONS,
		"XSLTDebugMask <[0-9]+>"),
  AP_INIT_RAW_ARGS("XSLTDebug", mxslt_ap2_set_debug, NULL, 
		OR_OPTIONS, "XSLTDebug <level>"),
  {NULL}
};

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA mxslt_module = {
  STANDARD20_MODULE_STUFF, 
  mxslt_create_dir_config,      /* create per-dir    config structures */
  mxslt_merge_dir_config,       /* merge  per-dir    config structures */
  NULL,                    /* create per-server config structures */
  NULL,                    /* merge  per-server config structures */
  mxslt_cmds,                   /* table of config file commands       */
  mxslt_register_hooks    /* register hooks                      */
};
