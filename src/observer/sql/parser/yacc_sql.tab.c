/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 2 "yacc_sql.y"


#include "sql/parser/parse_defs.h"
#include "sql/parser/yacc_sql.tab.h"
#include "sql/parser/lex.yy.h"
// #include "common/log/log.h" // 包含C++中的头文件

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct _ParserContext {
    Query * ssql;
    size_t select_length;
    size_t condition_length;
    size_t from_length;
    size_t value_length;
    Value values[MAX_NUM];
    Condition conditions[MAX_NUM];
    CompOp comp;
    char id[MAX_NUM];
} ParserContext;

typedef struct _ParserVector {
  ParserContext contexts[MAX_NUM];
  size_t len;
} ParserVector;
//获取子串
char *substr(const char *s,int n1,int n2)/*从s中提取下标为n1~n2的字符组成一个新字符串，然后返回这个新串的首地址*/
{
  char *sp = malloc(sizeof(char) * (n2 - n1 + 2));
  int i, j = 0;
  for (i = n1; i <= n2; i++) {
    sp[j++] = s[i];
  }
  sp[j] = 0;
  return sp;
}

void yyerror(yyscan_t scanner, const char *str)
{
  printf("parse sql failed. error=%s", str);
}

//ParserContext *get_context(yyscan_t scanner)
//{
//  return (ParserContext *)yyget_extra(scanner);
//}

ParserVector *get_vector(yyscan_t scanner)
{
  return (ParserVector *)yyget_extra(scanner);
}

ParserContext* get_context(yyscan_t scanner) {
ParserVector* v = get_vector(scanner);
size_t l = v->len;
return (ParserContext*)(&(v->contexts[l]));
}

ParserContext* get_last_context(yyscan_t scanner) {
ParserVector* v = get_vector(scanner);
size_t l = v->len;
return (ParserContext*)(&(v->contexts[l+1]));
}

ParserContext* get_parent_context(yyscan_t scanner) {
ParserVector* v = get_vector(scanner);
size_t l = v->len;
return (ParserContext*)(&(v->contexts[l-1]));
}

//#define CONTEXT get_context(scanner)
#define CONTEXT get_context(scanner)
#define PARENTCONTEXT get_parent_context(scanner)
#define CONTEXT_VECTOR get_vector(scanner)

#line 148 "yacc_sql.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_YACC_SQL_TAB_H_INCLUDED
# define YY_YY_YACC_SQL_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    SEMICOLON = 258,
    CREATE = 259,
    DROP = 260,
    TABLE = 261,
    TABLES = 262,
    INDEX = 263,
    SELECT = 264,
    DESC = 265,
    SHOW = 266,
    SYNC = 267,
    INSERT = 268,
    DELETE = 269,
    UPDATE = 270,
    LBRACE = 271,
    RBRACE = 272,
    COMMA = 273,
    TRX_BEGIN = 274,
    TRX_COMMIT = 275,
    TRX_ROLLBACK = 276,
    INT_T = 277,
    STRING_T = 278,
    FLOAT_T = 279,
    DATE_T = 280,
    HELP = 281,
    EXIT = 282,
    DOT = 283,
    INTO = 284,
    VALUES = 285,
    FROM = 286,
    WHERE = 287,
    AND = 288,
    SET = 289,
    ON = 290,
    LOAD = 291,
    DATA = 292,
    INFILE = 293,
    EQ = 294,
    LT = 295,
    GT = 296,
    LE = 297,
    GE = 298,
    NE = 299,
    COUNT = 300,
    AVG = 301,
    MIN = 302,
    MAX = 303,
    COUNT_1 = 304,
    INNER_JOIN = 305,
    NULLABLE = 306,
    NULLT = 307,
    NOT_NULLT = 308,
    GROUP_BY = 309,
    ORDER_BY = 310,
    UNIQUE = 311,
    OP_IS = 312,
    OP_NOT = 313,
    TEXT_T = 314,
    OP_IN = 315,
    OP_NOTIN = 316,
    ASC = 317,
    NUMBER = 318,
    FLOAT = 319,
    ID = 320,
    PATH = 321,
    SSS = 322,
    STAR = 323,
    STRING_V = 324,
    DATE = 325
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 146 "yacc_sql.y"

  struct _Attr *attr;
  struct _Condition *condition1;
  struct _Value *value1;
  char *string;
  int number;
  float floats;
  char *position;

#line 281 "yacc_sql.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void *scanner);

#endif /* !YY_YY_YACC_SQL_TAB_H_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

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

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
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

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

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
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
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
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   330

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  71
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  48
/* YYNRULES -- Number of rules.  */
#define YYNRULES  141
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  339

#define YYUNDEFTOK  2
#define YYMAXUTOK   325


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
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
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   176,   176,   178,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   202,   207,   212,   218,   224,   230,   236,   242,   248,
     255,   263,   268,   269,   274,   277,   282,   289,   298,   300,
     304,   315,   329,   332,   335,   340,   343,   344,   345,   346,
     347,   350,   359,   375,   376,   382,   385,   390,   393,   396,
     400,   404,   410,   420,   430,   448,   450,   455,   462,   463,
     468,   476,   479,   484,   490,   491,   496,   503,   506,   509,
     514,   519,   524,   529,   534,   540,   546,   552,   558,   564,
     570,   576,   582,   590,   592,   599,   606,   611,   617,   623,
     629,   635,   641,   647,   653,   659,   668,   670,   673,   677,
     679,   682,   683,   687,   689,   693,   695,   700,   721,   741,
     761,   783,   804,   825,   844,   855,   866,   877,   888,   901,
     901,   923,   924,   925,   926,   927,   928,   929,   930,   931,
     932,   937
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SEMICOLON", "CREATE", "DROP", "TABLE",
  "TABLES", "INDEX", "SELECT", "DESC", "SHOW", "SYNC", "INSERT", "DELETE",
  "UPDATE", "LBRACE", "RBRACE", "COMMA", "TRX_BEGIN", "TRX_COMMIT",
  "TRX_ROLLBACK", "INT_T", "STRING_T", "FLOAT_T", "DATE_T", "HELP", "EXIT",
  "DOT", "INTO", "VALUES", "FROM", "WHERE", "AND", "SET", "ON", "LOAD",
  "DATA", "INFILE", "EQ", "LT", "GT", "LE", "GE", "NE", "COUNT", "AVG",
  "MIN", "MAX", "COUNT_1", "INNER_JOIN", "NULLABLE", "NULLT", "NOT_NULLT",
  "GROUP_BY", "ORDER_BY", "UNIQUE", "OP_IS", "OP_NOT", "TEXT_T", "OP_IN",
  "OP_NOTIN", "ASC", "NUMBER", "FLOAT", "ID", "PATH", "SSS", "STAR",
  "STRING_V", "DATE", "$accept", "commands", "command", "exit", "help",
  "sync", "begin", "commit", "rollback", "drop_table", "show_tables",
  "desc_table", "create_index", "index_attr", "index_attr_lists", "unique",
  "drop_index", "create_table", "attr_def_list", "attr_def", "nullable",
  "number", "type", "ID_get", "insert", "values_list", "value_list",
  "value", "delete", "update", "select", "group", "group_list", "order",
  "order_list", "order_asc", "select_attr", "attr_list", "rel_list",
  "join_list", "on", "where", "condition_list", "condition", "sub_query",
  "$@1", "comOp", "load_data", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325
};
# endif

#define YYPACT_NINF (-303)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -303,   158,  -303,     4,    22,    73,   -44,    25,    39,    18,
      20,   -10,    93,   105,   137,   150,   157,    44,  -303,  -303,
    -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,
    -303,  -303,  -303,  -303,  -303,  -303,     9,  -303,   153,    99,
     100,   159,   165,   167,   177,   156,    -2,   156,   149,   192,
     194,  -303,   134,   135,   168,  -303,  -303,  -303,  -303,  -303,
     163,   187,   141,   211,   212,    38,   152,   161,   166,    78,
    -303,   169,  -303,  -303,   170,  -303,  -303,   188,   193,   171,
     162,   172,   197,  -303,  -303,     7,   216,    58,    65,    67,
     222,   223,   224,   225,   156,   119,   156,    -6,   226,    13,
     240,   189,   215,  -303,   227,    14,   181,   156,   182,   156,
     156,   183,   156,   184,   156,   185,    74,   186,   190,   191,
    -303,   195,  -303,  -303,   196,   198,   193,   160,  -303,  -303,
    -303,  -303,    71,  -303,  -303,   147,   219,  -303,   160,   247,
     172,   237,  -303,  -303,  -303,  -303,  -303,     1,   241,  -303,
     242,  -303,  -303,   245,  -303,   248,  -303,   249,    81,   250,
      88,   116,   118,   156,    -6,   229,   204,   251,   253,   259,
     206,  -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,
    -303,    27,   146,    13,  -303,   193,   207,   227,   270,   213,
    -303,  -303,  -303,   209,   156,   156,   156,   156,   156,   210,
     156,   156,   214,   156,   217,   156,   218,  -303,  -303,    13,
     228,   220,   231,   160,   260,   147,    73,   147,  -303,  -303,
    -303,   252,  -303,   219,   278,   281,  -303,  -303,  -303,   271,
     269,   272,  -303,  -303,  -303,  -303,  -303,   273,  -303,  -303,
     274,  -303,   275,  -303,   276,   219,   230,  -303,   138,   232,
     291,   251,   280,    -7,   265,    37,   282,   235,  -303,  -303,
    -303,    83,   236,  -303,   299,   156,   156,   156,   156,  -303,
     229,   238,   239,  -303,    -3,  -303,  -303,   289,   303,  -303,
     279,   243,  -303,   283,  -303,  -303,  -303,  -303,   269,  -303,
    -303,  -303,  -303,  -303,   228,   164,   292,  -303,   244,  -303,
     294,   160,  -303,   296,   254,    -6,   297,   255,  -303,  -303,
     256,  -303,  -303,     8,   257,  -303,   251,  -303,  -303,   193,
    -303,  -303,   292,   294,     5,   298,   204,  -303,  -303,   258,
     294,   280,   231,     8,  -303,  -303,  -303,   294,  -303
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,    34,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     3,    20,
      19,    14,    15,    16,    17,     9,    10,    11,    12,    13,
       8,     5,     7,     6,     4,    18,     0,    35,     0,     0,
       0,     0,     0,     0,     0,    93,    93,    93,     0,     0,
       0,    23,     0,     0,     0,    24,    25,    26,    22,    21,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      86,     0,    81,    80,     0,    29,    28,     0,   113,     0,
       0,     0,     0,    27,    36,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    93,    93,    93,   106,     0,     0,
       0,     0,     0,    51,    38,     0,     0,    93,     0,    93,
      93,     0,    93,     0,    93,     0,     0,     0,     0,     0,
      99,     0,    94,    82,     0,     0,   113,     0,   129,    61,
      57,    58,     0,    59,    60,     0,   115,    62,     0,     0,
       0,     0,    46,    47,    48,    49,    50,    42,     0,    83,
       0,    85,    89,     0,    88,     0,    87,     0,     0,     0,
       0,     0,     0,    93,   106,   111,    65,    55,     0,     0,
       0,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,     0,     0,     0,   114,   113,     0,    38,     0,     0,
      44,    43,    41,     0,    93,    93,    93,    93,    93,     0,
      93,    93,     0,    93,     0,    93,     0,    95,   107,     0,
     109,     0,    71,     0,     0,     0,     0,     0,   129,   119,
     117,   120,   118,   115,     0,     0,    39,    37,    45,     0,
      32,     0,    84,    92,    91,    90,    96,     0,    98,   102,
       0,   101,     0,   100,     0,   115,     0,   108,    68,     0,
       0,    55,    53,     0,     0,     0,     0,     0,   116,    63,
     141,    42,     0,    31,     0,    93,    93,    93,    93,   112,
     111,     0,     0,    66,    77,    64,    56,     0,     0,   129,
     127,     0,   129,     0,   121,   125,   122,    40,    32,    30,
      97,   105,   104,   103,   109,    68,    68,    79,     0,    78,
      74,     0,    52,     0,     0,   106,     0,     0,    33,   110,
       0,    70,    67,    77,     0,    72,    55,   128,   126,   113,
     124,   123,    68,    74,    77,     0,    65,    69,    73,     0,
      74,    53,    71,    77,    75,    54,   130,    74,    76
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,  -303,
    -303,  -303,  -303,  -303,    28,  -303,  -303,  -303,   130,   178,
      63,  -303,  -303,  -303,  -303,    -5,  -245,  -125,  -303,  -303,
    -303,    -1,  -276,    -4,  -296,  -302,   111,   -46,  -160,    35,
      60,  -123,  -200,  -169,  -210,  -303,  -130,  -303
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,   231,   263,    38,    29,    30,   141,   104,
     192,   229,   147,   105,    31,   278,   214,   135,    32,    33,
      34,   212,   273,   250,   315,   300,    48,    70,   126,   247,
     210,   100,   184,   136,   168,   169,   181,    35
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      72,    73,   167,   166,   208,   182,   276,   297,   256,   279,
      36,   323,   124,   185,   223,   297,    69,   189,   297,   311,
     312,    49,   330,   258,   107,   298,    71,   328,    39,   128,
      40,   337,    50,   329,   334,   108,   142,   143,   144,   145,
     245,   338,    51,   218,   125,   269,   327,    52,   120,   122,
     123,    53,   190,   282,   191,    54,   220,   222,   280,   299,
      37,   149,   224,   151,   152,   129,   154,   299,   156,   303,
     299,   325,   306,   146,    61,   110,   130,   131,   132,   129,
     133,    60,   112,   134,   114,   253,   111,   255,   251,   129,
     130,   131,   219,   113,   133,   115,    55,   134,   198,   170,
     130,   131,   283,    85,   133,   201,    86,   134,    56,   199,
     171,   172,   173,   174,   175,   176,   202,   207,    41,    42,
      43,    44,    45,    90,    91,    92,    93,    94,   177,   178,
     284,   179,   180,   203,   190,   205,   191,    69,    46,   158,
      57,    47,   159,    95,   204,   319,   206,   121,   232,   233,
     234,   235,   236,    58,   238,   239,   271,   241,     2,   243,
      59,    62,     3,     4,    63,    64,   272,     5,     6,     7,
       8,     9,    10,    11,    69,    65,   316,    12,    13,    14,
      74,    66,   271,    67,    15,    16,   171,   172,   173,   174,
     175,   176,   310,    68,    17,    75,   326,    76,   129,    77,
      78,    80,    79,    81,   177,   178,    82,   179,   180,   130,
     131,   221,   129,   133,    83,    84,   134,    87,    98,   290,
     291,   292,   293,   130,   131,    99,    88,   133,   138,   102,
     134,    89,   106,   109,    96,    97,   101,   103,   116,   117,
     118,   119,   127,   137,   139,   140,   148,   150,   153,   155,
     157,   160,   183,   186,   188,   161,   162,   193,   211,   194,
     163,   164,   195,   165,   209,   196,   197,   200,   216,   213,
     215,   217,   225,   227,   230,   237,   228,   252,   246,   240,
     257,   259,   242,   244,   260,   248,   249,   262,   261,   264,
     265,   266,   267,   268,   275,   270,   281,   274,   277,   285,
     286,   288,   289,   295,   296,   301,   302,   304,   305,   313,
     271,   307,   314,   317,   320,   331,   308,   226,   187,   318,
     321,   322,   324,   333,   287,   332,   335,   254,   336,   309,
     294
};

static const yytype_int16 yycheck[] =
{
      46,    47,   127,   126,   164,   135,   251,    10,   218,    16,
       6,   313,    18,   138,   183,    10,    18,    16,    10,   295,
     296,    65,   324,   223,    17,    28,    28,   323,     6,    16,
       8,   333,     7,    28,   330,    28,    22,    23,    24,    25,
     209,   337,     3,    16,    50,   245,   322,    29,    94,    95,
      96,    31,    51,    16,    53,    65,   181,   182,    65,    62,
      56,   107,   185,   109,   110,    52,   112,    62,   114,   279,
      62,   316,   282,    59,    65,    17,    63,    64,    65,    52,
      67,    37,    17,    70,    17,   215,    28,   217,   213,    52,
      63,    64,    65,    28,    67,    28,     3,    70,    17,    28,
      63,    64,    65,    65,    67,    17,    68,    70,     3,    28,
      39,    40,    41,    42,    43,    44,    28,   163,    45,    46,
      47,    48,    49,    45,    46,    47,    48,    49,    57,    58,
     255,    60,    61,    17,    51,    17,    53,    18,    65,    65,
       3,    68,    68,    65,    28,   305,    28,    28,   194,   195,
     196,   197,   198,     3,   200,   201,    18,   203,     0,   205,
       3,     8,     4,     5,    65,    65,    28,     9,    10,    11,
      12,    13,    14,    15,    18,    16,   301,    19,    20,    21,
      31,    16,    18,    16,    26,    27,    39,    40,    41,    42,
      43,    44,    28,    16,    36,     3,   319,     3,    52,    65,
      65,    38,    34,    16,    57,    58,    65,    60,    61,    63,
      64,    65,    52,    67,     3,     3,    70,    65,    30,   265,
     266,   267,   268,    63,    64,    32,    65,    67,    39,    67,
      70,    65,    35,    17,    65,    65,    65,    65,    16,    16,
      16,    16,    16,     3,    29,    18,    65,    65,    65,    65,
      65,    65,    33,     6,    17,    65,    65,    16,    54,    17,
      65,    65,    17,    65,    35,    17,    17,    17,     9,    18,
      17,    65,    65,     3,    65,    65,    63,    17,    50,    65,
      28,     3,    65,    65,     3,    65,    55,    18,    17,    17,
      17,    17,    17,    17,     3,    65,    31,    65,    18,    17,
      65,    65,     3,    65,    65,    16,     3,    28,    65,    65,
      18,    28,    18,    17,    17,    17,   288,   187,   140,    65,
      65,    65,    65,    65,   261,   326,   331,   216,   332,   294,
     270
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    72,     0,     4,     5,     9,    10,    11,    12,    13,
      14,    15,    19,    20,    21,    26,    27,    36,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    87,
      88,    95,    99,   100,   101,   118,     6,    56,    86,     6,
       8,    45,    46,    47,    48,    49,    65,    68,   107,    65,
       7,     3,    29,    31,    65,     3,     3,     3,     3,     3,
      37,    65,     8,    65,    65,    16,    16,    16,    16,    18,
     108,    28,   108,   108,    31,     3,     3,    65,    65,    34,
      38,    16,    65,     3,     3,    65,    68,    65,    65,    65,
      45,    46,    47,    48,    49,    65,    65,    65,    30,    32,
     112,    65,    67,    65,    90,    94,    35,    17,    28,    17,
      17,    28,    17,    28,    17,    28,    16,    16,    16,    16,
     108,    28,   108,   108,    18,    50,   109,    16,    16,    52,
      63,    64,    65,    67,    70,    98,   114,     3,    39,    29,
      18,    89,    22,    23,    24,    25,    59,    93,    65,   108,
      65,   108,   108,    65,   108,    65,   108,    65,    65,    68,
      65,    65,    65,    65,    65,    65,   112,    98,   115,   116,
      28,    39,    40,    41,    42,    43,    44,    57,    58,    60,
      61,   117,   117,    33,   113,    98,     6,    90,    17,    16,
      51,    53,    91,    16,    17,    17,    17,    17,    17,    28,
      17,    17,    28,    17,    28,    17,    28,   108,   109,    35,
     111,    54,   102,    18,    97,    17,     9,    65,    16,    65,
      98,    65,    98,   114,   112,    65,    89,     3,    63,    92,
      65,    84,   108,   108,   108,   108,   108,    65,   108,   108,
      65,   108,    65,   108,    65,   114,    50,   110,    65,    55,
     104,    98,    17,   117,   107,   117,   115,    28,   113,     3,
       3,    17,    18,    85,    17,    17,    17,    17,    17,   113,
      65,    18,    28,   103,    65,     3,    97,    18,    96,    16,
      65,    31,    16,    65,    98,    17,    65,    91,    65,     3,
     108,   108,   108,   108,   111,    65,    65,    10,    28,    62,
     106,    16,     3,   115,    28,    65,   115,    28,    85,   110,
      28,   103,   103,    65,    18,   105,    98,    17,    65,   109,
      17,    65,    65,   106,    65,    97,   112,   103,   105,    28,
     106,    17,   102,    65,   105,    96,   104,   106,   105
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    71,    72,    72,    73,    73,    73,    73,    73,    73,
      73,    73,    73,    73,    73,    73,    73,    73,    73,    73,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    85,    86,    86,    87,    88,    89,    89,
      90,    90,    91,    91,    91,    92,    93,    93,    93,    93,
      93,    94,    95,    96,    96,    97,    97,    98,    98,    98,
      98,    98,    99,   100,   101,   102,   102,   102,   103,   103,
     103,   104,   104,   104,   105,   105,   105,   106,   106,   106,
     107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   107,   107,   108,   108,   108,   108,   108,   108,   108,
     108,   108,   108,   108,   108,   108,   109,   109,   109,   110,
     110,   111,   111,   112,   112,   113,   113,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   116,
     115,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   118
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     2,     2,     2,     2,     2,     4,     3,     3,
      10,     2,     0,     3,     0,     1,     4,     8,     0,     3,
       6,     3,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     1,    10,     0,     6,     0,     3,     1,     1,     1,
       1,     1,     5,     8,     9,     0,     3,     5,     0,     5,
       3,     0,     4,     6,     0,     4,     6,     0,     1,     1,
       2,     2,     4,     5,     7,     5,     2,     5,     5,     5,
       7,     7,     7,     0,     3,     5,     6,     8,     6,     3,
       6,     6,     6,     8,     8,     8,     0,     3,     4,     0,
       4,     0,     3,     0,     3,     0,     3,     3,     3,     3,
       3,     5,     5,     7,     7,     5,     7,     5,     7,     0,
       9,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     8
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
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
        yyerror (scanner, YY_("syntax error: cannot back up")); \
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
                  Type, Value, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, void *scanner)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (scanner);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, void *scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep, scanner);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule, void *scanner)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              , scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, scanner); \
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
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
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
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
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
            else
              goto append;

          append:
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

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
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
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

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
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
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
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
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
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
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
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, void *scanner)
{
  YYUSE (yyvaluep);
  YYUSE (scanner);
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
yyparse (void *scanner)
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

    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

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
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
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
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
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
  case 21:
#line 202 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1695 "yacc_sql.tab.c"
    break;

  case 22:
#line 207 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1703 "yacc_sql.tab.c"
    break;

  case 23:
#line 212 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1711 "yacc_sql.tab.c"
    break;

  case 24:
#line 218 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1719 "yacc_sql.tab.c"
    break;

  case 25:
#line 224 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1727 "yacc_sql.tab.c"
    break;

  case 26:
#line 230 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1735 "yacc_sql.tab.c"
    break;

  case 27:
#line 236 "yacc_sql.y"
                            {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, (yyvsp[-1].string));
    }
#line 1744 "yacc_sql.tab.c"
    break;

  case 28:
#line 242 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1752 "yacc_sql.tab.c"
    break;

  case 29:
#line 248 "yacc_sql.y"
                      {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1761 "yacc_sql.tab.c"
    break;

  case 30:
#line 256 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-6].string), (yyvsp[-4].string), (yyvsp[-8].number));
		}
#line 1770 "yacc_sql.tab.c"
    break;

  case 31:
#line 263 "yacc_sql.y"
                   {
create_index_append(&CONTEXT->ssql->sstr.create_index, (yyvsp[-1].string));
}
#line 1778 "yacc_sql.tab.c"
    break;

  case 33:
#line 269 "yacc_sql.y"
                           {
create_index_append(&CONTEXT->ssql->sstr.create_index, (yyvsp[-1].string));
}
#line 1786 "yacc_sql.tab.c"
    break;

  case 34:
#line 274 "yacc_sql.y"
{
(yyval.number)=0;
}
#line 1794 "yacc_sql.tab.c"
    break;

  case 35:
#line 277 "yacc_sql.y"
         {
(yyval.number)=1;
}
#line 1802 "yacc_sql.tab.c"
    break;

  case 36:
#line 283 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, (yyvsp[-1].string));
		}
#line 1811 "yacc_sql.tab.c"
    break;

  case 37:
#line 290 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length;
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, (yyvsp[-5].string));
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
#line 1823 "yacc_sql.tab.c"
    break;

  case 39:
#line 300 "yacc_sql.y"
                                   {    }
#line 1829 "yacc_sql.tab.c"
    break;

  case 40:
#line 305 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-4].number), (yyvsp[-2].number), (yyvsp[0].number));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name =(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type = $2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length = $4;
			CONTEXT->value_length++;
		}
#line 1844 "yacc_sql.tab.c"
    break;

  case 41:
#line 316 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-1].number), 5, (yyvsp[0].number));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name=(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type=$2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length=4; // default attribute length
			CONTEXT->value_length++;
		}
#line 1859 "yacc_sql.tab.c"
    break;

  case 42:
#line 329 "yacc_sql.y"
{
(yyval.number) = 0;
}
#line 1867 "yacc_sql.tab.c"
    break;

  case 43:
#line 332 "yacc_sql.y"
            {
(yyval.number) = 0;
}
#line 1875 "yacc_sql.tab.c"
    break;

  case 44:
#line 335 "yacc_sql.y"
           {
(yyval.number) = 1;
}
#line 1883 "yacc_sql.tab.c"
    break;

  case 45:
#line 340 "yacc_sql.y"
                       {(yyval.number) = (yyvsp[0].number);}
#line 1889 "yacc_sql.tab.c"
    break;

  case 46:
#line 343 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1895 "yacc_sql.tab.c"
    break;

  case 47:
#line 344 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1901 "yacc_sql.tab.c"
    break;

  case 48:
#line 345 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1907 "yacc_sql.tab.c"
    break;

  case 49:
#line 346 "yacc_sql.y"
                { (yyval.number)=DATES; }
#line 1913 "yacc_sql.tab.c"
    break;

  case 50:
#line 347 "yacc_sql.y"
                {(yyval.number)=TEXTS; }
#line 1919 "yacc_sql.tab.c"
    break;

  case 51:
#line 351 "yacc_sql.y"
        {
		char *temp=(yyvsp[0].string); 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
#line 1928 "yacc_sql.tab.c"
    break;

  case 52:
#line 360 "yacc_sql.y"
                {
			// CONTEXT->values[CONTEXT->value_length++] = *$6;

			CONTEXT->ssql->flag=SCF_INSERT;//"insert";
			// CONTEXT->ssql->sstr.insertion.relation_name = $3;
			// CONTEXT->ssql->sstr.insertion.value_num = CONTEXT->value_length;
			// for(i = 0; i < CONTEXT->value_length; i++){
			// 	CONTEXT->ssql->sstr.insertion.values[i] = CONTEXT->values[i];
      // }
			inserts_init(&CONTEXT->ssql->sstr.insertion, (yyvsp[-7].string), CONTEXT->values, CONTEXT->value_length);

      //临时变量清零
      CONTEXT->value_length=0;
    }
#line 1947 "yacc_sql.tab.c"
    break;

  case 54:
#line 376 "yacc_sql.y"
                                                 {

}
#line 1955 "yacc_sql.tab.c"
    break;

  case 55:
#line 382 "yacc_sql.y"
    {
    	CONTEXT->ssql->sstr.insertion.offsets[CONTEXT->ssql->sstr.insertion.insert_num++] = CONTEXT->value_length;
    }
#line 1963 "yacc_sql.tab.c"
    break;

  case 56:
#line 385 "yacc_sql.y"
                              { 
  		// CONTEXT->values[CONTEXT->value_length++] = *$2;
	  }
#line 1971 "yacc_sql.tab.c"
    break;

  case 57:
#line 390 "yacc_sql.y"
          {	
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].number));
		}
#line 1979 "yacc_sql.tab.c"
    break;

  case 58:
#line 393 "yacc_sql.y"
          {
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].floats));
		}
#line 1987 "yacc_sql.tab.c"
    break;

  case 59:
#line 396 "yacc_sql.y"
         {
		(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 1996 "yacc_sql.tab.c"
    break;

  case 60:
#line 400 "yacc_sql.y"
          {
    		(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-1);
      		date_init_string(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
    		}
#line 2005 "yacc_sql.tab.c"
    break;

  case 61:
#line 404 "yacc_sql.y"
           {
      		value_init_null(&CONTEXT->values[CONTEXT->value_length++]);
      		    }
#line 2013 "yacc_sql.tab.c"
    break;

  case 62:
#line 411 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, (yyvsp[-2].string));
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;	
    }
#line 2025 "yacc_sql.tab.c"
    break;

  case 63:
#line 421 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			Value *value = &CONTEXT->values[0];
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-6].string), (yyvsp[-4].string), value, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
#line 2037 "yacc_sql.tab.c"
    break;

  case 64:
#line 431 "yacc_sql.y"
                {
			// CONTEXT->ssql->sstr.selection.relations[CONTEXT->from_length++]=$4;
			selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-5].string));
			CONTEXT->ssql->flag=SCF_SELECT;//"select";
			selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);

			// CONTEXT->ssql->sstr.selection.attr_num = CONTEXT->select_length;

			//临时变量清零
			CONTEXT->condition_length=0;
			CONTEXT->from_length=0;
			CONTEXT->select_length=0;
			CONTEXT->value_length = 0;
	}
#line 2056 "yacc_sql.tab.c"
    break;

  case 65:
#line 448 "yacc_sql.y"
{

}
#line 2064 "yacc_sql.tab.c"
    break;

  case 66:
#line 450 "yacc_sql.y"
                          {
            			RelAttr attr;
            			relation_attr_init(&attr, NULL, (yyvsp[-1].string), NO_AGG);
            			selects_append_groupbys(&CONTEXT->ssql->sstr.selection, &attr);
            		}
#line 2074 "yacc_sql.tab.c"
    break;

  case 67:
#line 455 "yacc_sql.y"
                                                {
            			RelAttr attr;
            			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string), NO_AGG);
            			selects_append_groupbys(&CONTEXT->ssql->sstr.selection, &attr);
            		}
#line 2084 "yacc_sql.tab.c"
    break;

  case 69:
#line 463 "yacc_sql.y"
                             {
RelAttr attr;
            			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string), NO_AGG);
            			selects_append_groupbys(&CONTEXT->ssql->sstr.selection, &attr);
}
#line 2094 "yacc_sql.tab.c"
    break;

  case 70:
#line 468 "yacc_sql.y"
                      {
RelAttr attr;
            			relation_attr_init(&attr, NULL, (yyvsp[-1].string), NO_AGG);
            			selects_append_groupbys(&CONTEXT->ssql->sstr.selection, &attr);
}
#line 2104 "yacc_sql.tab.c"
    break;

  case 71:
#line 476 "yacc_sql.y"
{

}
#line 2112 "yacc_sql.tab.c"
    break;

  case 72:
#line 479 "yacc_sql.y"
                                  {
RelAttr attr;
            			relation_attr_init(&attr, NULL, (yyvsp[-2].string), (yyvsp[-1].number));
            			selects_append_orderbys(&CONTEXT->ssql->sstr.selection, &attr);
}
#line 2122 "yacc_sql.tab.c"
    break;

  case 73:
#line 484 "yacc_sql.y"
                                         {
	RelAttr attr;
        relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), (yyvsp[-1].number));
        selects_append_orderbys(&CONTEXT->ssql->sstr.selection, &attr);
}
#line 2132 "yacc_sql.tab.c"
    break;

  case 75:
#line 491 "yacc_sql.y"
                                {
RelAttr attr;
relation_attr_init(&attr, NULL, (yyvsp[-2].string), (yyvsp[-1].number));
selects_append_orderbys(&CONTEXT->ssql->sstr.selection, &attr);
}
#line 2142 "yacc_sql.tab.c"
    break;

  case 76:
#line 496 "yacc_sql.y"
                                       {
RelAttr attr;
relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), (yyvsp[-1].number));
selects_append_orderbys(&CONTEXT->ssql->sstr.selection, &attr);
}
#line 2152 "yacc_sql.tab.c"
    break;

  case 77:
#line 503 "yacc_sql.y"
{
(yyval.number)=MIN_AGG;
}
#line 2160 "yacc_sql.tab.c"
    break;

  case 78:
#line 506 "yacc_sql.y"
      {
(yyval.number)=MIN_AGG;
}
#line 2168 "yacc_sql.tab.c"
    break;

  case 79:
#line 509 "yacc_sql.y"
       {
(yyval.number)=MAX_AGG;
}
#line 2176 "yacc_sql.tab.c"
    break;

  case 80:
#line 514 "yacc_sql.y"
                  {
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*", NO_AGG);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2186 "yacc_sql.tab.c"
    break;

  case 81:
#line 519 "yacc_sql.y"
                   {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string), NO_AGG);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2196 "yacc_sql.tab.c"
    break;

  case 82:
#line 524 "yacc_sql.y"
                              {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string), NO_AGG);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 2206 "yacc_sql.tab.c"
    break;

  case 83:
#line 529 "yacc_sql.y"
                                          {
	RelAttr attr;
        			relation_attr_init(&attr, NULL, (yyvsp[-2].string), COUNT_AGG);
        			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
                 	}
#line 2216 "yacc_sql.tab.c"
    break;

  case 84:
#line 534 "yacc_sql.y"
                                                                 {
                 	RelAttr attr;
                        			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), COUNT_AGG);
                        			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                                  	}
#line 2227 "yacc_sql.tab.c"
    break;

  case 85:
#line 540 "yacc_sql.y"
                                                            {
                 	RelAttr attr;
                        			relation_attr_init(&attr, NULL, "*", COUNT_AGG);
                        			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                 	}
#line 2238 "yacc_sql.tab.c"
    break;

  case 86:
#line 546 "yacc_sql.y"
                                           {
                 	RelAttr attr;
                        			relation_attr_init(&attr, NULL, "1", COUNT_AGG);
                        			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                 	}
#line 2249 "yacc_sql.tab.c"
    break;

  case 87:
#line 552 "yacc_sql.y"
                                                        {
                 	RelAttr attr;
                        			relation_attr_init(&attr, NULL, (yyvsp[-2].string), MAX_AGG);
                        			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                 	}
#line 2260 "yacc_sql.tab.c"
    break;

  case 88:
#line 558 "yacc_sql.y"
                                                        {
                 	RelAttr attr;
                        			relation_attr_init(&attr, NULL, (yyvsp[-2].string), MIN_AGG);
                        			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                 	}
#line 2271 "yacc_sql.tab.c"
    break;

  case 89:
#line 564 "yacc_sql.y"
                                                        {
                 	RelAttr attr;
                        			relation_attr_init(&attr, NULL, (yyvsp[-2].string), AVG_AGG);
                        			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                 	}
#line 2282 "yacc_sql.tab.c"
    break;

  case 90:
#line 570 "yacc_sql.y"
                                                               {
                 	RelAttr attr;
                        			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), MAX_AGG);
                        			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                                 	}
#line 2293 "yacc_sql.tab.c"
    break;

  case 91:
#line 576 "yacc_sql.y"
                                                                {
                         RelAttr attr;
                         			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), MIN_AGG);
                         			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                                 	}
#line 2304 "yacc_sql.tab.c"
    break;

  case 92:
#line 582 "yacc_sql.y"
                                                                {
                         RelAttr attr;
                         			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), AVG_AGG);
                         			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                                 	}
#line 2315 "yacc_sql.tab.c"
    break;

  case 94:
#line 592 "yacc_sql.y"
                         {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string), NO_AGG);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 2327 "yacc_sql.tab.c"
    break;

  case 95:
#line 599 "yacc_sql.y"
                                {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string), NO_AGG);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
#line 2339 "yacc_sql.tab.c"
    break;

  case 96:
#line 606 "yacc_sql.y"
                                                  {
            	RelAttr attr;
                    			relation_attr_init(&attr, NULL, (yyvsp[-2].string), COUNT_AGG);
                    			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
                             	}
#line 2349 "yacc_sql.tab.c"
    break;

  case 97:
#line 611 "yacc_sql.y"
                                                                               {
                             	RelAttr attr;
                                    			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), COUNT_AGG);
                                    			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                                              	}
#line 2360 "yacc_sql.tab.c"
    break;

  case 98:
#line 617 "yacc_sql.y"
                                                                          {
                             	RelAttr attr;
                                    			relation_attr_init(&attr, NULL, "*", COUNT_AGG);
                                    			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                             	}
#line 2371 "yacc_sql.tab.c"
    break;

  case 99:
#line 623 "yacc_sql.y"
                                                         {
                             	RelAttr attr;
                                    			relation_attr_init(&attr, NULL, "1", COUNT_AGG);
                                    			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                             	}
#line 2382 "yacc_sql.tab.c"
    break;

  case 100:
#line 629 "yacc_sql.y"
                                                                      {
                             	RelAttr attr;
                                    			relation_attr_init(&attr, NULL, (yyvsp[-2].string), MAX_AGG);
                                    			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                             	}
#line 2393 "yacc_sql.tab.c"
    break;

  case 101:
#line 635 "yacc_sql.y"
                                                                      {
                             	RelAttr attr;
                                    			relation_attr_init(&attr, NULL, (yyvsp[-2].string), MIN_AGG);
                                    			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                             	}
#line 2404 "yacc_sql.tab.c"
    break;

  case 102:
#line 641 "yacc_sql.y"
                                                                      {
                             	RelAttr attr;
                                    			relation_attr_init(&attr, NULL, (yyvsp[-2].string), AVG_AGG);
                                    			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                             	}
#line 2415 "yacc_sql.tab.c"
    break;

  case 103:
#line 647 "yacc_sql.y"
                                                                             {
                             	RelAttr attr;
                                    			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), MAX_AGG);
                                    			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                                             	}
#line 2426 "yacc_sql.tab.c"
    break;

  case 104:
#line 653 "yacc_sql.y"
                                                                                  {
                                     RelAttr attr;
                                     			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), MIN_AGG);
                                     			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                                             	}
#line 2437 "yacc_sql.tab.c"
    break;

  case 105:
#line 659 "yacc_sql.y"
                                                                                  {
                                     RelAttr attr;
                                     			relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string), AVG_AGG);
                                     			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                                             	}
#line 2448 "yacc_sql.tab.c"
    break;

  case 106:
#line 668 "yacc_sql.y"
               {
    }
#line 2455 "yacc_sql.tab.c"
    break;

  case 107:
#line 670 "yacc_sql.y"
                        {
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-1].string));
		  }
#line 2463 "yacc_sql.tab.c"
    break;

  case 108:
#line 673 "yacc_sql.y"
                                     {
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-2].string));
	}
#line 2471 "yacc_sql.tab.c"
    break;

  case 110:
#line 679 "yacc_sql.y"
                                     {
	selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-2].string));
	}
#line 2479 "yacc_sql.tab.c"
    break;

  case 112:
#line 683 "yacc_sql.y"
                                  {

    }
#line 2487 "yacc_sql.tab.c"
    break;

  case 114:
#line 689 "yacc_sql.y"
                                     {	
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2495 "yacc_sql.tab.c"
    break;

  case 116:
#line 695 "yacc_sql.y"
                                   {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2503 "yacc_sql.tab.c"
    break;

  case 117:
#line 701 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-2].string), NO_AGG);

			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$ = ( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;
			// $$->left_attr.relation_name = NULL;
			// $$->left_attr.attribute_name= $1;
			// $$->comp = CONTEXT->comp;
			// $$->right_is_attr = 0;
			// $$->right_attr.relation_name = NULL;
			// $$->right_attr.attribute_name = NULL;
			// $$->right_value = *$3;

		}
#line 2528 "yacc_sql.tab.c"
    break;

  case 118:
#line 722 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 2];
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$ = ( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 0;
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=NULL;
			// $$->left_value = *$1;
			// $$->comp = CONTEXT->comp;
			// $$->right_is_attr = 0;
			// $$->right_attr.relation_name = NULL;
			// $$->right_attr.attribute_name = NULL;
			// $$->right_value = *$3;

		}
#line 2552 "yacc_sql.tab.c"
    break;

  case 119:
#line 742 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, (yyvsp[-2].string), NO_AGG);
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[0].string), NO_AGG);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=$1;
			// $$->comp = CONTEXT->comp;
			// $$->right_is_attr = 1;
			// $$->right_attr.relation_name=NULL;
			// $$->right_attr.attribute_name=$3;

		}
#line 2576 "yacc_sql.tab.c"
    break;

  case 120:
#line 762 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, (yyvsp[0].string), NO_AGG);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;

			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 0;
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=NULL;
			// $$->left_value = *$1;
			// $$->comp=CONTEXT->comp;
			
			// $$->right_is_attr = 1;
			// $$->right_attr.relation_name=NULL;
			// $$->right_attr.attribute_name=$3;
		
		}
#line 2602 "yacc_sql.tab.c"
    break;

  case 121:
#line 784 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string), NO_AGG);
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;

			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;
			// $$->left_attr.relation_name=$1;
			// $$->left_attr.attribute_name=$3;
			// $$->comp=CONTEXT->comp;
			// $$->right_is_attr = 0;   //属性值
			// $$->right_attr.relation_name=NULL;
			// $$->right_attr.attribute_name=NULL;
			// $$->right_value =*$5;			
							
    }
#line 2627 "yacc_sql.tab.c"
    break;

  case 122:
#line 805 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];

			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string), NO_AGG);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 0;//属性值
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=NULL;
			// $$->left_value = *$1;
			// $$->comp =CONTEXT->comp;
			// $$->right_is_attr = 1;//属性
			// $$->right_attr.relation_name = $3;
			// $$->right_attr.attribute_name = $5;
									
    }
#line 2652 "yacc_sql.tab.c"
    break;

  case 123:
#line 826 "yacc_sql.y"
                {
			RelAttr left_attr;
			relation_attr_init(&left_attr, (yyvsp[-6].string), (yyvsp[-4].string), NO_AGG);
			RelAttr right_attr;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string), NO_AGG);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;		//属性
			// $$->left_attr.relation_name=$1;
			// $$->left_attr.attribute_name=$3;
			// $$->comp =CONTEXT->comp;
			// $$->right_is_attr = 1;		//属性
			// $$->right_attr.relation_name=$5;
			// $$->right_attr.attribute_name=$7;
    }
#line 2675 "yacc_sql.tab.c"
    break;

  case 124:
#line 844 "yacc_sql.y"
                                              {

    RelAttr left_attr;
    			relation_attr_init(&left_attr, (yyvsp[-6].string), (yyvsp[-4].string), NO_AGG);
    			RelAttr right_attr;
    			relation_attr_init(&right_attr, NULL, (yyvsp[-4].string), SUBQUERY);
    			Condition condition;
                        condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1+CONTEXT->ssql->sstr.selection.subquery_num, &right_attr, NULL);
              		CONTEXT->conditions[CONTEXT->condition_length++] = condition;

    }
#line 2691 "yacc_sql.tab.c"
    break;

  case 125:
#line 855 "yacc_sql.y"
                                       {

             RelAttr left_attr;
             			relation_attr_init(&left_attr, NULL, (yyvsp[-4].string), NO_AGG);
             			RelAttr right_attr;
             			relation_attr_init(&right_attr, NULL, (yyvsp[-4].string), SUBQUERY);
             			Condition condition;
                                condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1+CONTEXT->ssql->sstr.selection.subquery_num, &right_attr, NULL);
                                CONTEXT->conditions[CONTEXT->condition_length++] = condition;

             }
#line 2707 "yacc_sql.tab.c"
    break;

  case 126:
#line 866 "yacc_sql.y"
                                              {

        RelAttr left_attr;
        			relation_attr_init(&left_attr, NULL, (yyvsp[0].string), SUBQUERY);
        			RelAttr right_attr;
        			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string), NO_AGG);
        			Condition condition;
                            condition_init(&condition, CONTEXT->comp, 1+CONTEXT->ssql->sstr.selection.subquery_num, &left_attr, NULL, 1, &right_attr, NULL);
                  		CONTEXT->conditions[CONTEXT->condition_length++] = condition;

        }
#line 2723 "yacc_sql.tab.c"
    break;

  case 127:
#line 877 "yacc_sql.y"
                                           {

                 RelAttr left_attr;
                 			relation_attr_init(&left_attr, NULL, (yyvsp[0].string), SUBQUERY);
                 			RelAttr right_attr;
                 			relation_attr_init(&right_attr, NULL, (yyvsp[0].string), NO_AGG);
                 			Condition condition;
                                    condition_init(&condition, CONTEXT->comp, 1+CONTEXT->ssql->sstr.selection.subquery_num, &left_attr, NULL, 1, &right_attr, NULL);
                                    CONTEXT->conditions[CONTEXT->condition_length++] = condition;

                 }
#line 2739 "yacc_sql.tab.c"
    break;

  case 128:
#line 888 "yacc_sql.y"
                                                                         {
                         RelAttr left_attr;
                         			relation_attr_init(&left_attr, NULL, " ", SUBQUERY);
                         			RelAttr right_attr;
                         			relation_attr_init(&right_attr, NULL, " ", SUBQUERY);
                         			Condition condition;
                 condition_init(&condition, CONTEXT->comp, CONTEXT->ssql->sstr.selection.subquery_num, &left_attr, NULL, 1+CONTEXT->ssql->sstr.selection.subquery_num, &right_attr, NULL);
                                   		CONTEXT->conditions[CONTEXT->condition_length++] = condition;

                         }
#line 2754 "yacc_sql.tab.c"
    break;

  case 129:
#line 901 "yacc_sql.y"
        {
		CONTEXT_VECTOR->len++;
		CONTEXT->condition_length=0;
                CONTEXT->from_length=0;
                CONTEXT->select_length=0;
                CONTEXT->value_length = 0;
	}
#line 2766 "yacc_sql.tab.c"
    break;

  case 130:
#line 909 "yacc_sql.y"
                {
			// CONTEXT->ssql->sstr.selection.relations[CONTEXT->from_length++]=$4;
			selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-4].string));
			CONTEXT->ssql->flag=SCF_SELECT;//"select";
			selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);
			selects_append_selects(&PARENTCONTEXT->ssql->sstr.selection, &CONTEXT->ssql->sstr.selection);
			// CONTEXT->ssql->sstr.selection.attr_num = CONTEXT->select_length;
			//临时变量清零
			CONTEXT_VECTOR->len--;
	}
#line 2781 "yacc_sql.tab.c"
    break;

  case 131:
#line 923 "yacc_sql.y"
             { CONTEXT->comp = EQUAL_TO; }
#line 2787 "yacc_sql.tab.c"
    break;

  case 132:
#line 924 "yacc_sql.y"
         { CONTEXT->comp = LESS_THAN; }
#line 2793 "yacc_sql.tab.c"
    break;

  case 133:
#line 925 "yacc_sql.y"
         { CONTEXT->comp = GREAT_THAN; }
#line 2799 "yacc_sql.tab.c"
    break;

  case 134:
#line 926 "yacc_sql.y"
         { CONTEXT->comp = LESS_EQUAL; }
#line 2805 "yacc_sql.tab.c"
    break;

  case 135:
#line 927 "yacc_sql.y"
         { CONTEXT->comp = GREAT_EQUAL; }
#line 2811 "yacc_sql.tab.c"
    break;

  case 136:
#line 928 "yacc_sql.y"
         { CONTEXT->comp = NOT_EQUAL; }
#line 2817 "yacc_sql.tab.c"
    break;

  case 137:
#line 929 "yacc_sql.y"
            {CONTEXT->comp = IS;}
#line 2823 "yacc_sql.tab.c"
    break;

  case 138:
#line 930 "yacc_sql.y"
             {CONTEXT->comp = IS_NOT;}
#line 2829 "yacc_sql.tab.c"
    break;

  case 139:
#line 931 "yacc_sql.y"
            {CONTEXT->comp = IN;}
#line 2835 "yacc_sql.tab.c"
    break;

  case 140:
#line 932 "yacc_sql.y"
               {CONTEXT->comp = NOT_IN;}
#line 2841 "yacc_sql.tab.c"
    break;

  case 141:
#line 938 "yacc_sql.y"
                {
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, (yyvsp[-1].string), (yyvsp[-4].string));
		}
#line 2850 "yacc_sql.tab.c"
    break;


#line 2854 "yacc_sql.tab.c"

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
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

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
      yyerror (scanner, YY_("syntax error"));
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
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
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
        yyerror (scanner, yymsgp);
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
                      yytoken, &yylval, scanner);
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

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
                  yystos[yystate], yyvsp, scanner);
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
  yyerror (scanner, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp, scanner);
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
#line 943 "yacc_sql.y"

//_____________________________________________________________________
extern void scan_string(const char *str, yyscan_t scanner);

int sql_parse(const char *s, Query *sqls){
	ParserVector contextVector;
	memset(&contextVector, 0, sizeof(contextVector));

	for (int i = 1; i < MAX_NUM; i++) {
	contextVector.contexts[i].ssql = (Query*)malloc(sizeof(Query));
	memset(contextVector.contexts[i].ssql, 0, sizeof(Query));
	}

	yyscan_t scanner;
	yylex_init_extra(&contextVector, &scanner);
	contextVector.contexts[0].ssql = sqls;
	scan_string(s, scanner);
	int result = yyparse(scanner);
	yylex_destroy(scanner);

        for (int i = 1; i < MAX_NUM; i++) {
        	contextVector.contexts[i].ssql = (Query*)malloc(sizeof(Query));
        	memset(contextVector.contexts[i].ssql, 0, sizeof(Query));
        }
	return result;
}
