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

#ifndef YY_AWS_AWS_V2_PARSER_HPP_INCLUDED
# define YY_AWS_AWS_V2_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int aws_debug;
#endif
/* "%code requires" blocks.  */
#line 29 "aws_v2_parser.y" /* yacc.c:1909  */

#include "types.h"
#include "AWS_Interface.hpp"

extern void aws_get_scanner_input( void * yyscanner, t_str & str );

extern void aws_start_URL( void * yyscanner );
extern void aws_start_key( void * yyscanner );
extern void aws_start_UserAgent( void * yyscanner );
extern void aws_start_TLS_vers( void * yyscanner );
extern void aws_start_host_id( void * yyscanner );
extern void aws_start_time( void * yyscanner );
extern void aws_start_ipaddr( void * yyscanner );
extern void aws_start_rescode( void * yyscanner );
extern void aws_start_referer( void * yyscanner );

extern void aws_pop_state( void * yyscanner );

using namespace NCBI::Logging;

#line 65 "aws_v2_parser.hpp" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    STR = 258,
    STR1 = 259,
    MONTH = 260,
    IPV4 = 261,
    IPV6 = 262,
    METHOD = 263,
    VERS = 264,
    QSTR = 265,
    DASH = 266,
    I64 = 267,
    AMPERSAND = 268,
    EQUAL = 269,
    PERCENT = 270,
    SLASH = 271,
    QMARK = 272,
    PATHSTR = 273,
    PATHEXT = 274,
    ACCESSION = 275,
    SPACE = 276,
    TLS_VERSION = 277,
    X_AMZ_ID_2 = 278,
    S3_EXT_REQ_ID = 279,
    TIME_FMT = 280,
    RESULTCODE = 281,
    COLON = 282,
    QUOTE = 283,
    OB = 284,
    CB = 285,
    UNRECOGNIZED = 286,
    OS = 287,
    SRA_TOOLKIT = 288,
    LIBCVERSION = 289,
    AGENTSTR = 290,
    SRATOOLVERS = 291,
    PHIDVALUE = 292
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 51 "aws_v2_parser.y" /* yacc.c:1909  */

    t_str s;
    t_request req;
    t_agent agent;

#line 121 "aws_v2_parser.hpp" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int aws_parse (void * scanner, NCBI::Logging::AWSReceiver * lib);

#endif /* !YY_AWS_AWS_V2_PARSER_HPP_INCLUDED  */
