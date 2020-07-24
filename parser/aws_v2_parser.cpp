/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         aws_parse
#define yylex           aws_lex
#define yyerror         aws_error
#define yydebug         aws_debug
#define yynerrs         aws_nerrs


/* Copy the first part of user declarations.  */
#line 10 "aws_v2_parser.y" /* yacc.c:339  */

#define YYDEBUG 1

#include <stdint.h>

//#include "AWS_Interface.hpp"
#include "aws_v2_parser.hpp"
#include "aws_v2_scanner.hpp"
#include "helper.hpp"

using namespace std;
using namespace NCBI::Logging;

void aws_error( yyscan_t locp, NCBI::Logging::AWS_LogLinesInterface * lib, const char* msg );


#line 89 "aws_v2_parser.cpp" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "aws_v2_parser.hpp".  */
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
#line 28 "aws_v2_parser.y" /* yacc.c:355  */

#include "types.h"
#include "AWS_Interface.hpp"

extern void aws_get_scanner_input( void * yyscanner, t_str & str );

extern void aws_start_URL( void * yyscanner );
extern void aws_start_UserAgent( void * yyscanner );
extern void aws_start_TLS_vers( void * yyscanner );
extern void aws_start_host_id( void * yyscanner );

extern void aws_pop_state( void * yyscanner );

using namespace NCBI::Logging;

#line 135 "aws_v2_parser.cpp" /* yacc.c:355  */

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
    COLON = 280,
    QUOTE = 281,
    OB = 282,
    CB = 283,
    UNRECOGNIZED = 284,
    OS = 285,
    SRA_TOOLKIT = 286,
    LIBCVERSION = 287,
    AGENTSTR = 288,
    SRATOOLVERS = 289,
    PHIDVALUE = 290
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 45 "aws_v2_parser.y" /* yacc.c:355  */

    t_str s;
    t_timepoint tp;
    t_request req;
    t_agent agent;

#line 190 "aws_v2_parser.cpp" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int aws_parse (void * scanner, NCBI::Logging::AWS_LogLinesInterface * lib);

#endif /* !YY_AWS_AWS_V2_PARSER_HPP_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 206 "aws_v2_parser.cpp" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  12
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   196

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  36
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  49
/* YYNRULES -- Number of rules.  */
#define YYNRULES  100
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  182

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   290

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    72,    72,    73,    84,    93,    95,    98,   100,   105,
     105,    77,   161,   162,   166,   167,   168,   172,   181,   182,
     183,   184,   185,   188,   189,   190,   191,   192,   193,   194,
     195,   199,   207,   237,   238,   241,   244,   245,   249,   250,
     254,   255,   259,   260,   263,   265,   273,   278,   284,   285,
     288,   289,   290,   291,   294,   295,   298,   299,   300,   304,
     305,   309,   310,   329,   330,   331,   332,   333,   334,   341,
     342,   348,   356,   388,   388,   392,   398,   403,   408,   413,
     413,   417,   421,   422,   426,   427,   431,   432,   443,   444,
     445,   446,   450,   451,   482,   490,   501,   505,   509,   516,
     526
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "STR", "STR1", "MONTH", "IPV4", "IPV6",
  "METHOD", "VERS", "QSTR", "DASH", "I64", "AMPERSAND", "EQUAL", "PERCENT",
  "SLASH", "QMARK", "PATHSTR", "PATHEXT", "ACCESSION", "SPACE",
  "TLS_VERSION", "X_AMZ_ID_2", "S3_EXT_REQ_ID", "COLON", "QUOTE", "OB",
  "CB", "UNRECOGNIZED", "OS", "SRA_TOOLKIT", "LIBCVERSION", "AGENTSTR",
  "SRATOOLVERS", "PHIDVALUE", "$accept", "line", "log_aws", "$@1", "$@2",
  "$@3", "$@4", "$@5", "$@6", "$@7", "dash", "string_or_dash",
  "log_aws_err", "aws_owner", "aws_bucket", "aws_requester",
  "aws_request_id", "aws_operation", "key_token", "aws_key",
  "aws_quoted_key", "aws_error", "aws_bytes_sent", "aws_obj_size",
  "aws_total_time", "aws_turnaround_time", "aws_version_id", "x_amz_id_2",
  "aws_host_id", "aws_sig", "aws_cipher", "aws_auth", "aws_host_hdr",
  "aws_tls_vers", "ip", "method", "qstr_list", "url_token", "url_list",
  "url", "$@8", "request", "$@9", "result_code", "referer",
  "vdb_agent_token", "vdb_agent", "agent", "time", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290
};
# endif

#define YYPACT_NINF -82

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-82)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
       0,   -82,   -82,   -82,    -3,    15,   -82,   -82,   -82,   -82,
      46,    27,   -82,   -82,     0,   -82,   -82,    20,    38,    42,
     -82,    36,    47,    33,    67,   -82,   -82,   -82,    60,    72,
       0,    79,   -82,    71,    73,     0,    82,   -82,    75,    80,
       0,    96,   -82,    88,    89,   -82,   101,   114,   109,   -82,
     -82,   -82,   -82,   -82,   -82,   -82,   -82,   126,   -82,   149,
     117,   111,   123,   -82,    -4,   -82,   -82,   -82,    40,   129,
     -82,   -82,    -9,   157,    -2,   -82,   -82,    50,   -82,   -82,
     -82,   -82,   -82,   -82,   -82,   -82,   -82,   128,   -82,   -82,
     131,   157,    43,   -82,   -82,   -82,     0,   157,   127,   -82,
     -82,   135,   -82,    16,   -82,   -82,   136,    32,   -82,   -82,
     139,    34,   -82,   -82,   158,    35,   -82,   -82,   159,    10,
      76,   -82,   -82,   -82,    52,   160,   168,   -82,    44,   -82,
      -1,   -82,   -82,   -82,   -82,    91,   -82,   -82,   -82,   -82,
     -82,   -82,    69,   161,    85,   -82,   -82,   -82,     0,   -82,
     -82,   162,   -82,    51,   163,   164,   165,   166,   -82,   -82,
     -82,   -82,   167,     0,   -82,   -82,   170,     0,   -82,   171,
       0,   -82,   172,     0,   -82,   173,   -82,    57,   -82,   -82,
     -82,   -82
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    14,    15,    12,     0,     0,     2,    16,    18,     3,
       0,     0,     1,    17,     0,    13,    19,     0,     0,     0,
     100,     0,     0,     0,     0,    56,    57,    58,     0,     0,
       0,     0,    20,     0,     0,     0,     0,    21,     0,     0,
       0,     0,    22,     0,     0,     4,     0,     0,     0,    25,
      24,    27,    23,    26,    29,    30,    28,     0,    31,    33,
       0,     0,     0,    32,     0,    99,    34,    81,    79,     0,
      59,    60,     0,     0,     0,    73,    78,     0,    65,    64,
      67,    63,    66,    69,    70,    68,    71,     0,    82,    83,
       0,     0,     0,    77,    80,    72,     0,    74,     0,    76,
      35,     0,    75,     0,    36,    37,     0,     0,    38,    39,
       0,     0,    40,    41,     0,     0,    42,    43,     0,     0,
       0,    85,     5,    61,     0,     0,     0,    84,     0,    62,
       0,    98,     6,    90,    97,     0,    86,    88,    91,    87,
      89,    92,     0,     0,     0,    96,    94,    93,     0,    95,
      44,     0,     7,     0,     0,     0,     0,    47,     8,    45,
      48,    49,     0,     0,    46,    50,     0,     0,    51,     0,
       0,    52,     0,     0,    53,     0,     9,     0,    54,    55,
      10,    11
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,
     -18,   -12,   -82,   -82,   -82,   -82,   -82,   -82,   130,   -82,
     -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,   -82,
     -82,   -82,   -82,   -82,   -82,   -82,   -82,   -81,   104,   -82,
     -82,   -82,   -82,   -82,   -82,   -62,    61,   -82,   -82
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     5,     6,    47,   125,   143,   153,   163,   177,   181,
       7,     8,     9,    10,    17,    33,    38,    43,    58,    59,
      60,   101,   106,   110,   114,   118,   151,   157,   158,   166,
     169,   172,   175,   180,    28,    72,   124,    86,    87,    77,
      91,    69,    73,    90,   122,   141,   142,   132,    21
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      20,    88,    16,     1,     2,    27,    95,    67,    11,     3,
      11,     3,    75,     1,     2,    12,    95,    76,    32,   104,
     133,     3,    68,    37,     4,   134,     4,     3,    42,   135,
     136,   137,   138,   139,   140,   108,   120,   112,   116,    25,
      26,    18,     4,     3,     3,     3,     3,    13,    70,     3,
      71,     3,    98,    15,    22,     3,    89,    23,     4,     4,
       4,     4,     3,    24,     4,    19,     4,    14,     3,    99,
     130,    92,    29,   126,   154,   155,    93,     4,   127,   178,
     147,    30,   147,     4,   100,   105,   123,    11,    31,   109,
     133,    34,    35,   113,    39,   145,    40,   117,    36,   146,
     136,   137,   138,   139,   140,    41,   133,   121,    44,    45,
     131,   149,   133,    48,    46,   146,   136,   137,   138,   139,
     140,    61,   136,   137,   138,   139,   140,    49,    50,    51,
      52,    53,    54,    55,    56,   156,   150,    62,    64,    65,
      57,    78,    79,    80,    81,    82,    83,    84,    85,    66,
      74,   165,    96,   102,    94,   168,   103,   107,   171,   179,
     111,   174,    49,    50,    51,    52,    53,    54,    55,    56,
      78,    79,    80,    81,    82,    83,    84,    85,   129,   115,
     119,   128,   148,   152,   159,   160,   161,     0,   164,    63,
     162,   167,   170,   173,   176,    97,   144
};

static const yytype_int16 yycheck[] =
{
      18,     3,    14,     3,     4,    23,    87,    11,    11,    11,
      11,    11,    21,     3,     4,     0,    97,    26,    30,     3,
      21,    11,    26,    35,    26,    26,    26,    11,    40,    30,
      31,    32,    33,    34,    35,     3,    26,     3,     3,     6,
       7,    21,    26,    11,    11,    11,    11,     1,     8,    11,
      68,    11,     9,    26,    12,    11,    74,    21,    26,    26,
      26,    26,    11,    16,    26,    27,    26,    21,    11,    26,
      26,    21,     5,    21,    23,    24,    26,    26,    26,    22,
     142,    21,   144,    26,    96,   103,    10,    11,    16,   107,
      21,    12,    21,   111,    12,    26,    21,   115,    25,    30,
      31,    32,    33,    34,    35,    25,    21,   119,    12,    21,
     128,    26,    21,    12,    25,    30,    31,    32,    33,    34,
      35,    12,    31,    32,    33,    34,    35,    13,    14,    15,
      16,    17,    18,    19,    20,   153,   148,    11,    21,    28,
      26,    13,    14,    15,    16,    17,    18,    19,    20,    26,
      21,   163,    21,    26,    26,   167,    21,    21,   170,   177,
      21,   173,    13,    14,    15,    16,    17,    18,    19,    20,
      13,    14,    15,    16,    17,    18,    19,    20,    10,    21,
      21,    21,    21,    21,    21,    21,    21,    -1,    21,    59,
      24,    21,    21,    21,    21,    91,   135
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,    11,    26,    37,    38,    46,    47,    48,
      49,    11,     0,     1,    21,    26,    47,    50,    21,    27,
      46,    84,    12,    21,    16,     6,     7,    46,    70,     5,
      21,    16,    47,    51,    12,    21,    25,    47,    52,    12,
      21,    25,    47,    53,    12,    21,    25,    39,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    26,    54,    55,
      56,    12,    11,    54,    21,    28,    26,    11,    26,    77,
       8,    46,    71,    78,    21,    21,    26,    75,    13,    14,
      15,    16,    17,    18,    19,    20,    73,    74,     3,    46,
      79,    76,    21,    26,    26,    73,    21,    74,     9,    26,
      47,    57,    26,    21,     3,    46,    58,    21,     3,    46,
      59,    21,     3,    46,    60,    21,     3,    46,    61,    21,
      26,    47,    80,    10,    72,    40,    21,    26,    21,    10,
      26,    46,    83,    21,    26,    30,    31,    32,    33,    34,
      35,    81,    82,    41,    82,    26,    30,    81,    21,    26,
      47,    62,    21,    42,    23,    24,    46,    63,    64,    21,
      21,    21,    24,    43,    21,    47,    65,    21,    47,    66,
      21,    47,    67,    21,    47,    68,    21,    44,    22,    46,
      69,    45
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    36,    37,    37,    39,    40,    41,    42,    43,    44,
      45,    38,    46,    46,    47,    47,    47,    48,    49,    50,
      51,    52,    53,    54,    54,    54,    54,    54,    54,    54,
      54,    55,    55,    56,    56,    57,    58,    58,    59,    59,
      60,    60,    61,    61,    62,    63,    64,    64,    64,    64,
      65,    66,    67,    68,    69,    69,    70,    70,    70,    71,
      71,    72,    72,    73,    73,    73,    73,    73,    73,    73,
      73,    74,    74,    76,    75,    77,    77,    77,    77,    78,
      77,    77,    79,    79,    80,    80,    81,    81,    81,    81,
      81,    81,    82,    82,    82,    83,    83,    83,    83,    84,
      84
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     0,     0,     0,     0,     0,     0,
       0,    53,     1,     3,     1,     1,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     3,     1,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     0,     3,     6,     5,     4,     3,     0,
       4,     1,     1,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     4,     3,     2,     1,    14,
       1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (scanner, lib, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, scanner, lib); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void * scanner, NCBI::Logging::AWS_LogLinesInterface * lib)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (scanner);
  YYUSE (lib);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void * scanner, NCBI::Logging::AWS_LogLinesInterface * lib)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, scanner, lib);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, void * scanner, NCBI::Logging::AWS_LogLinesInterface * lib)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              , scanner, lib);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, scanner, lib); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, void * scanner, NCBI::Logging::AWS_LogLinesInterface * lib)
{
  YYUSE (yyvaluep);
  YYUSE (scanner);
  YYUSE (lib);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void * scanner, NCBI::Logging::AWS_LogLinesInterface * lib)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 72 "aws_v2_parser.y" /* yacc.c:1646  */
    { YYACCEPT; }
#line 1429 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 3:
#line 73 "aws_v2_parser.y" /* yacc.c:1646  */
    { YYACCEPT; }
#line 1435 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 4:
#line 84 "aws_v2_parser.y" /* yacc.c:1646  */
    { aws_start_URL( scanner ); }
#line 1441 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 5:
#line 93 "aws_v2_parser.y" /* yacc.c:1646  */
    { aws_start_UserAgent( scanner ); }
#line 1447 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 6:
#line 95 "aws_v2_parser.y" /* yacc.c:1646  */
    { aws_pop_state( scanner ); }
#line 1453 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 7:
#line 98 "aws_v2_parser.y" /* yacc.c:1646  */
    { aws_start_host_id( scanner ); }
#line 1459 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 8:
#line 100 "aws_v2_parser.y" /* yacc.c:1646  */
    { aws_pop_state( scanner ); /* the following space is consumed by aws_host_id */ }
#line 1465 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 9:
#line 105 "aws_v2_parser.y" /* yacc.c:1646  */
    { aws_start_TLS_vers( scanner ); }
#line 1471 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 10:
#line 105 "aws_v2_parser.y" /* yacc.c:1646  */
    { aws_pop_state( scanner ); }
#line 1477 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 11:
#line 106 "aws_v2_parser.y" /* yacc.c:1646  */
    {

/*        
        LogAWSEvent ev;
        ev . owner = $1;
        ev . bucket = $3;
        ev . time = $5;
        ev . ip = $7;
        ev . requester = $9;
        ev . request_id = $11;
        ev . operation = $13;

        ev . request = $18; // this has to happen before we use the key-decomposition
        // combine data from aws_key and request
        if ( $16 . path . n == 1 && $16 . path . p[ 0 ] == '-' )
        {
            EMPTY_TSTR( ev . key );
        }
        else if ( $16 . path . n == 3 && $16 . path . p[ 0 ] == '\"' && $16 . path . p[ 1 ] == '-' && $16 . path . p[ 2 ] == '\"' )
        {
            EMPTY_TSTR( ev . key );
        }
        else
        {
            ev . key = $16 . path;
        }
        if ( $16 . accession . n > 0 )
        {
            ev . request . accession = $16 . accession;
            ev . request . filename  = $16 . filename;
            ev . request . extension = $16 . extension;
        }

        ev . res_code           = $20;
        ev . error              = $22;
        ev . res_len            = $24;
        ev . obj_size           = $26;
        ev . total_time         = $28;
        ev . turnaround_time    = $30;
        ev . referer            = $32;
        ev . agent              = $35;
        ev . version_id         = $38;
        ev . host_id            = $41;
        ev . sig_ver            = $43;
        ev . cipher_suite       = $45;
        ev . auth_type          = $47;
        ev . host_header        = $49;
        ev . tls_version        = $52;
        
        lib -> acceptLine( ev );
*/        
    }
#line 1534 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 12:
#line 161 "aws_v2_parser.y" /* yacc.c:1646  */
    { EMPTY_TSTR((yyval.s)); }
#line 1540 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 13:
#line 162 "aws_v2_parser.y" /* yacc.c:1646  */
    { EMPTY_TSTR((yyval.s)); }
#line 1546 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 17:
#line 173 "aws_v2_parser.y" /* yacc.c:1646  */
    {
        // LogAWSEvent ev;
        // ev . owner = $1;
        // aws_get_scanner_input( scanner, ev . unparsed );
        // lib -> rejectLine( ev );
    }
#line 1557 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 23:
#line 188 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); }
#line 1563 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 24:
#line 189 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); }
#line 1569 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 25:
#line 190 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); }
#line 1575 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 26:
#line 191 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); }
#line 1581 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 27:
#line 192 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); }
#line 1587 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 28:
#line 193 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); (yyval.req) . accession = (yyvsp[0].s); }
#line 1593 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 29:
#line 194 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); (yyval.req) . filename = (yyvsp[0].s); }
#line 1599 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 30:
#line 195 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); (yyval.req) . extension = (yyvsp[0].s); }
#line 1605 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 31:
#line 200 "aws_v2_parser.y" /* yacc.c:1646  */
    {
            (yyval.req) = (yyvsp[0].req);
            if ( (yyvsp[0].req) . accession . n > 0 )
            {
                (yyval.req) . filename  = (yyvsp[0].req) . accession;
            }
        }
#line 1617 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 32:
#line 208 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            (yyval.req) . path . n += (yyvsp[0].req) . path . n;

            // clear the filename and extension after every slash - to make sure we catch only the last
            // filename and extension in case of middle segments existing
            if ( (yyvsp[0].req) . path . n == 1 && (yyvsp[0].req) . path . p[ 0 ] == '/' )
            {
                (yyval.req) . filename . n = 0;
                (yyval.req) . extension . n = 0;
            }
            else if ( (yyvsp[0].req) . accession . n > 0 )
            {
                // we will use the last non-empty accession-looking token
                (yyval.req) . accession = (yyvsp[0].req) . accession;
                (yyval.req) . filename  = (yyvsp[0].req) . accession;
            }
            else if ( (yyvsp[0].req) . filename . n > 0 )
            {
                (yyval.req) . filename  = (yyvsp[0].req) . filename;
                (yyval.req) . extension . n = 0;
            }
            else if ( (yyvsp[0].req) . extension . n > 0 )
            {
                (yyval.req) . extension = (yyvsp[0].req) . extension;
            }
        }
#line 1648 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 34:
#line 238 "aws_v2_parser.y" /* yacc.c:1646  */
    {  InitRequest( (yyval.req) ); }
#line 1654 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 45:
#line 266 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            (yyval.s) = (yyvsp[-1].s);
            MERGE_TSTR( (yyval.s) , (yyvsp[0].s) );
        }
#line 1663 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 46:
#line 274 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            (yyval.s) = (yyvsp[-2].s); // keep the space between the 2 parts of the Id
            MERGE_TSTR( (yyval.s) , (yyvsp[-1].s) );
        }
#line 1672 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 47:
#line 279 "aws_v2_parser.y" /* yacc.c:1646  */
    {
            (yyval.s) = (yyvsp[0].s);
            // trim the trailing space
            (yyval.s) . n --;
        }
#line 1682 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 61:
#line 309 "aws_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 1688 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 62:
#line 310 "aws_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[-2].s); (yyval.s).n += 1 + (yyvsp[0].s).n; (yyval.s).escaped = (yyvsp[-2].s).escaped || (yyvsp[0].s).escaped; }
#line 1694 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 63:
#line 329 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); }
#line 1700 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 64:
#line 330 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); (yyval.req).accession_mode = acc_after; }
#line 1706 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 65:
#line 331 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); (yyval.req).accession_mode = acc_after; }
#line 1712 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 66:
#line 332 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); (yyval.req).accession_mode = acc_after; }
#line 1718 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 67:
#line 333 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); }
#line 1724 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 68:
#line 335 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            InitRequest( (yyval.req) ); 
            (yyval.req) . path = (yyvsp[0].s); 
            (yyval.req) . accession = (yyvsp[0].s); 
            (yyval.req) . accession_mode = acc_inside; 
        }
#line 1735 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 69:
#line 341 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); (yyval.req) . filename = (yyvsp[0].s); }
#line 1741 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 70:
#line 342 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); (yyval.req) . extension = (yyvsp[0].s); }
#line 1747 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 71:
#line 349 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            (yyval.req) = (yyvsp[0].req); 
            if ( (yyvsp[0].req) . accession_mode == acc_after )            
            {   /* a delimiter seen before an accession */
                (yyval.req) . accession_mode = acc_before;
            }
        }
#line 1759 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 72:
#line 357 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            (yyval.req) = (yyvsp[-1].req); 
            MERGE_TSTR( (yyval.req) . path, (yyvsp[0].req) . path );
            switch ( (yyval.req).accession_mode )
            {
            case acc_before:
                if ( (yyvsp[0].req).accession_mode == acc_inside )
                {
                    (yyval.req) . accession = (yyvsp[0].req) . accession;
                    (yyval.req) . accession_mode = acc_inside;
                }
                break;
            case acc_inside:
                switch ( (yyvsp[0].req) . accession_mode )
                {
                case acc_inside:
                    (yyval.req) . filename  = (yyvsp[0].req) . accession;
                    break;
                case acc_after:
                    (yyval.req) . accession_mode = acc_after;
                    break;
                default:
                    if ( (yyvsp[0].req) . filename . n > 0 )    (yyval.req) . filename  = (yyvsp[0].req) . filename;
                    if ( (yyvsp[0].req) . extension . n > 0 )   (yyval.req) . extension = (yyvsp[0].req) . extension;
                }
                break;
            }
        }
#line 1792 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 73:
#line 388 "aws_v2_parser.y" /* yacc.c:1646  */
    { aws_start_URL( scanner ); }
#line 1798 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 74:
#line 388 "aws_v2_parser.y" /* yacc.c:1646  */
    { (yyval.req) = (yyvsp[0].req); }
#line 1804 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 75:
#line 393 "aws_v2_parser.y" /* yacc.c:1646  */
    {
        (yyval.req) = (yyvsp[-3].req);
        (yyval.req).method = (yyvsp[-4].s);
        (yyval.req).vers   = (yyvsp[-1].s);
    }
#line 1814 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 76:
#line 399 "aws_v2_parser.y" /* yacc.c:1646  */
    {
        (yyval.req) = (yyvsp[-2].req);
        (yyval.req).method = (yyvsp[-3].s);
     }
#line 1823 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 77:
#line 404 "aws_v2_parser.y" /* yacc.c:1646  */
    {
        (yyval.req) = (yyvsp[-1].req);
        (yyval.req).method = (yyvsp[-2].s);
    }
#line 1832 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 78:
#line 409 "aws_v2_parser.y" /* yacc.c:1646  */
    {
        InitRequest( (yyval.req) );
        (yyval.req).method = (yyvsp[-1].s);
    }
#line 1841 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 79:
#line 413 "aws_v2_parser.y" /* yacc.c:1646  */
    { aws_start_URL( scanner ); }
#line 1847 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 80:
#line 414 "aws_v2_parser.y" /* yacc.c:1646  */
    {
        (yyval.req) = (yyvsp[-1].req);
    }
#line 1855 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 81:
#line 417 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) );}
#line 1861 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 84:
#line 426 "aws_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[-1].s); }
#line 1867 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 86:
#line 431 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitAgent( (yyval.agent) ); (yyval.agent).original = (yyvsp[0].s); }
#line 1873 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 87:
#line 433 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            InitAgent( (yyval.agent) ); 
            (yyval.agent).original = (yyvsp[0].s); 
            const char * dot = strchr( (yyvsp[0].s) . p, '.' );
            (yyval.agent) . vdb_tool . p = (yyvsp[0].s) . p;
            (yyval.agent) . vdb_tool . n = dot - (yyvsp[0].s) . p;
            /* skip the leading dot */
            (yyval.agent) . vdb_release . p = dot + 1; 
            (yyval.agent) . vdb_release . n = (yyvsp[0].s) . n - ( dot - (yyvsp[0].s) . p ) - 1; 
        }
#line 1888 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 88:
#line 443 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitAgent( (yyval.agent) ); (yyval.agent).original = (yyvsp[0].s); (yyval.agent).vdb_libc = (yyvsp[0].s); }
#line 1894 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 89:
#line 444 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitAgent( (yyval.agent) ); (yyval.agent).original = (yyvsp[0].s); (yyval.agent).vdb_phid_compute_env = (yyvsp[0].s); }
#line 1900 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 90:
#line 445 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitAgent( (yyval.agent) ); (yyval.agent).original = (yyvsp[0].s); }
#line 1906 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 91:
#line 446 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitAgent( (yyval.agent) ); (yyval.agent).original = (yyvsp[0].s); }
#line 1912 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 92:
#line 450 "aws_v2_parser.y" /* yacc.c:1646  */
    { (yyval.agent) = (yyvsp[0].agent); }
#line 1918 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 93:
#line 452 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
        (yyval.agent) = (yyvsp[-1].agent);
        MERGE_TSTR( (yyval.agent) . original, (yyvsp[0].agent) . original );
        if ( (yyvsp[0].agent) . vdb_phid_compute_env . n > 0 )
        {
            (yyval.agent) . vdb_phid_compute_env . p = (yyvsp[0].agent) . vdb_phid_compute_env . p + 5;
            (yyval.agent) . vdb_phid_compute_env . n = 3;

            /*
                the whole token is either 14 or 15 chars long!
                guid is either 3 or 4 chars long, the other parts have fixed length.
            */
            int guid_len = (yyvsp[0].agent) . vdb_phid_compute_env . n - 11;
            (yyval.agent) . vdb_phid_guid . p = (yyvsp[0].agent) . vdb_phid_compute_env . p + 8;
            (yyval.agent) . vdb_phid_guid . n = guid_len;

            (yyval.agent) . vdb_phid_session_id . p = (yyval.agent) . vdb_phid_guid . p + guid_len;
            (yyval.agent) . vdb_phid_session_id . n = 3;
        }
        else if ( (yyvsp[0].agent) . vdb_libc . n > 0 )
        {
            (yyval.agent) . vdb_libc . p = (yyvsp[0].agent) . vdb_libc . p + 5;
            (yyval.agent) . vdb_libc . n = (yyvsp[0].agent) . vdb_libc . n - 5;
        }
        else if ( (yyvsp[0].agent) . vdb_tool . n > 0 )
        {
            (yyval.agent) . vdb_tool = (yyvsp[0].agent) . vdb_tool;
            (yyval.agent) . vdb_release = (yyvsp[0].agent) . vdb_release;
        }
    }
#line 1953 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 94:
#line 483 "aws_v2_parser.y" /* yacc.c:1646  */
    {
        (yyval.agent) = (yyvsp[-1].agent);
        MERGE_TSTR( (yyval.agent) . original, (yyvsp[0].s) );
    }
#line 1962 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 95:
#line 491 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            t_agent temp;
            InitAgent( temp ); 
            temp . original = (yyvsp[-2].s);
            MERGE_TSTR( temp . original, (yyvsp[-1].agent) . original );
            (yyval.agent) = (yyvsp[-1].agent);
            (yyval.agent) . original = temp . original;

            (yyval.agent).vdb_os = (yyvsp[-2].s);
        }
#line 1977 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 96:
#line 502 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            (yyval.agent) = (yyvsp[-1].agent);
        }
#line 1985 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 97:
#line 506 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            InitAgent( (yyval.agent) ); 
        }
#line 1993 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 98:
#line 510 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            InitAgent( (yyval.agent) ); 
        }
#line 2001 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 99:
#line 517 "aws_v2_parser.y" /* yacc.c:1646  */
    {
        (yyval.tp).day = atoi( (yyvsp[-12].s).p );
        (yyval.tp).month = month_int( &((yyvsp[-10].s)) );
        (yyval.tp).year = atoi( (yyvsp[-8].s).p );
        (yyval.tp).hour = atoi( (yyvsp[-6].s).p );
        (yyval.tp).minute = atoi( (yyvsp[-4].s).p );
        (yyval.tp).second = atoi( (yyvsp[-2].s).p );
        (yyval.tp).offset = atoi( (yyvsp[-1].s).p );
    }
#line 2015 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 100:
#line 526 "aws_v2_parser.y" /* yacc.c:1646  */
    { memset( &((yyval.tp)), 0, sizeof (yyval.tp) ); }
#line 2021 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;


#line 2025 "aws_v2_parser.cpp" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (scanner, lib, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (scanner, lib, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, scanner, lib);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, scanner, lib);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (scanner, lib, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, scanner, lib);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, scanner, lib);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 529 "aws_v2_parser.y" /* yacc.c:1906  */


void aws_error( yyscan_t locp, NCBI::Logging::AWS_LogLinesInterface * lib, const char * msg )
{
    // intentionally left empty, we communicate errors rejected lines
}
