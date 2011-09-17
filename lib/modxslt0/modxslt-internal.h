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

#ifndef MXSLT_INTERNAL_H
# define MXSLT_INTERNAL_H

typedef enum mxslt_pi_e {
  MXSLT_PI_NONE,
  MXSLT_PI_PARAM,
  MXSLT_PI_STDST,
  MXSLT_PI_MODST,
  MXSLT_PI_UBASE
} mxslt_pi_e;

  /* A generic processing instruction
   * found in the document */
struct mxslt_pi_t {
  int type;
  struct mxslt_pi_t * next;
  xmlNodePtr node;
};

  /* A stylesheet processing
   * instruction */
typedef struct mxslt_pi_style_t {
  int type;
  struct mxslt_pi_t * next;
  xmlNodePtr node;

  char * ctype;
  char * href;
  char * media;

} mxslt_pi_style_t;

  /* A param processing 
   * instruction */
typedef struct mxslt_pi_param_t {
  int type;
  struct mxslt_pi_t * next;
  xmlNodePtr node;
  
  char * param;
  char * value;
} mxslt_pi_param_t;

  /* A base processing 
   * instruction */
typedef struct mxslt_pi_base_t {
  int type;
  struct mxslt_pi_t * next;
  xmlNodePtr node;
  
  char * directory;
  char * file;
} mxslt_pi_base_t;

  /* Keeps temporary state 
   * of a wrapper http handler */
typedef struct mxslt_http_t {
  int handled;
  void * data;
} mxslt_http_t;




#endif 
