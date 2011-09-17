/* A Bison parser, made by GNU Bison 1.875d.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOKEN_STRING = 258,
     TOKEN_SCREEN = 259,
     TOKEN_ALL = 260,
     TOKEN_ANY = 261,
     TOKEN_AND = 262,
     TOKEN_LIT = 263,
     TOKEN_OPR_BOOL = 264,
     TOKEN_OPR_CMP = 265,
     EOS = 266
   };
#endif
#define TOKEN_STRING 258
#define TOKEN_SCREEN 259
#define TOKEN_ALL 260
#define TOKEN_ANY 261
#define TOKEN_AND 262
#define TOKEN_LIT 263
#define TOKEN_OPR_BOOL 264
#define TOKEN_OPR_CMP 265
#define EOS 266




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 39 "../../../lib/parser/modxslt-screen-expr.y"
typedef union YYSTYPE {
  int bval;
  char * string;
  const mxslt_opr_t * operator;
} YYSTYPE;
/* Line 1285 of yacc.c.  */
#line 65 "../../../lib/parser/modxslt-screen-expr.parser.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



#if ! defined (YYLTYPE) && ! defined (YYLTYPE_IS_DECLARED)
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif




