/*
 * $Header: server.h,v 2.0 92/08/05 08:43:29 vak Exp $
 * $Log:	server.h,v $
 * Revision 2.0  92/08/05  08:43:29  vak
 * 
 * 
 * Revision 1.3  92/06/19  22:20:31  usenet
 * Fix errors. Rudnev
 * 
 * Revision 1.2  92/06/19  22:12:10  usenet
 * 
 */
struct cmdtab {
	char *cmdname;          /* command name */
	int (*cmdfunc) ();      /* function to execute */
};
