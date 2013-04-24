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

  #include "../modxslt1/modxslt.h"
  #include "modxslt-screen-expr.parser.h"

  #include <stdio.h>
  #include <ctype.h>
  #include <stddef.h>
  
  #define YY_EXTRA_TYPE mxslt_scan_t * 
  #define mxslt_expr_yy_scan_string mxslt_expr_yy__scan_string

  extern int mxslt_expr_yy_parse(void *);

  static int mxslt_yy_str_get(mxslt_doc_t * doc, char ** toret, char * string, int len) {
    int retval;

      /* Remember end of string */
    string[len]='\0';
    retval=mxslt_doc_param_get(doc, string, (char **)toret);

    return retval == MXSLT_TABLE_NOT_FOUND ? MXSLT_FALSE : MXSLT_TRUE;
  }

  # ifndef IS_MAGIC
  #  define IS_MAGIC(ch) ((ch) == '\'' || (ch) == '"' || (ch) == '$')
  # endif

  void mxslt_yy_str_dereference(
      mxslt_doc_t * doc, char ** new_start, char ** new_cur, 
      char ** orig_start, char ** orig_cur, int * gain, size_t * size) {
    char * var_start, * var_cur, * str;
    int bracketed=0, cntr=0;
    size_t len;
    int status;

	     /* Initialize variables */
    var_start=*orig_cur;
    var_cur=*new_cur;

      /* Check if variable is bracketed */
    if(**orig_cur == '{') {
      bracketed=1;
      (*orig_cur)++;
    }

    do {
        /* Go to end of variable, continuing to unescape variables */
	/* XXX: this should really look like the regular expression specified to lex! */
      for(; **orig_cur && (isalnum(**orig_cur) || **orig_cur == '_' || **orig_cur == '.'); (*orig_cur)++) {
        if(**orig_cur == '\\' && IS_MAGIC(*((*orig_cur)+1)))
          *var_cur++=*++(*orig_cur);
        else
          *var_cur++=**orig_cur;
      }

        /* If we get to a }, stop the loop */
      if(**orig_cur == '}') {
        if(bracketed) 
	  (*orig_cur)++;
	
        break;
      }

        /* Switch } */
      switch(**orig_cur) {
        case '[':
	  do { 
            switch(**orig_cur) {
              case '\\':
                if(IS_MAGIC(*(*orig_cur+1)))
                  *var_cur++=*++(*orig_cur);
                else
                  *var_cur++=**orig_cur;
		(*orig_cur)++;
                break;
	      
	      default:
                *var_cur++=**orig_cur;
		(*orig_cur)++;
		break;
  
              case ']':
                *var_cur++=*(*orig_cur)++;
                cntr--;
                break;
  
              case '[':
                *var_cur++=*(*orig_cur)++;
                cntr++;
                break;
  
  	      case '$':
	        if(*(*orig_cur+1)) {
		  (*orig_cur)++;

                    /* new_cur points inside new_start. However, yy_str_dereference may realloc
		     * new_start making new_cur point outside the buffer - change pointer from
		     * absolute to relative and back
		     * XXX: wouldn't it be better to use array and indexes? I'm just too lazy
		     *      right now to change the whole function */
                  (*new_cur)=(char *)(*new_cur - *new_start);
	          mxslt_yy_str_dereference(doc, new_start, &var_cur, orig_start, orig_cur, gain, size);
                  (*new_cur)=*new_start + (ptrdiff_t)(*new_cur);
		}
  	        break;
            } 
	  } while(**orig_cur && cntr != 0);
  
            /* Ok, we got to end of
             * string without finding closing ] */
          if(cntr) {
            *var_cur='\0';
            mxslt_error(doc, "warning - ignoring possibly incomplete variable: %s\n", *new_cur);
            *new_cur=var_cur;
            return;
          }
          break;
  
        case '$':
          if(bracketed && *((*orig_cur)+1))  {
	    (*orig_cur)++;

              /* new_cur points inside new_start. However, yy_str_dereference may realloc
	       * new_start making new_cur point outside the buffer - change pointer from
	       * absolute to relative and back */
            (*new_cur)=(char *)(*new_cur - *new_start);
	    mxslt_yy_str_dereference(doc, new_start, &var_cur, orig_start, orig_cur, gain, size);
            (*new_cur)=*new_start + (ptrdiff_t)(*new_cur);
	  }
          break;
      }
    } while(bracketed);

        /* get variable value */
    status=mxslt_yy_str_get(doc, &str, (*new_cur), var_cur-(*new_cur));
    if(status == MXSLT_TRUE && str) {
      len=strlen(str)-((*orig_cur)-var_start);

        /* Prepare memory */
      if(len > 0 && len > *gain) {
          /* Try to use the memory we 
           * gained earlyer */
        (*size)+=len-*gain;
        (*gain)=0;

          /* Realloc memory - change pointers from absolute to 
           * relative and back */
        (*new_cur)=(char *)(*new_cur - *new_start);
        (*new_start)=(char *)(xrealloc(*new_start, (*size)+1));
        (*new_cur)=*new_start + (ptrdiff_t)(*new_cur);
      } else {
          /* Either decrease the gain by the amount
         * used or increment it by the value we are not using */
        (*gain)-=len;
      }

      memcpy(*new_cur, str, len+((*orig_cur)-var_start));
      (*new_cur)+=len+((*orig_cur)-var_start);
    }
  }

  char * mxslt_yy_str_parse(mxslt_doc_t * doc, char * orig_start, size_t size) {
    char * orig_cur=orig_start;
    char * new_start, * new_cur;
    int gain=0;

      /* Allocate memory */
    orig_start[size]='\0';
    new_start=xmalloc(size+1);

    for(new_cur=new_start; *orig_cur;)  {
      switch (*orig_cur) {
          /* Unescape '\\' */
        case '\\':
          if(IS_MAGIC(*(orig_cur+1))) {
            *new_cur++=*++orig_cur;
	    orig_cur++;
            gain++;
          } else
            *new_cur++=*orig_cur++;
          break;

        case '$':
          if(*(orig_cur+1)) {
	    orig_cur++;
            mxslt_yy_str_dereference(doc, &new_start, &new_cur, &orig_start, &orig_cur, &gain, &size);
	  }
          break;

        default:
          *new_cur++=*orig_cur++;
          break;
      }
    }
    *new_cur='\0';

    return new_start;
  }

%}

%x SCREEN EXPR

%option noyymore
%option noyywrap
%option reentrant

%%
  if(((mxslt_scan_t *)yyextra)->flags & MSF_WITHOUT_MEDIA) {
    BEGIN(EXPR);
  }

{

  "all"/[[:blank:]]* {
    return TOKEN_ALL;
  }

  ("screen")/[[:blank:]]*[^,]* {
    BEGIN(SCREEN);

    return TOKEN_SCREEN;
  }

  [a-zA-Z0-9$_-][^,]* {
    return TOKEN_ANY;
  }

  "," {
    return EOS;
  } 

  [[:blank:]] {}

  <SCREEN>"and" {
    BEGIN(EXPR);

    return TOKEN_AND;
  }

  <SCREEN>"," {
    BEGIN(INITIAL);
    
    return EOS;
  }

  <SCREEN>[^[:blank:],]* {
    return TOKEN_ANY;
  }

  <SCREEN>[[:blank:]] {}

  <EXPR>"!" { 
    return '!'; 
  }

  <EXPR>"(" { 
    return '('; 
  }

  <EXPR>")" { 
    return ')'; 
  }

  <EXPR>("and"|"or") { 
    mxslt_doc_t * doc;

      /* Get document from lexer state */
    doc=((mxslt_scan_t *)yyextra)->document;

      /* Lookup operator in operator table */
    (*yylval).operator = mxslt_opr_bool_lookup(yytext); 
    if(!(*yylval).operator)
      mxslt_error(doc, "unknown operator: %s\n", yytext);

    return TOKEN_OPR_BOOL; 
  }

  <EXPR>("=="|"="|"=~"|"!="|"!~"|">"|"<"|">="|"<=") { 
    mxslt_doc_t * doc;

      /* Get document from lexer state */
    doc=((mxslt_scan_t *)yyextra)->document;

      /* Lookup operator in operator table */
    (*yylval).operator = mxslt_opr_cmp_lookup(yytext); 
    if(!(*yylval).operator)
      mxslt_error(doc, "unknown operator: '%s'\n", yytext);
    
    return TOKEN_OPR_CMP; 
  }

  <EXPR>"," { 
    return EOS; 
  }

  <EXPR>([^$[:blank:]*+%/\"\',!><=~()-]*|"$"[[:blank:]+*%/,!><=~()\"\'-]+[^$[:blank:]*+%/\"\',!><=~()-]*) { 
    (*yylval).string = xstrndup(yytext, yyleng); 

    return TOKEN_LIT; 
  }

  <EXPR>"$"[^[:blank:]+*%/,!><=~()\"\'-]+(\[[^[:space:]+*%/,><=~()]*\]|[^[:blank:]+*%/,!><=~()\"\'-]*) {
    mxslt_scan_t * scan;
						
      /* Get parser state */
    scan=(mxslt_scan_t *)yyextra;


      /* Fill in variable structure */
    mxslt_yy_str_get(scan->document, &((*yylval).string), yytext+1, yyleng-1);
    if((*yylval).string) 
      (*yylval).string=xstrdup((*yylval).string);

    return TOKEN_LIT; 
  }


  <EXPR>\"(([^"]*)(\\\")?)*\"|\'(([^']*)(\\\')?)*\' {     
    mxslt_scan_t * scan;
						
      /* Get parser state */
    scan=(mxslt_scan_t *)yyextra;

      /* Unescape variables and return string */
    (*yylval).string=mxslt_yy_str_parse(scan->document, yytext+1, yyleng-2);
    mxslt_doc_debug_print(
      scan->document, MXSLT_DBG_DEBUG | MXSLT_DBG_PARSER, "string=\"%s\"\n", (*yylval).string);

    return TOKEN_LIT; 
  } 

  <EXPR>[[:blank:]] {}

  <EXPR>[^[:blank:]] {
    mxslt_doc_t * doc;

      /* Get document from lexer state */
    doc=((mxslt_scan_t *)yyextra)->document;
    mxslt_error(doc, "Unknown character: %s\n", yytext);

    yyterminate();
  }
}

%%
  /* Returns:
   *   MXSLT_OK -> if screen conditions are satisfied
   *   MXSLT_ERROR -> in case of error in the screen conditions
   *   MXSLT_SKIP -> if screen conditions are not satisfied */
  int mxslt_doc_screen_check(mxslt_doc_t * document, char ** ptr, int flags) {
    mxslt_scan_t scan; 
    yyscan_t scanner;
    int status;
    
    scan.document=document;
    scan.status=MXSLT_OK;
    scan.flags=flags;

      /* Check parameters */
    if(!ptr || !*ptr)
      return MXSLT_ERROR;

       /* Initialize flex state */
    if(mxslt_expr_yy_lex_init(&scanner) != 0) {
      mxslt_error(document, "yylex_init failed - error: %d\n", errno);
      return MXSLT_ERROR;
    }

      /* Setup lex to use the supplied buffer */
    mxslt_expr_yy_scan_string(*ptr, scanner);

      /* Setup lex to bring our structure with him */
    mxslt_expr_yy_set_extra(&scan, scanner);

      /* Call bison parser */
    status=mxslt_expr_yy_parse(scanner);

      /* Destroy flex state */
    mxslt_expr_yy_lex_destroy(scanner);

    return status ? MXSLT_ERROR : scan.status;
  }
