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

#include "aws_v2_parser.hpp"
#include "aws_v2_scanner.hpp"
#include "helper.hpp"

using namespace std;
using namespace NCBI::Logging;

void aws_error( yyscan_t locp, NCBI::Logging::AWSReceiver * lib, const char* msg );

#define SET_VALUE( selector, source ) ( lib -> set( (selector), (source) ) )


#line 90 "aws_v2_parser.cpp" /* yacc.c:339  */

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
#line 29 "aws_v2_parser.y" /* yacc.c:355  */

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

#line 141 "aws_v2_parser.cpp" /* yacc.c:355  */

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
#line 51 "aws_v2_parser.y" /* yacc.c:355  */

    t_str s;
    t_request req;
    t_agent agent;

#line 197 "aws_v2_parser.cpp" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int aws_parse (void * scanner, NCBI::Logging::AWSReceiver * lib);

#endif /* !YY_AWS_AWS_V2_PARSER_HPP_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 213 "aws_v2_parser.cpp" /* yacc.c:358  */

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
#define YYLAST   164

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  38
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  53
/* YYNRULES -- Number of rules.  */
#define YYNRULES  99
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  166

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   292

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
      35,    36,    37
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    77,    77,    78,    84,    85,    89,    91,    98,   100,
     103,   105,   110,   110,    82,   139,   140,   144,   145,   146,
     150,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   166,   167,   168,   169,   173,   181,   211,   212,
     216,   217,   221,   222,   226,   227,   231,   232,   235,   243,
     249,   256,   260,   267,   268,   272,   273,   274,   278,   279,
     298,   299,   300,   301,   302,   303,   310,   311,   317,   325,
     357,   357,   361,   367,   372,   378,   377,   386,   390,   391,
     395,   396,   400,   400,   406,   413,   414,   425,   426,   427,
     428,   432,   433,   464,   472,   483,   488,   493,   501,   502
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
  "TLS_VERSION", "X_AMZ_ID_2", "S3_EXT_REQ_ID", "TIME_FMT", "RESULTCODE",
  "COLON", "QUOTE", "OB", "CB", "UNRECOGNIZED", "OS", "SRA_TOOLKIT",
  "LIBCVERSION", "AGENTSTR", "SRATOOLVERS", "PHIDVALUE", "$accept", "line",
  "log_aws", "$@1", "$@2", "$@3", "$@4", "$@5", "$@6", "$@7", "$@8", "$@9",
  "$@10", "dash", "string_or_dash", "log_aws_err", "aws_owner",
  "aws_bucket", "aws_requester", "aws_request_id", "aws_operation",
  "aws_error", "aws_version_id", "aws_sig", "aws_cipher", "aws_auth",
  "aws_host_hdr", "key_token", "aws_key", "aws_quoted_key",
  "aws_bytes_sent", "aws_obj_size", "aws_total_time",
  "aws_turnaround_time", "x_amz_id_2", "aws_host_id", "aws_tls_vers", "ip",
  "method", "url_token", "url_list", "url", "$@11", "request", "$@12",
  "result_code", "qstr_list", "referer", "$@13", "vdb_agent_token",
  "vdb_agent", "agent", "time", YY_NULLPTR
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
     285,   286,   287,   288,   289,   290,   291,   292
};
# endif

#define YYPACT_NINF -39

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-39)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
       5,   -39,   -39,   -39,    -7,    10,   -39,   -39,   -39,   -39,
      16,    18,   -39,   -39,     5,   -39,   -39,   -10,   -39,    37,
     -39,   -39,    40,   -39,    43,   -39,   -39,   -39,    59,     5,
     -39,    62,     5,   -39,    66,     5,   -39,    70,   -39,   103,
     -39,   -39,   -39,   -39,    42,   -39,    57,    72,    75,   -39,
      56,   -39,   -39,    30,    83,   -39,   -39,   -16,   -39,   -39,
     -39,    84,    53,   132,    35,    85,   -39,   -39,    93,   -39,
     -39,   -39,   -39,   -39,   -39,   -39,   -39,   -39,   132,    87,
     -39,   -39,     5,   -39,   -39,   -39,    95,     3,   -39,   -39,
      96,    29,   -39,   -39,   108,    31,   -39,   -39,   109,    32,
     -39,   -39,   111,    19,    -7,   -39,   -39,   124,   114,   -39,
      -9,    58,   -39,   -39,    -8,   -39,   -39,   -39,   -39,    91,
     -39,   -39,   -39,   -39,   -39,   -39,    64,   115,    74,   -39,
     -39,   -39,     5,   -39,   -39,   117,   -39,    28,   118,   120,
     121,   129,   -39,   -39,   -39,   -39,   133,     5,   -39,   -39,
     134,     5,   -39,   135,     5,   -39,   136,     5,   -39,   137,
     -39,    44,   -39,   -39,   -39,   -39
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    17,    18,    15,     0,     0,     2,    19,    21,     3,
       0,     0,     1,    20,     0,    16,    22,     0,     4,     0,
      98,    99,     0,     5,     0,    55,    56,    57,     0,     0,
      23,     0,     0,    24,     0,     0,    25,     0,     6,     0,
      32,    34,    35,    33,     0,    36,    38,     0,     0,    37,
       0,    39,    77,     0,     0,    58,    59,     0,     7,    70,
      74,    75,     0,     0,     0,     0,    78,    79,     0,    62,
      61,    64,    60,    63,    66,    67,    65,    68,    71,     0,
      73,    76,     0,    69,    72,    26,     0,     0,    40,    41,
       0,     0,    42,    43,     0,     0,    44,    45,     0,     0,
      46,    47,     0,     0,    82,    84,     8,     0,     0,    80,
       0,     0,    81,    83,     0,    97,     9,    89,    96,     0,
      85,    87,    90,    86,    88,    91,     0,     0,     0,    95,
      93,    92,     0,    94,    27,     0,    10,     0,     0,     0,
       0,    50,    11,    48,    51,    52,     0,     0,    49,    28,
       0,     0,    29,     0,     0,    30,     0,     0,    31,     0,
      12,     0,    53,    54,    13,    14
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -39,   -39,   -39,   -39,   -39,   -39,   -39,   -39,   -39,   -39,
     -39,   -39,   -39,   -17,   -14,   -39,   -39,   -39,   -39,   -39,
     -39,   -39,   -39,   -39,   -39,   -39,   -39,   113,   -39,   -39,
     -39,   -39,   -39,   -39,   -39,   -39,   -39,   -39,   -39,    82,
     -39,   -39,   -39,   -39,   -39,   -39,   -39,   -39,   -39,   -38,
      45,   -39,   -39
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     5,     6,    19,    24,    39,    62,   108,   127,   137,
     147,   161,   165,     7,     8,     9,    10,    17,    31,    34,
      37,    86,   135,   150,   153,   156,   159,    45,    46,    47,
      90,    94,    98,   102,   141,   142,   164,    28,    57,    77,
      78,    61,    63,    54,    65,    68,   110,   106,   107,   125,
     126,   116,    22
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      16,   112,    21,    11,    11,    59,    88,    27,     1,     2,
      12,    18,    60,   117,     3,    30,     3,    13,    33,   113,
     118,    36,     1,     2,   119,   120,   121,   122,   123,   124,
       3,     4,    92,     4,    96,   100,    56,    14,    55,     3,
       3,     3,     3,     3,    79,    67,    15,   104,     3,    25,
      26,   138,   139,    48,     3,     3,     4,     4,     4,     4,
       4,    23,    20,    80,     3,     4,   162,    52,    85,     3,
      89,     4,     4,    40,    93,    41,    42,    43,    97,    66,
      29,     4,   101,    32,    53,   117,   114,    35,   131,   105,
     131,    38,   129,    50,   115,   117,   130,   120,   121,   122,
     123,   124,   133,    51,    58,    64,   130,   120,   121,   122,
     123,   124,   117,    81,    82,    84,    87,    91,   134,    40,
     140,    41,    42,    43,   120,   121,   122,   123,   124,    95,
      99,    44,   103,   149,   109,   111,   132,   152,   136,   143,
     155,   144,   145,   158,   163,    69,    70,    71,    72,    73,
      74,    75,    76,   146,   148,   151,   154,   157,   160,    49,
      83,     0,     0,     0,   128
};

static const yytype_int16 yycheck[] =
{
      14,    10,    19,    11,    11,    21,     3,    24,     3,     4,
       0,    21,    28,    21,    11,    29,    11,     1,    32,    28,
      28,    35,     3,     4,    32,    33,    34,    35,    36,    37,
      11,    28,     3,    28,     3,     3,    53,    21,     8,    11,
      11,    11,    11,    11,     9,    62,    28,    28,    11,     6,
       7,    23,    24,    11,    11,    11,    28,    28,    28,    28,
      28,    21,    25,    28,    11,    28,    22,    11,    82,    11,
      87,    28,    28,    16,    91,    18,    19,    20,    95,    26,
      21,    28,    99,    21,    28,    21,    28,    21,   126,   103,
     128,    21,    28,    21,   111,    21,    32,    33,    34,    35,
      36,    37,    28,    28,    21,    21,    32,    33,    34,    35,
      36,    37,    21,    28,    21,    28,    21,    21,   132,    16,
     137,    18,    19,    20,    33,    34,    35,    36,    37,    21,
      21,    28,    21,   147,    10,    21,    21,   151,    21,    21,
     154,    21,    21,   157,   161,    13,    14,    15,    16,    17,
      18,    19,    20,    24,    21,    21,    21,    21,    21,    46,
      78,    -1,    -1,    -1,   119
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,    11,    28,    39,    40,    51,    52,    53,
      54,    11,     0,     1,    21,    28,    52,    55,    21,    41,
      25,    51,    90,    21,    42,     6,     7,    51,    75,    21,
      52,    56,    21,    52,    57,    21,    52,    58,    21,    43,
      16,    18,    19,    20,    28,    65,    66,    67,    11,    65,
      21,    28,    11,    28,    81,     8,    51,    76,    21,    21,
      28,    79,    44,    80,    21,    82,    26,    51,    83,    13,
      14,    15,    16,    17,    18,    19,    20,    77,    78,     9,
      28,    28,    21,    77,    28,    52,    59,    21,     3,    51,
      68,    21,     3,    51,    69,    21,     3,    51,    70,    21,
       3,    51,    71,    21,    28,    52,    85,    86,    45,    10,
      84,    21,    10,    28,    28,    51,    89,    21,    28,    32,
      33,    34,    35,    36,    37,    87,    88,    46,    88,    28,
      32,    87,    21,    28,    52,    60,    21,    47,    23,    24,
      51,    72,    73,    21,    21,    21,    24,    48,    21,    52,
      61,    21,    52,    62,    21,    52,    63,    21,    52,    64,
      21,    49,    22,    51,    74,    50
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    38,    39,    39,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    40,    51,    51,    52,    52,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    65,    65,    65,    66,    66,    67,    67,
      68,    68,    69,    69,    70,    70,    71,    71,    72,    73,
      73,    73,    73,    74,    74,    75,    75,    75,    76,    76,
      77,    77,    77,    77,    77,    77,    77,    77,    78,    78,
      80,    79,    81,    81,    81,    82,    81,    81,    83,    83,
      84,    84,    86,    85,    85,    87,    87,    87,    87,    87,
      87,    88,    88,    88,    89,    89,    89,    89,    90,    90
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    56,     1,     3,     1,     1,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     3,
       1,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       0,     3,     6,     5,     3,     0,     5,     1,     1,     1,
       1,     2,     0,     4,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     4,     3,     2,     1,     1,     1
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void * scanner, NCBI::Logging::AWSReceiver * lib)
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void * scanner, NCBI::Logging::AWSReceiver * lib)
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, void * scanner, NCBI::Logging::AWSReceiver * lib)
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, void * scanner, NCBI::Logging::AWSReceiver * lib)
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
yyparse (void * scanner, NCBI::Logging::AWSReceiver * lib)
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
#line 77 "aws_v2_parser.y" /* yacc.c:1646  */
    { YYACCEPT; }
#line 1424 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 3:
#line 78 "aws_v2_parser.y" /* yacc.c:1646  */
    { YYACCEPT; }
#line 1430 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 4:
#line 84 "aws_v2_parser.y" /* yacc.c:1646  */
    { aws_start_time( scanner ); }
#line 1436 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 5:
#line 85 "aws_v2_parser.y" /* yacc.c:1646  */
    { aws_start_ipaddr( scanner ); }
#line 1442 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 6:
#line 89 "aws_v2_parser.y" /* yacc.c:1646  */
    { aws_start_key( scanner ); }
#line 1448 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 7:
#line 91 "aws_v2_parser.y" /* yacc.c:1646  */
    { aws_start_rescode( scanner ); }
#line 1454 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 8:
#line 98 "aws_v2_parser.y" /* yacc.c:1646  */
    { aws_start_UserAgent( scanner ); }
#line 1460 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 9:
#line 100 "aws_v2_parser.y" /* yacc.c:1646  */
    { aws_pop_state( scanner ); }
#line 1466 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 10:
#line 103 "aws_v2_parser.y" /* yacc.c:1646  */
    { aws_start_host_id( scanner ); }
#line 1472 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 11:
#line 105 "aws_v2_parser.y" /* yacc.c:1646  */
    { aws_pop_state( scanner ); /* the following space is consumed by aws_host_id */ }
#line 1478 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 12:
#line 110 "aws_v2_parser.y" /* yacc.c:1646  */
    { aws_start_TLS_vers( scanner ); }
#line 1484 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 13:
#line 110 "aws_v2_parser.y" /* yacc.c:1646  */
    { aws_pop_state( scanner ); }
#line 1490 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 14:
#line 111 "aws_v2_parser.y" /* yacc.c:1646  */
    {
        // numbers may be shifted!!!!

        // combine data from aws_key and request
        if ( (yyvsp[-38].req) . path . n == 1 && (yyvsp[-38].req) . path . p[ 0 ] == '-' )
        {
            t_str empty;
            EMPTY_TSTR( empty );
            SET_VALUE( AWSReceiver::key, empty );
        }
        else
        {
            SET_VALUE( AWSReceiver::key, (yyvsp[-38].req) . path );
        }

        t_request req = (yyvsp[-36].req);
        if ( (yyvsp[-38].req) . accession . n > 0 )
        {
            req . accession = (yyvsp[-38].req) . accession;
            req . filename  = (yyvsp[-38].req) . filename;
            req . extension = (yyvsp[-38].req) . extension;
        }

        lib -> setRequest( req );
    }
#line 1520 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 15:
#line 139 "aws_v2_parser.y" /* yacc.c:1646  */
    { EMPTY_TSTR((yyval.s)); }
#line 1526 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 16:
#line 140 "aws_v2_parser.y" /* yacc.c:1646  */
    { EMPTY_TSTR((yyval.s)); }
#line 1532 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 20:
#line 150 "aws_v2_parser.y" /* yacc.c:1646  */
    {  YYABORT; }
#line 1538 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 21:
#line 153 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::owner, (yyvsp[0].s) ); }
#line 1544 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 22:
#line 154 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::bucket, (yyvsp[0].s) ); }
#line 1550 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 23:
#line 155 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::requester, (yyvsp[0].s) ); }
#line 1556 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 24:
#line 156 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::request_id, (yyvsp[0].s) ); }
#line 1562 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 25:
#line 157 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::operation, (yyvsp[0].s) ); }
#line 1568 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 26:
#line 158 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::error, (yyvsp[0].s) ); }
#line 1574 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 27:
#line 159 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::version_id, (yyvsp[0].s) ); }
#line 1580 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 28:
#line 160 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::sig_ver, (yyvsp[0].s) ); }
#line 1586 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 29:
#line 161 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::cipher_suite, (yyvsp[0].s) ); }
#line 1592 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 30:
#line 162 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::auth_type, (yyvsp[0].s) ); }
#line 1598 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 31:
#line 163 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::host_header, (yyvsp[0].s) ); }
#line 1604 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 32:
#line 166 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); }
#line 1610 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 33:
#line 167 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); (yyval.req) . accession = (yyvsp[0].s); }
#line 1616 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 34:
#line 168 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); (yyval.req) . filename = (yyvsp[0].s); }
#line 1622 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 35:
#line 169 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); (yyval.req) . extension = (yyvsp[0].s); }
#line 1628 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 36:
#line 174 "aws_v2_parser.y" /* yacc.c:1646  */
    {
            (yyval.req) = (yyvsp[0].req);
            if ( (yyvsp[0].req) . accession . n > 0 )
            {
                (yyval.req) . filename  = (yyvsp[0].req) . accession;
            }
        }
#line 1640 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 37:
#line 182 "aws_v2_parser.y" /* yacc.c:1646  */
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
#line 1671 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 38:
#line 211 "aws_v2_parser.y" /* yacc.c:1646  */
    { (yyval.req) = (yyvsp[0].req); }
#line 1677 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 39:
#line 212 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); }
#line 1683 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 40:
#line 216 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::res_len, (yyvsp[0].s) ); }
#line 1689 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 41:
#line 217 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::res_len, (yyvsp[0].s) ); }
#line 1695 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 42:
#line 221 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::obj_size, (yyvsp[0].s) ); }
#line 1701 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 43:
#line 222 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::obj_size, (yyvsp[0].s) ); }
#line 1707 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 44:
#line 226 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::total_time, (yyvsp[0].s) ); }
#line 1713 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 45:
#line 227 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::total_time, (yyvsp[0].s) ); }
#line 1719 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 46:
#line 231 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::turnaround_time, (yyvsp[0].s) ); }
#line 1725 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 47:
#line 232 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::turnaround_time, (yyvsp[0].s) ); }
#line 1731 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 48:
#line 236 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            (yyval.s) = (yyvsp[-1].s);
            MERGE_TSTR( (yyval.s) , (yyvsp[0].s) );
        }
#line 1740 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 49:
#line 244 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            (yyval.s) = (yyvsp[-2].s); // keep the space between the 2 parts of the Id
            MERGE_TSTR( (yyval.s) , (yyvsp[-1].s) );
            SET_VALUE( AWSReceiver::host_id, (yyval.s) );
        }
#line 1750 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 50:
#line 250 "aws_v2_parser.y" /* yacc.c:1646  */
    {
            (yyval.s) = (yyvsp[0].s);
            // trim the trailing space
            (yyval.s) . n --;
            SET_VALUE( AWSReceiver::host_id, (yyval.s) );
        }
#line 1761 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 51:
#line 257 "aws_v2_parser.y" /* yacc.c:1646  */
    {
            SET_VALUE( AWSReceiver::host_id, (yyvsp[-1].s) );
        }
#line 1769 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 52:
#line 261 "aws_v2_parser.y" /* yacc.c:1646  */
    {
            SET_VALUE( AWSReceiver::host_id, (yyvsp[-1].s) );
        }
#line 1777 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 53:
#line 267 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::tls_version, (yyvsp[0].s) ); }
#line 1783 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 54:
#line 268 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::tls_version, (yyvsp[0].s) ); }
#line 1789 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 55:
#line 272 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::ip, (yyvsp[0].s) ); }
#line 1795 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 56:
#line 273 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::ip, (yyvsp[0].s) ); }
#line 1801 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 57:
#line 274 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::ip, (yyvsp[0].s) ); }
#line 1807 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 60:
#line 298 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); }
#line 1813 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 61:
#line 299 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); (yyval.req).accession_mode = acc_after; }
#line 1819 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 62:
#line 300 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); (yyval.req).accession_mode = acc_after; }
#line 1825 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 63:
#line 301 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); (yyval.req).accession_mode = acc_after; }
#line 1831 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 64:
#line 302 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); }
#line 1837 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 65:
#line 304 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            InitRequest( (yyval.req) ); 
            (yyval.req) . path = (yyvsp[0].s); 
            (yyval.req) . accession = (yyvsp[0].s); 
            (yyval.req) . accession_mode = acc_inside; 
        }
#line 1848 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 66:
#line 310 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); (yyval.req) . filename = (yyvsp[0].s); }
#line 1854 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 67:
#line 311 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); (yyval.req) . extension = (yyvsp[0].s); }
#line 1860 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 68:
#line 318 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            (yyval.req) = (yyvsp[0].req); 
            if ( (yyvsp[0].req) . accession_mode == acc_after )            
            {   /* a delimiter seen before an accession */
                (yyval.req) . accession_mode = acc_before;
            }
        }
#line 1872 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 69:
#line 326 "aws_v2_parser.y" /* yacc.c:1646  */
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
#line 1905 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 70:
#line 357 "aws_v2_parser.y" /* yacc.c:1646  */
    { aws_start_URL( scanner ); }
#line 1911 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 71:
#line 357 "aws_v2_parser.y" /* yacc.c:1646  */
    { (yyval.req) = (yyvsp[0].req); }
#line 1917 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 72:
#line 362 "aws_v2_parser.y" /* yacc.c:1646  */
    {
        (yyval.req) = (yyvsp[-3].req);
        (yyval.req).method = (yyvsp[-4].s);
        (yyval.req).vers   = (yyvsp[-1].s);
    }
#line 1927 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 73:
#line 368 "aws_v2_parser.y" /* yacc.c:1646  */
    {
        (yyval.req) = (yyvsp[-2].req);
        (yyval.req).method = (yyvsp[-3].s);
     }
#line 1936 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 74:
#line 373 "aws_v2_parser.y" /* yacc.c:1646  */
    {
        InitRequest( (yyval.req) );
        (yyval.req).method = (yyvsp[-1].s);
    }
#line 1945 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 75:
#line 378 "aws_v2_parser.y" /* yacc.c:1646  */
    { /* SPACE does that in the above branches, here have to pop state explicitly */
            aws_pop_state( scanner ); 
        }
#line 1953 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 76:
#line 382 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            (yyval.req) = (yyvsp[-2].req);
            (yyval.req).method = (yyvsp[-3].s);
        }
#line 1962 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 77:
#line 386 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) );}
#line 1968 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 78:
#line 390 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::res_code, (yyvsp[0].s) ); }
#line 1974 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 79:
#line 391 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::res_code, (yyvsp[0].s) ); }
#line 1980 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 80:
#line 395 "aws_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 1986 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 81:
#line 396 "aws_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[-1].s); MERGE_TSTR ( (yyval.s), (yyvsp[0].s) ); }
#line 1992 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 82:
#line 400 "aws_v2_parser.y" /* yacc.c:1646  */
    { aws_start_referer( scanner ); }
#line 1998 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 83:
#line 402 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            SET_VALUE( AWSReceiver::referer, (yyvsp[-1].s) ); 
            aws_pop_state( scanner ); // out of QUOTED into the global state
        }
#line 2007 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 84:
#line 407 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            SET_VALUE( AWSReceiver::referer, (yyvsp[0].s) ); 
        }
#line 2015 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 85:
#line 413 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitAgent( (yyval.agent) ); (yyval.agent).original = (yyvsp[0].s); }
#line 2021 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 86:
#line 415 "aws_v2_parser.y" /* yacc.c:1646  */
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
#line 2036 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 87:
#line 425 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitAgent( (yyval.agent) ); (yyval.agent).original = (yyvsp[0].s); (yyval.agent).vdb_libc = (yyvsp[0].s); }
#line 2042 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 88:
#line 426 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitAgent( (yyval.agent) ); (yyval.agent).original = (yyvsp[0].s); (yyval.agent).vdb_phid_compute_env = (yyvsp[0].s); }
#line 2048 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 89:
#line 427 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitAgent( (yyval.agent) ); (yyval.agent).original = (yyvsp[0].s); }
#line 2054 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 90:
#line 428 "aws_v2_parser.y" /* yacc.c:1646  */
    { InitAgent( (yyval.agent) ); (yyval.agent).original = (yyvsp[0].s); }
#line 2060 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 91:
#line 432 "aws_v2_parser.y" /* yacc.c:1646  */
    { (yyval.agent) = (yyvsp[0].agent); }
#line 2066 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 92:
#line 434 "aws_v2_parser.y" /* yacc.c:1646  */
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
#line 2101 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 93:
#line 465 "aws_v2_parser.y" /* yacc.c:1646  */
    {
        (yyval.agent) = (yyvsp[-1].agent);
        MERGE_TSTR( (yyval.agent) . original, (yyvsp[0].s) );
    }
#line 2110 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 94:
#line 473 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            t_agent temp;
            InitAgent( temp ); 
            temp . original = (yyvsp[-2].s);
            MERGE_TSTR( temp . original, (yyvsp[-1].agent) . original );
            (yyval.agent) = (yyvsp[-1].agent);
            (yyval.agent) . original = temp . original;
            (yyval.agent) . vdb_os = (yyvsp[-2].s);
            lib -> setAgent( (yyval.agent) );
        }
#line 2125 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 95:
#line 484 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            (yyval.agent) = (yyvsp[-1].agent);
            lib -> setAgent( (yyval.agent) );
        }
#line 2134 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 96:
#line 489 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            InitAgent( (yyval.agent) ); 
            lib -> setAgent( (yyval.agent) );
        }
#line 2143 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 97:
#line 494 "aws_v2_parser.y" /* yacc.c:1646  */
    { 
            InitAgent( (yyval.agent) ); 
            lib -> setAgent( (yyval.agent) );
        }
#line 2152 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 98:
#line 501 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::time, (yyvsp[0].s) ); }
#line 2158 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 99:
#line 502 "aws_v2_parser.y" /* yacc.c:1646  */
    { SET_VALUE( AWSReceiver::time, (yyvsp[0].s) ); }
#line 2164 "aws_v2_parser.cpp" /* yacc.c:1646  */
    break;


#line 2168 "aws_v2_parser.cpp" /* yacc.c:1646  */
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
#line 505 "aws_v2_parser.y" /* yacc.c:1906  */


void aws_error( yyscan_t locp, NCBI::Logging::AWSReceiver * lib, const char * msg )
{
    // intentionally left empty, we communicate errors via rejected lines
}
