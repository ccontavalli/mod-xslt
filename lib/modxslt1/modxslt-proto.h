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

#ifndef MXSLT_PROTO_H
# define MXSLT_PROTO_H

/* ./parser/modxslt-screen-expr.lexer.c */
extern int mxslt_doc_screen_check(mxslt_doc_t *, char **, int flags);

/* ./modxslt-opr-cmp.c */
extern const struct mxslt_opr_t *mxslt_opr_cmp_lookup(char *str);
/* ./modxslt-opr-bool.c */
extern const struct mxslt_opr_t *mxslt_opr_bool_lookup(char *str);

# ifdef MXSLT_LIBXML_HTTP_STATUS
extern void * mxslt_url_real_open(mxslt_doc_t *, char *);
# else
#  define mxslt_url_real_open(doc, uri) xmlNanoHTTPMethodRedir(uri, NULL, NULL, NULL, NULL, NULL, 0)
# endif

/* ./modxslt-url.c */
extern int mxslt_doc_url_decode(char *url);
extern int mxslt_doc_param_urlparse(mxslt_doc_t *document, const char *query);

/* ./modxslt-doc.c */
extern void mxslt_doc_param_hdr_add(mxslt_doc_t * doc, char * uc_key, char * uc_value);
extern mxslt_table_status_e mxslt_doc_param_add(mxslt_doc_t *, char *, char *); 
extern mxslt_table_status_e mxslt_doc_param_get(mxslt_doc_t *, char *, char **);
extern int mxslt_get_static_attr(const char *content, const mxslt_attr_search_t *attr, char **store, int nelems);
extern void mxslt_doc_init(mxslt_doc_t *document, char * tsapi, mxslt_shoot_t *shoot, mxslt_recursion_t * rec,
			   mxslt_error_hdlr_f errhdlr, void *errctx, void *ctx);
extern int mxslt_doc_load(mxslt_doc_t *document, xmlParserInputBufferPtr buf, char *filename, 
			  char *hostname, int port, char *path);
extern int mxslt_doc_done(mxslt_doc_t *document, mxslt_shoot_t *);
extern int mxslt_doc_parse(mxslt_doc_t *document, mxslt_header_set_f header_set, void *);
extern int mxslt_doc_send(mxslt_doc_t *document, mxslt_callback_t *callback, void *ctx);
extern int mxslt_doc_parse_stylesheet(mxslt_doc_t *document, char *media, int flags);
extern xsltStylesheetPtr mxslt_doc_load_stylesheet_file(mxslt_doc_t *document, char *file);
extern int mxslt_doc_load_stylesheet(mxslt_doc_t *document, char *href);
extern int mxslt_doc_parse_pi(mxslt_doc_t *document);
extern void mxslt_shoot_init(mxslt_shoot_t*);
extern void mxslt_xml_init(
    mxslt_shoot_t *, mxslt_url_handler_t *http_handler,
    mxslt_url_handler_t *file_handler);
extern void mxslt_xml_done(mxslt_shoot_t * shoot);

/* ./modxslt-io.c */
extern xmlParserInputBufferPtr mxslt_create_input_file(mxslt_doc_t *doc, char *tmpfile);
extern xmlParserInputBufferPtr mxslt_create_input_fd(mxslt_doc_t *doc, int fd);

/* ./modxslt-libxml.c */
extern void mxslt_sax_errhdlr(void *ctx, const char *msg, ...);
extern void mxslt_sax_processing_instruction(void *ctx, const xmlChar *target, const xmlChar *data);
extern xmlParserInputPtr mxslt_sax_resolve_entity(void *ctx, const xmlChar *publicId, const xmlChar *systemId);
extern xmlDocPtr mxslt_doc_xml_parse(mxslt_doc_t *document, xmlParserInputBufferPtr buf, char *localfile);
extern xmlDocPtr mxslt_doc_xml_load_entity(mxslt_doc_t *document, char *localfile);
extern char * mxslt_yy_str_parse(mxslt_doc_t * document, char * start, size_t size);
extern xmlDocPtr mxslt_doc_xml_apply_stylesheet(mxslt_doc_t *, xsltStylesheetPtr, xmlDocPtr, const char **);

# if !defined(HAVE_LIBXML_HACK) && defined(HAVE_LIBXML_THREADS) 
extern int xmlSetGlobalState(xmlGlobalStatePtr state, xmlGlobalStatePtr *);
extern void xmlFreeGlobalState(xmlGlobalStatePtr state);
extern xmlGlobalStatePtr xmlNewGlobalState(void);
# endif /* !defined(HAVE_LIBXML_HACK) && defined(HAVE_LIBXML_THREADS) */

# ifdef HAVE_PTHREADS
extern mxslt_state_t * mxslt_get_state(void);
# else
#  define mxslt_get_state() mxslt_global_state
extern mxslt_state_t * mxslt_global_state;
# endif

  /** Returns a number representing the current thread.
   * Note that the concept of Thread Identifier changes from platform to
   * platform, can be a number, can be a more complex structure.
   * This function is supposed to return an integer representing the thread
   * it's running in, which is supposed to be different from any other thread.
   * Use for debugging purposes, don't rely on it too much.
   * Current implementation is likely to not work on non-linux systems. */
extern unsigned long int mxslt_get_tid(void);

extern int mxslt_url_match(const char *);
extern int mxslt_local_match(const char *);
extern void * mxslt_url_open(const char *);
extern void * mxslt_local_open(const char *);
extern int mxslt_url_read(void *, char *, int);
extern int mxslt_url_close(void *);
extern void mxslt_xml_load(void);
extern void mxslt_xml_unload(void);

extern void mxslt_doc_null(void);

# define mxslt_url_recurse_level(rec) ((rec)->rec_level)
extern int mxslt_url_recurse_allowed(mxslt_recursion_t *, const char * uri);
extern void mxslt_url_recurse_push(mxslt_recursion_t *, const char * uri); 
extern void mxslt_url_recurse_pop(mxslt_recursion_t *, int n); 
extern void mxslt_url_recurse_dump(mxslt_recursion_t *, void (*)(void *, char *, ...), void *);

extern void mxslt_state_init(mxslt_shoot_t *);
extern void mxslt_recursion_init(mxslt_recursion_t * rec);

#endif 
