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

#ifndef YY_TW_TW_V2_PARSER_HPP_INCLUDED
# define YY_TW_TW_V2_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int tw_debug;
#endif
/* "%code requires" blocks.  */
#line 29 "tw_v2_parser.y" /* yacc.c:1909  */

#include "types.h"
#include "TW_Interface.hpp"

extern void tw_get_scanner_input( void * yyscanner, t_str & str );
extern void tw_start_ID1( void * yyscanner );
extern void tw_start_ID2( void * yyscanner );
extern void tw_start_ID3( void * yyscanner );
extern void tw_start_TIME( void * yyscanner );
extern void tw_start_SERVER( void * yyscanner );
extern void tw_start_IPADDR( void * yyscanner );
extern void tw_start_SID( void * yyscanner );
extern void tw_start_SERVICE( void * yyscanner );
extern void tw_start_EVENT( void * yyscanner );
extern void tw_start_MSG( void * yyscanner );

extern void tw_pop_state( void * yyscanner );

using namespace NCBI::Logging;

#line 65 "tw_v2_parser.hpp" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ID1 = 258,
    ID2 = 259,
    ID3 = 260,
    TIME = 261,
    SERVER = 262,
    IPADDR = 263,
    SID = 264,
    SERVICE = 265,
    EVENT = 266,
    MSG = 267,
    SEP = 268,
    UNRECOGNIZED = 269
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 51 "tw_v2_parser.y" /* yacc.c:1909  */

    t_str s;

#line 96 "tw_v2_parser.hpp" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int tw_parse (void * scanner, NCBI::Logging::TWReceiver * lib);

#endif /* !YY_TW_TW_V2_PARSER_HPP_INCLUDED  */
