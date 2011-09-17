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

#ifndef MXSLT_MEMORY_H
# define MXSLT_MEMORY_H

/* ./modxslt-memory.c */
# ifndef MXSLT_DEBUG_MEMORY
#  define xfree(ptr) free(ptr)
# else
extern void xfree(void *ptr);
# endif

extern void *xmalloc(size_t size);
extern void *xrealloc(const void *ptr, size_t size);
extern char *xstrdup(const char *str);
extern void *xstrndup(const char *str, size_t size);

#endif 
