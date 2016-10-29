/*	@(#)scan.c	1.4	*/


# include "mfile1"
# include "messages.h"

# ifndef ASMBUF
# define ASMBUF 50
# endif
char asmbuf[ASMBUF];
char *asmp;
int asm_esc = 0; /* asm escaped used in file */
	/* lexical actions */

# define A_ERR 0		/* illegal character */
# define A_LET 1		/* saw a letter */
# define A_DIG 2		/* saw a digit */
# define A_1C 3			/* return a single character */
# define A_STR 4		/* string */
# define A_CC 5			/* character constant */
# define A_BCD 6		/* GCOS BCD constant */
# define A_SL 7			/* saw a / */
# define A_DOT 8		/* saw a . */
# define A_PL 9		/* + */
# define A_MI 10		/* - */
# define A_EQ 11		/* = */
# define A_NOT 12		/* ! */
# define A_LT 13		/* < */
# define A_GT 14		/* > */
# define A_AND 16		/* & */
# define A_OR 17		/* | */
# define A_WS 18		/* whitespace (not \n) */
# define A_NL 19		/* \n */

	/* character classes */

# define LEXLET 01
# define LEXDIG 02
# define LEXOCT 04
# define LEXHEX 010
# define LEXWS 020
# define LEXDOT 040

	/* reserved word actions */

# define AR_TY 0		/* type word */
# define AR_RW 1		/* simple reserved word */
# define AR_CL 2		/* storage class word */
# define AR_S 3		/* struct */
# define AR_U 4		/* union */
# define AR_E 5		/* enum */
# define AR_A 6		/* asm */

	/* text buffer */
#ifdef FLEXNAMES
#	define LXTSZ BUFSIZ
#else
#	define LXTSZ 100
#endif
char yytext[LXTSZ];
char * lxgcp;
#ifdef FLEXNAMES
	int truncate_flag = 0;		/* force all names <= 8 chars. */
#endif
extern char *malloc (), *calloc ();
OFFSZ caloff();
	/* ARGSUSED */
mainp1( argc, argv ) int argc; char *argv[]; {  /* control multiple files */

	register i;
	register char *cp;
	extern int idebug, bdebug, tdebug, edebug, ddebug, xdebug;
	extern OFFSZ offsz;
	char *release = "PCC Elbrus-B   Copyright (C) 1987-1990 Demos/*";

	offsz = caloff();
	for( i=1; i<argc; ++i ){
		if( *(cp=argv[i]) == '-' && *++cp == 'X' ){
			while( *++cp ){
				switch( *cp ){

				case 'r':
					fprintf( stderr, "Release: %s\n",
						release );
					break;

				case 'd':
					++ddebug;
					break;
				case 'i':
					++idebug;
					break;
				case 'b':
					++bdebug;
					break;
				case 't':
					++tdebug;
					break;
				case 'e':
					++edebug;
					break;
				case 'x':
					++xdebug;
					break;
#ifdef FLEXNAMES
				case 'T':
					++truncate_flag;
					break;
#endif
					}
				}
			}
		}

# ifdef ONEPASS
	p2init( argc, argv );
# endif

	for( i=0; i<SYMTSZ; ++i ) stab[i].stype = TNULL;

	lxinit();
	tinit();
	mkdope();

	lineno = 1;

	/* dimension table initialization */

	dimtab[0] = 0;
	dimtab[CHAR] = SZCHAR;
	dimtab[INT] = SZINT;
	dimtab[FLOAT] = SZFLOAT;
	dimtab[DOUBLE] = SZDOUBLE;
	dimtab[LONG] = SZLONG;
	dimtab[SHORT] = SZSHORT;
	dimtab[UCHAR] = SZCHAR;
	dimtab[USHORT] = SZSHORT;
	dimtab[UNSIGNED] = SZINT;
	dimtab[ULONG] = SZLONG;
	/* starts past any of the above */
	curdim = 16;
	reached = 1;
	reachflg = 0;

	yyparse();
	yyaccpt();

	ejobcode( nerrors ? 1 : 0 );
	return(nerrors?1:0);

	}

# define CSMASK 0377
# define CSSZ 256

short lxmask[CSSZ+1];

lxenter( s, m ) register char *s; register short m; {
	/* enter a mask into lxmask */
	register c;

	while( c = (*s++) & CSMASK ) lxmask[c+1] |= m;

	}


# define lxget(c,m) (lxgcp=yytext,lxmore(c,m))

lxmore( c, m )  register c, m; {
	register char *cp;

	*(cp = lxgcp) = c;
	while( c=getchar(), lxmask[c+1]&m ){
		if( cp < &yytext[LXTSZ-1] ){
			*++cp = c;
			}
		}
	ungetc(c,stdin);
	*(lxgcp = cp+1) = '\0';
	}

struct lxdope {
	short lxch;	/* the character */
	short lxact;	/* the action to be performed */
	short lxtok;	/* the token number to be returned */
	short lxval;	/* the value to be returned */
	} lxdope[] = {

	'$',	A_ERR,	0,	0,	/* illegal characters go here... */
	'_',	A_LET,	0,	0,	/* letters point here */
	'0',	A_DIG,	0,	0,	/* digits point here */
	' ',	A_WS,	0,	0,	/* whitespace goes here */
	'\n',	A_NL,	0,	0,
	'"',	A_STR,	0,	0,	/* character string */
	'\'',	A_CC,	0,	0,	/* character constant */
	'`',	A_BCD,	0,	0,	/* GCOS BCD constant */
	'(',	A_1C,	LP,	0,
	')',	A_1C,	RP,	0,
	'{',	A_1C,	LC,	0,
	'}',	A_1C,	RC,	0,
	'[',	A_1C,	LB,	0,
	']',	A_1C,	RB,	0,
	'*',	A_1C,	MUL,	MUL,
	'?',	A_1C,	QUEST,	0,
	':',	A_1C,	COLON,	0,
	'+',	A_PL,	PLUS,	PLUS,
	'-',	A_MI,	MINUS,	MINUS,
	'/',	A_SL,	DIVOP,	DIV,
	'%',	A_1C,	DIVOP,	MOD,
	'&',	A_AND,	AND,	AND,
	'|',	A_OR,	OR,	OR,
	'^',	A_1C,	ER,	ER,
	'!',	A_NOT,	UNOP,	NOT,
	'~',	A_1C,	UNOP,	COMPL,
	',',	A_1C,	CM,	CM,
	';',	A_1C,	SM,	0,
	'.',	A_DOT,	STROP,	DOT,
	'<',	A_LT,	RELOP,	LT,
	'>',	A_GT,	RELOP,	GT,
	'=',	A_EQ,	ASSIGN,	ASSIGN,
	-1,	A_1C,	0,	0,
	};

struct lxdope *lxcp[CSSZ+1];

lxinit(){
	register struct lxdope *p;
	register i;
	register char *cp;
	/* set up character classes */

	lxenter( "_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\
юабцдефгхийклмнопярстужвьызшэщчъЮАБЦДЕФГХИЙКЛМНОПЯРСТУЖВЬЫЗШЭЩЧ", LEXLET );
	lxenter( "0123456789", LEXDIG );
	lxenter( "0123456789abcdefABCDEF", LEXHEX );
		/* \013 should become \v someday; \013 is OK for ASCII and EBCDIC */
	lxenter( " \t\r\b\f\013", LEXWS );
	lxenter( "01234567", LEXOCT );
	lxmask['.'+1] |= LEXDOT;

	/* make lxcp point to appropriate lxdope entry for each character */

	/* initialize error entries */

	for( i= 0; i<=CSSZ; ++i ) lxcp[i] = lxdope;

	/* make unique entries */

	for( p=lxdope; ; ++p ) {
		lxcp[p->lxch+1] = p;
		if( p->lxch < 0 ) break;
		}

	/* handle letters, digits, and whitespace */
	/* by convention, first, second, and third places */

	cp = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\
юабцдефгхийклмнопярстужвьызшэщчъЮАБЦДЕФГХИЙКЛМНОПЯРСТУЖВЬЫЗШЭЩЧ";
	while( *cp ) lxcp[((*cp++) & CSMASK) + 1] = &lxdope[1];
	cp = "123456789";
	while( *cp ) lxcp[*cp++ + 1] = &lxdope[2];
	cp = "\t\b\r\f\013";
	while( *cp ) lxcp[*cp++ + 1] = &lxdope[3];

	/* first line might have title */
	lxtitle();

	}

int lxmatch;  /* character to be matched in char or string constant */

lxstr(ct){
	/* match a string or character constant, up to lxmatch */

	register c;
	register val;
	register i;

	i=0;
	while( (c=getchar()) != lxmatch ){
		switch( c ) {

		case EOF:
			/* "unexpected EOF" */
			UERROR( MESSAGE( 113 ) );
			break;

		case '\n':
			/* "newline in string or char constant" */
			UERROR( MESSAGE( 78 ) );
			++lineno;
			break;

		case '\\':
			switch( c = getchar() ){

			case '\n':
				++lineno;
				continue;

			default:
				val = c;
				goto mkcc;

			case 'n':
				val = '\n';
				goto mkcc;

			case 'r':
				val = '\r';
				goto mkcc;

			case 'b':
				val = '\b';
				goto mkcc;

			case 't':
				val = '\t';
				goto mkcc;

			case 'f':
				val = '\f';
				goto mkcc;

			case 'v':
				val = '\013';
				goto mkcc;

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				val = c-'0';
				c=getchar();  /* try for 2 */
				if( lxmask[c+1] & LEXOCT ){
					val = (val<<3) | (c-'0');
					c = getchar();  /* try for 3 */
					if( lxmask[c+1] & LEXOCT ){
						val = (val<<3) | (c-'0');
						}
					else ungetc( c ,stdin);
					}
				else ungetc( c ,stdin);

				goto mkcc1;

				}
		default:
			val =c;
		mkcc:
			val = CCTRANS(val);
		mkcc1:
			if( lxmatch == '\'' ){
				val = CHARCAST(val);  /* it is, after all, a "character" constant */
				makecc( val, i );
				}
			else { /* stash the byte into the string */
				if( strflg ) {
					if( ct==0 || i<ct ) putbyte( val );
					/* "non-null byte ignored in string initializer" */
					else if( i == ct ) WERROR( MESSAGE( 81 ) );
					}
				else bycode( val, i );
				}
			++i;
			continue;
			}
		break;
		}
	/* end of string or  char constant */

	if( lxmatch == '"' ){
		if( strflg ){ /* end the string */
			if( ct==0 || i<ct ) putbyte( 0 );  /* the null at the end */
			}
		else {  /* the initializer gets a null byte */
			bycode( 0, i++ );
			bycode( -1, i );
			dimtab[curdim] = i;  /* in case of later sizeof ... */
			}
		}
	else { /* end the character constant */
		/* "empty character constant" */
		if( i == 0 ) UERROR( MESSAGE( 36 ) );
		if( i>(SZINT/SZCHAR) || ( (pflag||hflag)&&i>1) )
			/* "too many characters in character constant" */
			UERROR( MESSAGE( 107 ) );
		}
	}

lxcom(){
	register c;
	/* saw a /*: process a comment */

	for(;;){

		switch( c = getchar() ){

		case EOF:
			/* "unexpected EOF"  */
			UERROR( MESSAGE( 113 ) );
			return;

		case '\n':
			++lineno;

		default:
			continue;

		case '*':
			if( (c = getchar()) == '/' ) return;
			else ungetc( c ,stdin);
			continue;

# ifdef LINT
		case 'V':
			lxget( c, LEXLET|LEXDIG );
			{
				extern int vaflag;
				int i;
				i = yytext[7]?yytext[7]-'0':0;
				yytext[7] = '\0';
				if( strcmp( yytext, "VARARGS" ) ) continue;
				vaflag = i;
				continue;
				}
		case 'L':
			lxget( c, LEXLET );
			if( strcmp( yytext, "LINTLIBRARY" ) ) continue;
			{
				extern int libflag, vflag;
				libflag = 1;
				vflag = 0;
				}
			continue;

		case 'A':
			lxget( c, LEXLET );
			if( strcmp( yytext, "ARGSUSED" ) ) continue;
			{
				extern int argflag, vflag;
				argflag = 1;
				vflag = 0;
				}
			continue;

		case 'N':
			lxget( c, LEXLET );
			if( strcmp( yytext, "NOTREACHED" ) ) continue;
			reached = 0;
			{
			 /* 6/18/80 */
				extern int reachflg;
				reachflg = 1;
			}
			continue;
# endif
			}
		}
	}

yylex(){
	for(;;){

		register lxchar;
		register struct lxdope *p;
		register struct symtab *sp;
		int id;

		switch( (p=lxcp[(lxchar=getchar())+1])->lxact ){

		onechar:
			ungetc( lxchar ,stdin);

		case A_1C:
			/* eat up a single character, and return an opcode */

			yylval.intval = p->lxval;
			return( p->lxtok );

		case A_ERR:
			/* "illegal character: %03o (octal)" */
			UERROR( MESSAGE( 51 ), lxchar );
			break;

		case A_LET:
			/* collect an identifier, check for reserved word, and return */
			lxget( lxchar, LEXLET|LEXDIG );
			if( (lxchar=lxres()) > 0 ) return( lxchar ); /* reserved word */
			if( lxchar== 0 ) continue;
#ifdef FLEXNAMES
			if ( truncate_flag )
				yytext[8] = '\0';	/* truncate it */
			id = lookup( hash( yytext ),
#else
			id = lookup( yytext,
#endif
				/* tag name for struct/union/enum */
				(stwart&TAGNAME)? STAG:
				/* member name for struct/union */
				(stwart&(INSTRUCT|INUNION|FUNNYNAME))?SMOS:0 );
			sp = &stab[id];
			if( sp->sclass == TYPEDEF && !stwart ){
#ifdef CXREF
				ref(id, lineno);
#endif
				stwart = instruct;
				yylval.nodep = mkty( sp->stype, sp->dimoff, sp->sizoff );
				return( TYPE );
				}
			stwart = (stwart&SEENAME) ? instruct : 0;
			yylval.intval = id;
			return( NAME );

		case A_DIG:
			/* collect a digit string, then look at last one... */
			{
			CONSZ maxint;

			lastcon = 0;
# ifdef BIGCONSTS
			last2con = 0;
# endif
			lxget( lxchar, LEXDIG );
			switch( lxchar=getchar() ){

			case 'x':
			case 'X':
				/* "illegal hex constant"  */
				if( yytext[0] != '0' || yytext[1] ) UERROR( MESSAGE( 59 ) );
				lxmore( lxchar, LEXHEX );
				/* convert the value */
				{
					register char *cp;
					for( cp = yytext+2; *cp; ++cp ){
						/* this code won't work for all wild character sets,
						   but seems ok for ascii and ebcdic */
# ifdef BIGCONSTS
						last2con <<= 4;
						last2con |= lastcon >> (sizeof (lastcon) * 8 - 4);
# endif
						lastcon <<= 4;
						if( *cp>='0' && *cp<='9' )
							lastcon |= *cp-'0';
						else if( *cp>='A' && *cp<='Z' )
							lastcon |= *cp - 'A'+ 10;
						else
							lastcon |= *cp - 'a'+ 10;
						}
					}

			hexlong:
				/* criterion for longness for hex and octal
					constants is that it
				   does not fit within an int */
				maxint = ~( ~0L << SZINT );
				if( (SZINT != SZLONG) && (lastcon > maxint || lastcon < 0) )
					yylval.intval = 1;
				else
					yylval.intval = 0;

				goto islong;

			case '.':
				lxmore( lxchar, LEXDIG );

			getfp:
				if( (lxchar=getchar()) == 'e' || lxchar == 'E' ){ /* exponent */

			case 'e':
			case 'E':
					if( (lxchar=getchar()) == '+' || lxchar == '-' ){
						*lxgcp++ = 'e';
						}
					else {
						ungetc(lxchar,stdin);
						lxchar = 'e';
						}
					lxmore( lxchar, LEXDIG );
					/* now have the whole thing... */
					}
				else {  /* no exponent */
					ungetc( lxchar ,stdin);
					}
				return( isitfloat( yytext ) );

			default:
				ungetc( lxchar ,stdin);
				if( yytext[0] == '0' ){
					/* convert in octal */
					register char *cp;
					for( cp = yytext+1; *cp; ++cp ){
						if( *cp > '7' )
							/* bad octal digit */
							WERROR( MESSAGE( 124 ), *cp );
# ifdef BIGCONSTS
						last2con <<= 3;
						last2con |= lastcon >> (sizeof (lastcon) * 8 - 3);
# endif
						lastcon <<= 3;
						lastcon |= *cp - '0';
						}
					goto hexlong;
					}
				else {
					/* convert in decimal */
					register char *cp;
					for( cp = yytext; *cp; ++cp ){ /*don't let lastcon overflow needlessly */
						lastcon *= (CONSZ)10;
						lastcon += (CONSZ)(*cp - '0');
						}
					}

				/* decide if it is long or not (decimal case) */

				/* if it fits in SZINT-1 bits it is int; otherwise long */
				/* if there is an l or L following, all bets are off... */
				maxint = ~( ~0L << (SZINT-1) );
				if( (SZINT != SZLONG) && (lastcon > maxint || lastcon < 0) )
					yylval.intval = 1;
				else
					yylval.intval = 0;


			islong:
				/* finally, look for trailing L or l */
				if( (lxchar = getchar()) != 'L' && lxchar != 'l' )
					ungetc( lxchar ,stdin);
				else
					yylval.intval = 1;

				if( (SZINT != SZLONG) && !yylval.intval )
					lastcon = (int)lastcon;
				return( ICON );
				}
			} /* end of case A_DIG */

		case A_DOT:
			/* look for a dot: if followed by a digit, floating point */
			lxchar = getchar();
			if( lxmask[lxchar+1] & LEXDIG ){
				ungetc(lxchar,stdin);
				lxget( '.', LEXDIG );
				goto getfp;
				}
			stwart = FUNNYNAME;
			goto onechar;

		case A_STR:
			/* string constant */
			lxmatch = '"';
			return( STRING );

		case A_CC:
			/* character constant */
			lxmatch = '\'';
			lastcon = 0;
# ifdef BIGCONSTS
			last2con = 0;
# endif
			lxstr(0);
			yylval.intval = 0;
			return( ICON );

		case A_BCD:
			{
				register i;
				int j;
				for( i=0; i<LXTSZ; ++i ){
					if( ( j = getchar() ) == '`' ) break;
					if( j == '\n' ){
						/* "newline in BCD constant" */
						UERROR( MESSAGE( 77 ) );
						break;
						}
					yytext[i] = j;
					}
				yytext[i] = '\0';
				/* "BCD constant exceeds 6 characters" */
				if( i>6 ) UERROR( MESSAGE( 10 ) );
# ifdef gcos
				else strtob( yytext, ( unsigned * )&lastcon,( unsigned) i );
				lastcon = ( unsigned ) lastcon >> 6*(6-i);
# else
				/* "gcos BCD constant illegal" */
				UERROR( MESSAGE( 48 ) );
# endif
				yylval.intval = 0;  /* not long */
				return( ICON );
				}

		case A_SL:
			/* / */
			if( (lxchar=getchar()) != '*' ) goto onechar;
			lxcom();
		case A_WS:
			continue;

		case A_NL:
			++lineno;
			lxtitle();
			continue;

		case A_NOT:
			/* ! */
			if( (lxchar=getchar()) != '=' ) goto onechar;
			yylval.intval = NE;
			return( EQUOP );

		case A_MI:
			/* - */
			if( (lxchar=getchar()) == '-' ){
				yylval.intval = DECR;
				return( INCOP );
				}
			if( lxchar != '>' ) goto onechar;
			stwart = FUNNYNAME;
			yylval.intval=STREF;
			return( STROP );

		case A_PL:
			/* + */
			if( (lxchar=getchar()) != '+' ) goto onechar;
			yylval.intval = INCR;
			return( INCOP );

		case A_AND:
			/* & */
			if( (lxchar=getchar()) != '&' ) goto onechar;
			return( yylval.intval = ANDAND );

		case A_OR:
			/* | */
			if( (lxchar=getchar()) != '|' ) goto onechar;
			return( yylval.intval = OROR );

		case A_LT:
			/* < */
			if( (lxchar=getchar()) == '<' ){
				yylval.intval = LS;
				return( SHIFTOP );
				}
			if( lxchar != '=' ) goto onechar;
			yylval.intval = LE;
			return( RELOP );

		case A_GT:
			/* > */
			if( (lxchar=getchar()) == '>' ){
				yylval.intval = RS;
				return(SHIFTOP );
				}
			if( lxchar != '=' ) goto onechar;
			yylval.intval = GE;
			return( RELOP );

		case A_EQ:
			/* = */
			switch( lxchar = getchar() ){

			case '=':
				yylval.intval = EQ;
				return( EQUOP );

			case '-':
			case '*':
			case '&':
				/* "ambiguous assignment: simple assign, unary op assumed" */
				WERROR( MESSAGE( 12 ) );
				goto onechar;

			case '+':
			case '/':
			case '%':
			case '|':
			case '^':
			case '<':
			case '>':
				/* "old style assign-op causes syntax error" */
				WERROR( MESSAGE( 126 ) );

			default:
				goto onechar;

				}

		default:
			cerror( "yylex error, character %03o (octal)", lxchar );

			}

		/* ordinarily, repeat here... */
		cerror( "out of switch in yylex" );

		}

	}

struct lxrdope {
	/* dope for reserved, in alphabetical order */

	char *lxrch;	/* name of reserved word */
	short lxract;	/* reserved word action */
	short lxrval;	/* value to be returned */
	} lxrdope[] = {

	"asm",          AR_A,   0,
	"auto",         AR_CL,  AUTO,
	"break",        AR_RW,  BREAK,
	"char",         AR_TY,  CHAR,
	"case",         AR_RW,  CASE,
	"continue",     AR_RW,  CONTINUE,
	"double",       AR_TY,  DOUBLE,
	"default",      AR_RW,  DEFAULT,
	"do",           AR_RW,  DO,
	"extern",       AR_CL,  EXTERN,
	"else",         AR_RW,  ELSE,
	"enum",         AR_E,   ENUM,
	"for",          AR_RW,  FOR,
	"float",        AR_TY,  FLOAT,
	"fortran",      AR_CL,  FORTRAN,
	"goto",         AR_RW,  GOTO,
	"if",           AR_RW,  IF,
	"int",          AR_TY,  INT,
	"long",         AR_TY,  LONG,
	"return",       AR_RW,  RETURN,
	"register",     AR_CL,  REGISTER,
	"switch",       AR_RW,  SWITCH,
	"struct",       AR_S,   0,
	"sizeof",       AR_RW,  SIZEOF,
	"short",        AR_TY,  SHORT,
	"static",       AR_CL,  STATIC,
	"typedef",      AR_CL,  TYPEDEF,
	"unsigned",     AR_TY,  UNSIGNED,
	"union",        AR_U,   0,
	"void",         AR_TY,  UNDEF,  /* tymerge adds FTN */
	"while",        AR_RW,  WHILE,

	"авто",         AR_CL,  AUTO,
	"асм",          AR_A,   0,
	"бз",           AR_TY,  UNSIGNED,
	"вариант",      AR_RW,  CASE,
	"внешн",        AR_CL,  EXTERN,
	"возврат",      AR_RW,  RETURN,
	"выбор",        AR_RW,  SWITCH,
	"двойн",        AR_TY,  DOUBLE,
	"длин",         AR_TY,  LONG,
	"для",          AR_RW,  FOR,
	"если",         AR_RW,  IF,
	"запись",       AR_S,   0,
	"иначе",        AR_RW,  ELSE,
	"коротк",       AR_TY,  SHORT,
	"на",           AR_RW,  GOTO,
	"перечисл",     AR_E,   ENUM,
	"плав",         AR_TY,  FLOAT,
	"повтор",       AR_RW,  DO,
	"пока",         AR_RW,  WHILE,
	"продолж",      AR_RW,  CONTINUE,
	"прочие",       AR_RW,  DEFAULT,
	"пуст",         AR_TY,  UNDEF,
	"размер",       AR_RW,  SIZEOF,
	"разрыв",       AR_RW,  BREAK,
	"регистр",      AR_CL,  REGISTER,
	"симв",         AR_TY,  CHAR,
	"смесь",        AR_U,   0,
	"стат",         AR_CL,  STATIC,
	"тип",          AR_CL,  TYPEDEF,
	"фортран",      AR_CL,  FORTRAN,
	"цел",          AR_TY,  INT,

	"",             0,      0,     /* to stop the search */
	};

lxres() {
	/* check to see of yytext is reserved; if so,
	/* do the appropriate action and return */
	/* otherwise, return -1 */

	register c, ch;
	register struct lxrdope *p;

	ch = yytext[0];

	switch( ch ){

	case 'a':
		c=0; break;
	case 'b':
		c=2; break;
	case 'c':
		c=3; break;
	case 'd':
		c=6; break;
	case 'e':
		c=9; break;
	case 'f':
		c=12; break;
	case 'g':
		c=15; break;
	case 'i':
		c=16; break;
	case 'l':
		c=18; break;
	case 'r':
		c=19; break;
	case 's':
		c=21; break;
	case 't':
		c=26; break;
	case 'u':
		c=27; break;
	case 'v':
		c=29; break;
	case 'w':
		c=30; break;
	case (char) 'а':
		c=31; break;
	case (char) 'б':
		c=33; break;
	case (char) 'в':
		c=34; break;
	case (char) 'д':
		c=38; break;
	case (char) 'е':
		c=41; break;
	case (char) 'з':
		c=42; break;
	case (char) 'и':
		c=43; break;
	case (char) 'к':
		c=44; break;
	case (char) 'н':
		c=45; break;
	case (char) 'п':
		c=46; break;
	case (char) 'р':
		c=53; break;
	case (char) 'с':
		c=56; break;
	case (char) 'т':
		c=59; break;
	case (char) 'ф':
		c=60; break;
	case (char) 'ц':
		c=61; break;
	default:
		return( -1 );
		}

	for( p= lxrdope+c; p->lxrch[0] == ch; ++p ){
		if( !strcmp( yytext, p->lxrch ) ){ /* match */
			switch( p->lxract ){

			case AR_TY:
				/* type word */
				stwart = instruct;
				yylval.nodep = mkty( (TWORD)p->lxrval, 0, p->lxrval );
				return( TYPE );

			case AR_RW:
				/* ordinary reserved word */
				return( yylval.intval = p->lxrval );

			case AR_CL:
				/* class word */
				yylval.intval = p->lxrval;
				return( CLASS );

			case AR_S:
				/* struct */
				stwart = INSTRUCT|SEENAME|TAGNAME;
				yylval.intval = INSTRUCT;
				return( STRUCT );

			case AR_U:
				/* union */
				stwart = INUNION|SEENAME|TAGNAME;
				yylval.intval = INUNION;
				return( STRUCT );

			case AR_E:
				/* enums */
				stwart = SEENAME|TAGNAME;
				return( yylval.intval = ENUM );

			case AR_A:
				/* asm */
				asm_esc = 1; /* warn the world! */
				asmp = asmbuf;
				lxget( ' ', LEXWS );
				if( getchar() != '(' ) goto badasm;
				lxget( ' ', LEXWS );
				if( getchar() != '"' ) goto badasm;
				while( (c=getchar()) != '"' ){
					if( c=='\n' || c==EOF ) goto badasm;
					*asmp++ = c;
					if( asmp >= &asmbuf[ASMBUF-1] ) {
						uerror( "asm > %d chars",
							ASMBUF);
						}
					}
				lxget( ' ', LEXWS );
				if( getchar() != ')' ) goto badasm;
				*asmp++ = '\0';
				return( ASM );

			badasm:
				/* "bad asm construction" */
				UERROR( MESSAGE( 16 ) );
				return( 0 );

			default:
				cerror( "bad AR_?? action" );
				}
			}
		}
	return( -1 );
	}

lxtitle(){
	/* called after a newline; set linenumber and file name */

	register c, val;
	register char *cp;

	for(;;){  /* might be several such lines in a row */
		if( (c=getchar()) != '#' ){
			if( c != EOF ) ungetc(c,stdin);
			return;
			}

		lxget( ' ', LEXWS );
		val = 0;
		for( c=getchar(); c>='0' && c<='9'; c=getchar() ){
			val = val*10+ c - '0';
			}
		ungetc( c, stdin );
		lineno = val;
		lxget( ' ', LEXWS );
		if( (c=getchar()) != '\n' ){
			for( cp=ftitle; c!='\n'; c=getchar(),++cp ){
				*cp = c;
				}
			*cp = '\0';
#ifdef CXREF
			fprintf(outfp, "%s\n", ftitle);
#endif
			}
		}
	}

# ifndef MYASMOUT

asmout()
	/* write out asm string
	 * this is a null function for lint
	 */
{
# ifndef LINT
# ifndef ONEPASS
	putchar(')');
# endif
	printf( "%s\n", asmbuf );
# endif
}

# endif

#ifdef FLEXNAMES
#define NSAVETAB	4096
char	*savetab;
int	saveleft;

char *
savestr( cp )			/* place string into permanent string storage */
	register char *cp;
{
	register int len = strlen( cp ) + 1;

	if ( len > saveleft )
	{
		saveleft = NSAVETAB;
		if ( len > saveleft )
			saveleft = len;
		if ( ( savetab = (char *) malloc( (unsigned) saveleft ) ) == 0 )
			cerror( "out of memory [savestr()]" );
	}
	strncpy( savetab, cp, len );
	cp = savetab;
	savetab += len;
	saveleft -= len;
	return ( cp );
}

#ifdef LINT
#	define LNCHNAM	8	/* length of symbols to check for pflag */
#endif

/*
* The segmented hash tables.
*/
#define MAXHASH		20
#define HASHINC		1013
struct ht
{
	char	**ht_low;
	char	**ht_high;
	int	ht_used;
} htab[MAXHASH];


char *
hash( s )	/* look for s in seg. hash tables.  Not found, make new entry */
	char *s;
{
	register char **h;
	register int i;
	register char *cp;
	struct ht *htp;
	int sh;
#ifdef LINT
	char *found = 0;	/* set once LNCHNAM chars. matched for name */
#endif

	/*
	* Hash on the correct number of characters.  Lint needs to be able
	* to limit this so that it can note length of names for portablility
	* concerns.
	*/
	cp = s;
	i = 0;
#ifdef LINT
	while ( *cp && ( ( cp - s ) < LNCHNAM ) )
#else
	while ( *cp )
#endif
	{
		i = ( i << 1 ) + *cp++;
	}
	sh = ( i & 077777 ) % HASHINC;
	cp = s;
	/*
	* Look through each table for name.  If not found in the current
	* table, skip to the next one.
	*/
	for ( htp = htab; htp < &htab[MAXHASH]; htp++ )
	{
		if ( htp->ht_low == 0 )
		{
			register char **hp = (char **) calloc(
				sizeof (char **), HASHINC );

			if ( hp == 0 )
				cerror( "out of memory [hash()]" );
			htp->ht_low = hp;
			htp->ht_high = hp + HASHINC;
		}
		h = htp->ht_low + sh;
		/*
		* Use quadratic re-hash
		*/
		i = 1;
		do
		{
			if ( *h == 0 )
			{
				if ( htp->ht_used > ( HASHINC * 3 ) / 4 )
					break;
				htp->ht_used++;
				*h = savestr( cp );
#ifdef LINT
				if ( pflag && found )
				{
					/*
					* If pflag set, then warn of greater
					* than LNCHNAM character names which
					* differ past the LNCHNAM'th character.
					*/
					/*
					* "`%s' may be indistinguishable from
					*  `%s' due to internal name truncation
					*/
					WERROR( MESSAGE( 128 ), *h, found );
				}
#endif
				return ( *h );
			}
#ifdef LINT
			if ( pflag )
			{
				if ( **h == *cp &&
					strncmp( *h, cp, LNCHNAM ) == 0 )
				{
					/*
					* We have matched on LNCHNAM chars.
					* Now, look for the ``total'' name.
					*/
					found = *h;
					if ( strcmp( *h, cp ) == 0 )
					{
						/*
						* This entry really is
						* the name we want.
						*/
						return ( *h );
					}
				}
			}
			else	/* No pflag - use entire name length */
			{
				if ( **h == *cp && strcmp( *h, cp ) == 0 )
					return ( *h );
			}
#else
			if ( **h == *cp && strcmp( *h, cp ) == 0 )
				return ( *h );
#endif
			h += i;
			i += 2;
			if ( h >= htp->ht_high )
				h -= HASHINC;
		} while ( i < HASHINC );
	}
	cerror( "out of hash tables" );
}
#endif
