/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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

#ifndef YY_OP_OP_V2_PARSER_HPP_INCLUDED
# define YY_OP_OP_V2_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int op_debug;
#endif
/* "%code requires" blocks.  */
#line 29 "op_v2_parser.y" /* yacc.c:1909  */

#include "types.h"
#include "OP_Interface.hpp"

extern void op_get_scanner_input( void * yyscanner, t_str & str );
extern void op_start_URL( void * yyscanner );
extern void op_start_UserAgent( void * yyscanner );
extern void op_start_time( void * yyscanner );

extern void op_pop_state( void * yyscanner );

using namespace NCBI::Logging;

#line 58 "op_v2_parser.hpp" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    STR = 258,
    MONTH = 259,
    IPV4 = 260,
    IPV6 = 261,
    FLOAT = 262,
    METHOD = 263,
    VERS = 264,
    QSTR = 265,
    QSTR_ESC = 266,
    SPACE = 267,
    SLASH = 268,
    QMARK = 269,
    I64 = 270,
    DOT = 271,
    DASH = 272,
    COLON = 273,
    QUOTE = 274,
    OB = 275,
    CB = 276,
    PORT = 277,
    RL = 278,
    CR = 279,
    LF = 280,
    UNRECOGNIZED = 281,
    PATHSTR = 282,
    PATHEXT = 283,
    ACCESSION = 284,
    OS = 285,
    SRA_TOOLKIT = 286,
    LIBCVERSION = 287,
    AGENTSTR = 288,
    SRATOOLVERS = 289,
    PAREN_OPEN = 290,
    PAREN_CLOSE = 291,
    COMMA = 292,
    PHIDVALUE = 293,
    TIME_FMT = 294
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 44 "op_v2_parser.y" /* yacc.c:1909  */

    t_str s;
    t_timepoint tp;
    t_request req;
    t_agent agent;

#line 117 "op_v2_parser.hpp" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int op_parse (void * scanner, NCBI::Logging::OPReceiver * lib);

#endif /* !YY_OP_OP_V2_PARSER_HPP_INCLUDED  */
