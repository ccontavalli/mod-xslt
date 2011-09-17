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


#ifndef MXSLT_AP1_H
# define MXSLT_AP1_H

  /* Standard includes */
# include <modxslt0/modxslt.h>
# include <assert.h>
# include <unistd.h>
# include <stdlib.h>

  /* Apache includes */
# include "ap_config.h"
# include "httpd.h"
# include "http_log.h"
# include "http_config.h"
# include "http_request.h"
# include "http_protocol.h"
# include "http_conf_globals.h"
# include "buff.h"
# include "http_vhost.h"

  /* xslt handlers */
# define MXSLT_DYN_HANDLER "mod-xslt-dynamic"
# define MXSLT_FRC_HANDLER "mod-xslt-force"

# define MXSLT_NOTE_LOG "mod-xslt-tmp"

  /* String constants */
# define MXSLT_NOTE_HANDLER "MXSLT_HANDLER"

  /* Temporary file handling constants */
# define MXSLT_DEFAULT_TMP_DIR "/tmp/mod-xslt"
# define MXSLT_TMP_NAME "mod-xslt.XXXXXX"

  /* Some macros */
# define mxslt_engine_on(state) ((state) == MXSLT_STATE_ON)

# ifndef MXSLT_MAX_REDIR
#  define MXSLT_MAX_REDIR 15
# endif

  /* Boolean variables */
typedef enum mxslt_dir_config_state_e {
  MXSLT_STATE_UNSET=-1,
  MXSLT_STATE_OFF,
  MXSLT_STATE_ON,
} mxslt_dir_config_state_e;

typedef struct mxslt_dir_config_t {
  table * mime_styles;
  table * default_styles;
  table * filter_dynamic;
  table * filter_force;
  table * params;
  table * rules;

  char * tmpdir;
  int state;
  int unlink;
  int signature;

  unsigned int dbglevel;
} mxslt_dir_config_t;

/* ./mod_xslt.c */
extern mxslt_shoot_t ap1_mxslt_global_state;
extern mxslt_recursion_t mxslt_global_recursion;
/*
extern mxslt_doc_t * mxslt_global_document;
extern request_rec * mxslt_global_r;
*/

/* ./ap1-helpers.c */
extern int mxslt_ap1_mktemp_file(mxslt_dir_config_t * config, request_rec * r, char * dir, char ** file);

extern void mxslt_ap1_error(void *ctx, const char *msg, ...);
extern int mxslt_ap1_set_headers(mxslt_doc_t *document, request_rec *r);
extern request_rec *mxslt_ap1_sub_request_pass(mxslt_dir_config_t * config, request_rec *r, char * uri,
					  char **file, int * status);
extern request_rec *mxslt_ap1_sub_request(mxslt_dir_config_t *, request_rec *r, int fd, struct in_addr *, uri_components * uri, int * toret);
extern int mxslt_ap1_file_parse(mxslt_dir_config_t * config, request_rec *r, const char *filename, const char * defaultstyle, const char *forcestyle);
extern table * mxslt_ap1_merge_tables(pool * p, table * table_new, table * table_old);

extern void mxslt_ap1_debug(void * ctx, int level, int mask, const char * msg, ...);

#endif /* MXSLT_AP1_H */
