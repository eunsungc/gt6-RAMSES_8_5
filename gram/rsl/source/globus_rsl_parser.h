/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_GLOBUS_RSL_GLOBUS_RSL_PARSER_H_INCLUDED
# define YY_GLOBUS_RSL_GLOBUS_RSL_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int globus_rsldebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    RSL_STRING = 258,
    RSL_OP_AND = 259,
    RSL_OP_OR = 260,
    RSL_OP_MULTIREQ = 261,
    RSL_OP_EQ = 262,
    RSL_OP_NEQ = 263,
    RSL_OP_LT = 264,
    RSL_OP_LTEQ = 265,
    RSL_OP_GT = 266,
    RSL_OP_GTEQ = 267,
    RSL_OP_CONCATENATE = 268,
    RSL_LPAREN = 269,
    RSL_RPAREN = 270,
    RSL_VARIABLE_START = 271
  };
#endif
/* Tokens.  */
#define RSL_STRING 258
#define RSL_OP_AND 259
#define RSL_OP_OR 260
#define RSL_OP_MULTIREQ 261
#define RSL_OP_EQ 262
#define RSL_OP_NEQ 263
#define RSL_OP_LT 264
#define RSL_OP_LTEQ 265
#define RSL_OP_GT 266
#define RSL_OP_GTEQ 267
#define RSL_OP_CONCATENATE 268
#define RSL_LPAREN 269
#define RSL_RPAREN 270
#define RSL_VARIABLE_START 271

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 61 "../../../../gram/rsl/source/globus_rsl_parser.y" /* yacc.c:1909  */

  int	               Int;
  char               * String;

  globus_rsl_t       * RSL;
  globus_rsl_value_t * RSLval;
  globus_list_t      * List;

#line 95 "globus_rsl_parser.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



int globus_rslparse (void * scanner, struct globus_parse_state_s * parse_state);

#endif /* !YY_GLOBUS_RSL_GLOBUS_RSL_PARSER_H_INCLUDED  */
