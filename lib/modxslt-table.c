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


#include "modxslt1/modxslt-table.h"

  /* Allow those to be defined from compiler */
# ifndef mxslt_table_calc
#  define mxslt_table_calc(hash, key) (((hash)->mxslt_table_calc)(key, hash->table_size))
# endif
# ifndef mxslt_table_cmp
#  define mxslt_table_cmp(hash, key, key2) (((hash)->mxslt_table_cmp)(key, key2))
# endif
# ifndef mxslt_table_need_grow
#  define mxslt_table_need_grow(htable) (((htable)->table_used) >= ((htable)->table_size))
# endif
# ifndef mxslt_table_need_shrink
#  define mxslt_table_need_shrink(htable) (((htable)->table_used) <= (((htable)->table_size)>>1))
# endif
# ifndef mxslt_table_grow_size
#  define mxslt_table_grow_size(a) (((a) << 1)-1)
# endif
# ifndef mxslt_table_shrink_size
#  define mxslt_table_shrink_size(a) (((a)+1)>>1)
# endif

static inline void mxslt_table_table_redistribute(mxslt_table_t * hash) {
  mxslt_table_count_t i, found;
  mxslt_table_record_t * walk, * tmp;
  mxslt_table_record_t ** newt;
  mxslt_table_size_t rhash;

    /* Allocate necessary memory */
  newt=(mxslt_table_record_t **)(xmalloc(sizeof(mxslt_table_record_t *)*hash->table_size));
  memset(newt, 0, sizeof(mxslt_table_record_t *)*hash->table_size);

    /* Redistribute elements over the new interval */
  for(i=0, found=0; found < hash->table_used; i++) 
    for(walk=hash->table[i]; walk != NULL; found++) {
      rhash=mxslt_table_calc(hash, walk->key);

      tmp=walk->next;
      walk->next=newt[rhash];
      newt[rhash]=walk;
      walk=tmp;
    }

    /* Ok, free up old table */
  xfree(hash->table);

    /* Ok, assign new table */
  hash->table=newt;
 
  return;
}

  /* Initializes hash */
void mxslt_table_inith(mxslt_table_t * const hash, const mxslt_table_size_t hintsize) {
  hash->table_size=0;
  hash->table_used=0;
  hash->hint_size=hintsize ? hintsize : TABLE_DEFAULT_HINT;
  hash->mxslt_table_calc=TABLE_CALC_DEFAULT;
  hash->mxslt_table_cmp=(int (*)(void *, void *))strcmp;
  hash->table=NULL;

  return;
}

  /* Inserts a record in the hashing table */
mxslt_table_status_e mxslt_table_insert(mxslt_table_t * const hash, mxslt_table_record_t ** const store, 
				    const char * key, const void * data) {
  mxslt_table_size_t rhash, oldsize;
  mxslt_table_record_t * toadd;

    /* Check if adding the element 
     * would overflow the table */
  if(hash->table_used+1 < hash->table_used) {
    if(store)
      (*store)=NULL;
    return MXSLT_TABLE_FAILURE;
  }

    /* Prepare to add a new element */
  if(mxslt_table_need_grow(hash)) {
      /* Find a new good size */
    if(!(hash->table_size)) {
        /* This is the first malloc */
      hash->table_size=hash->hint_size;
      hash->table=(mxslt_table_record_t **)(xmalloc(sizeof(mxslt_table_record_t *)*hash->hint_size));
      memset(hash->table, 0, sizeof(mxslt_table_record_t *)*hash->hint_size);
    } else {
        /* If we can still increase table size */
      if(hash->table_size < TABLE_SIZE_MAX) {
        oldsize=hash->table_size;
        hash->table_size=mxslt_table_grow_size(hash->table_size);

          /* Use TABLE_SIZE_MAX in case of overflow */
        if(hash->table_size < oldsize)
          hash->table_size=TABLE_SIZE_MAX;
          
	  /* Redistribute elements over table */
        mxslt_table_table_redistribute(hash);
      }
    }
  }

    /* Allocate memory for data holder */
  toadd=(mxslt_table_record_t *)(xmalloc(sizeof(mxslt_table_record_t)));
  toadd->key=(char *)key;
  toadd->data=(void *)data;

    /* Calculate hash and insert in table */
  rhash=mxslt_table_calc(hash, key);
  toadd->next=hash->table[rhash];
  hash->table[rhash]=toadd;
  toadd->parent=hash->table+rhash;
  if(toadd->next)
    toadd->next->parent=&(toadd->next);

    /* Increment count of element used */
  hash->table_used++;

    /* Return hash table descriptor if required */
  if(store)
    (*store)=toadd;

  return MXSLT_TABLE_SUCCESS;
}

  /* Finds a record in the hashing table */
mxslt_table_status_e mxslt_table_search(mxslt_table_t * const hash, mxslt_table_record_t ** const store, 
					const char * key, void ** const toret) {
  mxslt_table_size_t rhash;
  mxslt_table_record_t * tmp;
  
    /* In case table is empty,
     * return immediately */
  if(!hash || !hash->table) {
    if(toret)
      (*toret)=NULL;
    if(store)
      *store=NULL;

    return MXSLT_TABLE_NOT_FOUND;
  }

     /* Calculate element hash */ 
  rhash=mxslt_table_calc(hash, key);

    /* Check for clustered keys */
  for(tmp=hash->table[rhash]; tmp != NULL; tmp=tmp->next) 
    if(!mxslt_table_cmp(hash, (void *)key, (void *)tmp->key)) {
      if(toret)
        (*toret)=tmp->data;
      if(store)
	(*store)=tmp;
      return MXSLT_TABLE_FOUND;
    }

  if(toret)
    (*toret)=NULL;
  if(store)
    *store=NULL;

  return MXSLT_TABLE_NOT_FOUND;
}

mxslt_table_status_e mxslt_table_record_remove(mxslt_table_t * const table, mxslt_table_record_t * record,
					       void (*freer)(const char * const, const void * const)) {
  *(record->parent)=record->next;
  if(record->next)
    record->next->parent=record->parent;

    /* Free up record */
  if(freer)
    freer(record->key, record->data);
  xfree(record);
  table->table_used-=1;

    /* Check if we need to shrink the 
     * table */
  if(table->table_size > table->hint_size && mxslt_table_need_shrink(table)) {
    table->table_size=mxslt_table_shrink_size(table->table_size);
    
      /* Never go under table->hint_size */
    if(table->table_size < table->hint_size)
      table->table_size=table->hint_size;

      /* Redistribute elements */
    mxslt_table_table_redistribute(table);
  }

  return MXSLT_TABLE_FOUND;
}

mxslt_table_status_e mxslt_table_remove(mxslt_table_t * const hash, const char * const key, 
				    void (*freer)(const char * const, const void * const)) {
  mxslt_table_size_t rhash;
  mxslt_table_record_t ** ptr;
  mxslt_table_record_t * tmp;

    /* In case table is empty,
     * return immediately */
  if(!hash->table || !hash)
    return MXSLT_TABLE_NOT_FOUND;
 
     /* Calculate element hash */ 
  rhash=mxslt_table_calc(hash, key);

    /* Check for clustered keys */
  for(ptr=hash->table+rhash; (*ptr) != NULL; ptr=&((*ptr)->next)) 
    if(!mxslt_table_cmp(hash, (void *)key, (void *)((*ptr)->key)))
      break;

    /* Check if we really found the key */
  if(!(*ptr))
    return MXSLT_TABLE_NOT_FOUND;
    
    /* Remember address of object */
  tmp=(*ptr);

    /* Remove pointer from linked list */
  (*ptr)=(*ptr)->next;
    /* Update next record parent */
  (*ptr)->parent=ptr;
    /* Free up on element */
  hash->table_used-=1;

    /* Free up key and data */
  if(freer)
    freer((*ptr)->key, (*ptr)->data);

    /* Free up mxslt_table_record_t */
  xfree(tmp);

    /* Check if we need to shrink the 
     * table */
  if(hash->table_size > hash->hint_size && mxslt_table_need_shrink(hash)) {
    hash->table_size=mxslt_table_shrink_size(hash->table_size);
    
      /* Never go under hash->hint_size */
    if(hash->table_size < hash->hint_size)
      hash->table_size=hash->hint_size;

      /* Redistribute elements */
    mxslt_table_table_redistribute(hash);
  }

  return MXSLT_TABLE_FOUND;
}

void mxslt_table_free(mxslt_table_t * const hash, void (*freer)(const char *, const void *)) {
  mxslt_table_record_t * walk, *next;
  mxslt_table_count_t found=0, i=0;

  if(!hash->table || !hash)
    return;

  for(; found < hash->table_used; i++) 
    for(walk=hash->table[i]; walk != NULL; walk=next, found++) {
      next=walk->next;
      if(freer)
	freer(walk->key, walk->data);
      xfree(walk);
    }

  xfree(hash->table);
  hash->table=NULL;
  hash->table_size=0;
  hash->table_used=0;

  return;
}
