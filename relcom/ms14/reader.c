#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "server.h"
#include "ut.h"

#define LINESZ  512     /* maximum input line length */
#define ARGVSZ  100     /* maximum number of arguments */

#define isspace(c)      ((c)==' '||(c)=='\t'||(c)=='\r')

char inputline [LINESZ];        /* input line */
char *inputptr;                 /* pointer to input line */

char *argvector [LINESZ];       /* command argument vector */
int argcount;                   /* command argument count */
jmp_buf jmploop;                /* exit from command loop */

extern struct cmdtab cmdtable [];

/*
 * Read line from stdin to internal buffer.
 * Echoes line to stdout.
 */
void getline ()
{
	char *s;
	int c;
again:
	c = getchar ();
	if (c < 0)
		doquit ();              /* End of letter */
	for (s=inputline; c>=0 && c!='\n'; c=getchar()) {
		*s++ = c;
		if (s-inputline >= LINESZ)
			fatal ("Too long line");
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
	char *p, *s;

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
void scanargs ()
{
	char *p;

	inputptr = inputline;
	argcount = 0;
	while ((p = getword ())) {
		if (argcount >= ARGVSZ)
			fatal ("Too many arguments");
		argvector [argcount++] = p;
	}
	argvector [argcount] = 0;
}

/*
 * Search command in cmdtable.
 * Return pointer to it or 0 if not found.
 */
struct cmdtab *searchcmd (char *name)
{
	struct cmdtab *t;

	for (t=cmdtable; t->cmdname; ++t)
		if (! strncmp (t->cmdname, name, 3))
			return (t);
	return (0);
}

/*
 * Main command loop.
 * Read commands from stdin and execute them.
 */
void mainloop ()
{
	struct cmdtab *t;

	if (setjmp (jmploop))
		return;
	for (;;) {
		getline ();
		if (*inputline == '\t' ||
		    strncmp (inputline, "    ", 4) == 0)
			continue;
		scanargs ();
		if (! argcount)
			continue;
		switch (argvector[0][0]) {
		case '-': case '_':
			doquit ();
		}
		strlower (argvector [0]);
		t = searchcmd (argvector [0]);
		if (! t)
			fatal ("Unknown command");
		if (! t->cmdfunc)
			fatal ("Command is not implemented yet");
		(*t->cmdfunc) (argcount, argvector);
	}
}

/*
 * Exit from command loop.
 */
void exitloop ()
{
	longjmp (jmploop, 1);
}
