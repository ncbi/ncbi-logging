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

#ifndef YY_GCP_GCP_V2_PARSER_HPP_INCLUDED
# define YY_GCP_GCP_V2_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int gcp_debug;
#endif
/* "%code requires" blocks.  */
#line 27 "gcp_v2_parser.y" /* yacc.c:1909  */

#include "types.h"
#include "GCP_Interface.hpp"

extern void gcp_get_scanner_input( void * yyscanner, t_str & str );
extern void gcp_start_URL( void * yyscanner );
extern void gcp_start_UserAgent( void * yyscanner );
extern void gcp_pop_state( void * yyscanner );

using namespace NCBI::Logging;


#line 57 "gcp_v2_parser.hpp" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    IPV4 = 258,
    IPV6 = 259,
    QSTR = 260,
    I64 = 261,
    PATHSTR = 262,
    PATHEXT = 263,
    ACCESSION = 264,
    SLASH = 265,
    EQUAL = 266,
    AMPERSAND = 267,
    QMARK = 268,
    PERCENT = 269,
    QUOTE = 270,
    COMMA = 271,
    UNRECOGNIZED = 272,
    OS = 273,
    SRA_TOOLKIT = 274,
    LIBCVERSION = 275,
    AGENTSTR = 276,
    SRATOOLVERS = 277,
    PHIDVALUE = 278
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 41 "gcp_v2_parser.y" /* yacc.c:1909  */

    t_str s;
    t_request req;
    t_agent agent;

#line 99 "gcp_v2_parser.hpp" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int gcp_parse (void * scanner, NCBI::Logging::GCPReceiver * lib);

#endif /* !YY_GCP_GCP_V2_PARSER_HPP_INCLUDED  */
