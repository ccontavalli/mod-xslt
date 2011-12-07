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


#ifndef MXSLT_MODXSLT_AP2_H
# define MXSLT_MODXSLT_AP2_H

  /* Standard includes */
# include <unistd.h>
# include <assert.h>
# include <stdlib.h>

  /* Apache includes */
# include "ap_config.h"
# include "ap_listen.h"
# include "httpd.h"
# include "http_config.h"
# include "http_protocol.h"
# include "http_log.h"
# include "util_filter.h"
# include "apr_buckets.h"
# include "apr_strings.h"
# include "apr_hash.h"
# include "apr_lib.h"
# include "apr_uri.h"

# include <modxslt1/modxslt.h>

# define mxslt_ap2_state_set(data) apr_threadkey_private_set(data, mxslt_ap2_global_state)
# define mxslt_ap2_state_get(data) apr_threadkey_private_get((void **)data, mxslt_ap2_global_state)

# define mxslt_ap2_recursion_set(data) apr_threadkey_private_set(data, mxslt_ap2_global_recursion)
# define mxslt_ap2_recursion_get(data) apr_threadkey_private_get((void **)data, mxslt_ap2_global_recursion)

# define mxslt_ap2_ectxt_set(data) apr_threadkey_private_set(data, mxslt_ap2_global_ectxt)
# define mxslt_ap2_ectxt_get(data) apr_threadkey_private_get(data, mxslt_ap2_global_ectxt)


  /* Keeps global libxml state */
extern apr_threadkey_t * mxslt_ap2_global_state;
  /* Keeps global libxslt error context */
extern apr_threadkey_t * mxslt_ap2_global_ectxt;
extern apr_threadkey_t * mxslt_ap2_global_recursion;
extern apr_threadkey_t * mxslt_ap2_global_dbgbuffer;

typedef struct mxslt_ap2_debug_buffer_t {
  char * buffer;
  int size;
  int used;
} mxslt_ap2_debug_buffer_t;

# ifdef MXSLT_DO_DEBUG
#  define AP2_DEBUG_DUMP_TABLE(str, table) \
  ({ int __i; \
     apr_array_header_t * __harr = (apr_array_header_t *)apr_table_elts(table);\
     apr_table_entry_t * __entry = (apr_table_entry_t *)__harr->elts;\
     if(__harr) { \
       MXSLT_DEBUG(str); \
       for(__i=0; __i < __harr->nelts; __i++) \
         MXSLT_DEBUG("    %s -> %s\n", __entry[__i].key, __entry[__i].val); \
     } })

#  define mxslt_ap2_get_config(a, b) \
    ({ mxslt_dir_config_t * __var; \
       __var=(mxslt_dir_config_t *)ap_get_module_config(a,b);\
       MXSLT_DEBUG("Fetched config\n");\
       MXSLT_DEBUG("* config: %08x\n", (int)__var);\
       __var; \
    })
# else 
#  define mxslt_ap2_get_config(a, b) (mxslt_dir_config_t *)ap_get_module_config(a, b)
# endif

typedef struct mxslt_ap2_ctx_t {
  mxslt_doc_t * doc;
  apr_bucket_brigade * brigade;
  apr_bucket * bucket;

  apr_size_t offset;

  char * buffer;
  apr_size_t size;
} mxslt_ap2_ctx_t;

typedef struct mxslt_dir_config_t {
  apr_table_t * mime_styles;
  apr_table_t * default_styles;
  apr_table_t * params;
  apr_table_t * rules;

  unsigned int dbglevel;
  int signature;
} mxslt_dir_config_t;

  /* Extern functions */
extern int mxslt_ap2_file_parse(ap_filter_t *, apr_bucket_brigade *, const char *, const char *, mxslt_dir_config_t * config);
extern void mxslt_ap2_debug(void * ctx, int level, int mask, const char * msg, ...);

#endif
