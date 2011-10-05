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

# define MSY_FNV_MAGIC 0x01000193

# define HASH_CALC_FNV 0
# define HASH_CALC_DRAGON 1

# ifdef HASH_CALC_FNV
  /* Fowler/Noll/Vo Hash function */
mxslt_table_size_t mxslt_table_calc_fnv(const char * tohash, const mxslt_table_size_t wrap) {
  static mxslt_table_size_t hash;
  unsigned char *start;

  start = (unsigned char *)(tohash);
  for (hash=0; *start; start++) {
    hash *= MSY_FNV_MAGIC;
    hash ^= *start;
  }

  return hash%wrap;
}
# endif /* HASH_CALC_FNV */

# ifdef HASH_CALC_DRAGON
  /* Dragon book hash function */
mxslt_table_size_t mxslt_table_calc_dragon(const char * tohash, mxslt_table_size_t wrap) {
  static mxslt_table_size_t hash;
  static mxslt_table_size_t tmp;
  
  for(hash=0; *tohash;) {
    hash=(hash << 4) + *tohash++;
    if((tmp=(hash & 0xf0000000)) != 0) {
      hash ^= (tmp >> 24);
      hash ^= tmp;
    }
  }

  return hash%wrap;
}
# endif /* HASH_CALC_DRAGON */
