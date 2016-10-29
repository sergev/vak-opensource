/*	@(#)lerror2.c	1.5	*/
/*	3.0 SID #	1.3	*/
/* lerror2.c
 *	This file contains routines for message and error handling for
 *	the second lint pass (lint2).
 *
 *	Functions:
 *	==========
 *		buffer		buffer a message
 *		catchsig	set up signals
 *		lerror		lint error message
 *		onintr		clean up after an interrupt
 *		tmpopen		open intermediate and temporary files
 *		un2buffer	dump second pass messages
 *		unbuffer	dump header messages from first pass
 */

# define OFFSZ long

# include	<stdio.h>
# include	"messages.h"
# include	"lerror.h"
# include	"manifest"
# include	"lmanifest"
# include	"lpass2.h"
# include	<signal.h>

/* tmpopen
 *	open source message buffer file for writing
 *  open header message file for reading
 *    initialize header file name and count list from header message file
 */

static char	ctmpname[ TMPLEN + 16 ] = "";
char		*htmpname = NULL;

static FILE     *ctmpfile = 0;
static FILE     *htmpfile = 0;

static HDRITEM	hdrlist[ NUMHDRS ];

tmpopen( )
{
    sprintf( ctmpname, "%s/clint%d", TMPDIR, getpid( ) );

    catchsig( );
    if (! (ctmpfile = fopen( ctmpname, "w" )))
	lerror( "cannot open message buffer file", FATAL | ERRMSG );

    if ( htmpname == NULL )
	return;
    if (! (htmpfile = fopen( htmpname, "r" )))
		lerror( "cannot open header message buffer file",
		  CCLOSE | FATAL | ERRMSG );
    if(fread( (char *)hdrlist, sizeof(HDRITEM),NUMHDRS,htmpfile) != NUMHDRS ) 
		lerror( "cannot read header message buffer file",
		  CCLOSE | HCLOSE | FATAL | ERRMSG );
}
/* lerror - lint error message routine
 *  if code is [CH]CLOSE error close and unlink appropriate files
 *  if code is FATAL exit
 */

lerror( message, code ) char *message; int code;
{
    if ( code & ERRMSG )
		fprintf( stderr, "lint pass2 error: %s\n", message );

    if ( code & CCLOSE )
		if ( ctmpfile ) {
			fclose( ctmpfile );
			unlink( ctmpname );
		}
    if ( code & HCLOSE )
		if ( htmpfile ) {
			fclose( htmpfile );
			unlink( htmpname );
		}
    if ( code & FATAL ) (void) exit( FATAL );
}

#ifdef FLEXNAMES
/* gethstr - reads in a null terminated string from htmpfile and
*		returns a pointer to it.
*/

char *
gethstr()
{
	static char buf[BUFSIZ];
	register char *cp = buf;
	register int ch;

	while ( ( ch = getc( htmpfile ) ) != EOF )
	{
		*cp++ = ch;
		if ( ch == '\0' )
			break;
	}
	if ( ch != '\0' )
		lerror( "Name string format error in header msg buffer file",
			HCLOSE | FATAL | ERRMSG );
	return ( buf );
}
#endif

/* unbuffer - writes out information saved in htmpfile */

unbuffer( )
{
    int		i, j, stop;
    HRECORD	record;

    if (fseek( htmpfile, (long) sizeof ( hdrlist ), 0 ) != OKFSEEK )  {
		lerror( "cannot seek in header message buffer file", HCLOSE | ERRMSG );
		return;
    }

    for ( i = 0; ( i < NUMHDRS ) && ( hdrlist[ i ].hcount != 0 ); ++i ) {
		stop = hdrlist[ i ].hcount;
#ifdef FLEXNAMES
		printf( "\n%s  (as included in %s)\n==============\n",
			hdrlist[ i ].hname, hdrlist[ i ].sname );
#else
		printf( "\n%.14s  (as included in %.14s)\n==============\n",
			hdrlist[ i ].hname, hdrlist[ i ].sname );
#endif
		for ( j = 0; j < stop; ++j ) {
			if ( fread( (char *) &record, HRECSZ, 1, htmpfile ) != 1 ) 
				lerror( "cannot read header message buffer file",
				  HCLOSE | FATAL | ERRMSG );

			printf( "(%d)  ", record.lineno );
			if ( record.code & WERRTY ) 
				printf( "warning: " );

			switch( record.code & ~( WERRTY | SIMPL ) ) {

			case DBLSTRTY:
#ifdef FLEXNAMES
				record.name2 = gethstr();
				record.arg1.name1 = gethstr();
#endif
				printf( msgtext[ record.msgndx ],
					record.arg1.name1, record.name2 );
				break;

			case STRINGTY:
#ifdef FLEXNAMES
				record.arg1.name1 = gethstr();
#endif
				printf( msgtext[ record.msgndx ], record.arg1.name1 );
				break;

			case CHARTY:
				printf( msgtext[ record.msgndx ], record.arg1.char1 );
				break;

			case NUMTY:
				printf( msgtext[ record.msgndx ], record.arg1.number );
				break;

			default:
				printf( msgtext[ record.msgndx ] );
				break;

			}
		printf( "\n" );
		}
    }
    fclose( htmpfile );
    unlink( htmpname );
}
/*  onintr - clean up after an interrupt
 *  ignores signals (interrupts) during its work
 */
onintr( )
{
    (void) signal(SIGINT, SIG_IGN);
    (void) signal(SIGHUP, SIG_IGN);
    (void) signal(SIGQUIT, SIG_IGN);
    (void) signal(SIGPIPE, SIG_IGN);
    (void) signal(SIGTERM, SIG_IGN);

	putc( '\n', stderr);
    lerror( "interrupt", CCLOSE | HCLOSE | FATAL );
    /* note that no message is printed */
}

/*  catchsig - set up signal handling */

catchsig( )
{
    if ((signal(SIGINT, SIG_IGN)) == SIG_DFL)
	(void) signal(SIGINT, onintr);

    if ((signal(SIGHUP, SIG_IGN)) == SIG_DFL)
	(void) signal(SIGHUP, onintr);

    if ((signal(SIGQUIT, SIG_IGN)) == SIG_DFL)
	(void) signal(SIGQUIT, onintr);

    if ((signal(SIGPIPE, SIG_IGN)) == SIG_DFL)
	(void) signal(SIGPIPE, onintr);

    if ((signal(SIGTERM, SIG_IGN)) == SIG_DFL)
	(void) signal(SIGTERM, onintr);
}

static int	msg2totals[ NUM2MSGS ];
# define nextslot(x)	(( PERC2SZ * (x) ) + ( C2RECSZ * msg2totals[(x)] ))

/* VARARGS2 */
buffer( msgndx, symptr, digit ) int	msgndx; STAB *symptr; int digit;
{
    extern char		*strncpy( );

    extern short	msg2type[ ];
    extern int		cfno;
    extern union rec	r;

    C2RECORD		record;

    if ( ( msgndx < 0 ) || ( msgndx > NUM2MSGS ) )
		lerror( "message buffering scheme flakey", CCLOSE | FATAL | ERRMSG );

    if ( msg2totals[ msgndx ] < MAX2BUF ) {
#ifdef FLEXNAMES
		record.name = symptr->name;
#else
		strncpy( record.name, symptr->name, LCHNM );
#endif

		switch( msg2type[ msgndx ] ) {

	    case ND2FNLN:
		record.number = digit;
		/* no break */

	    case NM2FNLN:
		record.file2 = cfno;
		record.line2 = r.l.fline;
		/* no break */

	    case NMFNLN:
		record.file1 = symptr->fno;
		record.line1 = symptr->fline;
		break;

	    default:
		break;
		}

		if ( fseek( ctmpfile, (long) nextslot( msgndx ), 0 ) == OKFSEEK ) {
			if ( fwrite( (char *) &record, C2RECSZ, 1, ctmpfile ) != 1 )
				lerror( "cannot write to message buffer file",
				  CCLOSE | FATAL | ERRMSG );
		}
		else
			lerror( "cannot seek in message buffer file",
			  CCLOSE | FATAL | ERRMSG );
    }
    ++msg2totals[ msgndx ];
}
/* un2buffer - dump the second pass messages */

un2buffer( )
{
    extern char		*msg2text[ ];
    extern short	msg2type[ ];
#ifdef FLEXNAMES
    extern char		*fnm[ LFNM ];
#else
    extern char		fnm[ ][ LFNM ];
#endif

    int		i, j, stop;
    int		toggle;
    enum boolean	codeflag;
    C2RECORD	record;

    fclose( ctmpfile );
    if (! (ctmpfile = fopen( ctmpname, "r" )))
		lerror( "cannot open message buffer file for reading",
		  CCLOSE | FATAL | ERRMSG );

    codeflag = false;

		/* note: ( msgndx == NUM2MSGS ) --> dummy message */
    for ( i = 0; i < NUM2MSGS ; ++i ) 
		if ( msg2totals[ i ] != 0 ) {
			if ( codeflag == false ) {
				printf( "\n\n==============\n" );
				codeflag = true;
			}
			toggle = 0;

			if ( fseek( ctmpfile, (PERC2SZ * i), 0 ) != OKFSEEK )
				lerror( "cannot seek in message buffer file",
				  CCLOSE | FATAL | ERRMSG );
			stop = msg2totals[ i ];
			if ( stop > MAX2BUF ) stop = MAX2BUF;

			printf( "%s\n", msg2text[ i ] );
			for ( j = 0; j < stop; ++j ) {
				if ( fread( (char *) &record, C2RECSZ, 1, ctmpfile ) != 1 )
					lerror( "cannot read message buffer file",
					  CCLOSE | FATAL | ERRMSG );
				switch( msg2type[ i ] ) {

				case NM2FNLN:
#ifdef FLEXNAMES
					printf( "    %s   \t%s(%d) :: %s(%d)\n",
#else
					printf( "    %.8s   \t%.15s(%d) :: %.15s(%d)\n",
#endif
					  record.name, fnm[ record.file1 ], record.line1,
					  fnm[ record.file2 ], record.line2 );
					break;

				case NMFNLN:
#ifdef FLEXNAMES
					printf( "    %s   \t%s(%d)\n",
#else
					printf( "    %.8s   \t%.15s(%d)\n",
#endif
					  record.name,
					  fnm[ record.file1 ], record.line1 );
					break;

				case NMONLY:
#ifdef FLEXNAMES
					printf( "    %s", record.name );
#else
					printf( "    %.8s", record.name );
#endif
					if ( ++toggle == 3 ) {
						printf( "\n" );
						toggle = 0;
					}
					else printf( "\t" );
					break;

				case ND2FNLN:
#ifdef FLEXNAMES
					printf( "    %s( arg %d )   \t%s(%d) :: %s(%d)\n",
#else
					printf( "    %.8s( arg %d )   \t%.15s(%d) :: %.15s(%d)\n",
#endif
					  record.name, record.number, fnm[ record.file1 ],
					  record.line1, fnm[ record.file2 ], record.line2 );
					break;

				default:
					break;
				}
			}
			if ( toggle != 0 ) printf( "\n" );
			if ( stop < msg2totals[ i ] ) 
				printf( "    %d messages suppressed for lack of space\n",
			msg2totals[ i ] - stop );
	    } /* end for, if */
    fclose( ctmpfile );
    unlink( ctmpname );
}
