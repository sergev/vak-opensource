/*	@(#)messages.h	1.4	*/
#ifdef FLEXNAMES
#	define	NUMMSGS	129
#else
#	define	NUMMSGS	128
#endif

#ifndef WERROR
#    define	WERROR	werror
#endif

#ifndef UERROR
#    define	UERROR	uerror
#endif

#ifndef MESSAGE
#    define	MESSAGE(x)	msgtext[ x ]
#endif

extern char	*msgtext[ ];
