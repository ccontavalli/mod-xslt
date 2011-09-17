%{
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../modxslt0/modxslt.h"
#include "modxslt-screen-expr.parser.h"

extern int mxslt_expr_yy_lex(YYSTYPE *, YYLTYPE *, void *);
extern void mxslt_expr_yy_set_extra(void *, void *);
extern mxslt_scan_t * mxslt_expr_yy_get_extra(void *);

# define mxslt_yy_accept(value) ((mxslt_expr_yy_get_extra(scanner)->status)=(value))
# define mxslt_yy_document ((mxslt_doc_t *)(mxslt_expr_yy_get_extra(scanner)->document))
# define mxslt_expr_yy_error(msg) mxslt_error(mxslt_yy_document, msg)

%}

%union {
  int bval;
  char * string;
  const mxslt_opr_t * operator;
}

  /* Allow parser to be reentrant as needed
   * by apache2 threading model */
%pure-parser

  /* Allow better error tracking */
%locations 

%token TOKEN_STRING
%token TOKEN_SCREEN
%token TOKEN_ALL
%token TOKEN_ANY
%token TOKEN_AND

%token <string> TOKEN_LIT
%left <operator> TOKEN_OPR_BOOL
%left <operator> TOKEN_OPR_CMP
%left <bval> '!'
%token '(' ')'
%token EOS

%destructor { if($$) xfree($$); } TOKEN_LIT

%type <bval> cmp_expr
%type <bval> bool_expr
%type <bval> screen_expr
%type <bval> media_expr
%type <bval> media_list_expr

%%

media_list_expr: 
 	media_expr { mxslt_yy_accept($1); YYACCEPT }
	| media_expr EOS media_list_expr
;

media_expr:
	TOKEN_ALL { mxslt_yy_accept(MXSLT_TRUE); YYACCEPT } 
	| screen_expr 
	| TOKEN_ANY { $$=MXSLT_FALSE } 
	/*| error { $$=MXSLT_FALSE }*/

screen_expr:
	TOKEN_SCREEN TOKEN_AND bool_expr { 
	    if($3 == MXSLT_TRUE) { 
	      mxslt_yy_accept($3); 
	      YYACCEPT;
	    }

	    $$=MXSLT_FALSE;
	  }
	| bool_expr {
	    if($1 == MXSLT_TRUE) {
	      mxslt_yy_accept($1);
	      YYACCEPT;
	    }

	    $$=MXSLT_FALSE;
	  }
	| TOKEN_SCREEN { 
	    mxslt_yy_accept(MXSLT_TRUE); 
	    YYACCEPT;
	  }
;

bool_expr: 
	cmp_expr  /* XXX: we could allow error recovery. we would just need some way
		   *  to tell flex to go back to the initial state. Any idea? */
;

cmp_expr: 
        '(' cmp_expr ')' { $$ = $2 }
	| '!' cmp_expr { $$ = ! $2; }
	| cmp_expr TOKEN_OPR_BOOL cmp_expr {
              /* Check a valid operator
	       * was found */
	    if(!$2) {
	      mxslt_yy_accept(MXSLT_ERROR);
	      YYABORT;
	    }

              /* Call operator handler */
	    $<bval>$=mxslt_opr_bool_call($2, mxslt_yy_document, $1, $3);

	      /* Check status of operator handler */
	    if($$ < 0) {
	      mxslt_yy_accept(MXSLT_ERROR);
	      YYABORT;
	    }
	  }
	| TOKEN_LIT TOKEN_OPR_CMP TOKEN_LIT { 
              /* Check if operator is valid */
	    if(!$2) {
	      mxslt_yy_accept(MXSLT_ERROR);

              if($1)
	        xfree($1);

	      if($3)
	        xfree($3);
	      YYABORT;
	    }

	      /* Can return MXSLT_TRUE, MXSLT_FALSE or an error value */
	    $<bval>$=mxslt_opr_cmp_call($2, mxslt_yy_document, $1, $3);

            if($1)
	      xfree($1);
	    if($3)
	      xfree($3);

	      /* In case of error, just leave! */
	    if($<bval>$ < 0) {
	      mxslt_yy_accept(MXSLT_ERROR);
	      YYABORT;
            }
	  } 
	  | TOKEN_LIT {
	      /* TRUE if found not null and not void */
	    if($1 && *$1)
	      $<bval>$=MXSLT_TRUE;
	    else
	      $<bval>$=MXSLT_FALSE;
            
	    if($1)
	      xfree($1);
	  }
	  | error { $$=MXSLT_FALSE; YYABORT; } 
;
