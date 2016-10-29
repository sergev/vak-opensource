/*
 * Copyright (C) 1991 Сергей Вакуленко, <vak@kiae.su>.
 * All rights reserved.
 */

# include <stdio.h>
# include <setjmp.h>
# include "server.h"
# include "lang.h"

# define LINESZ         512     /* maximum input line length */
# define ARGVSZ         100     /* maximum number of arguments */

# define isspace(c)     ((c)==' '||(c)=='\t'||(c)=='\r')

char inputline [LINESZ];        /* input line */
char *inputptr;                 /* pointer to input line */

char *argvector [LINESZ];       /* command argument vector */
int argcount;                   /* command argument count */
jmp_buf jmploop;                /* exit from command loop */

extern char *strlower ();

extern struct cmdtab cmdtable [];

/*
 * Read line from stdin to internal buffer.
 * Echoes line to stdout.
 */

getline ()
{
	register char *s;
	register c;
again:
	c = getchar ();
	if (c < 0)
		fatal (end_of_file);
	for (s=inputline; c>=0 && c!='\n'; c=getchar()) {
		if (c == '\r')
			continue;               /* ignore CR */
		*s++ = c;
		if (s-inputline >= LINESZ)
			fatal (too_long_line);
	}
	*s = 0;
	if (! *inputline)
		goto again;
	printf ("> %s\n", inputline);
}

/*
 * Get next word from input line.
 * Inputptr points to the beginning of line.
 */

char *getword ()
{
	register char *p, *s;

	p = inputptr;
	while (isspace (*p))
		++p;
	if (! *p)
		return (0);
	s = p;
	while (*p && ! isspace (*p))
		++p;
	if (*p)
		*p++ = 0;
	inputptr = p;
	return (s);
}

/*
 * Scan input line and fill argvector
 * with command arguments.
 * Argcount contains number of arguments.
 * Argvector [0] is the command name.
 */

scanargs ()
{
	register char *p;

	inputptr = inputline;
	argcount = 0;
	while (p = getword ()) {
		if (argcount >= ARGVSZ)
			fatal (too_many_arguments);
		argvector [argcount++] = p;
	}
	argvector [argcount] = 0;
}

/*
 * Search command in cmdtable.
 * Return pointer to it or 0 if not found.
 */

struct cmdtab *searchcmd (name)
char *name;
{
	register struct cmdtab *t;

	for (t=cmdtable; t->cmdname; ++t)
		if (! strcmp (t->cmdname, name))
			return (t);
	return (0);
}

/*
 * Main command loop.
 * Read commands from stdin and execute them.
 */

mainloop ()
{
	register struct cmdtab *t;

	if (setjmp (jmploop))
		return;
	for (;;) {
		getline ();
		scanargs ();
		if (! argcount)
			continue;
		strlower (argvector [0]);
		t = searchcmd (argvector [0]);
		if (! t)
			fatal (unknown_command);
		if (! t->cmdfunc)
			fatal (command_not_implemented);
		(*t->cmdfunc) (argcount, argvector);
	}
}

/*
 * Exit from command loop.
 */

exitloop ()
{
	longjmp (jmploop, 1);
}
