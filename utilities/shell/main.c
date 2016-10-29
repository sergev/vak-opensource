# include <stdio.h>
# include <signal.h>
# include <errno.h>
# include "env.h"
# include "shell.h"

/* # define DEBUG */
# define PROMPT "$$ "

static char *sysmsg [] = {
	0,
	"Hangup",               /* SIGHUP  1  hangup */
	"Interrupt",            /* SIGINT  2  interrupt (rubout) */
	"Quit",                 /* SIGQUIT 3  quit */
	"Illegal instruction",  /* SIGILL  4  illegal instruction */
	"Trace/BPT trap",       /* SIGTRAP 5  trace trap */
	"IOT trap",             /* SIGIOT  6  IOT instruction */
	"EMT trap",             /* SIGEMT  7  EMT instruction */
	"Floating exception",   /* SIGFPE  8  floating point exception */
	"Killed",               /* SIGKILL 9  kill */
	"Bus error",            /* SIGBUS  10 bus error */
	"Memory fault",         /* SIGSEGV 11 segmentation violation */
	"Bad system call",      /* SIGSYS  12 bad argument to system call */
	"Broken pipe",          /* SIGPIPE 13 write on a pipe with no one to read it */
	"Alarm call",           /* SIGALRM 14 alarm clock */
	"Terminated",           /* SIGTERM 15 software termination signal from kill */
};

static int numsysmsg = (sizeof sysmsg / sizeof sysmsg[0]);

static char *prompt = PROMPT;

extern char **environ;
extern char *sys_errlist [];
extern errno, sys_nerr;

main ()
{
	char line [512];
	register Shell *r;

	sigign ();
	for (;;) {
		printf (prompt);
		if (! gets (line))
			break;
		r = ShellParse (line);
		if (r) {
			ShellPrint (r);
			ShellExecute (r);
			ShellFree (r);
		}
	}
	return (0);
}

sigign ()
{
	signal (SIGTERM, SIG_IGN);
	signal (SIGQUIT, SIG_IGN);
}

sigdflt ()
{
	signal (SIGTERM, SIG_DFL);
	signal (SIGQUIT, SIG_DFL);
}

pwaitfor (pid)
register pid;
{
	register w, sig;
	int status;

# ifdef DEBUG
	outerr ("pexecute fork = %d\n", pid);
# endif
	do {
		status = pid;
		w = wait (&status);
	} while (w >= 0 && pid != w);
# ifdef DEBUG
	outerr ("wait (%d) = %d  status = %#x\n", pid, w, status);
# endif
	sig = status & 0177;
	if (! sig)
		return (status >> 8);
	if (sig == 0177)
		outerr ("Ptrace error");
	else if (sig < numsysmsg && sysmsg [sig])
		outerr (sysmsg [sig]);
	else
		outerr ("Signal %d", sig);
	if (status & 0200)
		outerr (" - core dumped\n");
	else
		outerr ("\n");
	return (-1);
}

static char *execat (s1, s2, si)
register char *s1, *s2;
char *si;
{
	register char *s;

	s = si;
	while (*s1 && *s1!=':')
		*s++ = *s1++;
	if (si != s)
		*s++ = '/';
	while (*s2)
		*s++ = *s2++;
	*s = 0;
	return (*s1 ? ++s1 : 0);
}

run (argv)
char **argv;
{
	static char *pathstr;
	char fname [128];
	char *newargs [256];
	int i;
	char *name;
	register char *cp;
	register unsigned etxtbsy = 1;
	register eacces = 0;

# ifdef DEBUG
	{
		register char **p;

		for (p=argv; *p; ++p)
			outerr ("%s ", *p);
		outerr ("\n");
	}
# endif
	sigdflt ();
	if (! pathstr && ! (pathstr = EnvGet ("PATH")))
		pathstr = ":/bin:/usr/bin";
	name = argv [0];
	for (cp=name; ; ++cp) {         /* check if name contains '/' */
		if (! *cp) {
			cp = pathstr;   /* if no, set cp to pathstr */
			break;
		}
		if (*cp == '/') {
			cp = "";        /* else path is empty */
			break;
		}
	}
	do {
		cp = execat (cp, name, fname);
retry:          (void) execve (fname, argv, environ);
		switch (errno) {
		case ENOEXEC:
			newargs [0] = "sh";
			newargs [1] = fname;
			for (i=1; newargs [i+1] = argv [i]; ++i) {
				if (i >= 254) {
					errno = E2BIG;
					return (-1);
				}
			}
			(void) execve ("/bin/sh", newargs, environ);
			return (-1);
		case ETXTBSY:
			if (++etxtbsy > 5)
				return (-1);
			(void) sleep (etxtbsy);
			goto retry;
		case EACCES:
			++eacces;
			break;
		case ENOMEM:
		case E2BIG:
			return (-1);
		}
	} while (cp);
	if (eacces)
		errno = EACCES;
	return (-1);
}

/* VARARGS1 */

outerr (s, a, b, c)
char *s, *a, *b, *c;
{
	char buf [200];

	sprintf (buf, s, a, b, c);
	write (2, buf, (unsigned) strlen (buf));
}

syserror ()
{
	outerr ("%s\n", errno < sys_nerr ? sys_errlist [errno] : "unknown error");
}
