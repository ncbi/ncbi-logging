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
#define yyparse         gcp_parse
#define yylex           gcp_lex
#define yyerror         gcp_error
#define yydebug         gcp_debug
#define yynerrs         gcp_nerrs


/* Copy the first part of user declarations.  */
#line 10 "gcp_v2_parser.y" /* yacc.c:339  */

#define YYDEBUG 1

#include <stdint.h>
#include "gcp_v2_parser.hpp"
#include "gcp_v2_scanner.hpp"

using namespace std;
using namespace NCBI::Logging;

void gcp_error( yyscan_t locp, NCBI::Logging::GCPReceiver * lib, const char* msg );

const t_str EmptyTSTR = { "", 0, false };


#line 88 "gcp_v2_parser.cpp" /* yacc.c:339  */

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
   by #include "gcp_v2_parser.hpp".  */
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
#line 27 "gcp_v2_parser.y" /* yacc.c:355  */

#include "types.h"
#include "GCP_Interface.hpp"

extern void gcp_get_scanner_input( void * yyscanner, t_str & str );
extern void gcp_start_URL( void * yyscanner );
extern void gcp_start_UserAgent( void * yyscanner );
extern void gcp_pop_state( void * yyscanner );

using namespace NCBI::Logging;


#line 131 "gcp_v2_parser.cpp" /* yacc.c:355  */

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
#line 41 "gcp_v2_parser.y" /* yacc.c:355  */

    t_str s;
    t_request req;
    t_agent agent;

#line 173 "gcp_v2_parser.cpp" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int gcp_parse (void * scanner, NCBI::Logging::GCPReceiver * lib);

#endif /* !YY_GCP_GCP_V2_PARSER_HPP_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 189 "gcp_v2_parser.cpp" /* yacc.c:358  */

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
#define YYFINAL  11
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   181

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  24
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  43
/* YYNRULES -- Number of rules.  */
#define YYNRULES  100
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  172

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   278

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
      15,    16,    17,    18,    19,    20,    21,    22,    23
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    63,    63,    65,    66,    67,    71,    75,    76,    80,
      81,    85,    86,    87,    92,    94,    97,    99,   101,   103,
      91,   123,   127,   128,   129,   133,   137,   138,   142,   146,
     150,   154,   158,   162,   163,   167,   168,   172,   173,   184,
     185,   186,   191,   192,   223,   231,   243,   248,   256,   257,
     258,   262,   263,   267,   268,   287,   288,   289,   290,   291,
     292,   299,   300,   306,   314,   346,   351,   359,   360,   361,
     362,   363,   364,   365,   369,   373,   378,   386,   387,   391,
     392,   396,   397,   398,   399,   403,   404,   408,   409,   413,
     414,   415,   416,   417,   421,   432,   437,   441,   445,   452,
     453
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IPV4", "IPV6", "QSTR", "I64", "PATHSTR",
  "PATHEXT", "ACCESSION", "SLASH", "EQUAL", "AMPERSAND", "QMARK",
  "PERCENT", "QUOTE", "COMMA", "UNRECOGNIZED", "OS", "SRA_TOOLKIT",
  "LIBCVERSION", "AGENTSTR", "SRATOOLVERS", "PHIDVALUE", "$accept", "line",
  "log_err", "log_hdr", "hdr_item_text", "hdr_item", "log_gcp", "$@1",
  "$@2", "$@3", "$@4", "$@5", "$@6", "time", "ip", "ip_type", "ip_region",
  "method", "status", "req_bytes", "res_bytes", "time_taken", "host",
  "referrer", "vdb_agent_token", "vdb_agent", "agent", "req_id",
  "operation", "bucket", "url_token", "url_list", "url", "object_token",
  "object_list", "file_opt", "file_list", "file_elem", "ext_opt",
  "ext_list", "ext_elem", "object", "q_i64", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278
};
# endif

#define YYPACT_NINF -118

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-118)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -14,    -2,    36,  -118,    -9,  -118,  -118,    -1,    22,    24,
    -118,  -118,    25,  -118,    74,  -118,  -118,     3,  -118,     2,
      82,    91,    92,  -118,   100,   101,  -118,   111,  -118,  -118,
    -118,  -118,    19,   114,  -118,   120,  -118,   126,  -118,     7,
     127,   129,  -118,   130,  -118,   137,   131,   133,  -118,  -118,
     134,    35,  -118,  -118,  -118,  -118,  -118,  -118,  -118,  -118,
    -118,  -118,  -118,    44,   135,  -118,  -118,   111,   136,  -118,
     111,   138,  -118,   111,   139,  -118,   111,   140,  -118,   142,
       9,   143,   145,  -118,   146,  -118,    14,   147,   149,  -118,
    -118,  -118,   150,    81,  -118,  -118,   117,  -118,  -118,  -118,
    -118,  -118,  -118,    90,   151,    99,  -118,  -118,  -118,   153,
    -118,     5,   154,   156,   157,  -118,   158,  -118,  -118,    16,
     159,   161,  -118,   162,  -118,    18,   163,   165,  -118,  -118,
    -118,     1,    80,    53,  -118,  -118,  -118,  -118,  -118,  -118,
    -118,  -118,  -118,  -118,    62,    87,  -118,    71,  -118,  -118,
    -118,  -118,   122,  -118,  -118,  -118,  -118,  -118,   124,   122,
    -118,  -118,   166,   116,  -118,  -118,  -118,  -118,  -118,  -118,
    -118,  -118
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     0,     5,     4,     7,     3,     0,     0,     0,
      10,     1,     0,     6,     0,     9,    21,     0,     8,     0,
       0,     0,     0,    13,     0,     0,    24,     0,    11,    12,
      22,    23,     0,     0,    25,     0,   100,     0,    99,     0,
       0,     0,    27,     0,    26,     0,     0,     0,    14,    28,
       0,     0,    15,    61,    62,    60,    55,    56,    57,    58,
      59,    66,    63,     0,     0,    65,    64,     0,     0,    29,
       0,     0,    30,     0,     0,    31,     0,     0,    32,     0,
       0,     0,     0,    34,     0,    33,     0,     0,     0,    36,
      16,    35,     0,     0,    17,    47,     0,    37,    39,    41,
      38,    40,    42,     0,     0,     0,    46,    44,    43,     0,
      45,     0,     0,     0,     0,    50,     0,    48,    49,     0,
       0,     0,    52,     0,    51,     0,     0,     0,    54,    18,
      53,     0,     0,     0,    19,    68,    69,    67,    70,    71,
      72,    73,    98,    74,     0,     0,    96,     0,    20,    76,
      97,    75,    78,    95,    82,    81,    83,    84,    86,    77,
      79,    87,     0,    85,    80,    94,    90,    91,    89,    92,
      93,    88
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -118,  -118,  -118,  -118,  -118,  -118,  -118,  -118,  -118,  -118,
    -118,  -118,  -118,  -118,  -118,  -118,  -118,  -118,  -118,  -118,
    -118,  -118,  -118,  -118,   -77,    50,  -118,  -118,  -118,  -118,
      95,  -118,  -118,  -117,    17,  -118,  -118,    -6,  -118,  -118,
    -118,  -118,   -35
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,     4,     5,    18,     6,    50,    64,    92,
     104,   131,   148,     7,    20,    33,    40,    46,    68,    71,
      74,    77,    81,    87,   102,   103,    94,   112,   120,   126,
      62,    63,    52,   143,   144,   158,   159,   160,   162,   163,
     171,   134,    34
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      13,     1,   132,     8,     9,    24,    25,    12,    21,    22,
     113,   114,    41,    10,    82,    14,   133,    26,    23,    88,
     115,   121,    42,   127,    83,    35,   108,   151,   108,    89,
     151,   122,    69,   128,    36,    72,    11,    15,    75,    16,
      17,    78,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    53,    54,    55,    56,    57,    58,    59,    60,    65,
     135,   136,   145,   137,   138,   139,   140,   141,   146,   135,
     136,   149,   137,   138,   139,   140,   141,   150,   135,   136,
     149,   137,   138,   139,   140,   141,   153,   135,   136,    19,
     137,   138,   139,   140,   141,   142,    95,   152,    27,    96,
      97,    98,    99,   100,   101,   106,    28,    29,   107,    97,
      98,    99,   100,   101,   110,    30,    31,   107,    97,    98,
      99,   100,   101,   166,   167,   168,    32,   169,   170,   154,
      37,   155,   161,   156,   157,    38,    97,    98,    99,   100,
     101,    39,    47,    43,    44,    45,   105,    48,    49,    51,
     147,    67,    70,   164,    73,    76,    79,    80,    66,    84,
      85,    86,     0,    90,    91,    93,     0,   109,   111,     0,
     116,   117,   118,   119,     0,   123,   124,   125,     0,   129,
     130,   165
};

static const yytype_int16 yycheck[] =
{
       1,    15,     1,     5,     6,     3,     4,    16,     5,     6,
       5,     6,     5,    15,     5,    16,    15,    15,    15,     5,
      15,     5,    15,     5,    15,     6,   103,   144,   105,    15,
     147,    15,    67,    15,    15,    70,     0,    15,    73,    15,
      15,    76,     7,     8,     9,    10,    11,    12,    13,    14,
      15,     7,     8,     9,    10,    11,    12,    13,    14,    15,
       7,     8,     9,    10,    11,    12,    13,    14,    15,     7,
       8,     9,    10,    11,    12,    13,    14,    15,     7,     8,
       9,    10,    11,    12,    13,    14,    15,     7,     8,    15,
      10,    11,    12,    13,    14,    15,    15,    10,    16,    18,
      19,    20,    21,    22,    23,    15,    15,    15,    18,    19,
      20,    21,    22,    23,    15,    15,    15,    18,    19,    20,
      21,    22,    23,     7,     8,     9,    15,    11,    12,     7,
      16,     9,     8,    11,    12,    15,    19,    20,    21,    22,
      23,    15,     5,    16,    15,    15,    96,    16,    15,    15,
     133,    16,    16,   159,    16,    16,    16,    15,    63,    16,
      15,    15,    -1,    16,    15,    15,    -1,    16,    15,    -1,
      16,    15,    15,    15,    -1,    16,    15,    15,    -1,    16,
      15,    15
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    15,    25,    26,    27,    28,    30,    37,     5,     6,
      15,     0,    16,     1,    16,    15,    15,    15,    29,    15,
      38,     5,     6,    15,     3,     4,    15,    16,    15,    15,
      15,    15,    15,    39,    66,     6,    15,    16,    15,    15,
      40,     5,    15,    16,    15,    15,    41,     5,    16,    15,
      31,    15,    56,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    54,    55,    32,    15,    54,    16,    42,    66,
      16,    43,    66,    16,    44,    66,    16,    45,    66,    16,
      15,    46,     5,    15,    16,    15,    15,    47,     5,    15,
      16,    15,    33,    15,    50,    15,    18,    19,    20,    21,
      22,    23,    48,    49,    34,    49,    15,    18,    48,    16,
      15,    15,    51,     5,     6,    15,    16,    15,    15,    15,
      52,     5,    15,    16,    15,    15,    53,     5,    15,    16,
      15,    35,     1,    15,    65,     7,     8,    10,    11,    12,
      13,    14,    15,    57,    58,     9,    15,    58,    36,     9,
      15,    57,    10,    15,     7,     9,    11,    12,    59,    60,
      61,     8,    62,    63,    61,    15,     7,     8,     9,    11,
      12,    64
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    24,    25,    25,    25,    25,    26,    27,    27,    28,
      28,    29,    29,    29,    31,    32,    33,    34,    35,    36,
      30,    37,    38,    38,    38,    39,    40,    40,    41,    42,
      43,    44,    45,    46,    46,    47,    47,    48,    48,    48,
      48,    48,    49,    49,    49,    50,    50,    50,    51,    51,
      51,    52,    52,    53,    53,    54,    54,    54,    54,    54,
      54,    54,    54,    55,    55,    56,    56,    57,    57,    57,
      57,    57,    57,    57,    58,    58,    58,    59,    59,    60,
      60,    61,    61,    61,    61,    62,    62,    63,    63,    64,
      64,    64,    64,    64,    65,    65,    65,    65,    65,    66,
      66
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     1,     1,     1,     2,     1,     3,     3,
       2,     3,     3,     2,     0,     0,     0,     0,     0,     0,
      39,     3,     3,     3,     2,     1,     3,     2,     3,     1,
       1,     1,     1,     3,     2,     3,     2,     1,     1,     1,
       1,     1,     1,     2,     2,     4,     3,     2,     3,     3,
       2,     3,     2,     3,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     3,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     1,     0,     1,
       2,     1,     1,     1,     1,     1,     0,     1,     2,     1,
       1,     1,     1,     1,     6,     3,     2,     3,     2,     3,
       2
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void * scanner, NCBI::Logging::GCPReceiver * lib)
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void * scanner, NCBI::Logging::GCPReceiver * lib)
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, void * scanner, NCBI::Logging::GCPReceiver * lib)
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, void * scanner, NCBI::Logging::GCPReceiver * lib)
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
yyparse (void * scanner, NCBI::Logging::GCPReceiver * lib)
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
        case 3:
#line 65 "gcp_v2_parser.y" /* yacc.c:1646  */
    { YYACCEPT; }
#line 1400 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 4:
#line 66 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib -> SetCategory( ReceiverInterface::cat_ignored ); YYACCEPT; }
#line 1406 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 5:
#line 67 "gcp_v2_parser.y" /* yacc.c:1646  */
    { YYABORT; }
#line 1412 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 6:
#line 71 "gcp_v2_parser.y" /* yacc.c:1646  */
    {}
#line 1418 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 7:
#line 75 "gcp_v2_parser.y" /* yacc.c:1646  */
    {}
#line 1424 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 8:
#line 76 "gcp_v2_parser.y" /* yacc.c:1646  */
    {}
#line 1430 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 9:
#line 80 "gcp_v2_parser.y" /* yacc.c:1646  */
    {}
#line 1436 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 10:
#line 81 "gcp_v2_parser.y" /* yacc.c:1646  */
    {}
#line 1442 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 11:
#line 85 "gcp_v2_parser.y" /* yacc.c:1646  */
    {}
#line 1448 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 12:
#line 86 "gcp_v2_parser.y" /* yacc.c:1646  */
    {}
#line 1454 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 13:
#line 87 "gcp_v2_parser.y" /* yacc.c:1646  */
    {}
#line 1460 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 14:
#line 92 "gcp_v2_parser.y" /* yacc.c:1646  */
    { gcp_start_URL( scanner ); }
#line 1466 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 15:
#line 94 "gcp_v2_parser.y" /* yacc.c:1646  */
    { gcp_pop_state( scanner ); }
#line 1472 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 16:
#line 97 "gcp_v2_parser.y" /* yacc.c:1646  */
    { gcp_start_UserAgent( scanner ); }
#line 1478 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 17:
#line 99 "gcp_v2_parser.y" /* yacc.c:1646  */
    { gcp_pop_state( scanner ); }
#line 1484 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 18:
#line 101 "gcp_v2_parser.y" /* yacc.c:1646  */
    { gcp_start_URL( scanner ); }
#line 1490 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 19:
#line 103 "gcp_v2_parser.y" /* yacc.c:1646  */
    { gcp_pop_state( scanner ); }
#line 1496 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 20:
#line 104 "gcp_v2_parser.y" /* yacc.c:1646  */
    {
        t_request req = (yyvsp[-27].req);

        if ( (yyvsp[-1].req) . accession . n > 0 )
        {   // the cloud did populate the object-field
            req = (yyvsp[-1].req);
        }

        if ( req . filename . n == 0 && req . extension . n == 0 )
        {   // reuse the accession as the filename
            req . filename = req . accession;
        }

        req . method = (yyvsp[-30].s);
        lib -> setRequest( req );
    }
#line 1517 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 21:
#line 123 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::time, (yyvsp[-1].s) ); }
#line 1523 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 22:
#line 127 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::ip, (yyvsp[-1].s) ); }
#line 1529 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 23:
#line 128 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::ip, (yyvsp[-1].s) ); }
#line 1535 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 24:
#line 129 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::ip, EmptyTSTR ); }
#line 1541 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 25:
#line 133 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::ip_type, (yyvsp[0].s) ); }
#line 1547 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 26:
#line 137 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::ip_region, (yyvsp[-1].s) ); }
#line 1553 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 27:
#line 138 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::ip_region, EmptyTSTR ); }
#line 1559 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 28:
#line 142 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[-1].s); }
#line 1565 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 29:
#line 146 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::status, (yyvsp[0].s) ); }
#line 1571 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 30:
#line 150 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::request_bytes, (yyvsp[0].s) ); }
#line 1577 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 31:
#line 154 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::result_bytes, (yyvsp[0].s) ); }
#line 1583 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 32:
#line 158 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::time_taken, (yyvsp[0].s) ); }
#line 1589 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 33:
#line 162 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::host, (yyvsp[-1].s) ); }
#line 1595 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 34:
#line 163 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::host, EmptyTSTR ); }
#line 1601 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 35:
#line 167 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::referer, (yyvsp[-1].s) ); }
#line 1607 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 36:
#line 168 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::referer, EmptyTSTR ); }
#line 1613 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 37:
#line 172 "gcp_v2_parser.y" /* yacc.c:1646  */
    { InitAgent( (yyval.agent) ); (yyval.agent).original = (yyvsp[0].s); }
#line 1619 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 38:
#line 174 "gcp_v2_parser.y" /* yacc.c:1646  */
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
#line 1634 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 39:
#line 184 "gcp_v2_parser.y" /* yacc.c:1646  */
    { InitAgent( (yyval.agent) ); (yyval.agent).original = (yyvsp[0].s); (yyval.agent).vdb_libc = (yyvsp[0].s); }
#line 1640 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 40:
#line 185 "gcp_v2_parser.y" /* yacc.c:1646  */
    { InitAgent( (yyval.agent) ); (yyval.agent).original = (yyvsp[0].s); (yyval.agent).vdb_phid_compute_env = (yyvsp[0].s); }
#line 1646 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 41:
#line 186 "gcp_v2_parser.y" /* yacc.c:1646  */
    { InitAgent( (yyval.agent) ); (yyval.agent).original = (yyvsp[0].s); }
#line 1652 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 42:
#line 191 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.agent) = (yyvsp[0].agent); }
#line 1658 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 43:
#line 193 "gcp_v2_parser.y" /* yacc.c:1646  */
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
#line 1693 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 44:
#line 224 "gcp_v2_parser.y" /* yacc.c:1646  */
    {
        (yyval.agent) = (yyvsp[-1].agent);
        MERGE_TSTR( (yyval.agent) . original, (yyvsp[0].s) );
    }
#line 1702 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 45:
#line 232 "gcp_v2_parser.y" /* yacc.c:1646  */
    { 
            t_agent temp;
            InitAgent( temp ); 
            temp . original = (yyvsp[-2].s);
            MERGE_TSTR( temp . original, (yyvsp[-1].agent) . original );
            (yyval.agent) = (yyvsp[-1].agent);
            (yyval.agent) . original = temp . original;

            (yyval.agent) . vdb_os = (yyvsp[-2].s);
            lib->setAgent( (yyval.agent) );
        }
#line 1718 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 46:
#line 244 "gcp_v2_parser.y" /* yacc.c:1646  */
    { 
            (yyval.agent) = (yyvsp[-1].agent);
            lib->setAgent( (yyval.agent) );
        }
#line 1727 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 47:
#line 249 "gcp_v2_parser.y" /* yacc.c:1646  */
    { 
            InitAgent( (yyval.agent) ); 
            lib->setAgent( (yyval.agent) );
        }
#line 1736 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 48:
#line 256 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::request_id, (yyvsp[-1].s) ); }
#line 1742 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 49:
#line 257 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::request_id, (yyvsp[-1].s) ); }
#line 1748 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 50:
#line 258 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::request_id, EmptyTSTR ); }
#line 1754 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 51:
#line 262 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::operation, (yyvsp[-1].s) ); }
#line 1760 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 52:
#line 263 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::operation, EmptyTSTR ); }
#line 1766 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 53:
#line 267 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::bucket, (yyvsp[-1].s) ); }
#line 1772 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 54:
#line 268 "gcp_v2_parser.y" /* yacc.c:1646  */
    { lib->set( GCPReceiver::bucket, EmptyTSTR ); }
#line 1778 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 55:
#line 287 "gcp_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); }
#line 1784 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 56:
#line 288 "gcp_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); (yyval.req).accession_mode = acc_after; }
#line 1790 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 57:
#line 289 "gcp_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); (yyval.req).accession_mode = acc_after; }
#line 1796 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 58:
#line 290 "gcp_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); (yyval.req).accession_mode = acc_after; }
#line 1802 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 59:
#line 291 "gcp_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); }
#line 1808 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 60:
#line 293 "gcp_v2_parser.y" /* yacc.c:1646  */
    { 
            InitRequest( (yyval.req) ); 
            (yyval.req) . path = (yyvsp[0].s); 
            (yyval.req) . accession = (yyvsp[0].s); 
            (yyval.req) . accession_mode = acc_inside; 
        }
#line 1819 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 61:
#line 299 "gcp_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); (yyval.req) . filename = (yyvsp[0].s); }
#line 1825 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 62:
#line 300 "gcp_v2_parser.y" /* yacc.c:1646  */
    { InitRequest( (yyval.req) ); (yyval.req) . path = (yyvsp[0].s); (yyval.req) . extension = (yyvsp[0].s); }
#line 1831 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 63:
#line 307 "gcp_v2_parser.y" /* yacc.c:1646  */
    { 
            (yyval.req) = (yyvsp[0].req); 
            if ( (yyvsp[0].req) . accession_mode == acc_after )            
            {   /* a delimiter seen before an accession */
                (yyval.req) . accession_mode = acc_before;
            }
        }
#line 1843 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 64:
#line 315 "gcp_v2_parser.y" /* yacc.c:1646  */
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
#line 1876 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 65:
#line 347 "gcp_v2_parser.y" /* yacc.c:1646  */
    {
            (yyval.req) = (yyvsp[-1].req);
            lib -> set( GCPReceiver::uri, (yyval.req).path );
        }
#line 1885 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 66:
#line 352 "gcp_v2_parser.y" /* yacc.c:1646  */
    {
            InitRequest( (yyval.req) );
            lib -> set( GCPReceiver::uri, (yyval.req).path );
        }
#line 1894 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 67:
#line 359 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 1900 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 68:
#line 360 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 1906 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 69:
#line 361 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 1912 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 70:
#line 362 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 1918 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 71:
#line 363 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 1924 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 72:
#line 364 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 1930 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 73:
#line 365 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 1936 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 74:
#line 370 "gcp_v2_parser.y" /* yacc.c:1646  */
    {
        (yyval.s) = (yyvsp[0].s);
    }
#line 1944 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 75:
#line 374 "gcp_v2_parser.y" /* yacc.c:1646  */
    {
        (yyval.s) = (yyvsp[-1].s);
        MERGE_TSTR( (yyval.s), (yyvsp[0].s) );
    }
#line 1953 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 76:
#line 379 "gcp_v2_parser.y" /* yacc.c:1646  */
    {
        (yyval.s) = (yyvsp[-1].s);
        MERGE_TSTR( (yyval.s), (yyvsp[0].s) );
    }
#line 1962 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 77:
#line 386 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 1968 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 78:
#line 387 "gcp_v2_parser.y" /* yacc.c:1646  */
    { EMPTY_TSTR((yyval.s)); }
#line 1974 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 79:
#line 391 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 1980 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 80:
#line 392 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[-1].s); MERGE_TSTR( (yyval.s), (yyvsp[0].s) ); }
#line 1986 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 81:
#line 396 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 1992 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 82:
#line 397 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 1998 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 83:
#line 398 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 2004 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 84:
#line 399 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 2010 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 85:
#line 403 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 2016 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 86:
#line 404 "gcp_v2_parser.y" /* yacc.c:1646  */
    { EMPTY_TSTR((yyval.s)); }
#line 2022 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 87:
#line 408 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 2028 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 88:
#line 409 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[-1].s); MERGE_TSTR( (yyval.s), (yyvsp[0].s) ); }
#line 2034 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 89:
#line 413 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 2040 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 90:
#line 414 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 2046 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 91:
#line 415 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 2052 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 92:
#line 416 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 2058 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 93:
#line 417 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 2064 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 94:
#line 422 "gcp_v2_parser.y" /* yacc.c:1646  */
    { 
            InitRequest( (yyval.req) );
             
            (yyval.req).path . p = (yyvsp[-4].s) . p;
            (yyval.req).path . n = (yyvsp[-4].s) . n + 1 + (yyvsp[-2].s) . n + (yyvsp[-1].s) . n;

            (yyval.req).accession = (yyvsp[-4].s);
            (yyval.req).filename  = (yyvsp[-2].s);
            (yyval.req).extension = (yyvsp[-1].s);
        }
#line 2079 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 95:
#line 433 "gcp_v2_parser.y" /* yacc.c:1646  */
    {   // TODO: somehow signal to the back end that we did not understand the pre-parsed object
            InitRequest( (yyval.req) );
            (yyval.req) . path = (yyvsp[-1].s);
        }
#line 2088 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 96:
#line 438 "gcp_v2_parser.y" /* yacc.c:1646  */
    { 
             InitRequest( (yyval.req) );
        }
#line 2096 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 97:
#line 442 "gcp_v2_parser.y" /* yacc.c:1646  */
    {   // TODO: somehow signal to the back end that we did not understand the pre-parsed object
            InitRequest( (yyval.req) );
        }
#line 2104 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 98:
#line 446 "gcp_v2_parser.y" /* yacc.c:1646  */
    {   // TODO: somehow signal to the back end that we did not understand the pre-parsed object
            InitRequest( (yyval.req) );
        }
#line 2112 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 99:
#line 452 "gcp_v2_parser.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[-1].s); }
#line 2118 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;

  case 100:
#line 453 "gcp_v2_parser.y" /* yacc.c:1646  */
    { EMPTY_TSTR((yyval.s)); }
#line 2124 "gcp_v2_parser.cpp" /* yacc.c:1646  */
    break;


#line 2128 "gcp_v2_parser.cpp" /* yacc.c:1646  */
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
#line 456 "gcp_v2_parser.y" /* yacc.c:1906  */


void gcp_error( yyscan_t locp, NCBI::Logging::GCPReceiver * lib, const char * msg )
{
    // intentionally left empty, we communicate errors by rejecting lines
}
