# define NAME 2
# define STRING 3
# define ICON 4
# define FCON 5
# define PLUS 6
# define MINUS 8
# define MUL 11
# define AND 14
# define OR 17
# define ER 19
# define QUEST 21
# define COLON 22
# define ANDAND 23
# define OROR 24
# define ASOP 25
# define RELOP 26
# define EQUOP 27
# define DIVOP 28
# define SHIFTOP 29
# define INCOP 30
# define UNOP 31
# define STROP 32
# define TYPE 33
# define CLASS 34
# define STRUCT 35
# define RETURN 36
# define GOTO 37
# define IF 38
# define ELSE 39
# define SWITCH 40
# define BREAK 41
# define CONTINUE 42
# define WHILE 43
# define DO 44
# define FOR 45
# define DEFAULT 46
# define CASE 47
# define SIZEOF 48
# define ENUM 49
# define LP 50
# define RP 51
# define LC 52
# define RC 53
# define LB 54
# define RB 55
# define CM 56
# define SM 57
# define ASSIGN 58
# define ASM 59

# line 121 "gram.y"
# include "mfile1"
# include "messages.h"
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern int yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;
typedef int yytabelem;

# line 141 "gram.y"
	extern int wloop_level;	/* specifies while loop code generation */
	extern int floop_level;	/* specifies for loop code generation */
	static int fake = 0;
#ifdef FLEXNAMES
	static char fakename[24];	/* room enough for pattern */
#else
	static char fakename[NCHNAM+1];
#endif
# define YYERRCODE 256

# line 1077 "gram.y"


NODE *
mkty( t, d, s ) unsigned t; {
	return( block( TYPE, NIL, NIL, t, d, s ) );
	}

NODE *
bdty( op, p, v ) NODE *p; {
	register NODE *q;

	q = block( op, p, NIL, INT, 0, INT );

	switch( op ){

	case UNARY MUL:
	case UNARY CALL:
		break;

	case LB:
		q->in.right = bcon((CONSZ) v);
		break;

	case NAME:
		q->tn.rval = v;
		break;

	default:
		cerror( "bad bdty" );
		}

	return( q );
	}

dstash( n ){ /* put n into the dimension table */
	if( curdim >= DIMTABSZ-1 ){
		cerror( "dimension table overflow");
		}
	dimtab[ curdim++ ] = n;
	}

savebc() {
	if( psavbc > & asavbc[BCSZ-4 ] ){
		cerror( "whiles, fors, etc. too deeply nested");
		}
	*psavbc++ = brklab;
	*psavbc++ = contlab;
	*psavbc++ = flostat;
	*psavbc++ = swx;
	flostat = 0;
	}

resetbc(mask){

	swx = *--psavbc;
	flostat = *--psavbc | (flostat&mask);
	contlab = *--psavbc;
	brklab = *--psavbc;

	}

addcase(p) NODE *p; { /* add case to switch */
  int type;
	p = optim( p );  /* change enum to ints */
	if( p->in.op != ICON ){
		/* "non-constant case expression" */
		UERROR( MESSAGE( 80 ));
		return;
		}
	type = BTYPE( p->in.type );
	if( ( SZLONG > SZINT ) && ( type == LONG || type == ULONG ) )
		/* long in case or switch statement may be truncated */
		WERROR( MESSAGE( 123 ));
	if( swp == swtab ){
		/* "case not in switch" */
		UERROR( MESSAGE( 20 ));
		return;
		}
	if( swp >= &swtab[SWITSZ] ){
		cerror( "switch table overflow");
		}
	swp->sval = p->tn.lval;
	deflab( swp->slab = getlab() );
	++swp;
	tfree(p);
	}

adddef(){ /* add default case to switch */
	if( swtab[swx].slab >= 0 ){
		/* "duplicate default in switch" */
		UERROR( MESSAGE( 34 ));
		return;
		}
	if( swp == swtab ){
		/* "default not inside switch" */
		UERROR( MESSAGE( 29 ));
		return;
		}
	deflab( swtab[swx].slab = getlab() );
	}

swstart(){
	/* begin a switch block */
	if( swp >= &swtab[SWITSZ] ){
		cerror( "switch table overflow");
		}
	swx = swp - swtab;
	swp->slab = -1;
	++swp;
	}

swend(){ /* end a switch block */

	register struct sw *swbeg, *p, *q, *r, *r1;
	CONSZ temp;
	int tempi;

	swbeg = &swtab[swx+1];

	/* sort */

	r1 = swbeg;
	r = swp-1;

	while( swbeg < r ){
		/* bubble largest to end */
		for( q=swbeg; q<r; ++q ){
			if( q->sval > (q+1)->sval ){
				/* swap */
				r1 = q+1;
				temp = q->sval;
				q->sval = r1->sval;
				r1->sval = temp;
				tempi = q->slab;
				q->slab = r1->slab;
				r1->slab = tempi;
				}
			}
		r = r1;
		r1 = swbeg;
		}

	/* it is now sorted */

	for( p = swbeg+1; p<swp; ++p ){
		if( p->sval == (p-1)->sval ){
			/* "duplicate case in switch, %d" */
			UERROR( MESSAGE( 33 ), tempi=p->sval );
			return;
			}
		}

	genswitch( swbeg-1, swp-swbeg );
	swp = swbeg-1;
	}

setdim() { /*  store dimtab info on block entry */
	dimptr++;
	if (dimptr >= &dimrec[BNEST]) uerror("block nesting too deep");
	dimptr->index  = curdim;
	dimptr->cextern = 0;
	}

cleardim(){ /* clear dimtab info on block exit */
	if(dimptr->cextern == 0) {
		curdim = dimptr->index;
		dimptr--;
		}
	else {
		dimptr--;
		if (dimptr >= dimrec) dimptr->cextern = 1;
		}
	}
yytabelem yyexca[] ={
-1, 1,
	0, -1,
	2, 22,
	11, 22,
	50, 22,
	57, 22,
	-2, 0,
-1, 21,
	56, 83,
	57, 83,
	-2, 8,
-1, 25,
	51, 71,
	-2, 69,
-1, 26,
	58, 80,
	-2, 82,
-1, 28,
	58, 81,
	-2, 86,
-1, 34,
	52, 45,
	-2, 43,
-1, 36,
	52, 37,
	-2, 35,
-1, 56,
	53, 49,
	57, 49,
	-2, 0,
	};
# define YYNPROD 184
# define YYLAST 1177
yytabelem yyact[]={

   234,   232,   288,    70,   253,    90,    88,    89,    29,    94,
    81,   199,    29,    79,    29,   132,    80,    23,    11,    10,
    15,    23,    29,    23,   131,    99,    93,    78,    97,     7,
    20,    23,    83,    82,    17,   236,   147,   109,   245,   246,
   250,    58,   252,   243,   244,   239,   251,   241,   255,   254,
    84,    76,    85,    65,   109,   233,    24,    55,   101,   247,
    24,   235,    24,   164,    11,    10,    15,   111,   307,   230,
    24,    50,    22,    38,   261,    95,   306,    19,   133,   276,
    17,   298,    21,   275,    29,   291,   148,   152,   156,   208,
     5,   271,   107,    23,    38,    37,   270,   263,   104,    43,
    45,   203,   165,    75,    97,    92,   202,   140,   141,   142,
   143,   144,   145,    18,   192,   156,   167,   168,   169,   171,
   173,   175,   177,   179,   180,   182,   184,   186,   187,   188,
   189,   190,    24,   102,   163,   133,   103,   100,   194,    26,
   112,   136,   160,   138,   224,   113,   157,   148,    40,    68,
   155,   158,    41,   260,   283,   161,   224,   191,   258,   193,
   114,   139,    40,    42,    44,   137,    41,     9,    48,    67,
   110,   209,    49,   210,    48,   211,    30,   212,    49,   213,
    74,    48,   214,   196,   215,    49,   216,    64,   253,    90,
    88,    89,   206,   133,    81,   257,    96,    79,   218,    52,
    80,    56,   159,   228,   154,    11,    10,    15,   219,   201,
    35,   205,   226,   227,   229,    33,    83,    82,   256,   220,
   310,    17,   245,   246,   250,   223,   252,   243,   244,   239,
   251,   241,   255,   254,    84,   295,    85,   231,   109,   264,
   265,   108,   267,   247,   269,   235,   273,    96,   200,   225,
   223,   277,    29,   149,   201,   281,   280,    71,   248,   222,
    56,    23,    98,   197,    73,    72,    28,   286,    28,   289,
    11,    63,    15,   297,   284,   279,    28,   278,   259,   292,
   293,   268,   103,   266,   153,   153,    17,     4,    51,   287,
   285,    10,    53,   200,    32,   282,     8,   274,   302,   289,
    24,   303,   118,   304,   119,   195,    31,   121,   166,   289,
   123,   309,   311,   124,   149,   125,   121,   128,    59,   126,
   127,   129,   115,   122,   117,   120,    36,   118,    34,   119,
    66,   249,   121,   117,   118,   308,   119,   301,    98,   121,
   118,   106,   119,   134,    96,   121,    27,   305,   123,   117,
   120,   124,   116,   125,   130,   128,   117,   126,   127,   129,
   115,   122,   117,   120,    60,    47,    69,    46,   204,    91,
    54,    57,   162,   207,   105,    62,   118,    61,   119,    39,
     3,   121,     2,   151,   123,   300,    86,   124,    12,   125,
   116,   128,   130,   126,   127,   129,   115,   122,   117,   120,
    13,     6,    25,    14,    16,   242,   240,   237,   238,     1,
     0,     0,   118,     0,   119,     0,     0,   121,     0,     0,
   123,   299,     0,   124,     0,   125,   116,   128,   130,   126,
   127,   129,   115,   122,   117,   120,     0,     0,     0,     0,
     0,     0,   248,     0,     0,     0,     0,     0,     0,     0,
   118,     0,   119,     0,     0,   121,     0,   296,   123,     0,
     0,   124,   116,   125,   130,   128,   294,   126,   127,   129,
   115,   122,   117,   120,   118,     0,   119,     0,     0,   121,
     0,     0,   123,     0,     0,   124,     0,   125,     0,   128,
     0,   126,   127,   129,   115,   122,   117,   120,     0,   118,
   116,   119,   130,     0,   121,     0,     0,   123,     0,     0,
   124,     0,   125,     0,   128,     0,   126,   127,   129,   115,
   122,   117,   120,     0,   116,   290,   130,     0,     0,     0,
     0,     0,     0,   118,     0,   119,     0,     0,   121,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   116,
   262,   130,   118,   115,   119,   117,   120,   121,     0,     0,
   123,     0,     0,   124,     0,   125,     0,   128,     0,   126,
   127,   129,   115,   122,   117,   120,   118,     0,   119,     0,
     0,   121,     0,     0,   123,     0,     0,   124,     0,   125,
     0,   128,   217,   126,   127,   129,   115,   122,   117,   120,
     0,   221,   116,     0,   130,     0,     0,     0,     0,     0,
     0,     0,   118,     0,   119,     0,   118,   121,   119,     0,
   123,   121,     0,   124,   123,   125,   116,   128,   130,   126,
   127,   129,   115,   122,   117,   120,   115,   122,   117,   120,
     0,     0,     0,     0,     0,     0,     0,     0,   118,     0,
   119,     0,     0,   121,     0,     0,   123,   198,     0,   124,
     0,   125,   116,   128,   130,   126,   127,   129,   115,   122,
   117,   120,   118,     0,   119,     0,     0,   121,     0,     0,
   123,     0,     0,   124,     0,   125,     0,   128,     0,   126,
   127,   129,   115,   122,   117,   120,     0,     0,   116,     0,
   130,    87,    90,    88,    89,     0,     0,    81,     0,     0,
    79,     0,     0,    80,    87,    90,    88,    89,     0,     0,
    81,     0,     0,    79,   130,     0,    80,     0,     0,    83,
    82,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    83,    82,     0,     0,     0,    84,     0,    85,
     0,     0,    87,    90,    88,    89,     0,   185,    81,     0,
    84,    79,    85,     0,    80,    87,    90,    88,    89,     0,
   183,    81,     0,     0,    79,     0,     0,    80,     0,     0,
    83,    82,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    83,    82,     0,     0,     0,    84,     0,
    85,     0,     0,    87,    90,    88,    89,     0,   181,    81,
     0,    84,    79,    85,     0,    80,    87,    90,    88,    89,
     0,   178,    81,     0,     0,    79,     0,     0,    80,     0,
     0,    83,    82,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    83,    82,     0,     0,     0,    84,
     0,    85,     0,     0,    87,    90,    88,    89,     0,   176,
    81,     0,    84,    79,    85,     0,    80,    87,    90,    88,
    89,     0,   174,    81,     0,     0,    79,     0,     0,    80,
     0,     0,    83,    82,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    83,    82,     0,     0,     0,
    84,     0,    85,     0,     0,    87,    90,    88,    89,     0,
   172,    81,     0,    84,    79,    85,     0,    80,    87,    90,
    88,    89,     0,   170,    81,     0,     0,    79,     0,     0,
    80,     0,     0,    83,    82,   118,     0,   119,     0,     0,
   121,     0,     0,   123,     0,     0,    83,    82,   125,     0,
     0,    84,     0,    85,     0,   115,   122,   117,   120,     0,
   272,     0,     0,     0,    84,     0,    85,     0,   135,    87,
    90,    88,    89,     0,     0,    81,     0,     0,    79,     0,
     0,    80,    87,    90,    88,    89,     0,     0,    81,     0,
     0,    79,     0,     0,    80,     0,     0,    83,    82,     0,
    11,     0,    15,     0,     0,     0,     0,     0,     0,     0,
    83,    82,     0,     0,     0,    84,    17,    85,     0,     0,
     0,     0,    87,    90,    88,    89,     0,     0,    84,     0,
    85,     0,    77,    87,    90,    88,    89,     0,     0,    81,
     0,     0,    79,     0,     0,    80,    87,    90,    88,    89,
    83,    82,    81,     0,     0,    79,     0,     0,    80,     0,
     0,    83,    82,     0,     0,     0,     0,     0,    84,     0,
    85,     0,     0,     0,    83,    82,     0,     0,   118,    84,
   119,    85,   150,   121,    87,    90,    88,    89,     0,     0,
    81,     0,    84,    79,    85,     0,    80,     0,   115,   122,
   117,   120,     0,   118,     0,   119,     0,     0,   121,     0,
     0,   123,    83,    82,   124,     0,   125,     0,   128,     0,
   126,   127,     0,   115,   122,   117,   120,     0,     0,   118,
    84,   119,   146,     0,   121,     0,     0,   123,     0,     0,
   124,     0,   125,     0,     0,     0,   126,     0,     0,   115,
   122,   117,   120,   118,     0,   119,     0,     0,   121,     0,
     0,   123,     0,     0,   124,     0,   125,     0,     0,     0,
     0,     0,     0,   115,   122,   117,   120 };
yytabelem yypact[]={

 -1000,    31, -1000, -1000, -1000,    56,    20, -1000,   237,   257,
 -1000,   261, -1000, -1000,   163,   326,   158,   324, -1000, -1000,
    38,   112, -1000,   250,   250, -1000,   131,    13, -1000,   238,
 -1000,   237,   259,   237, -1000,   316, -1000, -1000, -1000, -1000,
   220,   132,   131,   112,   118,    98,     1,   214,   213,   125,
   980, -1000, -1000, -1000,    48, -1000,    82,    81, -1000,     0,
    10, -1000,   -15, -1000, -1000,   115,  1044, -1000, -1000,    89,
 -1000, -1000, -1000, -1000, -1000,   105,   666,   916,   111,  1044,
  1044,  1044,  1044,  1044,  1082,   967,  1031,   235, -1000, -1000,
 -1000,   151,   237,    59, -1000,   112,   124, -1000, -1000,   149,
   316, -1000, -1000,   112, -1000, -1000, -1000,     6,    45, -1000,
 -1000,   666, -1000,   306, -1000,  1044,  1044,   865,   852,   814,
   801,   763,  1044,   750,   712,   699,  1044,  1044,  1044,  1044,
  1044,    58, -1000,   666,   916, -1000, -1000,  1044,   303, -1000,
   111,   111,   111,   111,   111,   111,   967,   212,   606,   243,
 -1000,    50,   666, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
 -1000, -1000,   172,    32, -1000, -1000, -1000,   321,   666, -1000,
  1044,   305,  1044,   305,  1044,   328,  1044, -1000,  1044,   527,
  1072,  1044,   929,  1044,   610,  1044,  1147,  1123,   570,   666,
   666,   145,   916,    58,   546, -1000,   208,  1044, -1000,    90,
   198,   243,  1044, -1000,    82, -1000,    12,     2, -1000,   666,
   666,   666,   666,   666,   666,   666,   666,  1044, -1000, -1000,
   142, -1000,  1020,   111,   103, -1000,   102,    90,   666, -1000,
 -1000,    17, -1000, -1000,   493,    40, -1000,   186,   186,   233,
   186,   231,   186,    39,    34,   903,   295, -1000,    26,   186,
   227, -1000,   225,   234,  1044,   273,  1097, -1000, -1000,    99,
   224, -1000, -1000, -1000,   251, -1000,  1044,   246,  1044, -1000,
 -1000, -1000, -1000,   468,    28, -1000, -1000, -1000,  1044,  1044,
 -1000,   444, -1000, -1000,   184, -1000,   406,   223,    24,   642,
 -1000, -1000,   370,   334, -1000, -1000, -1000,  1044,  1044, -1000,
 -1000,   186,   296,    19, -1000,    11, -1000, -1000,  1044,   169,
   186, -1000 };
yytabelem yypgo[]={

     0,   409,    53,   408,   407,   406,   405,   404,   403,   402,
     0,     2,    27,    29,   401,   167,   400,   388,    36,    11,
   386,     9,    75,   139,   383,   382,   380,    30,   379,   377,
   375,    35,   374,   373,     1,   372,    26,   296,   371,    25,
    41,   370,   369,    57,   368,   367,   366,   365,    72,   364,
   346,    24,    15,   343,   341,   337,   335,   331,   330 };
yytabelem yyr1[]={

     0,     1,     1,    25,    25,    25,    26,    26,    28,    26,
    29,    30,    30,    33,    33,    35,    35,    35,    32,    32,
    32,    14,    14,    13,    13,    13,    13,    13,    37,    15,
    15,    15,    15,    15,    16,    16,     7,     7,    38,    38,
    40,    40,    17,    17,     8,     8,    41,    41,    43,    43,
    36,    44,    36,    21,    21,    21,    21,    21,    23,    23,
    23,    23,    23,    23,    22,    22,    22,    22,    22,    45,
    22,    47,    22,     9,    46,    46,    46,    27,    49,    27,
    50,    50,    48,    48,    48,    48,    48,    51,    51,    52,
    52,    39,    39,    42,    42,    53,    31,    54,    34,    34,
    34,    34,    34,    55,    34,    34,    56,    34,    34,    34,
    34,    34,    34,    34,    34,    34,    34,    34,    57,    57,
    57,     5,     4,     3,     6,    58,     2,    11,    11,    24,
    24,    10,    10,    10,    10,    10,    10,    10,    10,    10,
    10,    10,    10,    10,    10,    10,    10,    10,    10,    10,
    10,    10,    10,    10,    10,    10,    12,    12,    12,    12,
    12,    12,    12,    12,    12,    12,    12,    12,    12,    12,
    12,    12,    12,    12,    18,    19,    19,    19,    19,    19,
    19,    19,    20,    20 };
yytabelem yyr2[]={

     0,     4,     1,     3,     3,     5,     5,     7,     1,     9,
     4,     4,     1,     4,     1,     7,     9,     0,     7,     5,
     5,     2,     1,     5,     4,     3,     3,     7,     3,     2,
     5,     7,     2,     2,    11,     5,     3,     5,     2,     6,
     3,     7,    11,     5,     3,     5,     2,     6,     5,     3,
     3,     1,     9,     2,     2,     7,     5,     3,     5,     7,
     7,     9,     3,     7,     5,     7,     7,     9,     7,     1,
     9,     1,     7,     5,     3,     7,     2,     2,     1,     8,
     3,     2,     3,     3,     7,    13,     2,     2,     6,     3,
     9,     0,     2,     0,     2,     3,     9,     3,     5,     5,
     2,     5,     5,     1,    13,    15,     1,    21,     5,     5,
     5,     5,     7,     7,     2,     4,     4,     4,     5,     7,
     5,     3,     9,     7,     9,     1,     5,     2,     1,     2,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     9,     9,     9,     9,     9,     9,
     9,     9,    11,     7,     7,     2,     5,     5,     5,     5,
     5,     5,     5,     9,     9,     9,     5,     7,     7,     3,
     3,     3,     3,     7,     5,     1,     5,    11,     5,     7,
     9,     7,     5,     4 };
yytabelem yychk[]={

 -1000,    -1,   -25,   -26,   256,    59,   -14,   -13,   -37,   -15,
    34,    33,   -17,   -16,    -8,    35,    -7,    49,    57,    57,
   -27,   -22,   -48,    11,    50,    -9,   -23,   -50,   256,     2,
   -15,   -37,    33,    52,     2,    52,     2,    57,    56,   -28,
    50,    54,   -23,   -22,   -23,   -22,   -45,   -47,    50,    54,
    58,    50,   -15,    33,   -41,   -43,   -15,   -38,   -40,     2,
   -49,   -29,   -30,    51,    55,    -2,   -58,    51,    51,   -46,
     2,   256,    51,    51,    55,    -2,   -10,    52,   -12,    11,
    14,     8,    31,    30,    48,    50,   -20,     2,     4,     5,
     3,   -42,    57,   -36,   -21,   -22,   -23,    22,   256,   -39,
    56,    58,   -48,   -22,   -31,   -32,   -54,   -13,   256,    52,
    55,   -10,    51,    56,    55,    26,    56,    28,     6,     8,
    29,    11,    27,    14,    17,    19,    23,    24,    21,    25,
    58,   -51,   -52,   -10,   -53,    52,    30,    54,    32,    50,
   -12,   -12,   -12,   -12,   -12,   -12,    50,   -18,   -10,   -15,
    51,   -24,   -10,    50,    53,   -43,    56,    22,    -2,    53,
   -40,    -2,   -35,   -36,    57,    57,     2,   -10,   -10,   -10,
    58,   -10,    58,   -10,    58,   -10,    58,   -10,    58,   -10,
   -10,    58,   -10,    58,   -10,    58,   -10,   -10,   -10,   -10,
   -10,   -39,    56,   -51,   -10,     2,   -18,    51,    51,   -19,
    50,    11,    56,    51,   -44,    -2,   -13,   -33,    57,   -10,
   -10,   -10,   -10,   -10,   -10,   -10,   -10,    22,    53,   -52,
   -39,    55,    51,   -12,    54,    51,   -19,   -19,   -10,   -21,
    57,   -27,   -34,    53,   -10,    59,   -31,    -4,    -3,    43,
    -5,    45,    -6,    41,    42,    36,    37,    57,   256,   -57,
    38,    44,    40,     2,    47,    46,   -10,    53,    55,    -2,
    51,    57,    57,    57,   -34,   -34,    50,   -34,    50,   -34,
    57,    57,    57,   -10,     2,    57,    53,   -34,    50,    50,
    22,   -10,    22,    55,    50,    39,   -10,    43,   -11,   -10,
    57,    57,   -10,   -10,    22,    51,    51,    50,    57,    51,
    51,   -55,   -10,   -11,   -34,    51,    57,    57,   -56,   -11,
    51,   -34 };
yytabelem yydef[]={

     2,    -2,     1,     3,     4,     0,     0,    21,    25,    26,
    28,    29,    32,    33,     0,    44,     0,    36,     5,     6,
     0,    -2,    77,     0,     0,    -2,    -2,     0,    -2,    62,
    23,    24,    30,     0,    -2,     0,    -2,     7,    78,    12,
     0,   125,    58,    64,     0,     0,     0,     0,     0,   125,
     0,    73,    27,    31,    93,    46,    -2,    91,    38,    40,
     0,     9,     0,    65,    66,     0,     0,    63,    68,     0,
    74,    76,    72,    59,    60,     0,    84,     0,   155,     0,
     0,     0,     0,     0,     0,     0,     0,   169,   170,   171,
   172,     0,    94,    48,    50,    53,    54,   125,    57,     0,
    92,   125,    79,    83,    10,    11,    17,     0,     0,    97,
    67,   126,    70,     0,    61,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    91,    87,    89,     0,    95,   156,     0,     0,   183,
   157,   158,   159,   160,   161,   162,     0,     0,     0,   175,
   166,     0,   129,   182,    42,    47,    51,   125,    56,    34,
    39,    41,    14,     0,    19,    20,    75,   131,   132,   133,
     0,   134,     0,   135,     0,   136,     0,   137,     0,   138,
   139,     0,   140,     0,   141,     0,   142,   143,     0,   153,
   154,     0,    92,    91,     0,   168,     0,     0,   173,   174,
   175,   175,     0,   167,     0,    55,     0,     0,    18,   145,
   146,   147,   148,   144,   149,   150,   151,     0,    85,    88,
     0,   165,   164,   163,   125,   176,     0,   178,   130,    52,
    15,     0,    13,    96,     0,     0,   100,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   114,     0,     0,
     0,   121,     0,   169,     0,     0,   152,    90,   179,     0,
   181,    16,    98,    99,   101,   102,     0,     0,   128,   108,
   109,   110,   111,     0,     0,   115,   116,   117,     0,     0,
   118,     0,   120,   180,     0,   123,     0,     0,     0,   127,
   112,   113,     0,     0,   119,   177,   103,     0,   128,   122,
   124,     0,     0,     0,   104,     0,   106,   105,   128,     0,
     0,   107 };
typedef struct { char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

yytoktype yytoks[] =
{
	"NAME",	2,
	"STRING",	3,
	"ICON",	4,
	"FCON",	5,
	"PLUS",	6,
	"MINUS",	8,
	"MUL",	11,
	"AND",	14,
	"OR",	17,
	"ER",	19,
	"QUEST",	21,
	"COLON",	22,
	"ANDAND",	23,
	"OROR",	24,
	"ASOP",	25,
	"RELOP",	26,
	"EQUOP",	27,
	"DIVOP",	28,
	"SHIFTOP",	29,
	"INCOP",	30,
	"UNOP",	31,
	"STROP",	32,
	"TYPE",	33,
	"CLASS",	34,
	"STRUCT",	35,
	"RETURN",	36,
	"GOTO",	37,
	"IF",	38,
	"ELSE",	39,
	"SWITCH",	40,
	"BREAK",	41,
	"CONTINUE",	42,
	"WHILE",	43,
	"DO",	44,
	"FOR",	45,
	"DEFAULT",	46,
	"CASE",	47,
	"SIZEOF",	48,
	"ENUM",	49,
	"LP",	50,
	"RP",	51,
	"LC",	52,
	"RC",	53,
	"LB",	54,
	"RB",	55,
	"CM",	56,
	"SM",	57,
	"ASSIGN",	58,
	"ASM",	59,
	"-unknown-",	-1	/* ends search */
};

char * yyreds[] =
{
	"-no such reduction-",
	"ext_def_list : ext_def_list external_def",
	"ext_def_list : /* empty */",
	"external_def : data_def",
	"external_def : error",
	"external_def : ASM SM",
	"data_def : oattributes SM",
	"data_def : oattributes init_dcl_list SM",
	"data_def : oattributes fdeclarator",
	"data_def : oattributes fdeclarator function_body",
	"function_body : arg_dcl_list compoundstmt",
	"arg_dcl_list : arg_dcl_list declaration",
	"arg_dcl_list : /* empty */",
	"stmt_list : stmt_list statement",
	"stmt_list : /* empty */",
	"dcl_stat_list : dcl_stat_list attributes SM",
	"dcl_stat_list : dcl_stat_list attributes init_dcl_list SM",
	"dcl_stat_list : /* empty */",
	"declaration : attributes declarator_list SM",
	"declaration : attributes SM",
	"declaration : error SM",
	"oattributes : attributes",
	"oattributes : /* empty */",
	"attributes : class type",
	"attributes : type class",
	"attributes : class",
	"attributes : type",
	"attributes : type class type",
	"class : CLASS",
	"type : TYPE",
	"type : TYPE TYPE",
	"type : TYPE TYPE TYPE",
	"type : struct_dcl",
	"type : enum_dcl",
	"enum_dcl : enum_head LC moe_list optcomma RC",
	"enum_dcl : ENUM NAME",
	"enum_head : ENUM",
	"enum_head : ENUM NAME",
	"moe_list : moe",
	"moe_list : moe_list CM moe",
	"moe : NAME",
	"moe : NAME ASSIGN con_e",
	"struct_dcl : str_head LC type_dcl_list optsemi RC",
	"struct_dcl : STRUCT NAME",
	"str_head : STRUCT",
	"str_head : STRUCT NAME",
	"type_dcl_list : type_declaration",
	"type_dcl_list : type_dcl_list SM type_declaration",
	"type_declaration : type declarator_list",
	"type_declaration : type",
	"declarator_list : declarator",
	"declarator_list : declarator_list CM",
	"declarator_list : declarator_list CM declarator",
	"declarator : fdeclarator",
	"declarator : nfdeclarator",
	"declarator : nfdeclarator COLON con_e",
	"declarator : COLON con_e",
	"declarator : error",
	"nfdeclarator : MUL nfdeclarator",
	"nfdeclarator : nfdeclarator LP RP",
	"nfdeclarator : nfdeclarator LB RB",
	"nfdeclarator : nfdeclarator LB con_e RB",
	"nfdeclarator : NAME",
	"nfdeclarator : LP nfdeclarator RP",
	"fdeclarator : MUL fdeclarator",
	"fdeclarator : fdeclarator LP RP",
	"fdeclarator : fdeclarator LB RB",
	"fdeclarator : fdeclarator LB con_e RB",
	"fdeclarator : LP fdeclarator RP",
	"fdeclarator : name_lp",
	"fdeclarator : name_lp name_list RP",
	"fdeclarator : name_lp",
	"fdeclarator : name_lp RP",
	"name_lp : NAME LP",
	"name_list : NAME",
	"name_list : name_list CM NAME",
	"name_list : error",
	"init_dcl_list : init_declarator",
	"init_dcl_list : init_dcl_list CM",
	"init_dcl_list : init_dcl_list CM init_declarator",
	"xnfdeclarator : nfdeclarator",
	"xnfdeclarator : error",
	"init_declarator : nfdeclarator",
	"init_declarator : fdeclarator",
	"init_declarator : xnfdeclarator ASSIGN e",
	"init_declarator : xnfdeclarator ASSIGN LC init_list optcomma RC",
	"init_declarator : error",
	"init_list : initializer",
	"init_list : init_list CM initializer",
	"initializer : e",
	"initializer : ibrace init_list optcomma RC",
	"optcomma : /* empty */",
	"optcomma : CM",
	"optsemi : /* empty */",
	"optsemi : SM",
	"ibrace : LC",
	"compoundstmt : begin dcl_stat_list stmt_list RC",
	"begin : LC",
	"statement : e SM",
	"statement : ASM SM",
	"statement : compoundstmt",
	"statement : ifprefix statement",
	"statement : ifelprefix statement",
	"statement : WHILE LP e RP",
	"statement : WHILE LP e RP statement",
	"statement : doprefix statement WHILE LP e RP SM",
	"statement : FOR LP .e SM .e SM",
	"statement : FOR LP .e SM .e SM .e RP statement",
	"statement : switchpart statement",
	"statement : BREAK SM",
	"statement : CONTINUE SM",
	"statement : RETURN SM",
	"statement : RETURN e SM",
	"statement : GOTO NAME SM",
	"statement : SM",
	"statement : error SM",
	"statement : error RC",
	"statement : label statement",
	"label : NAME COLON",
	"label : CASE e COLON",
	"label : DEFAULT COLON",
	"doprefix : DO",
	"ifprefix : IF LP e RP",
	"ifelprefix : ifprefix statement ELSE",
	"switchpart : SWITCH LP e RP",
	"con_e : /* empty */",
	"con_e : e",
	".e : e",
	".e : /* empty */",
	"elist : e",
	"elist : elist CM e",
	"e : e RELOP e",
	"e : e CM e",
	"e : e DIVOP e",
	"e : e PLUS e",
	"e : e MINUS e",
	"e : e SHIFTOP e",
	"e : e MUL e",
	"e : e EQUOP e",
	"e : e AND e",
	"e : e OR e",
	"e : e ER e",
	"e : e ANDAND e",
	"e : e OROR e",
	"e : e MUL ASSIGN e",
	"e : e DIVOP ASSIGN e",
	"e : e PLUS ASSIGN e",
	"e : e MINUS ASSIGN e",
	"e : e SHIFTOP ASSIGN e",
	"e : e AND ASSIGN e",
	"e : e OR ASSIGN e",
	"e : e ER ASSIGN e",
	"e : e QUEST e COLON e",
	"e : e ASOP e",
	"e : e ASSIGN e",
	"e : term",
	"term : term INCOP",
	"term : MUL term",
	"term : AND term",
	"term : MINUS term",
	"term : UNOP term",
	"term : INCOP term",
	"term : SIZEOF term",
	"term : LP cast_type RP term",
	"term : SIZEOF LP cast_type RP",
	"term : term LB e RB",
	"term : funct_idn RP",
	"term : funct_idn elist RP",
	"term : term STROP NAME",
	"term : NAME",
	"term : ICON",
	"term : FCON",
	"term : STRING",
	"term : LP e RP",
	"cast_type : type null_decl",
	"null_decl : /* empty */",
	"null_decl : LP RP",
	"null_decl : LP null_decl RP LP RP",
	"null_decl : MUL null_decl",
	"null_decl : null_decl LB RB",
	"null_decl : null_decl LB con_e RB",
	"null_decl : LP null_decl RP",
	"funct_idn : NAME LP",
	"funct_idn : term LP",
};
#endif /* YYDEBUG */
/*	@(#) yaccpar.src 1.1 87/05/20 
 *
 *	Copyright (C) The Santa Cruz Operation, 1985.
 *	This Module contains Proprietary Information of
 *	The Santa Cruz Operation, Microsoft Corporation
 *	and AT&T, and should be treated as Confidential.
 */

/*
** Skeleton parser driver for yacc output
*/

/*
** yacc user known macros and defines
*/
#define YYERROR		goto yyerrlab
#define YYACCEPT	return(0)
#define YYABORT		return(1)
#define YYBACKUP( newtoken, newvalue )\
{\
	if ( yychar >= 0 || ( yyr2[ yytmp ] >> 1 ) != 1 )\
	{\
		yyerror( "syntax error - cannot backup" );\
		goto yyerrlab;\
	}\
	yychar = newtoken;\
	yystate = *yyps;\
	yylval = newvalue;\
	goto yynewstate;\
}
#define YYRECOVERING()	(!!yyerrflag)
#ifndef YYDEBUG
#	define YYDEBUG	1	/* make debugging available */
#endif

/*
** user known globals
*/
int yydebug;			/* set to 1 to get debugging */

/*
** driver internal defines
*/
#define YYFLAG		(-1000)

/*
** global variables used by the parser
*/
YYSTYPE yyv[ YYMAXDEPTH ];	/* value stack */
int yys[ YYMAXDEPTH ];		/* state stack */

YYSTYPE *yypv;			/* top of value stack */
int *yyps;			/* top of state stack */

int yystate;			/* current state */
int yytmp;			/* extra var (lasts between blocks) */

int yynerrs;			/* number of errors */
int yyerrflag;			/* error recovery flag */
int yychar;			/* current input token number */



/*
** yyparse - return 0 if worked, 1 if syntax error not recovered from
*/
int
yyparse()
{
	register YYSTYPE *yypvt;	/* top of value stack for $vars */

	/*
	** Initialize externals - yyparse may be called more than once
	*/
	yypv = &yyv[-1];
	yyps = &yys[-1];
	yystate = 0;
	yytmp = 0;
	yynerrs = 0;
	yyerrflag = 0;
	yychar = -1;

	goto yystack;
	{
		register YYSTYPE *yy_pv;	/* top of value stack */
		register int *yy_ps;		/* top of state stack */
		register int yy_state;		/* current state */
		register int  yy_n;		/* internal state number info */

		/*
		** get globals into registers.
		** branch to here only if YYBACKUP was called.
		*/
	yynewstate:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;
		goto yy_newstate;

		/*
		** get globals into registers.
		** either we just started, or we just finished a reduction
		*/
	yystack:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;

		/*
		** top of for (;;) loop while no reductions done
		*/
	yy_stack:
		/*
		** put a state and value onto the stacks
		*/
#if YYDEBUG
		/*
		** if debugging, look up token value in list of value vs.
		** name pairs.  0 and negative (-1) are special values.
		** Note: linear search is used since time is not a real
		** consideration while debugging.
		*/
		if ( yydebug )
		{
			register int yy_i;

			printf( "State %d, token ", yy_state );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ++yy_ps >= &yys[ YYMAXDEPTH ] )	/* room on stack? */
		{
			yyerror( "yacc stack overflow" );
			YYABORT;
		}
		*yy_ps = yy_state;
		*++yy_pv = yyval;

		/*
		** we have a new state - find out what to do
		*/
	yy_newstate:
		if ( ( yy_n = yypact[ yy_state ] ) <= YYFLAG )
			goto yydefault;		/* simple state */
#if YYDEBUG
		/*
		** if debugging, need to mark whether new token grabbed
		*/
		yytmp = yychar < 0;
#endif
		if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
			yychar = 0;		/* reached EOF */
#if YYDEBUG
		if ( yydebug && yytmp )
		{
			register int yy_i;

			printf( "Received token " );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ( ( yy_n += yychar ) < 0 ) || ( yy_n >= YYLAST ) )
			goto yydefault;
		if ( yychk[ yy_n = yyact[ yy_n ] ] == yychar )	/*valid shift*/
		{
			yychar = -1;
			yyval = yylval;
			yy_state = yy_n;
			if ( yyerrflag > 0 )
				yyerrflag--;
			goto yy_stack;
		}

	yydefault:
		if ( ( yy_n = yydef[ yy_state ] ) == -2 )
		{
#if YYDEBUG
			yytmp = yychar < 0;
#endif
			if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
				yychar = 0;		/* reached EOF */
#if YYDEBUG
			if ( yydebug && yytmp )
			{
				register int yy_i;

				printf( "Received token " );
				if ( yychar == 0 )
					printf( "end-of-file\n" );
				else if ( yychar < 0 )
					printf( "-none-\n" );
				else
				{
					for ( yy_i = 0;
						yytoks[yy_i].t_val >= 0;
						yy_i++ )
					{
						if ( yytoks[yy_i].t_val
							== yychar )
						{
							break;
						}
					}
					printf( "%s\n", yytoks[yy_i].t_name );
				}
			}
#endif /* YYDEBUG */
			/*
			** look through exception table
			*/
			{
				register int *yyxi = yyexca;

				while ( ( *yyxi != -1 ) ||
					( yyxi[1] != yy_state ) )
				{
					yyxi += 2;
				}
				while ( ( *(yyxi += 2) >= 0 ) &&
					( *yyxi != yychar ) )
					;
				if ( ( yy_n = yyxi[1] ) < 0 )
					YYACCEPT;
			}
		}

		/*
		** check for syntax error
		*/
		if ( yy_n == 0 )	/* have an error */
		{
			/* no worry about speed here! */
			switch ( yyerrflag )
			{
			case 0:		/* new error */
				yyerror( "syntax error" );
				goto skip_init;
			yyerrlab:
				/*
				** get globals into registers.
				** we have a user generated syntax type error
				*/
				yy_pv = yypv;
				yy_ps = yyps;
				yy_state = yystate;
				yynerrs++;
			skip_init:
			case 1:
			case 2:		/* incompletely recovered error */
					/* try again... */
				yyerrflag = 3;
				/*
				** find state where "error" is a legal
				** shift action
				*/
				while ( yy_ps >= yys )
				{
					yy_n = yypact[ *yy_ps ] + YYERRCODE;
					if ( yy_n >= 0 && yy_n < YYLAST &&
						yychk[yyact[yy_n]] == YYERRCODE)					{
						/*
						** simulate shift of "error"
						*/
						yy_state = yyact[ yy_n ];
						goto yy_stack;
					}
					/*
					** current state has no shift on
					** "error", pop stack
					*/
#if YYDEBUG
#	define _POP_ "Error recovery pops state %d, uncovers state %d\n"
					if ( yydebug )
						printf( _POP_, *yy_ps,
							yy_ps[-1] );
#	undef _POP_
#endif
					yy_ps--;
					yy_pv--;
				}
				/*
				** there is no state on stack with "error" as
				** a valid shift.  give up.
				*/
				YYABORT;
			case 3:		/* no shift yet; eat a token */
#if YYDEBUG
				/*
				** if debugging, look up token in list of
				** pairs.  0 and negative shouldn't occur,
				** but since timing doesn't matter when
				** debugging, it doesn't hurt to leave the
				** tests here.
				*/
				if ( yydebug )
				{
					register int yy_i;

					printf( "Error recovery discards " );
					if ( yychar == 0 )
						printf( "token end-of-file\n" );
					else if ( yychar < 0 )
						printf( "token -none-\n" );
					else
					{
						for ( yy_i = 0;
							yytoks[yy_i].t_val >= 0;
							yy_i++ )
						{
							if ( yytoks[yy_i].t_val
								== yychar )
							{
								break;
							}
						}
						printf( "token %s\n",
							yytoks[yy_i].t_name );
					}
				}
#endif /* YYDEBUG */
				if ( yychar == 0 )	/* reached EOF. quit */
					YYABORT;
				yychar = -1;
				goto yy_newstate;
			}
		}/* end if ( yy_n == 0 ) */
		/*
		** reduction by production yy_n
		** put stack tops, etc. so things right after switch
		*/
#if YYDEBUG
		/*
		** if debugging, print the string that is the user's
		** specification of the reduction which is just about
		** to be done.
		*/
		if ( yydebug )
			printf( "Reduce by (%d) \"%s\"\n",
				yy_n, yyreds[ yy_n ] );
#endif
		yytmp = yy_n;			/* value to switch over */
		yypvt = yy_pv;			/* $vars top of value stack */
		/*
		** Look in goto table for next state
		** Sorry about using yy_state here as temporary
		** register variable, but why not, if it works...
		** If yyr2[ yy_n ] doesn't have the low order bit
		** set, then there is no action to be done for
		** this reduction.  So, no saving & unsaving of
		** registers done.  The only difference between the
		** code just after the if and the body of the if is
		** the goto yy_stack in the body.  This way the test
		** can be made before the choice of what to do is needed.
		*/
		{
			/* length of production doubled with extra bit */
			register int yy_len = yyr2[ yy_n ];

			if ( !( yy_len & 01 ) )
			{
				yy_len >>= 1;
				yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
				yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
					*( yy_ps -= yy_len ) + 1;
				if ( yy_state >= YYLAST ||
					yychk[ yy_state =
					yyact[ yy_state ] ] != -yy_n )
				{
					yy_state = yyact[ yypgo[ yy_n ] ];
				}
				goto yy_stack;
			}
			yy_len >>= 1;
			yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
			yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
				*( yy_ps -= yy_len ) + 1;
			if ( yy_state >= YYLAST ||
				yychk[ yy_state = yyact[ yy_state ] ] != -yy_n )
			{
				yy_state = yyact[ yypgo[ yy_n ] ];
			}
		}
					/* save until reenter driver code */
		yystate = yy_state;
		yyps = yy_ps;
		yypv = yy_pv;
	}
	/*
	** code supplied by user is placed in this switch
	*/
	switch( yytmp )
	{
		
case 2:
# line 153 "gram.y"
{
			beg_file();	/* do implementation dep. stuff now */
			ftnend();
		} break;
case 3:
# line 159 "gram.y"
{ curclass = SNULL;  blevel = 0; } break;
case 4:
# line 161 "gram.y"
{ curclass = SNULL;  blevel = 0; } break;
case 5:
# line 163 "gram.y"
{ asmout();  curclass = SNULL; blevel = 0; } break;
case 6:
# line 167 "gram.y"
{  yypvt[-1].nodep->in.op = FREE; } break;
case 7:
# line 169 "gram.y"
{  yypvt[-2].nodep->in.op = FREE; } break;
case 8:
# line 170 "gram.y"
{
				defid( tymerge(yypvt[-1].nodep,yypvt[-0].nodep), curclass==STATIC?STATIC:EXTDEF );
				 bbcode(); 
				} break;
case 9:
# line 174 "gram.y"
{  
			    if( blevel ) cerror( "function level error" );
			    if( reached ) retstat |= NRETVAL; 
			    yypvt[-3].nodep->in.op = FREE;
			    ftnend();
			    } break;
case 12:
# line 185 "gram.y"
{  blevel = 1; } break;
case 14:
# line 190 "gram.y"
{  bccode();
			    locctr(PROG);
			    } break;
case 15:
# line 196 "gram.y"
{  yypvt[-1].nodep->in.op = FREE; } break;
case 16:
# line 198 "gram.y"
{  yypvt[-2].nodep->in.op = FREE; } break;
case 18:
# line 202 "gram.y"
{ curclass = SNULL;  yypvt[-2].nodep->in.op = FREE; } break;
case 19:
# line 204 "gram.y"
{ curclass = SNULL;  yypvt[-1].nodep->in.op = FREE; } break;
case 20:
# line 206 "gram.y"
{  curclass = SNULL; } break;
case 22:
# line 210 "gram.y"
{  yyval.nodep = mkty(INT,0,INT);  curclass = SNULL; } break;
case 23:
# line 213 "gram.y"
{  yyval.nodep = yypvt[-0].nodep; } break;
case 25:
# line 216 "gram.y"
{  yyval.nodep = mkty(INT,0,INT); } break;
case 26:
# line 218 "gram.y"
{ curclass = SNULL ; } break;
case 27:
# line 220 "gram.y"
{  yypvt[-2].nodep->in.type =
				types( yypvt[-2].nodep->in.type, yypvt[-0].nodep->in.type, UNDEF );
			    yypvt[-0].nodep->in.op = FREE;
			    } break;
case 28:
# line 228 "gram.y"
{  curclass = yypvt[-0].intval; } break;
case 30:
# line 233 "gram.y"
{  yypvt[-1].nodep->in.type = types( yypvt[-1].nodep->in.type, yypvt[-0].nodep->in.type, UNDEF );
			    yypvt[-0].nodep->in.op = FREE;
			    } break;
case 31:
# line 237 "gram.y"
{  yypvt[-2].nodep->in.type = types( yypvt[-2].nodep->in.type, yypvt[-1].nodep->in.type, yypvt[-0].nodep->in.type );
			    yypvt[-1].nodep->in.op = yypvt[-0].nodep->in.op = FREE;
			    } break;
case 34:
# line 245 "gram.y"
{ yyval.nodep = dclstruct(yypvt[-4].intval); } break;
case 35:
# line 247 "gram.y"
{  yyval.nodep = rstruct(yypvt[-0].intval,0);  stwart = instruct;
				 ref(yypvt[-0].intval, lineno);  } break;
case 36:
# line 252 "gram.y"
{  yyval.intval = bstruct(-1,0); stwart = SEENAME; } break;
case 37:
# line 254 "gram.y"
{  yyval.intval = bstruct(yypvt[-0].intval,0); stwart = SEENAME;
				 ref(yypvt[-0].intval, lineno);  } break;
case 40:
# line 263 "gram.y"
{  moedef( yypvt[-0].intval );  def(yypvt[-0].intval, lineno);  } break;
case 41:
# line 265 "gram.y"
{  strucoff = yypvt[-0].intval;  moedef( yypvt[-2].intval );
				 def(yypvt[-2].intval, lineno);  } break;
case 42:
# line 270 "gram.y"
{ yyval.nodep = dclstruct(yypvt[-4].intval);  } break;
case 43:
# line 272 "gram.y"
{  yyval.nodep = rstruct(yypvt[-0].intval,yypvt[-1].intval);  ref(yypvt[-0].intval, lineno);  } break;
case 44:
# line 276 "gram.y"
{  yyval.intval = bstruct(-1,yypvt[-0].intval);  stwart=0; } break;
case 45:
# line 278 "gram.y"
{  yyval.intval = bstruct(yypvt[-0].intval,yypvt[-1].intval);  stwart=0;
				 def(yypvt[-0].intval, lineno);  } break;
case 48:
# line 287 "gram.y"
{ curclass = SNULL;  stwart=0; yypvt[-1].nodep->in.op = FREE; } break;
case 49:
# line 289 "gram.y"
{  if( curclass != MOU ){
				curclass = SNULL;
				}
			    else {
				sprintf( fakename, "$%dFAKE", fake++ );
				/*
				* We will not be looking up this name in the
				* symbol table, so there is no reason to save
				* it anywhere.  (I think.)
				*/
				defid( tymerge(yypvt[-0].nodep,
					bdty(NAME,NIL,
					lookup( fakename, SMOS ))), curclass );
				/*"structure typed union member must be named"*/
				WERROR(MESSAGE( 106 ));
				}
			    stwart = 0;
			    yypvt[-0].nodep->in.op = FREE;
			    } break;
case 50:
# line 312 "gram.y"
{ defid( tymerge(yypvt[-1].nodep,yypvt[-0].nodep), curclass);  stwart = instruct; } break;
case 51:
# line 313 "gram.y"
{yyval.nodep=yypvt[-2].nodep;} break;
case 52:
# line 314 "gram.y"
{ defid( tymerge(yypvt[-4].nodep,yypvt[-0].nodep), curclass);  stwart = instruct; } break;
case 55:
# line 321 "gram.y"
{  if( !(instruct&INSTRUCT) ) UERROR( MESSAGE( 38 ) );
			    if( yypvt[-0].intval<0 || yypvt[-0].intval >= FIELD ){
				/* "illegal field size" */
				UERROR( MESSAGE( 56 ) );
				yypvt[-0].intval = 1;
				}
			    defid( tymerge(yypvt[-3].nodep,yypvt[-2].nodep), FIELD|yypvt[-0].intval );
			    yyval.nodep = NIL;
			    } break;
case 56:
# line 333 "gram.y"
{  if( !(instruct&INSTRUCT) ) UERROR( MESSAGE( 38 ) );
			    falloc( stab, yypvt[-0].intval, -1, yypvt[-2].nodep );  /* alignment or hole */
			    yyval.nodep = NIL;
			    } break;
case 57:
# line 338 "gram.y"
{  yyval.nodep = NIL; } break;
case 58:
# line 343 "gram.y"
{  umul:
				yyval.nodep = bdty( UNARY MUL, yypvt[-0].nodep, 0 ); } break;
case 59:
# line 346 "gram.y"
{  uftn:
				yyval.nodep = bdty( UNARY CALL, yypvt[-2].nodep, 0 );  } break;
case 60:
# line 349 "gram.y"
{  uary:
				yyval.nodep = bdty( LB, yypvt[-2].nodep, 0 );  } break;
case 61:
# line 352 "gram.y"
{  bary:
				/* "zero or negative subscript" */
				if( (int)yypvt[-1].intval <= 0 ) WERROR( MESSAGE( 119 ) );
				yyval.nodep = bdty( LB, yypvt[-3].nodep, yypvt[-1].intval );  } break;
case 62:
# line 357 "gram.y"
{  yyval.nodep = bdty( NAME, NIL, yypvt[-0].intval );
				 def(yypvt[-0].intval, lineno);  } break;
case 63:
# line 360 "gram.y"
{ yyval.nodep=yypvt[-1].nodep; } break;
case 64:
# line 363 "gram.y"
{  goto umul; } break;
case 65:
# line 365 "gram.y"
{  goto uftn; } break;
case 66:
# line 367 "gram.y"
{  goto uary; } break;
case 67:
# line 369 "gram.y"
{  goto bary; } break;
case 68:
# line 371 "gram.y"
{ yyval.nodep = yypvt[-1].nodep; } break;
case 69:
# line 373 "gram.y"
{if(paramno)uerror("arg list in declaration");} break;
case 70:
# line 377 "gram.y"
{
				/* "function declaration in bad context" */
				if( blevel!=0 ) UERROR(MESSAGE( 44 ));
				yyval.nodep = bdty( UNARY CALL, bdty(NAME,NIL,yypvt[-3].intval), 0 );
				stwart = 0;
				} break;
case 71:
# line 383 "gram.y"
{if(paramno)uerror("arg list in declaration");} break;
case 72:
# line 384 "gram.y"
{
				yyval.nodep = bdty( UNARY CALL, bdty(NAME,NIL,yypvt[-2].intval), 0 );
				stwart = 0;
				} break;
case 73:
# line 391 "gram.y"
{
				/* turn off typedefs for argument names */
				 newf(yypvt[-1].intval, lineno); def(yypvt[-1].intval, lineno); 
				stwart = SEENAME;
				if( stab[yypvt[-1].intval].sclass == SNULL )
				    stab[yypvt[-1].intval].stype = FTN;
				} break;
case 74:
# line 401 "gram.y"
{ ftnarg( yypvt[-0].intval );  stwart = SEENAME;
				 ref(yypvt[-0].intval, lineno);  } break;
case 75:
# line 404 "gram.y"
{ ftnarg( yypvt[-0].intval );  stwart = SEENAME;
				 ref(yypvt[-0].intval, lineno);  } break;
case 78:
# line 411 "gram.y"
{yyval.nodep=yypvt[-2].nodep;} break;
case 80:
# line 415 "gram.y"
{  defid( yypvt[-0].nodep = tymerge(yypvt[-1].nodep,yypvt[-0].nodep), curclass);
			    beginit(yypvt[-0].nodep->tn.rval);
			    } break;
case 82:
# line 422 "gram.y"
{  nidcl( tymerge(yypvt[-1].nodep,yypvt[-0].nodep) ); } break;
case 83:
# line 424 "gram.y"
{  defid( tymerge(yypvt[-1].nodep,yypvt[-0].nodep), uclass(curclass) );
			} break;
case 84:
# line 436 "gram.y"
{  doinit( yypvt[-0].nodep );
			    endinit(); } break;
case 85:
# line 439 "gram.y"
{  endinit(); } break;
case 89:
# line 449 "gram.y"
{  doinit( yypvt[-0].nodep ); } break;
case 90:
# line 451 "gram.y"
{  irbrace(); } break;
case 95:
# line 472 "gram.y"
{  ilbrace(); } break;
case 96:
# line 478 "gram.y"
{  --blevel;
			     becode(); 
			    if( blevel == 1 ) blevel = 0;
			    clearst( blevel );
			    cleardim();
			    checkst( blevel );
			    autooff = *--psavbc;
			    regvar = *--psavbc;
			    } break;
case 97:
# line 490 "gram.y"
{  setdim();
			    if( blevel == 1 ) dclargs();
			     else if (blevel > 1) bbcode(); 
			    ++blevel;
			    if( psavbc > &asavbc[BCSZ-2] ) cerror( "nesting too deep" );
			    *psavbc++ = regvar;
			    *psavbc++ = autooff;
			    } break;
case 98:
# line 501 "gram.y"
{ ecomp( yypvt[-1].nodep ); } break;
case 99:
# line 503 "gram.y"
{ asmout(); } break;
case 101:
# line 506 "gram.y"
{ deflab(yypvt[-1].intval);
			   reached = 1;
			   } break;
case 102:
# line 510 "gram.y"
{  if( yypvt[-1].intval != NOLAB ){
				deflab( yypvt[-1].intval );
				reached = 1;
				}
			    } break;
case 103:
# line 516 "gram.y"
{
				savebc();
				if (!reached)
					/* "loop not entered at top" */
					WERROR(MESSAGE(75));
				reached = 1;
				brklab = getlab();
				contlab = getlab();
				switch (wloop_level)
				{
				default:
					cerror("bad while loop code gen value");
					/*NOTREACHED*/
				case LL_TOP:	/* test at loop top */
					deflab(contlab);
					if (yypvt[-1].nodep->in.op == ICON &&
						yypvt[-1].nodep->tn.lval != 0)
					{
						flostat = FLOOP;
						tfree(yypvt[-1].nodep);
					}
					else
						ecomp(buildtree(CBRANCH, yypvt[-1].nodep,
							bcon((CONSZ) brklab)));
					break;
				case LL_BOT:	/* test at loop bottom */
					if (yypvt[-1].nodep->in.op == ICON &&
						yypvt[-1].nodep->tn.lval != 0)
					{
						flostat = FLOOP;
						tfree(yypvt[-1].nodep);
						deflab(contlab);
					}
					else
					{
						branch(contlab);
						deflab(yyval.intval = getlab());
					}
					break;
				case LL_DUP:	/* dup. test at top & bottom */
					if (yypvt[-1].nodep->in.op == ICON &&
						yypvt[-1].nodep->tn.lval != 0)
					{
						flostat = FLOOP;
						tfree(yypvt[-1].nodep);
						deflab(yyval.intval = contlab);
					}
					else
					{
#ifndef LINT
						register NODE *sav;
						extern NODE *tcopy();

						sav = tcopy(yypvt[-1].nodep);
						ecomp(buildtree(CBRANCH,yypvt[-1].nodep,
							bcon((CONSZ) brklab)));
						yypvt[-1].nodep = sav;
#endif
						deflab(yyval.intval = getlab());
					}
					break;
				}
			} break;
case 104:
# line 580 "gram.y"
{
			switch (wloop_level)
			{
			default:
				cerror("bad while loop code gen. value");
				/*NOTREACHED*/
			case LL_TOP:	/* test at loop top */
				branch(contlab);
				break;
			case LL_BOT:	/* test at loop bottom */
				if (flostat & FLOOP)
					branch(contlab);
				else
				{
					reached = 1;
					deflab(contlab);
					ecomp(buildtree(CBRANCH,
						buildtree(NOT, yypvt[-3].nodep, NIL),
						bcon((CONSZ) yypvt[-1].intval)));
				}
				break;
			case LL_DUP:	/* dup. test at top & bottom */
				if (flostat & FLOOP)
					branch(contlab);
				else
				{
					if (flostat & FCONT)
					{
						reached = 1;
						deflab(contlab);
					}
					ecomp(buildtree(CBRANCH,
						buildtree(NOT, yypvt[-3].nodep, NIL),
						bcon((CONSZ) yypvt[-1].intval)));
				}
				break;
			}
			if ((flostat & FBRK) || !(flostat & FLOOP))
				reached = 1;
			else
				reached = 0;
			deflab(brklab);
			resetbc(0);
		} break;
case 105:
# line 625 "gram.y"
{  deflab( contlab );
			    if( flostat & FCONT ) reached = 1;
			    ecomp( buildtree( CBRANCH, buildtree( NOT, yypvt[-2].nodep, NIL ), bcon( (CONSZ) yypvt[-6].intval ) ) );
			    deflab( brklab );
			    reached = 1;
			    resetbc(0);
			    } break;
case 106:
# line 633 "gram.y"
{
				if (yypvt[-3].nodep)
					ecomp(yypvt[-3].nodep);
				else if (!reached)
					/* "loop not entered at top" */
					WERROR(MESSAGE(75));
				savebc();
				contlab = getlab();
				brklab = getlab();
				reached = 1;
				switch (floop_level)
				{
				default:
					cerror("bad for loop code gen. value");
					/*NOTREACHED*/
				case LL_TOP:	/* test at loop top */
					deflab(yyval.intval = getlab());
					if (!yypvt[-1].nodep)
						flostat |= FLOOP;
					else if (yypvt[-1].nodep->in.op == ICON &&
						yypvt[-1].nodep->tn.lval != 0)
					{
						flostat |= FLOOP;
						tfree(yypvt[-1].nodep);
						yypvt[-1].nodep = (NODE *)0;
					}
					else
						ecomp(buildtree(CBRANCH, yypvt[-1].nodep,
							bcon((CONSZ) brklab)));
					break;
				case LL_BOT:	/* test at loop bottom */
					if (!yypvt[-1].nodep)
						flostat |= FLOOP;
					else if (yypvt[-1].nodep->in.op == ICON &&
						yypvt[-1].nodep->tn.lval != 0)
					{
						flostat |= FLOOP;
						tfree(yypvt[-1].nodep);
						yypvt[-1].nodep = (NODE *)0;
					}
					else
						branch(yypvt[-5].intval = getlab());
					deflab(yyval.intval = getlab());
					break;
				case LL_DUP:	/* dup. test at top & bottom */
					if (!yypvt[-1].nodep)
						flostat |= FLOOP;
					else if (yypvt[-1].nodep->in.op == ICON &&
						yypvt[-1].nodep->tn.lval != 0)
					{
						flostat |= FLOOP;
						tfree(yypvt[-1].nodep);
						yypvt[-1].nodep = (NODE *)0;
					}
					else
					{
#ifndef LINT
						register NODE *sav;
						extern NODE *tcopy();

						sav = tcopy(yypvt[-1].nodep);
						ecomp(buildtree(CBRANCH, yypvt[-1].nodep,
							bcon((CONSZ) brklab)));
						yypvt[-1].nodep = sav;
#endif
					}
					deflab(yyval.intval = getlab());
					break;
				}
			} break;
case 107:
# line 704 "gram.y"
{
			if (flostat & FCONT)
			{
				deflab(contlab);
				reached = 1;
			}
			if (yypvt[-2].nodep)
				ecomp(yypvt[-2].nodep);
			switch (floop_level)
			{
			default:
				cerror("bad for loop code gen. value");
				/*NOTREACHED*/
			case LL_TOP:	/* test at loop top */
				branch(yypvt[-3].intval);
				break;
			case LL_BOT:	/* test at loop bottom */
				if (yypvt[-5].nodep)
					deflab(yypvt[-9].intval);
				/*FALLTHROUGH*/
			case LL_DUP:	/* dup. test at top & bottom */
				if (yypvt[-5].nodep)
				{
					ecomp(buildtree(CBRANCH,
						buildtree(NOT, yypvt[-5].nodep, NIL),
						bcon((CONSZ) yypvt[-3].intval)));
				}
				else
					branch(yypvt[-3].intval);
				break;
			}
			deflab(brklab);
			if ((flostat & FBRK) || !(flostat & FLOOP))
				reached = 1;
			else
				reached = 0;
			resetbc(0);
		} break;
case 108:
# line 743 "gram.y"
{  if( reached ) branch( brklab );
			    deflab( yypvt[-1].intval );
			   swend();
			    deflab(brklab);
			    if( (flostat&FBRK) || !(flostat&FDEF) ) reached = 1;
			    resetbc(FCONT);
			    } break;
case 109:
# line 752 "gram.y"
{  if( brklab == NOLAB ) UERROR( MESSAGE( 50 ));
			    else if(reached) branch( brklab );
			    flostat |= FBRK;
			    if( brkflag ) goto rch;
			    reached = 0;
			    } break;
case 110:
# line 760 "gram.y"
{  if( contlab == NOLAB ) UERROR( MESSAGE( 55 ));
			    else branch( contlab );
			    flostat |= FCONT;
			    goto rch;
			    } break;
case 111:
# line 766 "gram.y"
{  retstat |= NRETVAL;
			    branch( retlab );
			rch:
			    /* "statement not reached" */
			    if( !reached && !reachflg ) WERROR( MESSAGE( 100 ));
			    reached = 0;
			    reachflg = 0;
			    } break;
case 112:
# line 775 "gram.y"
{  register NODE *temp;
			    idname = curftn;
			    temp = buildtree( NAME, NIL, NIL );
			    if(temp->in.type == TVOID)
				/* "void function %s cannot return value" */
				UERROR(MESSAGE( 116 ),
					stab[idname].sname);
			    temp->in.type = DECREF( temp->in.type );
			    temp = buildtree( RETURN, temp, yypvt[-1].nodep );
			    /* now, we have the type of the RHS correct */
			    temp->in.left->in.op = FREE;
			    temp->in.op = FREE;
			    ecomp( buildtree( FORCE, temp->in.right, NIL ) );
			    retstat |= RETVAL;
			    branch( retlab );
			    reached = 0;
			    reachflg = 0;
			    } break;
case 113:
# line 794 "gram.y"
{  register NODE *q;
			    q = block( FREE, NIL, NIL, INT|ARY, 0, INT );
			    q->tn.rval = idname = yypvt[-1].intval;
			    defid( q, ULABEL );
			    stab[idname].suse = -lineno;
			    branch( stab[idname].offset );
			     ref(yypvt[-1].intval, lineno); 
			    goto rch;
			    } break;
case 118:
# line 809 "gram.y"
{  register NODE *q;
			    q = block( FREE, NIL, NIL, INT|ARY, 0, LABEL );
			    q->tn.rval = yypvt[-1].intval;
			    defid( q, LABEL );
			    reached = 1;
			     def(yypvt[-1].intval, lineno); 
			    } break;
case 119:
# line 817 "gram.y"
{  addcase(yypvt[-1].nodep);
			    reached = 1;
			    } break;
case 120:
# line 821 "gram.y"
{  reached = 1;
			    adddef();
			    flostat |= FDEF;
			    } break;
case 121:
# line 827 "gram.y"
{  savebc();
			    /* "loop not entered at top" */
			    if( !reached ) WERROR( MESSAGE( 75 ));
			    brklab = getlab();
			    contlab = getlab();
			    deflab( yyval.intval = getlab() );
			    reached = 1;
			    } break;
case 122:
# line 837 "gram.y"
{  ecomp( buildtree( CBRANCH, yypvt[-1].nodep, bcon( (CONSZ) (yyval.intval=getlab())) ) ) ;
			    reached = 1;
			    } break;
case 123:
# line 842 "gram.y"
{  if( reached ) branch( yyval.intval = getlab() );
			    else yyval.intval = NOLAB;
			    deflab( yypvt[-2].intval );
			    reached = 1;
			    } break;
case 124:
# line 850 "gram.y"
{	int type;
				savebc();
			    brklab = getlab();
				type = BTYPE( yypvt[-1].nodep->in.type );
				if( ( SZLONG > SZINT ) && ( type == LONG || type == ULONG ) )
					/* long in case or switch statement may be truncated */
					WERROR( MESSAGE( 123 ));
			    ecomp( buildtree( FORCE, yypvt[-1].nodep, NIL ) );
			    branch( yyval.intval = getlab() );
			    swstart();
			    reached = 0;
			    } break;
case 125:
# line 864 "gram.y"
{ yyval.intval=instruct; stwart=instruct=0; } break;
case 126:
# line 866 "gram.y"
{  yyval.intval = icons( yypvt[-0].nodep );  instruct=yypvt[-1].intval; } break;
case 128:
# line 870 "gram.y"
{ yyval.nodep=0; } break;
case 130:
# line 875 "gram.y"
{  goto bop; } break;
case 131:
# line 879 "gram.y"
{
			preconf:
			    if( yychar==RELOP||yychar==EQUOP||yychar==AND||yychar==OR||yychar==ER ){
			    precplaint:
				/* "precedence confusion possible: parenthesize!" */
				if( hflag ) WERROR( MESSAGE( 92 ) );
				}
			bop:
			    yyval.nodep = buildtree( yypvt[-1].intval, yypvt[-2].nodep, yypvt[-0].nodep );
			    } break;
case 132:
# line 890 "gram.y"
{  yypvt[-1].intval = COMOP;
			    goto bop;
			    } break;
case 133:
# line 894 "gram.y"
{  goto bop; } break;
case 134:
# line 896 "gram.y"
{  if(yychar==SHIFTOP) goto precplaint; else goto bop; } break;
case 135:
# line 898 "gram.y"
{  if(yychar==SHIFTOP ) goto precplaint; else goto bop; } break;
case 136:
# line 900 "gram.y"
{  if(yychar==PLUS||yychar==MINUS) goto precplaint; else goto bop; } break;
case 137:
# line 902 "gram.y"
{  goto bop; } break;
case 138:
# line 904 "gram.y"
{  goto preconf; } break;
case 139:
# line 906 "gram.y"
{  if( yychar==RELOP||yychar==EQUOP ) goto preconf;  else goto bop; } break;
case 140:
# line 908 "gram.y"
{  if(yychar==RELOP||yychar==EQUOP) goto preconf; else goto bop; } break;
case 141:
# line 910 "gram.y"
{  if(yychar==RELOP||yychar==EQUOP) goto preconf; else goto bop; } break;
case 142:
# line 912 "gram.y"
{  goto bop; } break;
case 143:
# line 914 "gram.y"
{  goto bop; } break;
case 144:
# line 916 "gram.y"
{  abop:
				yyval.nodep = buildtree( ASG yypvt[-2].intval, yypvt[-3].nodep, yypvt[-0].nodep );
				} break;
case 145:
# line 920 "gram.y"
{  goto abop; } break;
case 146:
# line 922 "gram.y"
{  goto abop; } break;
case 147:
# line 924 "gram.y"
{  goto abop; } break;
case 148:
# line 926 "gram.y"
{  goto abop; } break;
case 149:
# line 928 "gram.y"
{  goto abop; } break;
case 150:
# line 930 "gram.y"
{  goto abop; } break;
case 151:
# line 932 "gram.y"
{  goto abop; } break;
case 152:
# line 934 "gram.y"
{  yyval.nodep=buildtree(QUEST, yypvt[-4].nodep, buildtree( COLON, yypvt[-2].nodep, yypvt[-0].nodep ) );
			    } break;
case 153:
# line 938 "gram.y"
{  UERROR( MESSAGE( 87 ) );  goto bop; } break;
case 154:
# line 940 "gram.y"
{  goto bop; } break;
case 156:
# line 944 "gram.y"
{  yyval.nodep = buildtree( yypvt[-0].intval, yypvt[-1].nodep, bcon((CONSZ) 1) ); } break;
case 157:
# line 946 "gram.y"
{ ubop:
			    yyval.nodep = buildtree( UNARY yypvt[-1].intval, yypvt[-0].nodep, NIL );
			    } break;
case 158:
# line 950 "gram.y"
{  if( ISFTN(yypvt[-0].nodep->in.type) || ISARY(yypvt[-0].nodep->in.type) ){
				/* "& before array or function: ignored" */
				WERROR( MESSAGE( 7 ) );
				yyval.nodep = yypvt[-0].nodep;
				}
			    else goto ubop;
			    } break;
case 159:
# line 958 "gram.y"
{  goto ubop; } break;
case 160:
# line 960 "gram.y"
{
			    yyval.nodep = buildtree( yypvt[-1].intval, yypvt[-0].nodep, NIL );
			    } break;
case 161:
# line 964 "gram.y"
{  yyval.nodep = buildtree( yypvt[-1].intval==INCR ? ASG PLUS : ASG MINUS,
						yypvt[-0].nodep,
						bcon((CONSZ) 1)  );
			    } break;
case 162:
# line 969 "gram.y"
{  yyval.nodep = doszof( yypvt[-0].nodep ); } break;
case 163:
# line 971 "gram.y"
{  yyval.nodep = buildtree( CAST, yypvt[-2].nodep, yypvt[-0].nodep );

		/* carefully, now...
		/* paintdown in buildtree returns the right child, thus
		/* we don't have to FREE nodes and return the right child here--
		/* we know this by the cast being tossed and having an ICON in
		/* hand upon the return from buildtree */

			    if (yyval.nodep->in.op != ICON)
			    {
				    yyval.nodep->in.left->in.op = FREE;
				    yyval.nodep->in.op = FREE;
				    yyval.nodep = yyval.nodep->in.right;
			    }
			} break;
case 164:
# line 987 "gram.y"
{  yyval.nodep = doszof( yypvt[-1].nodep ); } break;
case 165:
# line 989 "gram.y"
{  yyval.nodep = buildtree( UNARY MUL, buildtree( PLUS, yypvt[-3].nodep, yypvt[-1].nodep ), NIL ); } break;
case 166:
# line 991 "gram.y"
{  yyval.nodep=buildtree(UNARY CALL,yypvt[-1].nodep,NIL); } break;
case 167:
# line 993 "gram.y"
{  yyval.nodep=buildtree(CALL,yypvt[-2].nodep,yypvt[-1].nodep); } break;
case 168:
# line 995 "gram.y"
{  if( yypvt[-1].intval == DOT ){
				/* "structure reference must be addressable" */
				if( notlval( yypvt[-2].nodep ) )UERROR(MESSAGE( 105 ));
				yypvt[-2].nodep = buildtree( UNARY AND, yypvt[-2].nodep, NIL );
				}
			    idname = yypvt[-0].intval;
			    yyval.nodep = buildtree( STREF, yypvt[-2].nodep, buildtree( NAME, NIL, NIL ) );
			     ref(yypvt[-0].intval, lineno); 
			    } break;
case 169:
# line 1005 "gram.y"
{  idname = yypvt[-0].intval;
			    /* recognize identifiers in initializations */
			    if( blevel==0 && stab[idname].stype == UNDEF ) {
				register NODE *q;
				/* "undeclared initializer name %.8s" */
				/* "undeclared initializer name %s" */
				WERROR( MESSAGE( 111 ), stab[idname].sname );
				q = block( FREE, NIL, NIL, INT, 0, INT );
				q->tn.rval = idname;
				defid( q, EXTERN );
				}
			    yyval.nodep=buildtree(NAME,NIL,NIL);
			    stab[yypvt[-0].intval].suse = -lineno;
			     ref(yypvt[-0].intval, lineno); 
			} break;
case 170:
# line 1021 "gram.y"
{  yyval.nodep=bcon((CONSZ) 0);
			    yyval.nodep->tn.lval = lastcon;
# ifdef BIGCONSTS
			    yyval.nodep->tn.l2val = last2con;
# endif
			    yyval.nodep->tn.rval = NONAME;
			    if( yypvt[-0].intval ) yyval.nodep->fn.csiz = yyval.nodep->in.type = ctype(LONG);
			    } break;
case 171:
# line 1030 "gram.y"
{  yyval.nodep=buildtree(FCON,NIL,NIL);
			    yyval.nodep->fpn.dval = dcon;
			    } break;
case 172:
# line 1034 "gram.y"
{  yyval.nodep = getstr(); /* get string contents */ } break;
case 173:
# line 1036 "gram.y"
{ yyval.nodep=yypvt[-1].nodep; } break;
case 174:
# line 1040 "gram.y"
{
			yyval.nodep = tymerge( yypvt[-1].nodep, yypvt[-0].nodep );
			yyval.nodep->in.op = NAME;
			yypvt[-1].nodep->in.op = FREE;
			} break;
case 175:
# line 1048 "gram.y"
{ yyval.nodep = bdty( NAME, NIL, -1 ); } break;
case 176:
# line 1050 "gram.y"
{ yyval.nodep = bdty( UNARY CALL, bdty(NAME,NIL,-1),0); } break;
case 177:
# line 1052 "gram.y"
{  yyval.nodep = bdty( UNARY CALL, yypvt[-3].nodep, 0 ); } break;
case 178:
# line 1054 "gram.y"
{  goto umul; } break;
case 179:
# line 1056 "gram.y"
{  goto uary; } break;
case 180:
# line 1058 "gram.y"
{  goto bary;  } break;
case 181:
# line 1060 "gram.y"
{ yyval.nodep = yypvt[-1].nodep; } break;
case 182:
# line 1064 "gram.y"
{  if( stab[yypvt[-1].intval].stype == UNDEF ){
				register NODE *q;
				q = block( FREE, NIL, NIL, FTN|INT, 0, INT );
				q->tn.rval = yypvt[-1].intval;
				defid( q, EXTERN );
				}
			    idname = yypvt[-1].intval;
			    yyval.nodep=buildtree(NAME,NIL,NIL);
			    stab[idname].suse = -lineno;
			     ref(yypvt[-1].intval, lineno); 
			} break;
	}
	goto yystack;		/* reset registers in driver code */
}
