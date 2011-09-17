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


#include "modxslt0/modxslt.h"
#include <string.h>
#include <pcre.h>
#include <errno.h>
#include <stdlib.h>

/* Part of this code was originally produced using ``gperf'' and tweaked manually to
 * optimize the code for the reduced number of operators and the fixed length of 
 * the strings (either 1 or 2 characters) */

/* Command-line: gperf -m100000 -TI -Kopr -CLC -Wmxslt_cmp_opr -Hmxslt_cmp_opr_hash \
 * 			-Nmxslt_cmp_opr_lookup -t ./modxslt-cmp-opr.gperf  */
/* Computed positions: -k'1-2' */

static int mxslt_opr_cmp_equal(mxslt_doc_t * doc, char * str1, char * str2) {
  if(str1 && str2)
    return strcmp(str1, str2) ? MXSLT_FALSE : MXSLT_TRUE;

  return MXSLT_ERROR;
}

static int mxslt_opr_cmp_notequal(mxslt_doc_t * doc, char * str1, char * str2) {
  if(str1 && str2)
    return strcmp(str1, str2) ? MXSLT_TRUE : MXSLT_FALSE;
 
  return MXSLT_ERROR;
}

static int mxslt_opr_cmp_greater(mxslt_doc_t * doc, char * str1, char * str2) {
  double n1, n2;
  char *t1, *t2;

  if(!str1 || !str2)
    return MXSLT_ERROR;

  n1=strtod(str1, &t1);
  if(errno == ERANGE || str1 == t1 || !t1 || *t1 != '\0')
    return MXSLT_ERROR;

  n2=strtod(str2, &t2);
  if(errno == ERANGE || str2 == t2 || !t2 || *t2 != '\0')
    return MXSLT_ERROR;

    /* A negative value is meant to be an error, 
     * that's why I cannot simply return n1 - n2 */
  return (n1 > n2 ? MXSLT_TRUE : MXSLT_FALSE);
}

static int mxslt_opr_cmp_greaterequal(mxslt_doc_t * doc, char * str1, char * str2) {
  double n1, n2;
  char *t1, *t2;

  if(!str1 || !str2)
    return MXSLT_ERROR;

  n1=strtod(str1, &t1);
  if(errno == ERANGE || str1 == t1 || !t1 || *t1 != '\0')
    return MXSLT_ERROR;

  n2=strtod(str2, &t2);
  if(errno == ERANGE || str2 == t2 || !t2 || *t2 != '\0')
    return MXSLT_ERROR;

    /* A negative value is meant to be an error, 
     * that's why I cannot simply return n1 - n2 */
  return (n1 >= n2 ? MXSLT_TRUE : MXSLT_FALSE);
}

static int mxslt_opr_cmp_less(mxslt_doc_t * doc, char * str1, char * str2) {
  double n1, n2;
  char *t1, *t2;

  if(!str1 || !str2)
    return MXSLT_ERROR;

  n1=strtod(str1, &t1);
  if(errno == ERANGE || str1 == t1 || !t1 || *t1 != '\0')
    return MXSLT_ERROR;

  n2=strtod(str2, &t2);
  if(errno == ERANGE || str2 == t2 || !t2 || *t2 != '\0')
    return MXSLT_ERROR;

    /* A negative value is meant to be an error, 
     * that's why I cannot simply return n1 - n2 */
  return (n1 < n2 ? MXSLT_TRUE : MXSLT_FALSE);
}

static int mxslt_opr_cmp_lessequal(mxslt_doc_t * doc, char * str1, char * str2) {
  double n1, n2;
  char *t1, *t2;

  if(!str1 || !str2)
    return MXSLT_ERROR;

  n1=strtod(str1, &t1);
  if(errno == ERANGE || str1 == t1 || !t1 || *t1 != '\0')
    return MXSLT_ERROR;

  n2=strtod(str2, &t2);
  if(errno == ERANGE || str2 == t2 || !t2 || *t2 != '\0')
    return MXSLT_ERROR;

    /* A negative value is meant to be an error, 
     * that's why I cannot simply return n1 - n2 */
  return (n1 <= n2 ? MXSLT_TRUE : MXSLT_FALSE);
} 

#ifdef HAS_LIBPCRE
static char * reg_pattern(mxslt_doc_t * doc, char ** string) {
  char * store;
  char * tmp;
  char term;

  term=**string;
  if(term == '\\') {
    mxslt_error(doc, "cannot use `\\' as terminator in pattern: %s\n", *string);
    return NULL;
  }

  if(term == '\0') {
    mxslt_error(doc, "regular expression must be enclosed between two terminal characters: %s\n", *string);
    return NULL;
  }

    /* Look for end of string */
  for(store=tmp=(*string)+1; *tmp && *tmp != term; tmp++, store++) {
    if(*tmp == '\\' && *(tmp+1) == term) { 
      *store=*++tmp;
      continue;
    }
      /* Copy any other character we 
       * may have found */
    *store=*tmp;	/* It probably takes longer to check if we need to
			 * copy the byte than just copy it */
  }

  if(!*tmp) {
    mxslt_error(doc, "missing terminal character in regular expression: %s\n", *string);
    return NULL;
  }

    /* Store \0 and make string point to the 
     * following character */
  *store='\0';
  store=(*string)+1;
  *string=tmp+1;

  return store;
}

static int reg_options(char ** str, int * toret) {
  *toret=PCRE_NO_AUTO_CAPTURE;

  for(; 1; (*str)++) {
    switch(**str) {
      case 'i':
	(*toret) |= PCRE_CASELESS;
	break;
      
      case 'e':
	(*toret) |= PCRE_DOLLAR_ENDONLY;
	break;

      case 'a':
	(*toret) |= PCRE_ANCHORED;
	break;

      case 's':
	(*toret) |= PCRE_DOTALL;
	break;

      case 'x':
	(*toret) |= PCRE_EXTENDED;
	break;

      case 'X':
	(*toret) |= PCRE_EXTRA;
	break;

      case 'm': 
	(*toret) |= PCRE_MULTILINE;
	break;

      case 'y':
	(*toret) |= PCRE_UNGREEDY;
	break;

      case 'u':
	(*toret) |= PCRE_UTF8;
	break;
      
      case '\0':
	return MXSLT_OK;

      default:
	return MXSLT_ERROR;
    }
  }

    /* Never reached */
  return MXSLT_OK;
}

static int mxslt_opr_cmp_regequal(mxslt_doc_t * doc, char * str1, char * str2) {
  pcre * regex;		/* Compiled regular expression */

  char * pattern;	/* Portion of str1 */
  int options;

  int str1len;

  int status;
  int erroroffset;
  const char * err;
  
  if(!str2 || !str1)
    return MXSLT_ERROR;

  pattern=reg_pattern(doc, &str2);
  if(!pattern)
    return MXSLT_ERROR;

  status=reg_options(&str2, &options); 
  if(status != MXSLT_OK) {
    mxslt_error(doc, "unknown option at end of pattern: %c (%.10s)\n", *str2, str2);
    return status;
  }

    /* XXX it would be nice to have a compiled regex cache */
  regex=pcre_compile(pattern, options, (const char **)&err, &erroroffset, NULL);
  if(regex == NULL) {
    mxslt_error(doc, "can't compile regular expression: %s, %s\n", pattern, err);
    return MXSLT_ERROR;
  }

  str1len=strlen(str1);
  if(str1len < 0) {
    mxslt_error(doc, "string too long is overflowing an int in regular expression compare\n"); 
    return MXSLT_ERROR; 
  }

  status=pcre_exec(regex, NULL,  str1, str1len, 0, PCRE_NOTEMPTY, NULL, 0);
  free(regex);

  if(status >= 0)
    return MXSLT_TRUE;

  switch(status) {
    case PCRE_ERROR_NOMATCH:
      break;

    case PCRE_ERROR_NULL:
    case PCRE_ERROR_BADOPTION:
    case PCRE_ERROR_BADMAGIC:
    case PCRE_ERROR_UNKNOWN_NODE:
      mxslt_error(doc, "pcre error while executing regular expression: %s - please report this as a bug\n", pattern);
      break;

    case PCRE_ERROR_NOMEMORY:
      mxslt_error(doc, "pcre out of memory while compiling: %s\n", pattern);
      break;

    case PCRE_ERROR_MATCHLIMIT:
      mxslt_error(doc, "pcre matchlimit reached while compiling: %s\n", pattern);
      break;
  }

  
  return MXSLT_FALSE;
} 

#else /* HAS_PCRE */
static int mxslt_opr_cmp_regequal(mxslt_doc_t * doc, char * str1, char * str2) {
  return MXSLT_FALSE;
}
#endif

static int mxslt_opr_cmp_notregequal(mxslt_doc_t * doc, char * str1, char * str2) {
  int status;
  status=mxslt_opr_cmp_regequal(doc, str1, str2);

  return status != MXSLT_ERROR ? status : MXSLT_ERROR;
} 

const struct mxslt_opr_t * mxslt_opr_cmp_lookup (char * str) {
  static const struct mxslt_opr_t mxslt_cmp_opr[] = {
    {NULL}, {NULL},
    {"=", (mxslt_opr_call_f)mxslt_opr_cmp_equal},
    {">", (mxslt_opr_call_f)mxslt_opr_cmp_greater},
    {"==", (mxslt_opr_call_f)mxslt_opr_cmp_equal},
    {">=", (mxslt_opr_call_f)mxslt_opr_cmp_greaterequal},
    {"<", (mxslt_opr_call_f)mxslt_opr_cmp_less},
    {"=~", (mxslt_opr_call_f)mxslt_opr_cmp_regequal},
    {"<=", (mxslt_opr_call_f)mxslt_opr_cmp_lessequal},
    {"!=", (mxslt_opr_call_f)mxslt_opr_cmp_notequal},
    {NULL}, {NULL},
    {"!~", (mxslt_opr_call_f)mxslt_opr_cmp_notregequal}
  };

  static const unsigned char asso_values[] = {
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13,  6, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
       5,  1,  2, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13,  4, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13
  };

  register int key;
  register const char *s; 

    /* This code was changed upon the assumption that a string
     * is either 1 or 2 characters long 
     *
     * Most checks were also removed upon the assumption that flex 
     * has already performed input validation */
  if(str[1] != '\0') 
    key=2+asso_values[(unsigned char)str[1]];
  else 
    key=1;
  key+=asso_values[(unsigned char)str[0]];

  s = mxslt_cmp_opr[key].opr;
  if (*str == *s && *(str+1) == *(s+1))
    return &mxslt_cmp_opr[key];

  return NULL;
}
