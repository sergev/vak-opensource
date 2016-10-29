/*	@(#)cpp.c	1.11	*/
char	release[] = "@(#)C rel 6.0";

# ifdef FLEXNAMES
#	define NCPS	128
	int ncps = 128;		/* default name length */
# else
#	define NCPS	8
	int ncps = 8;		/* default name length */
# endif

# include "stdio.h"
# include "ctype.h"

/* C command - C preprocessor */

# define STATIC

# define STDIN 0
# define STDOUT 1
# define STDERR 2

# define CLASSCODE      27      /* exit status if #class seen */

# define READ   "r"
# define WRITE  "w"
# define SALT   '#'

# ifndef BUFSIZ
#	define BUFSIZ 512
# endif

char *pbeg, *pbuf, *pend;
char *outp, *inp;
char *newp;
char cinit;

# ifdef CXREF
char *xcopy();
FILE *outfp;
static int ready = 0;
int xline;
# endif

/* some code depends on whether characters are sign or zero extended */
/*	#if '\377' < 0		not used here, old cpp doesn't understand */
# if pdp11 | vax | M_XENIX
#	define COFF 128
# else
#	define COFF 0
# endif

# if gcos
#	define ALFSIZ 512	/* alphabet size */
# else
#	define ALFSIZ 256	/* alphabet size */
# endif

char macbit[ ALFSIZ + 11 ];
char toktyp[ ALFSIZ ];

# define BLANK 1
# define IDENT 2
# define NUMBR 3

/* a superimposed code is used to reduce the number of calls to the
/* symbol table lookup routine.  (if the kth character of an identifier
/* is 'a' and there are no macro names whose kth character is 'a'
/* then the identifier cannot be a macro name, hence there is no need
/* to look in the symbol table.)  'scw1' enables the test based on
/* single characters and their position in the identifier.  'scw2'
/* enables the test based on adjacent pairs of characters and their
/* position in the identifier.  scw1 typically costs 1 indexed fetch,
/* an AND, and a jump per character of identifier, until the identifier
/* is known as a non-macro name or until the end of the identifier.
/* scw1 is inexpensive.  scw2 typically costs 4 indexed fetches,
/* an add, an AND, and a jump per character of identifier, but it is also
/* slightly more effective at reducing symbol table searches.
/* scw2 usually costs too much because the symbol table search is
/* usually short; but if symbol table search should become expensive,
/* the code is here.
/* using both scw1 and scw2 is of dubious value.
*/
# define scw1 1
# define scw2 0

# if scw2
char t21[ ALFSIZ ], t22[ ALFSIZ ], t23[ ALFSIZ + NCPS ];
# endif

# if scw1
#	define b0 1
#	define b1 2
#	define b2 4
#	define b3 8
#	define b4 16
#	define b5 32
#	define b6 64
#	define b7 128
# endif

# define IB 1
# define SB 2
# define NB 4
# define CB 8
# define QB 16
# define WB 32

char fastab[ ALFSIZ ];
char slotab[ ALFSIZ ];
char *ptrtab;

# define isslo          ( ptrtab == ( slotab + COFF ) )
# define isid(a)        ( ( fastab + COFF )[a] & IB )
# define isspc(a)       ( ptrtab[a] & SB )
# define isnum(a)       ( ( fastab + COFF )[a] & NB )
# define iscom(a)       ( ( fastab + COFF )[a] & CB )
# define isquo(a)       ( ( fastab + COFF )[a] & QB )
# define iswarn(a)      ( ( fastab + COFF )[a] & WB )

# define eob(a)         ( ( a ) >= pend )
# define bob(a)         ( pbeg >= ( a ) )

char buffer[ NCPS + BUFSIZ + BUFSIZ + NCPS ];

# ifdef FLEXNAMES
#	define SBSIZE 36000
# else
#	define SBSIZE 24000
# endif

char	sbf[ SBSIZE ];
char	*savch	= sbf;

# define DROP ((char) '\277')   /* special character not legal ASCII (no EBCDIC !) */
# define WARN DROP
# define SAME 0
# define MAXINC 12
# define MAXFRE 14	/* max buffers of macro pushback */
# define MAXFRM 31	/* max number of formals/actuals to a macro */

# define START  1
# define CONT   2
# define BACK   3

# define NPREDEF 20

static char warnc = WARN;

int mactop, fretop;
char *instack[ MAXFRE ], *bufstack[ MAXFRE ], *endbuf[ MAXFRE ];

int plvl;	/* parenthesis level during scan for macro actuals */
int maclin;	/* line number of macro call requiring actuals */
char *macfil;	/* file name of macro call requiring actuals */
char *macnam;	/* name of macro requiring actuals */
int maclvl;	/* # calls since last decrease in nesting level */
char *macforw;	/* pointer which must be exceeded to decrease nesting level */
int macdam;	/* offset to macforw due to buffer shifting */

# if tgp
	int tgpscan;	/* flag for dump(); */
# endif

STATIC	int	inctop[ MAXINC ];
STATIC	char	*fnames[ MAXINC ];
STATIC	char	*dirnams[ MAXINC ];	/* actual directory of #include files */
STATIC	FILE	*fins[ MAXINC ];
STATIC	int	lineno[ MAXINC ];

STATIC	char	*dirs[ MAXINC ];	/* -I and <> directories */
char *copy(), *subst(), *trmdir(), *strchr(), *strrchr();
struct symtab *stsym();

STATIC	FILE	*fin	= stdin;
STATIC	FILE	*fout	= stdout;
STATIC  FILE    *mout;  /* file to place dependencies on */
STATIC	int	nd	= 1;
STATIC	int	pflag;	/* don't put out lines "# 12 foo.c" */
STATIC	int	passcom;	/* don't delete comments */
STATIC	int	incomm;	/* set while in comment so that EOF can be detected */
STATIC	int	rflag;	/* allow macro recursion */
STATIC  int     mflag;  /* generate makefile dependencies */
STATIC  char    *infile; /* name of .o file to build dependencies from */
STATIC	int	ifno;
STATIC  char    *prespc[ NPREDEF ];
STATIC  char    **predef = prespc;
STATIC  char    *punspc[ NPREDEF ];
STATIC  char    **prund = punspc;
STATIC	int	exfail;

struct symtab
{
	char	*name;
	char	*value;
} *lastsym, *lookup(), *slookup();

# if gcos
#	include <setjmp.h>
	static jmp_buf env;
#	define main	mainpp
#	undef exit
#	define exit(S)	longjmp(env, 1)
#	define symsiz 500
#	define LINEFORM	"# %d %s\n"
#	define ERRFORM	"*%c*   %s, line "
# else
#	define symsiz 2000
#	define LINEFORM	"# %d \"%s\"\n"
#	define ERRFORM	"*%c*   \"%s\", line "
# endif

STATIC	struct symtab stab[ symsiz ];

STATIC  struct symtab   *defloc, *rdefloc;
STATIC  struct symtab   *udfloc, *rudfloc;
STATIC  struct symtab   *incloc, *rincloc;
STATIC  struct symtab   *ifloc, *rifloc;
STATIC  struct symtab   *elsloc, *relsloc;
STATIC  struct symtab   *eifloc, *reifloc;
STATIC  struct symtab   *ifdloc, *rifdloc;
STATIC  struct symtab   *ifnloc, *rifnloc;
STATIC  struct symtab   *lneloc, *rlneloc;
STATIC  struct symtab   *ulnloc, *rulnloc;
STATIC  struct symtab   *uflloc, *ruflloc;
STATIC  struct symtab   *clsloc, *rclsloc;
STATIC  struct symtab   *idtloc, *ridtloc;
STATIC  struct symtab   *ysysloc;
STATIC  struct symtab   *varloc;

STATIC	int	trulvl;
STATIC	int	flslvl;

# define MSG(l,r) (msg ? (r) : (l))

char msg;

initmsg ()
{
	register char *p;
	extern char *getenv ();

	msg = (p = getenv ("MSG")) && *p == 'r';
}

sayline(where)
	int where;
{
	if ( mflag && where==START )
		fprintf( mout, "%s: %s\n", infile, fnames[ifno] );
	if ( pflag == 0 )
		fprintf( fout, LINEFORM, lineno[ifno], fnames[ifno] );
}

/* data structure guide
/*
/* most of the scanning takes place in the buffer:
/*
/*  (low address)                                             (high address)
/*  pbeg                           pbuf                                 pend
/*  |      <-- BUFSIZ chars -->      |         <-- BUFSIZ chars -->        |
/*  _______________________________________________________________________
/* |_______________________________________________________________________|
/*          |               |               |
/*          |<-- waiting -->|               |<-- waiting -->
/*          |    to be      |<-- current -->|    to be
/*          |    written    |    token      |    scanned
/*          |               |               |
/*          outp            inp             p
/*
/*  *outp   first char not yet written to output file
/*  *inp    first char of current token
/*  *p      first char not yet scanned
/*
/* macro expansion: write from *outp to *inp (chars waiting to be written),
/* ignore from *inp to *p (chars of the macro call), place generated
/* characters in front of *p (in reverse order), update pointers,
/* resume scanning.
/*
/* symbol table pointers point to just beyond the end of macro definitions;
/* the first preceding character is the number of formal parameters.
/* the appearance of a formal in the body of a definition is marked by
/* 2 chars: the char WARN, and a char containing the parameter number.
/* the first char of a definition is preceded by a zero character.
/*
/* when macro expansion attempts to back up over the beginning of the
/* buffer, some characters preceding *pend are saved in a side buffer,
/* the address of the side buffer is put on 'instack', and the rest
/* of the main buffer is moved to the right.  the end of the saved buffer
/* is kept in 'endbuf' since there may be nulls in the saved buffer.
/*
/* similar action is taken when an 'include' statement is processed,
/* except that the main buffer must be completely emptied.  the array
/* element 'inctop[ifno]' records the last side buffer saved when
/* file 'ifno' was included.  these buffers remain dormant while
/* the file is being read, and are reactivated at end-of-file.
/*
/* instack[0 : mactop] holds the addresses of all pending side buffers.
/* instack[inctop[ifno]+1 : mactop-1] holds the addresses of the side
/* buffers which are "live"; the side buffers instack[0 : inctop[ifno]]
/* are dormant, waiting for end-of-file on the current file.
/*
/* space for side buffers is obtained from 'savch' and is never returned.
/* bufstack[0:fretop-1] holds addresses of side buffers which
/* are available for use.
*/

dump()
{
	/* write part of buffer which lies between  outp  and  inp .
	/* this should be a direct call to 'write', but the system slows
	/* to a crawl if it has to do an unaligned copy.  thus we buffer.
	/*? this silly loop is 15% of the total time, thus even the 'putc'
	/*? macro is too slow.
	*/
	register char *p1;
	register FILE *f;
# if tgp
	register char *p2;
# endif

	if ( ( p1 = outp ) == inp || flslvl != 0 )
		return;
# if tgp
#	define MAXOUT 80
	if ( !tgpscan )		/* scan again to insure <= MAXOUT
	{			/* chars between linefeeds */
		register char c, *pblank;
		char savc, stopc, brk;

		tgpscan = 1;
		brk = stopc = pblank = 0;
		p2 = inp;
		savc = *p2;
		*p2 = '\0';
		while ( c = *p1++ )
		{
			if ( c == '\\' )
				c = *p1++;
			if ( stopc == c )
				stopc = 0;
			else if ( c == '"' || c == '\'' )
				stopc = c;
			if ( p1 - outp > MAXOUT && pblank != 0 )
			{
				*pblank++ = '\n';
				inp = pblank;
				dump();
				brk = 1;
				pblank = 0;
			}
			if ( c == ' ' && stopc == 0 )
				pblank = p1 - 1;
		}
		if ( brk )
			sayline(CONT);
		*p2 = savc;
		inp = p2;
		p1 = outp;
		tgpscan = 0;
	}
# endif
	f = fout;
# if gcos
	/* filter out "$ program c" card if first line of input */
	/* gmatch is a simple pattern matcher in the GCOS Standard Library */
	{
		static int gmfirst = 0;

		if ( !gmfirst )
		{
			++gmfirst;
			if ( gmatch( p1, "^$*program[ \t]*c*") )
				p1 = strchr( p1, '\n' );
		}
	}
# endif
	if ( p1 < inp )
		p1 += fwrite( p1, sizeof( char ), inp - p1, f );
	outp = p1;
}

char *
refill( p )
	register char *p;
{
	/* dump buffer.  save chars from inp to p.  read into buffer at pbuf,
	/* contiguous with p.  update pointers, return new p.
	*/
	register char *np, *op;
	register int ninbuf;

	dump();
	np = pbuf - ( p - inp );
	op = inp;
	if ( bob( np + 1 ) )
	{
		pperror( MSG ("token too long", "слишком длинная лексема") );
		np = pbeg;
		p = inp + BUFSIZ;
	}
	macdam += np - inp;
	outp = inp = np;
	while ( op < p )
		*np++ = *op++;
	p = np;
	for ( ;; )
	{
			/* retrieve hunk of pushed-back macro text */
		if ( mactop > inctop[ifno] )
		{
			op = instack[ --mactop ];
			np = pbuf;
			do
			{
				while ( *np++ = *op++ )
					;
			}
			while ( op < endbuf[mactop] );
			pend = np - 1;
			/* make buffer space avail for 'include' processing */
			if ( fretop < MAXFRE )
				bufstack[fretop++] = instack[mactop];
			return( p );
		}
		else 	/* get more text from file(s) */
		{
			maclvl = 0;
			if ( 0 < ( ninbuf = fread( pbuf, sizeof( char ), BUFSIZ, fin ) ) )
			{
				pend = pbuf + ninbuf;
				*pend = '\0';
				return( p );
			}
			/* end of #include file */
			if ( ifno == 0 )	/* end of input */
			{
				if ( plvl != 0 )
				{
					int n = plvl, tlin = lineno[ifno];
					char *tfil = fnames[ifno];

					lineno[ifno] = maclin;
					fnames[ifno] = macfil;
					pperror( MSG ("%s: unterminated macro call",
						"%s: незакрытый вызов макро"),
						macnam );
					lineno[ifno] = tlin;
					fnames[ifno] = tfil;
					np = p;
					/*shut off unterminated quoted string*/
					*np++ = '\n';
						/* supply missing parens */
					while ( --n >= 0 )
						*np++ = ')';
					pend = np;
					*np = '\0';
					if ( plvl < 0 )
						plvl = 0;
					return( p );
				}
				if ( incomm )
					pperror( MSG ("Unexpected EOF in comment",
						"преждевременный конец файла в комментарии") );
				inp = p;
				dump();
				if ( fout && ferror( fout ) )
					pperror( MSG ("Problems with writing output file; probably out of temp space",
						"ошибка при записи в выходной файл") );
				exit( exfail ? ( exfail ==
					CLASSCODE ? CLASSCODE : 2 ) : 0 );
			}
			fclose( fin );
			fin = fins[--ifno];
			dirs[0] = dirnams[ifno];
			sayline(BACK);
# ifdef CXREF
 			fprintf(outfp, "\"%s\"\n", fnames[ifno]);
# endif
		}
	}
}

# define BEG 0
# define LF 1

char *
cotoken( p )
	register char *p;
{
	register int c, i;
	char quoc;
	static int state = BEG;
	static int speakup = 0;

	if ( state != BEG )
		goto prevlf;
	for ( ;; )
	{
	again:
		while ( !isspc( *p++ ) )
			;
		switch ( *( inp = p - 1 ) )
		{
		case 0:
			if ( eob( --p ) )
			{
				p = refill( p );
				goto again;
			}
			else
				++p;	/* ignore null byte */
			break;
		case '|':
		case '&':
			for ( ;; )	/* sloscan only */
			{
				if ( *p++ == *inp )
					break;
				if ( eob( --p ) )
					p = refill( p );
				else
					break;
			}
			break;
		case '=':
		case '!':
			for ( ;; )	/* sloscan only */
			{
				if ( *p++ == '=' )
					break;
				if ( eob( --p ) )
					p = refill( p );
				else
					break;
			}
			break;
		case '<':
		case '>':
			for ( ;; )	/* sloscan only */
			{
				if ( *p++ == '=' || p[-2] == p[-1] )
					break;
				if ( eob( --p ) )
					p = refill( p );
				else
					break;
			}
			break;
		case '\\':
			for ( ;; )
			{
				if ( *p++ == '\n' )
				{
					++lineno[ifno];
					break;
				}
				if ( eob( --p ) )
					p = refill( p );
				else
				{
					++p;
					break;
				}
			}
			break;
		case '/':
			for ( ;; )
			{
				if ( *p++ == '*' )	/* comment */
				{
					incomm = 1;
					if ( !passcom )
					{
						inp = p - 2;
						dump();
						++flslvl;
					}
					for ( ;; )
					{
						while ( !iscom( *p++ ) )
							;
						if ( p[-1] == '*' )
							for ( ;; )
							{
								if ( *p++ == '/' )
									goto endcom;
								if ( eob( --p ) )
								{
									if ( !passcom )
									{
										inp = p;
										p = refill( p );
									}
									/* split long comment */
									else if ( ( p - inp ) >= BUFSIZ )
									{
										*p++ = '*';
										*p++ = '/';
										inp = p;
										p = refill( p );
										outp = inp = p -= 2;
										*p++ = '/';
										*p++ = '*';
									}
									else
										p = refill( p );
								}
								else
									break;
							}
						else if ( p[-1] == '\n' )
						{
							++lineno[ifno];
							if ( !passcom && flslvl <= 1 )
								putc( '\n', fout );
						}
						else if ( eob( --p ) )
						{
							if ( !passcom )
							{
								inp = p;
								p = refill( p );
							}
							/* split long comment */
							else if ( ( p - inp ) >= BUFSIZ )
							{
								*p++ = '*';
								*p++ = '/';
								inp = p;
								p = refill( p );
								outp = inp = p -= 2;
								*p++ = '/';
								*p++ = '*';
							}
							else
								p = refill( p );
						}
						else
							++p;	/* ignore null byte */
					}
				endcom:
					incomm = 0;
					if ( !passcom )
					{
						outp = inp = p;
						--flslvl;
						goto again;
					}
					break;
				}
				if ( eob( --p ) )
					p = refill( p );
				else
					break;
			}
			break;
# if gcos
		case '`':
# endif
		case '"':
		case '\'':
			quoc = p[-1];
			for ( ;; )
			{
				while ( !isquo( *p++ ) )
					;
				if ( p[-1] == quoc )
					break;
				if ( p[-1] == '\n' )	/* bare \n terminates quotation */
				{
					--p;
					break;
				}
				if ( p[-1] == '\\' )
					for ( ;; )
					{
						if ( *p++ == '\n' )	/* escaped \n ignored */
						{
							++lineno[ifno];
							break;
						}
						if ( eob( --p ) )
							p = refill( p );
						else
						{
							++p;
							break;
						}
					}
				else if ( eob( --p ) )
					p = refill( p );
				else
					++p;	/* it was a different quote character */
			}
			break;
		case WARN:
			{
				int ii;

				dump();
				speakup = 0;
				for ( ii = sizeof(int) / sizeof(char); --ii >= 0; )
				{
					if ( eob( p ) )
						p = refill( p );
					speakup |= ( *p++ & 0xFF ) << ( ii * 8 );
				}
				inp = outp = p;
				break;
			}
		case '\n':
			++lineno[ifno];
			if ( isslo )
			{
				state=LF;
				return( p );
			}
		prevlf:
			if ( speakup )
			{
				inp = p;
				dump();
				lineno[ifno] = speakup + 1;
				sayline(CONT);
				speakup = 0;
			}
			state = BEG;
			for ( ;; )
			{
				/*
				* ignore formfeeds and vertical tabs
				* which may be just before the SALT
				*/
				if ( *p == '\f' || *p == '\v' )
				{
					register char *s = p;

					while ( *++s == '\f' || *s == '\v' )
						;
					if ( *s == SALT )
					{
						/*
						* get the SALT to the front!
						*/
						*s = *p;
						*p = SALT;
					}
				}
				if ( *p++ == SALT )
					return( p );
				if ( eob( inp = --p ) )
					p = refill( p );
				else
					goto again;
			}
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			for ( ;; )
			{
				while ( isnum( *p++ ) )
					;
				if ( eob( --p ) )
					p = refill( p );
				else
					break;
			}
			break;
		case 'A': case 'B': case 'C': case 'D': case 'E':
		case 'F': case 'G': case 'H': case 'I': case 'J':
		case 'K': case 'L': case 'M': case 'N': case 'O':
		case 'P': case 'Q': case 'R': case 'S': case 'T':
		case 'U': case 'V': case 'W': case 'X': case 'Y':
		case 'Z': case '_':
		case 'a': case 'b': case 'c': case 'd': case 'e':
		case 'f': case 'g': case 'h': case 'i': case 'j':
		case 'k': case 'l': case 'm': case 'n': case 'o':
		case 'p': case 'q': case 'r': case 's': case 't':
		case 'u': case 'v': case 'w': case 'x': case 'y':
		case 'z':
		case (char) 'ю': case (char) 'а': case (char) 'б':
		case (char) 'ц': case (char) 'д': case (char) 'е':
		case (char) 'ф': case (char) 'г': case (char) 'х':
		case (char) 'и': case (char) 'й': case (char) 'к':
		case (char) 'л': case (char) 'м': case (char) 'н':
		case (char) 'о': case (char) 'п': case (char) 'я':
		case (char) 'р': case (char) 'с': case (char) 'т':
		case (char) 'у': case (char) 'ж': case (char) 'в':
		case (char) 'ь': case (char) 'ы': case (char) 'з':
		case (char) 'ш': case (char) 'э': case (char) 'щ':
		case (char) 'ч': case (char) 'ъ': case (char) 'Ю':
		case (char) 'А': case (char) 'Б': case (char) 'Ц':
		case (char) 'Д': case (char) 'Е': case (char) 'Ф':
		case (char) 'Г': case (char) 'Х': case (char) 'И':
		case (char) 'Й': case (char) 'К': case (char) 'Л':
		case (char) 'М': case (char) 'Н': case (char) 'О':
		case (char) 'П': case (char) 'Я': case (char) 'Р':
		case (char) 'С': case (char) 'Т': case (char) 'У':
		case (char) 'Ж': case (char) 'В': case (char) 'Ь':
		case (char) 'Ы': case (char) 'З': case (char) 'Ш':
		case (char) 'Э': case (char) 'Щ': case (char) 'Ч':
# if scw1
#	define tmac1( c, bit )		if ( !xmac1( c, bit, & ) ) \
						goto nomac
#	define xmac1( c, bit, op )	( ( macbit + COFF )[c] op ( bit ) )
# else
#	define tmac1( c, bit )
#	define xmac1( c, bit, op )
# endif

# if scw2
#	define tmac2( c0, c1, cpos )	if ( !xmac2( c0, c1, cpos, & ) ) \
						goto nomac
#	define xmac2( c0, c1, cpos, op ) \
		( ( macbit + COFF )[ ( t21 + COFF )[c0] + \
		( t22 + COFF )[c1]] op ( t23 + COFF + cpos )[c0] )
# else
#	define tmac2( c0, c1, cpos )
#	define xmac2( c0, c1, cpos, op )
# endif

			if ( flslvl )
				goto nomac;
			for ( ;; )
			{
				c = p[-1];
				tmac1( c, b0 );
				i = *p++;
				if ( !isid( i ) )
					goto endid;
				tmac1( i, b1 );
				tmac2( c, i, 0 );
				c = *p++;
				if ( !isid( c ) )
					goto endid;
				tmac1( c, b2 );
				tmac2( i, c, 1 );
				i = *p++;
				if ( !isid( i ) )
					goto endid;
				tmac1( i, b3 );
				tmac2( c, i, 2 );
				c = *p++;
				if ( !isid( c ) )
					goto endid;
				tmac1( c, b4 );
				tmac2( i, c, 3 );
				i = *p++;
				if ( !isid( i ) )
					goto endid;
				tmac1( i, b5 );
				tmac2( c, i, 4 );
				c = *p++;
				if ( !isid( c ) )
					goto endid;
				tmac1( c, b6 );
				tmac2( i, c, 5 );
				i = *p++;
				if ( !isid( i ) )
					goto endid;
				tmac1( i, b7 );
				tmac2( c, i, 6 );
				tmac2( i, 0, 7 );
				while ( isid( *p++ ) )
					;
				if ( eob( --p ) )
				{
					refill( p );
					p = inp + 1;
					continue;
				}
				goto lokid;
			endid:
				if ( eob( --p ) )
				{
					refill( p );
					p = inp + 1;
					continue;
				}
				tmac2( p[-1], 0, -1 + ( p - inp ) );
			lokid:
				slookup( inp, p, 0 );
				if ( newp )
				{
					p = newp;
					goto again;
				}
				else
					break;
			nomac:
				while ( isid( *p++ ) )
					;
				if ( eob( --p ) )
				{
					p = refill( p );
					goto nomac;
				}
				else
					break;
			}
			break;
		} /* end of switch */
	
		if ( isslo )
			return( p );
	} /* end of infinite loop */
}

char *
skipbl( p )		/* get next non-blank token */
	register char *p;
{
	do
	{
		outp = inp = p;
		p = cotoken( p );
	}
	while ( ( toktyp + COFF )[*inp] == BLANK );
	return( p );
}

char *
unfill( p )
	register char *p;
{
	/* take <= BUFSIZ chars from right end of buffer and put
	/* them on instack.  slide rest of buffer to the right,
	/* update pointers, return new p.
	*/
	register char *np, *op;
	register int d;

	if ( mactop >= MAXFRE )
	{
		pperror( MSG ("%s: too much pushback",
			"%s: слишком много возвратов"), macnam );
		p = inp = pend;		 /* begin flushing pushback */
		dump();
		while ( mactop > inctop[ifno] )
		{
			p = refill( p );
			p = inp = pend;
			dump();
		}
	}
	if ( fretop > 0 )
		np = bufstack[--fretop];
	else
	{
		np = savch;
		savch += BUFSIZ;
		if ( savch >= sbf + SBSIZE )
		{
			pperror( MSG ("no space", "нет места") );
			exit( exfail ? ( exfail == CLASSCODE ? CLASSCODE : 2 )
				: 0 );
		}
		*savch++ = '\0';
	}
	instack[mactop] = np;
	op = pend - BUFSIZ;
	if ( op < p )
		op = p;
	for ( ;; )		/* out with old */
	{
		while ( *np++ = *op++ )
			;
		if ( eob( op ) )
			break;
	}
	endbuf[mactop++] = np;	/* mark end of saved text */
	np = pbuf + BUFSIZ;
	op = pend - BUFSIZ;
	pend = np;
	if ( op < p )
		op = p;
	while ( outp < op )		/* slide over new */
		*--np = *--op;
	if ( bob( np ) )
		pperror( MSG ("token too long", "слишком длинная лексема") );
	d = np - outp;
	outp += d;
	inp += d;
	macdam += d;
	return( p + d );
}

char *
doincl( p )
	register char *p;
{
	int filok, inctype;
	register char *cp;
	char **dirp, *nfil;
	char filname[BUFSIZ];

	p = skipbl( p );
	cp = filname;
	if ( *inp++ == '<' )	/* special <> syntax */
	{
		inctype = 1;
		++flslvl;	/* prevent macro expansion */
		for ( ;; )
		{
			outp = inp = p;
			p = cotoken( p );
			if ( *inp == '\n' )
			{
				--p;
				*cp = '\0';
				break;
			}
			if ( *inp == '>' )
			{
				*cp = '\0';
				break;
			}
# ifdef gimpel
			if ( *inp == '.' && !intss() )
				*inp = '#';
# endif
			while ( inp < p )
				*cp++ = *inp++;
		}
		--flslvl;	/* reenable macro expansion */
	}
	else if ( inp[-1] == '"' )	/* regular "" syntax */
	{
		inctype = 0;
# ifdef gimpel
		while ( inp < p )
		{
			if ( *inp == '.' && !intss() )
				*inp = '#';
				*cp++ = *inp++;
		}
# else
		while ( inp < p )
			*cp++ = *inp++;
# endif
		if ( *--cp == '"' )
			*cp = '\0';
	}
	else
	{
		pperror( MSG ("bad include syntax",
			"плохой синтаксис #include"), 0 );
		inctype = 2;
	}
	/* flush current file to \n , then write \n */
	++flslvl;
	do
	{
		outp = inp = p;
		p = cotoken( p );
	}
	while ( *inp != '\n' );
	--flslvl;
	inp = p;
	dump();
	if ( inctype == 2 )
		return( p );
	/* look for included file */
	if ( ifno + 1 >= MAXINC )
	{
		pperror( MSG ("Unreasonable include nesting",
			"слишком большая вложенность #include"), 0 );
		return( p );
	}
	if ( ( nfil = savch ) > sbf + SBSIZE - BUFSIZ )
	{
		pperror( MSG ("no space", "нет места") );
		exit( exfail ? ( exfail == CLASSCODE ? CLASSCODE : 2 ) : 0 );
	}
	filok = 0;
	for ( dirp = dirs + inctype; *dirp; ++dirp )
	{
		if (
# if gcos
			strchr( filname, '/' )
# else
			filname[0] == '/' 
# endif
				|| **dirp == '\0' )
		{
			strcpy( nfil, filname );
		}
		else
		{
			strcpy( nfil, *dirp );
# if unix || gcos
			strcat( nfil, "/" );
# endif
# ifdef ibm
#	ifndef gimpel
			strcat( nfil, "." );
#	endif
# endif
			strcat( nfil, filname );
		}
		if ( fins[ifno + 1] = fopen( nfil, READ ) )
		{
			filok = 1;
			fin = fins[++ifno];
			break;
		}
	}
	if ( filok == 0 )
		pperror( MSG ("Can't find include file %s",
			"не могу найти файл-заголовок %s"), filname );
	else
	{
		lineno[ifno] = 1;
		fnames[ifno] = cp = nfil;
		while ( *cp++)
			;
		savch = cp;
		dirnams[ifno] = dirs[0] = trmdir( copy( nfil ) );
		sayline(START);
# ifdef CXREF
 		fprintf(outfp, "\"%s\"\n", fnames[ifno]);
# endif
		/* save current contents of buffer */
		while ( !eob( p ) )
			p = unfill( p );
		inctop[ifno] = mactop;
	}
	return( p );
}

equfrm( a, p1, p2 )
	register char *a, *p1, *p2;
{
	register char c;
	int flag;

	c = *p2;
	*p2 = '\0';
	flag = strcmp( a, p1 );
	*p2 = c;
	return( flag == SAME );
}

char *
dodef( p )		/* process '#define' */
	char *p;
{
	register char *pin, *psav, *cf;
	char **pf, **qf;
	int b, c, params;
	int ex_blank;	/* used to ignore extra blanks in token-string */
	int sav_passcom = passcom;	/* saved passcom, used to reset it */
	struct symtab *np;
	char *oldval, *oldsavch;
	char *formal[MAXFRM]; /* formal[n] is name of nth formal */
	char formtxt[BUFSIZ]; /* space for formal names */

	if ( savch > sbf + SBSIZE - BUFSIZ )
	{
		pperror( MSG ("too much defining",
			"слишком много макроопределений") );
		return( p );
	}
	oldsavch = savch; /* to reclaim space if redefinition */
	++flslvl;	/* prevent macro expansion during 'define' */
	p = skipbl( p );
	pin = inp;
	if ( ( toktyp + COFF )[*pin] != IDENT )
	{
		ppwarn( MSG ("illegal macro name", "плохое имя макро") );
		while ( *inp != '\n' )
			p = skipbl( p );
		return( p );
	}
	np = slookup( pin, p, 1 );
	if ( oldval = np->value )	/* was previously defined */
		savch = oldsavch;
# ifdef CXREF
 	def(np->name, lineno[ifno]);
# endif
	b = 1;
	cf = pin;
	while ( cf < p )		/* update macbit */
	{
		c = *cf++;
		xmac1( c, b, |= );
		b = ( b + b ) & 0xFF;
		if ( cf != p )
			xmac2( c, *cf, -1 + ( cf - pin ), |= );
		else
			xmac2( c, 0, -1 + ( cf - pin ), |= );
	}
	params = 0;
	outp = inp = p;
	p = cotoken( p );
	pin = inp;
	if ( *pin == '(' )	/* with parameters; identify the formals */
	{
# ifdef CXREF
 		newf(np->name, lineno[ifno]);
# endif
		cf = formtxt;
		pf = formal;
		for ( ;; )
		{
			p = skipbl( p );
			pin = inp;
			if ( *pin == '\n' )
			{
				--lineno[ifno];
				--p;
				pperror( MSG ("%s: missing )",
					"%s: пропущена )"), np->name );
				break;
			}
			if ( *pin == ')' )
				break;
			if ( *pin == ',' )
				continue;
			if ( ( toktyp + COFF )[*pin] != IDENT )
			{
				c = *p;
				*p = '\0';
				pperror( MSG ("bad formal: %s",
					"плохой параметр: %s"), pin );
				*p = c;
			}
			else if ( pf >= &formal[MAXFRM] )
			{
				c = *p;
				*p = '\0';
				pperror( MSG ("too many formals: %s",
					"слишком много параметров: %s"),
					pin );
				*p = c;
			}
			else
			{
				*pf++ = cf;
				while ( pin < p )
					*cf++ = *pin++;
				*cf++ = '\0';
				++params;
# ifdef CXREF
 				def(*(pf-1), lineno[ifno]);
# endif
			}
		}
		if ( params == 0 )	/* #define foo() ... */
			--params;
	}
	else if ( *pin == '\n' )
	{
		--lineno[ifno];
		--p;
	}
	/*
	* remember beginning of macro body, so that we can
	* warn if a redefinition is different from old value.
	*/
	oldsavch = psav = savch;
	passcom = 1;	/* make cotoken() return comments as tokens */
	ex_blank = 1;	/* must have some delimiter - might as well be blank */
	for ( ;; )	/* accumulate definition until linefeed */
	{
		outp = inp = p;
		p = cotoken( p );
		pin = inp;
		if ( *pin == '\\' && pin[1] == '\n' )	/* ignore escaped lf */
		{
			if ( !ex_blank )	/* replace it with a blank */
			{
				*psav++ = ' ';
				ex_blank = 1;
			}
			putc( '\n', fout );
			continue;
		}
		if ( *pin == '\n' )
			break;
		if ( ( toktyp + COFF )[*pin] == BLANK )	/* skip extra blanks */
		{
			if ( ex_blank )
				continue;
			*pin = ' ';	/* force it to be a "real" blank */
			ex_blank = 1;
		}
		else
			ex_blank = 0;
		if ( *pin == '/' && pin[1] == '*' )	/* skip comment */
		{					/* except for \n's */
			while ( pin < p )
				if ( *pin++ == '\n' )
					putc( '\n', fout );
			continue;
		}
		if ( params )	/* mark the appearance of formals in the definiton */
		{
			if ( ( toktyp + COFF )[*pin] == IDENT )
			{
				for ( qf = pf; --qf >= formal; )
				{
					if ( equfrm( *qf, pin, p ) )
					{
# ifdef CXREF
# ifndef NO_MACRO_FORMAL
 						ref(*qf, lineno[ifno]);
# endif
# endif
						*psav++ = qf - formal + 1;
						*psav++ = WARN;
						pin = p;
						break;
					}
				}
			}
			/* inside quotation marks, too */
			else if ( *pin == '"' || *pin == '\''
# if gcos
					|| *pin == '`'
# endif
						)
			{
				char quoc = *pin;

				for ( *psav++ = *pin++; pin < p && *pin != quoc; )
				{
					while ( pin < p && !isid( *pin ) )
					{
						if ( *pin == '\n'
							&& pin[-1] == '\\' )
						{
							putc( '\n', fout );
							psav--;	/* no \ */
							pin++;	/* no \n */
						}
						else
							*psav++ = *pin++;
					}
					cf = pin;
					while ( cf < p && isid( *cf ) )
						++cf;
					for ( qf = pf; --qf >= formal; )
					{
						if ( equfrm( *qf, pin, cf ) )
						{
							*psav++ = qf - formal + 1;
							*psav++ = WARN;
							pin = cf;
							break;
						}
					}
					while ( pin < cf )
						*psav++ = *pin++;
				}
			}
		}
		while ( pin < p )
			if ( *pin == '\n' && pin[-1] == '\\' )
			{
				putc( '\n', fout );
				psav--;	/* no \ */
				pin++;	/* no \n */
			}
			else
				*psav++ = *pin++;
	}
	passcom = sav_passcom;	/* restore to "real" value */
	if ( psav[-1] == ' ' )	/* if token-string ended with a blank */
		psav--;		/* then it is unnecessary - throw away */
	*psav++ = params;
	*psav++ = '\0';
	if ( cf = oldval )      /* redefinition */
	{
		--cf;	/* skip no. of params, which may be zero */
		while ( *--cf )		/* go back to the beginning */
			;
		if ( 0 != strcmp( ++cf, oldsavch ) )	/* redefinition different from old */
		{
			--lineno[ifno];
			ppwarn( MSG ("%s redefined", "%s переопределено"),
				np->name );
			++lineno[ifno];
			np->value = psav - 1;
		}
		else
			psav = oldsavch; /* identical redef.; reclaim space */
	}
	else
		np->value = psav - 1;
	--flslvl;
	inp = pin;
	savch = psav;
	return( p );
}

# define fasscan()      ptrtab = fastab + COFF
# define sloscan()      ptrtab = slotab + COFF

char *
control( p )		/* find and handle preprocessor control lines */
	register char *p;
{
	register struct symtab *np;

	for ( ;; )
	{
		fasscan();
		p = cotoken( p );
		if ( *inp == '\n' )
			++inp;
		dump();
		sloscan();
		p = skipbl( p );
		*--inp = SALT;
		outp = inp;
		++flslvl;
		np = slookup( inp, p, 0 );
		--flslvl;
		if ( np == defloc || np == rdefloc )    /* define */
		{
			if ( flslvl == 0 )
			{
				p = dodef( p ); 
				continue;
			}
		}
		else if ( np == incloc || np == rincloc )       /* include */
		{
			if ( flslvl == 0 )
			{
				p = doincl( p );
				continue;
			}
		}
		else if ( np == ifnloc || np == rifnloc )       /* ifndef */
		{
			++flslvl;
			p = skipbl( p );
			np = slookup( inp, p, 0 );
			--flslvl;
			if ( flslvl == 0 && np->value == 0 )
				++trulvl;
			else
				++flslvl;
# ifdef CXREF
			ref(xcopy(inp, p), lineno[ifno]);
# endif
		}
		else if ( np == ifdloc || np == rifdloc )       /* ifdef */
		{
			++flslvl;
			p = skipbl( p );
			np = slookup( inp, p, 0 );
			--flslvl;
			if ( flslvl == 0 && np->value != 0 )
				++trulvl;
			else
				++flslvl;
# ifdef CXREF
			ref(xcopy(inp, p), lineno[ifno]);
# endif
		}
		else if ( np == eifloc || np == reifloc )       /* endif */
		{
			if ( flslvl )
			{
				if ( --flslvl == 0 )
					sayline(CONT);
			}
			else if ( trulvl )
				--trulvl;
			else
				pperror( MSG ("If-less endif",
					"#endif без #if"), 0 );
		}
		else if ( np == elsloc || np == relsloc )        /* else */
		{
			if ( flslvl )
			{
				if ( --flslvl != 0 )
					++flslvl;
				else
				{
					++trulvl;
					sayline(CONT);
				}
			}
			else if ( trulvl )
			{
				++flslvl;
				--trulvl;
			}
			else
				pperror( MSG ("If-less else",
					"#else без #if"), 0 );
		}
		else if ( np == udfloc || np == rudfloc )       /* undefine */
		{
			if ( flslvl == 0 )
			{
				++flslvl;
				p = skipbl( p );
				slookup( inp, p, DROP );
				--flslvl;
			}
# ifdef CXREF
			ref(xcopy(inp, p), lineno[ifno]);
# endif
		}
		else if ( np == ifloc || np == rifloc )         /* if */
		{
# if tgp
			pperror( MSG (" IF not implemented, true assumed",
				"#if не реализован, считается true"), 0 );
			if ( flslvl == 0 )
				++trulvl;
			else
				++flslvl;
# else
			newp = p;
# ifdef CXREF
			xline = lineno[ifno];
# endif
			if ( flslvl == 0 && yyparse() )
				++trulvl;
			else
				++flslvl;
			p = newp;
# endif
		}
		else if ( np == lneloc || np == rlneloc )        /* line */
		{
			if ( flslvl == 0 && pflag == 0 )
			{
				register char *s;
				register int ln;

				outp = inp = p;
			do_line:
				*--outp = '#';
				/*
				* make sure that the whole
				* directive has been read
				*/
				s = p;
				while ( *s && *s != '\n' )
					s++;
				if ( eob( s ) )
					p = refill( s );
				/*
				* eat the line number
				*/
				s = inp;
				while ( ( toktyp + COFF )[*s] == BLANK )
					s++;
				ln = 0;
				while ( isdigit( *s ) )
					ln = ln * 10 + *s++ - '0';
				if ( ln )
					lineno[ifno] = ln - 1;
				else
					pperror( MSG ("bad number for #line",
						"плохой номер в команде #line") );
				/*
				* eat the optional "filename"
				*/
				while ( ( toktyp + COFF )[*s] == BLANK )
					s++;
				if ( *s != '\n' )
				{
					if ( *s != '"' )
						pperror( MSG ("bad file for #line",
							"плохое имя файла в команде #line") );
					else
					{
						register char *t = savch;

						for ( ;; )
						{
							if ( *++s == '"' )
								break;
							else if ( *s == '\n' ||
								*s == '\0' )
							{
								pperror( MSG ("bad file for #line",
									"плохое имя файла в команде #line") );
								break;
							}
							*t++ = *s;
						}
						*t++ = '\0';
						if ( strcmp( savch, fnames[ifno] ) )
						{
							fnames[ifno] = savch;
							savch = t;
						}
					}
				}
				/*
				* push it all along to be eventually printed
				*/
				while ( *inp != '\n' )
					p = cotoken( p );
				continue;
			}
		}
		else if ( np == clsloc || np == rclsloc )       /* class */
			exfail = CLASSCODE;		/* return value */
		else if ( np == idtloc || np == ridtloc )       /* ident */
		{
			/*
			* For now, just ignore the information on this line
			*/
		}
		else if ( *++inp == '\n' )	/* allows blank line after # */
			outp = inp;
		else if ( isdigit( *inp ) )	/* pass thru line directives */
		{
			outp = p = inp;
			goto do_line;
		}
		else
			pperror( MSG ("undefined control",
				"неверная команда препроцессора"), 0 );
		/* flush to lf */
		++flslvl;
		while ( *inp != '\n' )
		{
			outp = inp = p;
			p = cotoken( p );
		}
		--flslvl;
	}
}

struct symtab *
stsym( s )
	register char *s;
{
	char buf[BUFSIZ];
	register char *p;

	/* make definition look exactly like end of #define line */
	/* copy to avoid running off end of world when param list is at end */
	p = buf;
	while ( *p++ = *s++ )
		;
	p = buf;
	while ( isid( *p++ ) )		/* skip first identifier */
		;
	if ( *--p == '=' )
	{
		*p++ = ' '; 
		while ( *p++ )
			;
	}
	else
	{
		s = " 1";
		while ( *p++ = *s++ )
			;
	}
	pend = p;
	*--p = '\n';
	sloscan();
	dodef( buf );
	return( lastsym );
}

struct symtab *
ppsym( s )		/* kludge */
	char *s;
{
	register struct symtab *sp;

	cinit = SALT;
	*savch++ = SALT;
	sp = stsym( s );
	--sp->name;
	cinit = 0;
	return( sp );
}


int yy_errflag;		/* TRUE when pperror called by yyerror() */

/* VARARGS1 */
pperror( s, x, y )
	char *s;
{
	if ( fnames[ifno][0] )
# if gcos
		fprintf( stderr, ERRFORM, exfail >= 0 ? 'F' : 'W',fnames[ifno]);
# else
		fprintf( stderr, "%s: ", fnames[ifno] );
# endif
	fprintf( stderr, "%d: ", lineno[ifno] );
	fprintf( stderr, s, x, y );
	if ( yy_errflag )
		fprintf( stderr, MSG (" (in preprocessor if)\n",
			" (в команде #if)\n") );
	else
		fprintf( stderr, "\n" );
	if ( exfail < CLASSCODE - 1 )
		++exfail;
}

yyerror( s, a, b )
	char *s;
{
	yy_errflag = 1;
	pperror( s, a, b );
	yy_errflag = 0;
}

ppwarn( s, x )
	char *s;
{
	int fail = exfail;

	exfail = -1;
	pperror( s, x );
	exfail = fail;
}

struct symtab *
lookup( namep, enterf )
	char *namep;
{
	register char *np, *snp;
	register int c, i;
	int around;
	register struct symtab *sp;

	/* namep had better not be too long (currently, <=ncps chars) */
	np = namep;
	around = 0;
	i = cinit;
	while ( c = *np++ )
		i += i + c;
	c = i;	/* c=i for register usage on pdp11 */
	c %= symsiz;
	if ( c < 0 )
		c += symsiz;
	sp = &stab[c];
	while ( snp = sp->name )
	{
		np = namep;
		while ( *snp++ == *np )
			if ( *np++ == '\0' )
			{
				if ( enterf == DROP )
				{
					sp->name[0] = DROP;
					sp->value = 0;
				}
				return( lastsym = sp );
			}
		if ( --sp < &stab[0] )
			if ( around )
			{
				pperror( MSG ("too many defines",
					"слишком много #define"), 0 );
				exit( exfail ? ( exfail ==
					CLASSCODE ? CLASSCODE : 2 ) : 0 );
			}
			else
			{
				++around;
				sp = &stab[symsiz - 1];
			}
	}
	if ( enterf == 1 )
		sp->name = namep;
	return( lastsym = sp );
}

struct symtab *
slookup( p1, p2, enterf )
	register char *p1, *p2;
	int enterf;
{
	register char *p3;
	char c2, c3;
	struct symtab *np;

	c2 = *p2;	/* mark end of token */
	*p2 ='\0';
	if ( ( p2 - p1 ) > ncps )
		p3 = p1 + ncps;
	else
		p3 = p2;
	c3 = *p3;	/* truncate to ncps chars or less */
	*p3 ='\0';
	if ( enterf == 1 )
		p1 = copy( p1 );
	np = lookup( p1, enterf );
	*p3 = c3;
	*p2 = c2;
	if ( np->value != 0 && flslvl == 0 )
		newp = subst( p2, np );
	else
		newp = 0;
	return( np );
}

char *
subst( p, sp )
	register char *p;
	struct symtab *sp;
{
	register char *ca, *vp;
	int params;
	char *actual[MAXFRM]; /* actual[n] is text of nth actual */
	char acttxt[BUFSIZ]; /* space for actuals */

	if ( 0 == ( vp = sp->value ) )
		return( p );
	if ( ( p - macforw ) <= macdam )
	{
		if ( ++maclvl > symsiz && !rflag )
		{
			pperror( MSG ("%s: macro recursion",
				"%s: рекурсивный макрос"), sp->name );
			return( p );
		}
	}
	else
		maclvl = 0;	/* level decreased */
	macforw = p;	/* new target for decrease in level */
	macdam = 0;
	macnam = sp->name;
# ifdef CXREF
	ref(macnam, lineno[ifno]);
# endif
	dump();
	if ( sp == ulnloc || sp == rulnloc )
	{
		vp = acttxt;
		*vp++ = '\0';
		sprintf( vp, "%d", lineno[ifno] );
		while ( *vp++ )
			;
	}
	else if ( sp == uflloc || sp == ruflloc )
	{
		vp = acttxt;
		*vp++ = '\0';
		sprintf( vp, "\"%s\"", fnames[ifno] );
		while ( *vp++ )
			;
	}
	if ( 0 != ( params = *--vp & 0xFF ) )	/*definition calls for params */
	{
		register char **pa;

		ca = acttxt;
		pa = actual;
		if ( params == 0xFF )   /* #define foo() ... */
			params = 1;
		sloscan();
		++flslvl; /* no expansion during search for actuals */
		plvl = -1;
		maclin = lineno[ifno];
		macfil = fnames[ifno];
		do
		{
			p = skipbl( p );
		}
		while ( *inp == '\n' );		/* skip \n too */
		if ( *inp == '(' )
		{
			for ( plvl = 1; plvl != 0; )
			{
				*ca++ = '\0';
				for ( ;; )
				{
					outp = inp = p;
					p = cotoken( p );
					if ( *inp == '(' )
						++plvl;
					if ( *inp == ')' && --plvl == 0 )
					{
						--params;
						break;
					}
					if ( plvl == 1 && *inp == ',' )
					{
						--params;
						break;
					}
					while ( inp < p )
					{
						/*
						*  toss newlines in arguments
						*  to macros - keep problems
						*  to a minimum.
						*  if a backslash is just
						*  before the newline, assume
						*  it is in a string and
						*  leave it in.
						*/
						if ( *inp == '\n'
							&& inp[-1] != '\\' )
						{
							*inp = ' ';
						}
						*ca++ = *inp++;
					}
					if ( ca > &acttxt[BUFSIZ] )
						pperror( MSG ("%s: actuals too long",
							"%s: слишком длинные фактические параметры"),
							sp->name );
				}
				if ( pa >= &actual[MAXFRM] )
					ppwarn( MSG ("%s: argument mismatch",
						"%s: несоответствие аргументов"),
						sp->name );
				else
					*pa++ = ca;
			}
		}
		if ( maclin != lineno[ifno] )	/* embedded linefeeds in macro call */
		{
			int i, j = lineno[ifno];

			for ( i = sizeof( int ) / sizeof( char ); --i >= 0; )
			{
				if ( bob( p ) )
				{
					outp = inp = p;
					p = unfill( p );
				}
				*--p = j;
				j >>= 8;
			}
			if ( bob( p ) )
			{
				outp = inp = p;
				p = unfill( p );
			}
			*--p = warnc;
		}
		if ( params != 0 )
			ppwarn( MSG ("%s: argument mismatch",
				"%s: несоответствие аргументов"),
				sp->name );
		while ( --params >= 0 )
			*pa++ = "" + 1;	/* null string for missing actuals */
		--flslvl;
		fasscan();
	}
	for ( ;; )	/* push definition onto front of input stack */
	{
		while ( !iswarn( *--vp ) )
		{
			if ( bob( p ) )
			{
				outp = inp = p;
				p = unfill( p );
			}
			*--p = *vp;
		}
		if ( *vp == warnc )	/* insert actual param */
		{
			ca = actual[*--vp - 1];
			while ( *--ca )
			{
				if ( bob( p ) )
				{
					outp = inp = p;
					p = unfill( p );
				}
				*--p = *ca;
			}
		}
		else
			break;
	}
	outp = inp = p;
	return( p );
}




char *
trmdir( s )
	register char *s;
{
	register char *p = s;

	while ( *p++ )
		;
	--p;
	while ( p > s && *--p != '/' )
		;
# if unix
	if ( p == s )
		*p++ = '.';
# endif
	*p = '\0';
	return( s );
}

STATIC char *
copy( s )
	register char *s;
{
	register char *old;

	old = savch;
	while ( *savch++ = *s++ )
		;
	return( old );
}

yywrap()
{
	return( 1 );
}

main( argc, argv )
	char *argv[];
{
	register int i, c;
	register char *p;
	char *tf, **cp2;

	initmsg ();
# if gcos
	if ( setjmp( env ) )
		return( exfail ? ( exfail == CLASSCODE ? CLASSCODE : 2 ) : 0 );
# endif
	p = "0123456789_\
ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\
юабцдефгхийклмнопярстужвьызшэщчъЮАБЦДЕФГХИЙКЛМНОПЯРСТУЖВЬЫЗШЭЩЧ";
	i = 0;
	while ( c = *p++ )
	{
		( fastab + COFF )[c] |= IB | NB | SB;
		( toktyp + COFF )[c] = IDENT;
# if scw2
		/* 53 == 63-10; digits rarely appear in identifiers,
		/* and can never be the first char of an identifier.
		/* 11 == 53*53/sizeof(macbit) .
		*/
		++i;
		( t21 + COFF )[c] = ( 53 * i ) / 11;
		( t22 + COFF )[c] = i % 11;
# endif
	}
	p = "0123456789.";
	while ( c = *p++ )
	{
		( fastab + COFF )[c] |= NB | SB;
                ( toktyp + COFF )[c] = NUMBR;
	}
# if gcos
	p = "\n\"'`/\\";
# else
	p = "\n\"'/\\";
# endif
	while ( c = *p++ )
		( fastab + COFF )[c] |= SB;
# if gcos
	p = "\n\"'`\\";
# else
	p = "\n\"'\\";
# endif
	while ( c = *p++ )
		( fastab + COFF )[c] |= QB;
	p = "*\n";
	while ( c = *p++ )
		( fastab + COFF)[c] |= CB;
	( fastab + COFF )[warnc] |= WB | SB;
	( fastab + COFF )['\0'] |= CB | QB | SB | WB;
	for ( i = ALFSIZ; --i >= 0; )
		slotab[i] = fastab[i] | SB;
	p = " \t\v\f\r";	/* note no \n; */
	while ( c = *p++ )
		( toktyp + COFF )[c] = BLANK;
# if scw2
	for ( ( t23 + COFF )[i = ALFSIZ + 7 - COFF] = 1; --i >= -COFF; )
		if ( ( ( t23 + COFF )[i] = ( t23 + COFF + 1 )[i] << 1 ) == 0 )
			( t23 + COFF )[i] = 1;
# endif

# if unix
	fnames[ifno = 0] = "";
	dirnams[0] = dirs[0] = ".";
# endif
# if ibm
	fnames[ifno = 0] = "";
# endif
# if gcos
	if ( inquire( stdin, _TTY ) )
		freopen( "*src", "rt", stdin );
# endif
# if gimpel || gcos
	fnames[ifno = 0] = (char *) inquire( stdin, _FILENAME );
	dirnams[0] = dirs[0] = trmdir( copy( fnames[0] ) );
# endif
	for ( i = 1; i < argc; i++ )
	{
		switch ( argv[i][0] )
		{
		case '-':
# if gcos
					/* case-independent on GCOS */
			switch ( toupper( argv[i][1] ) )
# else
			switch( argv[i][1] )
# endif
			{
			case 'M':
				mflag++;
			case 'P':
				pflag++;
			case 'E':
				continue;
			case 'R':
				++rflag;
				continue;
			case 'C':
				passcom++;
				continue;
# ifdef CXREF
			case 'F':
				if (! (outfp = fopen(argv[++i],"w")))
				{
					fprintf(stderr,
						MSG ("Can't open %s\n",
						    "не могу открыть %s\n"),
						argv[i]);
					exit(1);
				}
# endif
			case 'D':
				if ( predef > prespc + NPREDEF )
				{
					pperror( MSG ("too many -D options, ignoring %s",
						"слишком много флагов -D, %s пропущено"),
						argv[i] );
					continue;
				}
				/* ignore plain "-D" (no argument) */
				if ( *( argv[i] + 2 ) )
					*predef++ = argv[i] + 2;
				continue;
			case 'U':
				if ( prund > punspc + NPREDEF )
				{
					pperror( MSG ("too many -U options, ignoring %s",
						"слишком много флагов -U, %s пропущено"),
						argv[i] );
					continue;
				}
				*prund++ = argv[i] + 2;
				continue;
			case 'I':
				if ( nd > MAXINC - 4 )
					pperror( MSG ("excessive -I file (%s) ignored",
						"слишком много флагов -I, файл %s пропущен"),
						argv[i] );
				else
					dirs[nd++] = argv[i] + 2;
				continue;
			case '\0':
				continue;
			case 'T':
				ncps = 8;	/* backward name compatabilty */
				continue;
			default: 
				pperror( MSG ("unknown flag %s",
					"неизвестный флаг %s"), argv[i] );
				continue;
			}
		default:
			if ( fin == stdin )
			{
				if ( ! ( fin = fopen( argv[i], READ ) ) )
				{
					pperror( MSG ("No source file %s",
						"нет исходного файла %s"),
						argv[i] );
					exit(2);
				}
				fnames[ifno] = copy( argv[i] );
				infile = copy ( argv[i] );
# ifdef CXREF
				fprintf(outfp,"\"%s\"\n", fnames[ifno]);
# endif
				dirs[0] = dirnams[ifno] = trmdir( argv[i] );
# ifndef gcos
/* too dangerous to have file name in same syntactic position
   be input or output file depending on file redirections,
   so force output to stdout, willy-nilly
	[i don't see what the problem is.  jfr]
*/
			}
			else if ( fout == stdout )
			{
				static char _sobuf[BUFSIZ];

				if ( ! ( fout = fopen( argv[i], WRITE ) ) )
				{
					pperror( MSG ("Can't create %s",
						"не могу создать %s"),
						argv[i] );
					exit(2);
				}
				else
				{
					fclose( stdout );
					setbuf( fout, _sobuf );
				}
# endif
			}
			else
				pperror( MSG ("extraneous name %s",
					"лишний параметр %s"),
					argv[i] );
		}
	}

	if (mflag) {
		if (! infile) {
			fprintf (stderr,
				"no input file specified with -M flag\n");
			exit (8);
		}
		tf = strrchr (infile, '.');
		if (! tf) {
			fprintf (stderr, "missing component name on %s\n",
				infile);
			exit (8);
		}
		tf[1] = 'o';
		tf = strrchr (infile, '/');
		if (tf)
			infile = tf + 1;
		mout = fout;
		if (! (fout = fopen ("/dev/null", "w"))) {
			pperror ("Can't open /dev/null");
			exit (8);
		}
	}
	fins[ifno] = fin;
	exfail = 0;
	/* after user -I files here are the standard include libraries */
# if unix
	dirs[nd++] = "/usr/include";
# endif
# if gcos
	dirs[nd++] = "cc/include";
# endif
# if ibm
#	ifndef gimpel
		dirs[nd++] = "BTL$CLIB";
#	endif
# endif
# ifdef gimpel
	dirs[nd++] = intss() ?  "SYS3.C." : "";
# endif
# ifdef compool
	dirs[nd++] = "/compool";
# endif
	dirs[nd++] = 0;
	defloc = ppsym( "define" );     rdefloc = ppsym( "опред" );
	udfloc = ppsym( "undef" );      rudfloc = ppsym( "неопр" );
	incloc = ppsym( "include" );    rincloc = ppsym( "включ" );
	elsloc = ppsym( "else" );       relsloc = ppsym( "иначе" );
	eifloc = ppsym( "endif" );      reifloc = ppsym( "конес" );
	ifdloc = ppsym( "ifdef" );      rifdloc = ppsym( "есопр" );
	ifnloc = ppsym( "ifndef" );     rifnloc = ppsym( "еснопр" );
	ifloc = ppsym( "if" );          rifloc = ppsym( "если" );
	lneloc = ppsym( "line" );       rlneloc = ppsym( "строка" );
	clsloc = ppsym( "class" );      rclsloc = ppsym( "класс" );
	idtloc = ppsym( "ident" );      ridtloc = ppsym( "имя" );
	for ( i = sizeof( macbit ) / sizeof( macbit[0] ); --i >= 0; )
		macbit[i] = 0;
	ulnloc = stsym( "__LINE__" );   rulnloc = stsym( "__СТРОКА__" );
	uflloc = stsym( "__FILE__" );   ruflloc = stsym( "__ФАЙЛ__" );
# if unix
	ysysloc = stsym( "unix" );
# endif
# if gcos
	ysysloc = stsym( "gcos" );
# endif
# if ibm
	ysysloc = stsym( "ibm" );
# endif
# if interdata
	varloc = stsym( "interdata" );
# endif
# if pdp11
	varloc = stsym( "pdp11" );
# endif
# if vax
	varloc = stsym( "vax" );
# endif
# if besm
	varloc = stsym( "besm" );
# endif
# if svsb
	varloc = stsym( "svsb" );
# endif
# if u370
	varloc = stsym( "u370" );
# endif
# if u3b
	varloc = stsym( "u3b" );
# endif
# if u3b5
	varloc = stsym( "u3b5" );
# endif
# if tss
	varloc = stsym( "tss" );
# endif
# if os
	varloc = stsym( "os" );
# endif
# if mert
	varloc = stsym( "mert" );
# endif
# if RT
	varloc = stsym( "RT" );
# endif
# if RES
	varloc = stsym( "RES" );
# endif

	tf = fnames[ifno];
	fnames[ifno] = "command line";
	lineno[ifno] = 1;
	cp2 = prespc;
	while ( cp2 < predef )
		stsym( *cp2++ );
	cp2 = punspc;
	while ( cp2 < prund )
	{
		if ( p = strchr( *cp2, '=' ) )
			*p++ = '\0';
		/*
		* Truncate to ncps characters if needed.
		*/
		if ( strlen( *cp2 ) > ncps )
			(*cp2)[ncps] = '\0';
		lookup( *cp2++, DROP );
	}
	fnames[ifno] = tf;
	pbeg = buffer + ncps;
	pbuf = pbeg + BUFSIZ;
	pend = pbuf + BUFSIZ;

	trulvl = 0;
	flslvl = 0;
	lineno[0] = 1;
	sayline(START);
	outp = inp = pend;
# ifdef CXREF
	ready = 1;
# endif
	control( pend );
	if ( fout && ferror( fout ) )
		pperror( MSG ("Problems with writing output file; probably out of temp space",
			"ошибка при записи в выходной файл") );
	exit( exfail ? ( exfail == CLASSCODE ? CLASSCODE : 2 ) : 0 );
}
 
# ifdef CXREF
ref( name, line )
	char *name;
	int line;
{
# ifdef FLEXNAMES
	fprintf(outfp, "R%s\t%05d\n", name, line);
# else
	fprintf(outfp, "R%.8s\t%05d\n", name, line);
# endif
}

def( name, line )
	char *name;
	int line;
{
	if (ready)
# ifdef FLEXNAMES
		fprintf(outfp, "D%s\t%05d\n", name, line);
# else
		fprintf(outfp, "D%.8s\t%05d\n", name, line);
# endif
}

newf( name, line )
	char *name;
	int line;
{
# ifdef FLEXNAMES
	fprintf(outfp, "F%s\t%05d\n", name, line);
# else
	fprintf(outfp, "F%.8s\t%05d\n", name, line);
# endif
}

char *
xcopy( ptr1, ptr2 )
	register char *ptr1, *ptr2;
{
	static char name[NCPS];
	char *saveptr, ch;
	register char *ptr3 = name;

	/* locate end of name; save character there */
	if ((ptr2 - ptr1) > ncps)
	{
		saveptr = ptr1 + ncps;
		ch = *saveptr;
		*saveptr = '\0';
	}
	else {
		ch = *ptr2;
		*ptr2 = '\0';
		saveptr = ptr2;
	}
	while (*ptr3++ = *ptr1++) ;	/* copy name */
	*saveptr = ch;	/* replace character */
	return( name );
}
# endif
