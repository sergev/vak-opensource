/*	@(#)lerror.c	1.4	*/
/*	3.0 SID #	1.3	*/
/* lerror.c
 *	This file was added to support lint message buffering.  Message
 *	buffering works in the following fashion:
 *
 *	A message is printed out via lwerror (warning error) or luerror
 *	(exceptions: lint errors, output via lerror, and compiler
 *	errors, output via cerror).  lwerror and luerror check the
 *	type of the message, and determine the appropriate routine to call.
 *	There are three message "types" :  the message is to be buffered,
 *	the message is not to be buffered, or the message is from a header file.
 *
 *	All (non-header file) buffered messages go into the ctmpfile.  This file
 *	is "dumped" before lint1 completes.  Non-header unbuffered messages
 *	are printed immediately.  Header file messages are always buffered
 *	in the htmpfile.  The rationale is to complain about header files
 *	only once.  This means that the htmpfile is saved between calls
 *	to lint1.  The second pass, lint2, is responsible for dumping these
 *	messages.
 *
 *	Functions:
 *	==========
 *		bufhdr - buffer a header file message
 *		bufsource - buffer a source file message
 *		catchsig - set signals so they are caught
 *		hdrclose - close header file
 *		iscfile - checks to see if file is source or header
 *		lerror - lint error message routine
 *		luerror - lint uerror message
 *		lwerror - lint werror message (warning)
 *		onintr - handle interrupts
 *		tmpopen - open temp files and set up signal processing
 *		unbuffer - dump the ctmpfile buffered messages
 */

# include	<stdio.h>
# include	"messages.h"
# include	"lerror.h"
# include	<signal.h>

extern void	exit();
extern char	*strncpy( );

extern int	lineno;
extern char	ftitle[ ];

/* iscfile
 *  compares name with sourcename (file name from command line)
 *  if it is the same then
 *    if fileflag is false then print the source file name as a title
 *    return true
 *  otherwise return false
 */

static enum boolean	fileflag = false;

enum boolean
iscfile( name ) char *name;
{
	extern char	sourcename[ ];

#ifdef FLEXNAMES
	if ( !strcmp( name, sourcename ) ) {
#else
	if ( !strncmp( name, sourcename, LFNM ) ) {
#endif
		if ( fileflag == false ) {
			fileflag = true;
#ifdef FLEXNAMES
			fprintf( stderr, "\n%s\n==============\n", name );
#else
			fprintf( stderr, "\n%.14s\n==============\n", name );
#endif
		}
		return( true );
	}
	return( false );
}
/* onintr - cleans up after an interrupt  */
onintr( )
{
	(void) signal(SIGINT, SIG_IGN);
	(void) signal(SIGHUP, SIG_IGN);
	(void) signal(SIGQUIT, SIG_IGN);
	(void) signal(SIGPIPE, SIG_IGN);
	(void) signal(SIGTERM, SIG_IGN);

	putc( '\n', stderr );
	lerror( "", CCLOSE | HCLOSE | FATAL );
	/* note that no error message is printed */
}


/* catchsig - set up traps to field interrupts
 *	the onintr routine is the trap handler
 */
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
/* tmpopen - open temp files etc.
 *  open source message buffer file for writing
 *  open header message file for updating
 *    if open fails, open it for writing
 *  otherwise
 *    initialize header file name and count list from header message file
 *
 *  if opens succeed return success
 *  otherwise return failure
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
		lerror( "cannot open message buffer file", ERRMSG | FATAL );

	if (! (htmpfile = fopen( htmpname, "r+" ))) {
		/* the file does not exist -- create it */
		if (! (htmpfile = fopen( htmpname, "w" )))
			lerror( "cannot open header message buffer file",
			  CCLOSE | FATAL | ERRMSG );
		if (fwrite((char *) hdrlist, sizeof(HDRITEM), NUMHDRS, htmpfile)
		  != NUMHDRS )
			lerror("cannot write header message buffer file",
			  HCLOSE | CCLOSE | ERRMSG | FATAL );
	}
	else {
		/* the file already exists -- initialize header information */
		rewind( htmpfile );
		if ( fread((char *) hdrlist, sizeof( HDRITEM ), NUMHDRS, htmpfile)
		  != NUMHDRS ) 
			lerror( "cannot read header message buffer file",
			  CCLOSE | HCLOSE | FATAL | ERRMSG );
		if ( fseek( htmpfile, 0L, 2 ) != OKFSEEK ) 
			lerror( "cannot seek in header message buffer file",
			  CCLOSE | HCLOSE | FATAL | ERRMSG );
	}
}
/* hdrclose - write header file name/count to header message buffer file
 *	then close the file
 */
hdrclose( )
{
	rewind( htmpfile );
	if ( fwrite( (char *) hdrlist, sizeof( HDRITEM ), NUMHDRS, htmpfile )
	  != NUMHDRS ) 
		lerror( "cannot write header message buffer file", HCLOSE | ERRMSG );
}
/* lerror - lint error message routine
 *  if code is [CH]CLOSE error close and unlink appropriate files
 *  if code is FATAL exit
 */
lerror( message, code ) char *message; int code;
{
	if ( code & ERRMSG ) 
		fprintf( stderr, "lint error: %s\n", message );
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
	if ( code & FATAL ) 
		exit( FATAL );
}
/* lwerror - lint warning error message
 *	if the message is to be buffered, call the appropriate routine:
 *    bufhdr( ) for a header file
 *    bufsource( ) for a source file
 *
 *  if not, call werror( )
 */

/* VARARGS1 */
lwerror( msgndx, arg1, arg2 ) int	msgndx;
{
	extern char		*strip( );
	extern enum boolean	iscfile( );
	extern char		*msgtext[ ];
	extern short	msgbuf[ ];
	char		*filename;

	filename = strip( ftitle );

	if ( iscfile( filename ) == true ) 
		if ( msgbuf[ msgndx ] == 0 ) 
			werror( msgtext[ msgndx ], arg1, arg2 );
		else 
			bufsource( WERRTY, msgndx, arg1, arg2 );
	else
		bufhdr( WERRTY, filename, msgndx, arg1, arg2 );
}
/* luerror - lint error message
 *	if the message is to be buffered, call the appropriate routine:
 *    bufhdr( ) for a header file
 *    bufsource( ) for a source file
 *
 *  if not, call uerror( )
 */
/* VARARGS1 */
luerror( msgndx, arg1 )	short msgndx;
{
	extern char		*strip( );

	extern char		*msgtext[ ];
	extern short	msgbuf[ ];

	char		*filename;

	filename = strip( ftitle );

	if ( iscfile( filename ) == true ) 
		if ( msgbuf[ msgndx ] == 0 ) 
			uerror( msgtext[ msgndx ], arg1 );
		else 
			bufsource( UERRTY, msgndx, arg1 );
	else 
		bufhdr( UERRTY, filename, msgndx, arg1 );
}
/* bufsource - buffer a message for the source file */
/*
* With FLEXNAMES, keep the actual pointer to the name strings in
* ctmpfile, then when the file's contents are examined leter, the
* strings will still be in core, so the pointers will still be ok.
*/

# define nextslot(x)	((PERMSG * ((x) - 1)) + (CRECSZ * msgtotals[(x)]))
static int	msgtotals[ NUMBUF ];

/* VARARGS2 */
bufsource( code, msgndx, arg1, arg2 ) int code, msgndx;
{
	extern short msgbuf[ ], msgtype[ ];

	int		bufndx;
	CRECORD	record;

	bufndx = msgbuf[ msgndx ];
	if (  bufndx == 0  ||  bufndx >= NUMBUF )
		lerror( "message buffering scheme flakey",
		  CCLOSE | HCLOSE | FATAL | ERRMSG );
	else 
		if ( msgtotals[ bufndx ] < MAXBUF ) {
			record.code = code | msgtype[ msgndx ];
			record.lineno = lineno;

			switch( msgtype[ msgndx ]  & ~SIMPL ) {

#ifdef FLEXNAMES
			case DBLSTRTY:
				record.name2 = (char *) arg2;
				/* no break */

			case STRINGTY:
				record.arg1.name1 = (char *) arg1;
				break;
#else
			case DBLSTRTY:
				strncpy( record.name2, (char *) arg2, LCHNM );
				/* no break */

			case STRINGTY:
				strncpy( record.arg1.name1, (char *) arg1, LCHNM );
				break;
#endif

			case CHARTY:
				record.arg1.char1 = (char) arg1;
				break;

			case NUMTY:
				record.arg1.number = (int) arg1;
				break;

			default:
				break;
			}

			/* seek to slot in file for the message */
			if ( fseek( ctmpfile, (long) nextslot( bufndx ), 0 ) != OKFSEEK )
				lerror( "cannot seek in message buffer file",
				  CCLOSE | HCLOSE | FATAL | ERRMSG );
			/* and write the message in the slot */
			if ( fwrite( (char *) &record, CRECSZ, 1, ctmpfile ) != 1 ) 
				lerror( "cannot write to message buffer file",
				  CCLOSE | HCLOSE | FATAL | ERRMSG );
		}
		++msgtotals[ bufndx ];
}
/* bufhdr - buffer a message for a header file */
/*
* With FLEXNAMES, since htmpfile lives until lint2 walks over it, the
* name strings are dumped like they are to the normal output - as a null
* terminated string after the record which would normally contain the
* name(s).
*/

static int		curhdr = 0;
static enum boolean	activehdr = false;

/* VARARGS3 */
bufhdr( code, filename, msgndx, arg1, arg2 )
  int code; char *filename; int	msgndx;
{
	extern char			sourcename[ ];
	extern short		msgtype[ ];

	int		i,
	emptyslot;
	HRECORD	record;

#ifdef FLEXNAMES
	if ( activehdr == false ||
		( strcmp( hdrlist[ curhdr ].hname, filename ) != 0 ) ) {
#else
	if( activehdr == false  ||
	    ( strncmp( hdrlist[ curhdr ].hname, filename, LFNM ) != 0 )) {
#endif
		/* that is, if we do not have a new (active) header file
		 * or if this header file is not the same as the last one
		 * see if we have already seen it
		 */

		activehdr = false;
		i = curhdr;
		emptyslot = curhdr;

#ifdef FLEXNAMES
		while( strcmp( hdrlist[ i ].hname, filename ) != 0 ) {
#else
		while( strncmp( hdrlist[ i ].hname, filename, LFNM ) != 0 ) {
#endif
			/* that is, while we haven't found a match on the filename */
			if ( hdrlist[ i ].hname[ 0 ] == '\0' ) {
				emptyslot = i;
				i = 0;
			}
			else i = (i+1) % NUMHDRS;
			if ( i == curhdr ) 
				if ( hdrlist[ emptyslot ].hname[ 0 ] != '\0' ) {
					lerror( "too many header files", ERRMSG );
					return;
				}
				else {
					activehdr = true;
#ifdef FLEXNAMES
					strcpy( hdrlist[ emptyslot ].hname,
						filename );
#else
					strncpy( hdrlist[ emptyslot ].hname,
						filename, LFNM );
#endif
					strncpy( hdrlist[ emptyslot ].sname,
						sourcename, LFNM );
#ifdef FLEXNAMES
					hdrlist[emptyslot].sname[LFNM-1] = '\0';
#endif
					i = emptyslot;
					curhdr = emptyslot;
				}
		}
		if ( activehdr == false ) return;
	}

	/* activehdr is true, curhdr points to current header file name, buffer */
	++hdrlist[ curhdr ].hcount;
	record.msgndx = msgndx;
	record.code = code | msgtype[ msgndx ];
	record.lineno = lineno;

	switch( msgtype[ msgndx ]  & ~SIMPL ) {

#ifndef FLEXNAMES
	case DBLSTRTY:
		strncpy( record.name2, (char *) arg2, LCHNM );
		/* no break */

	case STRINGTY:
		strncpy( record.arg1.name1, (char *) arg1, LCHNM );
		break;
#endif

	case CHARTY:
		record.arg1.char1 = (char) arg1;
		break;

	case NUMTY:
		record.arg1.number = (int) arg1;
		break;

	default:
		break;

	}

	if ( fwrite( (char *) &record, HRECSZ, 1, htmpfile ) != 1 ) 
		lerror( "cannot write to header message buffer file",
		  CCLOSE | HCLOSE | FATAL | ERRMSG );
#ifdef FLEXNAMES
	switch ( msgtype[ msgndx ] & ~SIMPL )
	{
	case DBLSTRTY:
		if ( fwrite( (char *) arg2, strlen( arg2 ) + 1, 1, htmpfile )
			!= 1 )
		{
			lerror( "Cannot write to header message file",
				CCLOSE | HCLOSE | FATAL | ERRMSG );
		}
		/* FALL THROUGH */
	case STRINGTY:
		if ( fwrite( (char *) arg1, strlen( arg1 ) + 1, 1, htmpfile )
			!= 1 )
		{
			lerror( "Cannot write to header message file",
				CCLOSE | HCLOSE | FATAL | ERRMSG );
		}
	}
#endif
}
/* unbuffer - write out information saved in ctmpfile */
unbuffer( )
{
	extern char *outmsg[ ], *outformat[ ];
	int	i, j, stop;
	int	perline, toggle;
	enum boolean	codeflag;
	CRECORD	record;

	fclose( ctmpfile );
	if (! (ctmpfile = fopen( ctmpname, "r" )))
		lerror( "cannot open source buffer file for reading",
		  CCLOSE | FATAL | ERRMSG );

	/* loop for each message type - outer loop */
	for ( i = 1; i < NUMBUF; ++i ) 
		if ( msgtotals[ i ] != 0 ) {
			codeflag = false;

			if ( fseek( ctmpfile, (PERMSG * (i - 1)), 0 ) != OKFSEEK ) 
				lerror( "cannot seek in source message buffer file",
				  CCLOSE | FATAL | ERRMSG );
			stop = msgtotals[ i ];
			if ( stop > MAXBUF ) stop = MAXBUF;

			/* loop for each occurrence of message - inner loop */
			for ( j = 0; j < stop; ++j ) {
				if ( fread( (char *) &record, CRECSZ, 1, ctmpfile ) != 1 )
					lerror( "cannot read source message buffer file",
					  CCLOSE | FATAL | ERRMSG );

				if ( codeflag == false ) {
					if ( record.code & WERRTY ) fprintf( stderr, "warning: " );
					perline = 1;
					toggle = 0;
					if ( record.code & SIMPL ) perline = 2;
					else
						if ( !( record.code & ~WERRTY ) ) 
							/* PLAINTY */
							perline = 3;
					fprintf( stderr, "%s\n", outmsg[ i ] );
					codeflag = true;
				}
				fprintf( stderr, "    (%d)  ", record.lineno );
				switch( record.code & ~( WERRTY | SIMPL ) ) {

				case DBLSTRTY:
					fprintf( stderr, outformat[ i ], record.arg1.name1,
					    record.name2 );
					break;

				case STRINGTY:
					fprintf( stderr, outformat[ i ], record.arg1.name1 );
					break;

				case CHARTY:
					fprintf( stderr, outformat[ i ], record.arg1.char1 );
					break;

				case NUMTY:
					fprintf( stderr, outformat[ i ], record.arg1.number );
					break;

				default:
					fprintf( stderr, outformat[ i ] );
					break;

				}
				if ( ++toggle == perline ) {
					fprintf( stderr, "\n" );
					toggle = 0;
				}
				else fprintf( stderr, "\t" );
			} /* end, inner for loop */

			if ( toggle != 0 ) fprintf( stderr, "\n" );
			if ( stop < msgtotals[ i ] ) 
				fprintf( stderr,
				  "    %d messages suppressed for lack of space\n",
				    msgtotals[ i ] - stop );
	} /* end, outer for loop */

	fclose( ctmpfile );
	unlink( ctmpname );
}
