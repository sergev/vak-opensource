# define number 257
# define stop 258
# define DEFINED 259
# define EQ 260
# define NE 261
# define LE 262
# define GE 263
# define LS 264
# define RS 265
# define ANDAND 266
# define OROR 267
# define UMINUS 268
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern int yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
#ifndef YYSTYPE
#define YYSTYPE int
#endif
YYSTYPE yylval, yyval;
typedef int yytabelem;
# define YYERRCODE 256

# line 80 "../cpp/cpy.y"

# include "yylex.c"
yytabelem yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
-1, 44,
	262, 0,
	263, 0,
	60, 0,
	62, 0,
	-2, 9,
-1, 45,
	262, 0,
	263, 0,
	60, 0,
	62, 0,
	-2, 10,
-1, 46,
	262, 0,
	263, 0,
	60, 0,
	62, 0,
	-2, 11,
-1, 47,
	262, 0,
	263, 0,
	60, 0,
	62, 0,
	-2, 12,
-1, 48,
	260, 0,
	261, 0,
	-2, 13,
-1, 49,
	260, 0,
	261, 0,
	-2, 14,
	};
# define YYNPROD 30
# define YYLAST 363
yytabelem yyact[]={

    13,    24,    35,    58,    13,    11,    14,    30,    15,    11,
    12,    60,    13,    24,    12,     1,    57,    11,    14,    30,
    15,    59,    12,    18,    13,    19,    29,     0,     0,    11,
    14,     0,    15,     0,    12,    18,     3,    19,    29,    13,
    24,    31,    32,    33,    11,    14,    30,    15,     0,    12,
    13,    24,     0,     0,     0,    11,    14,    25,    15,     5,
    12,     0,    18,     0,    19,    29,     7,     0,     0,    25,
     0,     4,     0,    18,     0,    19,    29,     0,    13,    24,
     0,     0,     0,    11,    14,     0,    15,    26,    12,     0,
     0,     0,    13,    24,     0,     0,    25,    11,    14,    26,
    15,    18,    12,    19,    13,    24,     0,    25,     0,    11,
    14,     0,    15,    13,    12,    18,     0,    19,    11,    14,
     0,    15,     0,    12,     0,     0,    26,    18,     0,    19,
     0,     0,     0,     0,    13,    25,    18,    26,    19,    11,
    14,     0,    15,     0,    12,    13,     0,     0,     0,    25,
    11,    14,     6,    15,     0,    12,     0,    18,     0,    19,
     0,     0,     0,     0,     0,    26,     0,     0,     0,     0,
     0,     0,     2,     0,     0,     0,     0,     0,     0,    26,
    34,     0,     0,     0,    37,    38,    39,    40,    41,    42,
    43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
    53,    54,    55,    56,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    36,
     0,     0,     0,    22,    23,    20,    21,    16,    17,    27,
    28,     0,    61,     0,     0,    22,    23,    20,    21,    16,
    17,    27,    28,     0,     0,     0,     0,     0,     0,     0,
     0,    16,    17,     0,     0,     0,     0,     0,     0,     0,
    10,     0,    22,    23,    20,    21,    16,    17,    27,    28,
     0,     0,     0,    22,    23,    20,    21,    16,    17,    27,
    28,     0,     0,     9,     0,     8,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    22,    23,    20,    21,    16,    17,    27,     0,     0,
     0,     0,     0,     0,     0,    22,    23,    20,    21,    16,
    17,     0,     0,     0,     0,     0,     0,    22,    23,    20,
    21,    16,    17,     0,     0,     0,    22,    23,    20,    21,
    16,    17,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    20,
    21,    16,    17 };
yytabelem yypact[]={

    26, -1000,     2, -1000,    26,    26,    26,    26,   -38, -1000,
 -1000,    26,    26,    26,    26,    26,    26,    26,    26,    26,
    26,    26,    26,    26,    26,    26,    26,    26,    26,    26,
    26, -1000, -1000, -1000,   -25,  -254, -1000, -1000, -1000, -1000,
   -33,   -33,   108,   108,   -13,   -13,   -13,   -13,    97,    97,
    76,    67,    67,    55,    41,   -37,    13, -1000,   -30,    26,
 -1000,    13 };
yytabelem yypgo[]={

     0,    15,   172,    36 };
yytabelem yyr1[]={

     0,     1,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     3,     3,     3,     3,     3,     3,     3 };
yytabelem yyr2[]={

     0,     5,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
    11,     7,     3,     5,     5,     5,     7,     9,     5,     3 };
yytabelem yychk[]={

 -1000,    -1,    -2,    -3,    45,    33,   126,    40,   259,   257,
   258,    42,    47,    37,    43,    45,   264,   265,    60,    62,
   262,   263,   260,   261,    38,    94,   124,   266,   267,    63,
    44,    -3,    -3,    -3,    -2,    40,   257,    -2,    -2,    -2,
    -2,    -2,    -2,    -2,    -2,    -2,    -2,    -2,    -2,    -2,
    -2,    -2,    -2,    -2,    -2,    -2,    -2,    41,   257,    58,
    41,    -2 };
yytabelem yydef[]={

     0,    -2,     0,    22,     0,     0,     0,     0,     0,    29,
     1,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    23,    24,    25,     0,     0,    28,     2,     3,     4,
     5,     6,     7,     8,    -2,    -2,    -2,    -2,    -2,    -2,
    15,    16,    17,    18,    19,     0,    21,    26,     0,     0,
    27,    20 };
typedef struct { char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

yytoktype yytoks[] =
{
	"number",	257,
	"stop",	258,
	"DEFINED",	259,
	"EQ",	260,
	"NE",	261,
	"LE",	262,
	"GE",	263,
	"LS",	264,
	"RS",	265,
	"ANDAND",	266,
	"OROR",	267,
	",",	44,
	"=",	61,
	"?",	63,
	":",	58,
	"|",	124,
	"^",	94,
	"&",	38,
	"<",	60,
	">",	62,
	"+",	43,
	"-",	45,
	"*",	42,
	"/",	47,
	"%",	37,
	"!",	33,
	"~",	126,
	"UMINUS",	268,
	"(",	40,
	".",	46,
	"-unknown-",	-1	/* ends search */
};

char * yyreds[] =
{
	"-no such reduction-",
	"S : e stop",
	"e : e '*' e",
	"e : e '/' e",
	"e : e '%' e",
	"e : e '+' e",
	"e : e '-' e",
	"e : e LS e",
	"e : e RS e",
	"e : e '<' e",
	"e : e '>' e",
	"e : e LE e",
	"e : e GE e",
	"e : e EQ e",
	"e : e NE e",
	"e : e '&' e",
	"e : e '^' e",
	"e : e '|' e",
	"e : e ANDAND e",
	"e : e OROR e",
	"e : e '?' e ':' e",
	"e : e ',' e",
	"e : term",
	"term : '-' term",
	"term : '!' term",
	"term : '~' term",
	"term : '(' e ')'",
	"term : DEFINED '(' number ')'",
	"term : DEFINED number",
	"term : number",
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
		
case 1:
# line 20 "../cpp/cpy.y"
{return(yypvt[-1]);} break;
case 2:
# line 24 "../cpp/cpy.y"
{yyval = yypvt[-2] * yypvt[-0];} break;
case 3:
# line 26 "../cpp/cpy.y"
{yyval = yypvt[-2] / yypvt[-0];} break;
case 4:
# line 28 "../cpp/cpy.y"
{yyval = yypvt[-2] % yypvt[-0];} break;
case 5:
# line 30 "../cpp/cpy.y"
{yyval = yypvt[-2] + yypvt[-0];} break;
case 6:
# line 32 "../cpp/cpy.y"
{yyval = yypvt[-2] - yypvt[-0];} break;
case 7:
# line 34 "../cpp/cpy.y"
{yyval = yypvt[-2] << yypvt[-0];} break;
case 8:
# line 36 "../cpp/cpy.y"
{yyval = yypvt[-2] >> yypvt[-0];} break;
case 9:
# line 38 "../cpp/cpy.y"
{yyval = yypvt[-2] < yypvt[-0];} break;
case 10:
# line 40 "../cpp/cpy.y"
{yyval = yypvt[-2] > yypvt[-0];} break;
case 11:
# line 42 "../cpp/cpy.y"
{yyval = yypvt[-2] <= yypvt[-0];} break;
case 12:
# line 44 "../cpp/cpy.y"
{yyval = yypvt[-2] >= yypvt[-0];} break;
case 13:
# line 46 "../cpp/cpy.y"
{yyval = yypvt[-2] == yypvt[-0];} break;
case 14:
# line 48 "../cpp/cpy.y"
{yyval = yypvt[-2] != yypvt[-0];} break;
case 15:
# line 50 "../cpp/cpy.y"
{yyval = yypvt[-2] & yypvt[-0];} break;
case 16:
# line 52 "../cpp/cpy.y"
{yyval = yypvt[-2] ^ yypvt[-0];} break;
case 17:
# line 54 "../cpp/cpy.y"
{yyval = yypvt[-2] | yypvt[-0];} break;
case 18:
# line 56 "../cpp/cpy.y"
{yyval = yypvt[-2] && yypvt[-0];} break;
case 19:
# line 58 "../cpp/cpy.y"
{yyval = yypvt[-2] || yypvt[-0];} break;
case 20:
# line 60 "../cpp/cpy.y"
{yyval = yypvt[-4] ? yypvt[-2] : yypvt[-0];} break;
case 21:
# line 62 "../cpp/cpy.y"
{yyval = yypvt[-0];} break;
case 22:
# line 64 "../cpp/cpy.y"
{yyval = yypvt[-0];} break;
case 23:
# line 67 "../cpp/cpy.y"
{yyval = -yypvt[-0];} break;
case 24:
# line 69 "../cpp/cpy.y"
{yyval = !yypvt[-0];} break;
case 25:
# line 71 "../cpp/cpy.y"
{yyval = ~yypvt[-0];} break;
case 26:
# line 73 "../cpp/cpy.y"
{yyval = yypvt[-1];} break;
case 27:
# line 75 "../cpp/cpy.y"
{yyval= yypvt[-1];} break;
case 28:
# line 77 "../cpp/cpy.y"
{yyval = yypvt[-0];} break;
case 29:
# line 79 "../cpp/cpy.y"
{yyval= yypvt[-0];} break;
	}
	goto yystack;		/* reset registers in driver code */
}
