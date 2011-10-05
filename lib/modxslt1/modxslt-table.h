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


#ifndef MXSLT_TABLE_H
# define MXSLT_TABLE_H

# include <string.h>
# include <limits.h>

# define TABLE_SIZE_MAX INT_MAX
# define TABLE_CALC_DEFAULT mxslt_table_calc_dragon

# define TABLE_DEFAULT_HINT 7

# define mxslt_table_init_static() { NULL, TABLE_CALC_DEFAULT, (int (*)(void *, void *))strcmp, 0, 0, TABLE_DEFAULT_HINT }
# define mxslt_table_inith_static(hint) { NULL, TABLE_CALC_DEFAULT, (int (*)(void *, void *))strcmp, 0, 0, hint }
# define mxslt_table_init(ptr) mxslt_table_inith(ptr, 0)

  /* XXX: they both should support one more argument: the state ... */
# define mxslt_table_set_hash(table, hash) (((table)->mxslt_table_calc)=(hash))
# define mxslt_table_set_cmp(table, cmp) (((table)->mxslt_table_cmp)=(cmp))

  /* _MUST_ be of unsigned type, at least table_size */
typedef unsigned int mxslt_table_size_t;
typedef unsigned int mxslt_table_count_t;

typedef struct mxslt_table_record_t { 
  char * key;
  void * data;

  struct mxslt_table_record_t ** parent;
  struct mxslt_table_record_t * next;
} mxslt_table_record_t;

typedef mxslt_table_record_t data_t;

typedef struct mxslt_table_t {
  struct mxslt_table_record_t ** table;
  mxslt_table_size_t (*mxslt_table_calc)(const char *, mxslt_table_size_t);
  int (*mxslt_table_cmp)(void *, void *);

  mxslt_table_count_t table_used; /* Elements in the table */
  mxslt_table_size_t table_size;	/* Size of hash index */
  mxslt_table_size_t hint_size;	/* Suggested size for hash */
} mxslt_table_t;

typedef enum mxslt_table_status_e {
  MXSLT_TABLE_SUCCESS=0,
  MXSLT_TABLE_FOUND=0,
  MXSLT_TABLE_FAILURE=-1,
  MXSLT_TABLE_NOT_FOUND=-1
} mxslt_table_status_e;

# include "modxslt.h"

/* ./hash-algo.c */
extern mxslt_table_size_t mxslt_table_calc_fnv(const char * tohash, const mxslt_table_size_t wrap);
extern mxslt_table_size_t mxslt_table_calc_dragon(const char * tohash, const mxslt_table_size_t wrap);

/* ./dscm-hash.c */
extern void mxslt_table_inith(mxslt_table_t *const hash, const mxslt_table_size_t hintsize);
extern mxslt_table_status_e mxslt_table_record_remove(mxslt_table_t * const table, mxslt_table_record_t *,
						      void (*freer)(const char * const, const void * const));

extern mxslt_table_status_e mxslt_table_insert(mxslt_table_t *const hash, mxslt_table_record_t ** const store, 
					   const char *key, const void *data);
extern mxslt_table_status_e mxslt_table_search(mxslt_table_t * const hash, mxslt_table_record_t **, 
					       const char *key, void ** const toret);

extern void mxslt_table_free(mxslt_table_t * const hash, void (*freer)(const char *, const void *));

# define mxslt_table_nelems(table) ((table)->table_used)
/* # define mxslt_doc_param_get(doc, key, value) mxslt_table_search(&((doc)->table), NULL, key, value) */
# define mxslt_table_done(table) mxslt_table_free(table, mxslt_doc_param_free)

#endif 
