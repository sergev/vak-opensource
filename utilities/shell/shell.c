/*
 *      Shell *ShellParse (char *command)
 *              Parse string and build shell tree.
 *              Return 0 on syntax error.
 *
 *      ShellPrint (Shell *r)
 *              Print nice-looking representation of shell tree.
 *
 *      ShellExecute (Shell *r)
 *              Execute shell tree. Return exit code.
 *
 *      ShellFree (Shell *r)
 *              Free space, allocated for shell tree.
 */

/*
 *      Syntax of shell command line.
 *
 *      list =                          &           &           ;
 *              cond                   / \         / \         / \
 *              cond & [list]       cond list   cond  0     cond list
 *              cond ; [list]
 *
 *      cond =                          &&          ||
 *              pipe                   /  \        /  \
 *              pipe && cond        pipe  cond  pipe  cond
 *              pipe || cond
 *
 *      pipe =                          |
 *              proc                   / \
 *              proc | pipe         proc pipe
 *
 *      proc =                          <>
 *              item                   /  \
 *              proc item           argv  files
 *
 *      item =                            argv =    ,
 *              word                               / \
 *              ( list )                        ,    string
 *              < word                         / \
 *              > word                    string  ...
 *              >> word
 *              >& word
 *              >>& word
 */

# include "mem.h"
# include "shell.h"

# define MAXMEM         5120
# define QUANT          10

/* node types */

# define TNODE          1       /* binary op */
# define TPROC          2       /* in/out files */
# define TARG           3       /* arg string */

/* binary op types */

# define OPAND          1       /* cmd & [cmd] */
# define OPSEMIC        2       /* cmd ; cmd */
# define OPANDAND       3       /* cmd && cmd */
# define OPOROR         4       /* cmd || cmd */
# define OPORAND        5       /* cmd |& cmd */
# define OPOR           6       /* cmd | cmd */
# define OPLIST         7       /* arg arg */

/* in/out files flags */

# define FLAPPEND       1       /* >> */
# define FLSTDERR       2       /* >& */

/* types of lexemas */

# define LANDAND        1       /* && */
# define LOROR          2       /* || */
# define LORAND         3       /* |& */
# define LRSRS          4       /* >> */
# define LRSAND         5       /* >& */
# define LRSRSAND       6       /* >>& */
# define LWORD          7       /* argument */
# define LCOMMAND       8       /* ( command ) */

static Shell *nalloc (), *get0expr (), *new ();
static tbackground (), pexecute (), pexeclist (), texecpipe ();
static tprint (), texecute (), makearg ();

static char *lptr;              /* command line pointer */
static char *word;              /* word of last getlex==LWORD */
static ungetflag;               /* flag of ungetlex */
static ungetval;                /* value of ungetlex */

extern long lseek ();

static Shell *nalloc (type)
{
	register Shell *r;

	r = (Shell *) MemAlloc (sizeof (Shell));
	r->type = type;
	return (r);
}

static nfree (r)
Shell *r;
{
	MemFree ((mem *) r);
}

static char *getword (c)
register c;
{
	register char *s, *str;

	s = str = (char *) MemAlloc (MAXMEM);
	for (*s++=c; ; *s++=c) {
		switch (c = *lptr++) {
		case '\n':      case ' ':       case '\t':
		case 0:         case '(':       case ')':       case ';':
		case '<':       case '&':       case '|':       case '>':
			break;
		default:
			continue;
		}
		break;
	}
	--lptr;
	*s++ = 0;
	MemTrunc ((mem *) str, s - str);
	return (str);
}

static ungetlex (c)
{
	ungetval = c;
	ungetflag = 1;
}

static getlex ()
{
	register c;

	if (ungetflag) {
		ungetflag = 0;
		return (ungetval);
	}
loop:
	switch (c = *lptr++) {
	case 0:
		--lptr;
		return (0);                             /* end of string */
	case '\n':
	case ' ':
	case '\t':
		goto loop;
	case '(':
	case ')':
	case ';':
	case '<':
		return (c);
	case '&':
		if (*lptr == '&') {
			++lptr;
			return (LANDAND);               /* && */
		}
		return (c);
	case '|':
		if (*lptr == '|') {
			++lptr;
			return (LOROR);                 /* || */
		}
		if (*lptr == '&') {
			++lptr;
			return (LORAND);                /* |& */
		}
		return (c);
	case '>':
		if (*lptr == '&') {
			++lptr;
			return (LRSAND);                /* >& */
		}
		if (*lptr != '>')
			return (c);
		if (*++lptr == '&') {
			++lptr;
			return (LRSRSAND);              /* >>& */
		}
		return (LRSRS);                         /* >> */
	}
	word = getword (c);
	return (LWORD);                                 /* argument */
}

static getterm (pcmd)
Shell **pcmd;
{
	register Shell *l;
	register c;

	/*
	 * item =                    argv =    ,
	 *         word                       / \
	 *         ( list )                ,    string
	 *         < word                 / \
	 *         > word            string  ...
	 *         >> word
	 *         >& word
	 *         >>& word
	 */
	switch (c = getlex ()) {
	default:
		ungetlex (c);
		return (0);
	case LWORD:
		return (LWORD);
	case '(':
		l = get0expr ();
		if (! l)
			return (0);
		if (getlex () != ')') {
error:                  outerr ("syntax error\n");
			ShellFree (l);
			return (0);
		}
		*pcmd = l;
		return (LCOMMAND);
	case '<':
		if (getlex () != LWORD)
			goto error;
		return ('<');
	case '>':
		if (getlex () != LWORD)
			goto error;
		return ('>');
	case LRSRS:             /* >> */
		if (getlex () != LWORD)
			goto error;
		return (LRSRS);
	case LRSAND:            /* >& */
		if (getlex () != LWORD)
			goto error;
		return (LRSAND);
	case LRSRSAND:          /* >>& */
		if (getlex () != LWORD)
			goto error;
		return (LRSRSAND);
	}
}

static Shell *getproc ()
{
	register Shell *proc;
	Shell *arg;
	int flag;

	/*
	 * proc =                       <>
	 *         item                /  \
	 *         proc item        argv  files
	 */
	/* create empty proc */
	proc = nalloc (TPROC);
	proc->u.p.flags = 0;
	proc->u.p.left = 0;
	proc->u.p.infile = 0;
	proc->u.p.outfile = 0;
	for (;;) switch (getterm (&arg)) {
	case 0:
		if (! proc->u.p.left) {
			if (proc->u.p.infile || proc->u.p.outfile) {
				outerr ("invalid null command\n");
				if (proc->u.p.infile)
					MemFree (proc->u.p.infile);
				if (proc->u.p.outfile)
					MemFree (proc->u.p.outfile);
			}
			nfree (proc);
			return (0);
		}
		return (proc);
	case LWORD:             /* arg */
		arg = nalloc (TARG);
		arg->u.a.str = word;
		makearg (proc, arg);
		continue;
	case LCOMMAND:          /* ( command ) */
		makearg (proc, arg);
		continue;
	case '<':               /* < filename */
		if (proc->u.p.infile) {
			outerr ("too many input files\n");
			MemFree (proc->u.p.infile);
		}
		proc->u.p.infile = word;
		continue;
	case '>':               /* > filename */
		flag = 0;
		goto makeout;
	case LRSRS:             /* >> filename */
		flag = FLAPPEND;
		goto makeout;
	case LRSAND:            /* >& filename */
		flag = FLSTDERR;
		goto makeout;
	case LRSRSAND:          /* >>& filename */
		flag = FLAPPEND | FLSTDERR;
makeout:
		if (proc->u.p.outfile) {
			outerr ("too many output files\n");
			MemFree (proc->u.p.outfile);
		}
		proc->u.p.outfile = word;
		proc->u.p.flags |= flag;
		continue;
	}
}

static Shell *get2expr ()
{
	register Shell *l, *r;
	int c;

	/*
	 * pipe =                       |
	 *         proc                / \
	 *         proc | pipe      proc pipe
	 */
	if (! (l = getproc ()))
		return (0);
	switch (c = getlex ()) {
	default:
		ungetlex (c);
	case 0:
		return (l);
	case '|':
		r = get2expr ();
		if (! r) {
			outerr ("syntax error\n");
			ShellFree (l);
			return (0);
		}
		return (new (OPOR, l, r));
	}
}

static Shell *get1expr ()
{
	register Shell *l, *r;
	int c;

	/*
	 * cond =                          &&          ||
	 *         pipe                   /  \        /  \
	 *         pipe && cond        pipe  cond  pipe  cond
	 *         pipe || cond
	 */
	if (! (l = get2expr ()))
		return (0);
	switch (c = getlex ()) {
	default:
		ungetlex (c);
	case 0:
		return (l);
	case LANDAND:
		r = get1expr ();
		if (! r) {
error:                  outerr ("syntax error\n");
			ShellFree (l);
			return (0);
		}
		return (new (OPANDAND, l, r));
	case LOROR:
		r = get1expr ();
		if (! r)
			goto error;
		return (new (OPOROR, l, r));
	}
}

static Shell *get0expr ()
{
	register Shell *l, *r;

	/*
	 *      list =                          &           &           ;
	 *              cond                   / \         / \         / \
	 *              cond & [list]       cond list   cond  0     cond list
	 *              cond ; [list]
	 */
	if (! (l = get1expr ()))
		return (0);
	switch (getlex ()) {
	default:
		outerr ("syntax error\n");
		ShellFree (l);
		return (0);
	case 0:
		return (l);
	case '&':
		r = get0expr ();
		return (new (OPAND, l, r));
	case ';':
		r = get0expr ();
		return (new (OPSEMIC, l, r));
	}
}

Shell *ShellParse (command)
char *command;
{
	register Shell *r;

	ungetflag = 0;
	lptr = command;
	r = get0expr ();
	if (r)
		canon (r);
	return (r);
}

static Shell *new (op, left, right)
Shell *left, *right;
{
	register Shell *r;

	r = nalloc (TARG);
	r->type = TNODE;
	r->u.n.op = op;
	r->u.n.left = left;
	r->u.n.right = right;
	return (r);
}

ShellPrint (r)
register Shell *r;
{
	tprint (r);
	printf ("\n");
}

static tprint (r)
register Shell *r;
{
	if (! r)
		return;
	switch (r->type) {
	case TNODE:                     /* binary op */
		tprint (r->u.n.left);
		switch (r->u.n.op) {
		case OPAND:             printf (" & ");         break;
		case OPSEMIC:           printf (" ; ");         break;
		case OPANDAND:          printf (" && ");        break;
		case OPOROR:            printf (" || ");        break;
		case OPORAND:           printf (" |& ");        break;
		case OPOR:              printf (" | ");         break;
		case OPLIST:            printf (" ");           break;
		}
		tprint (r->u.n.right);
		break;
	case TPROC:                     /* in/out files */
		tprint (r->u.p.left);
		if (r->u.p.infile)
			printf (" < %s", r->u.p.infile);
		if (! r->u.p.outfile)
			break;
		switch (r->u.p.flags) {
		case 0:                 printf (" > ");         break;
		case FLSTDERR:          printf (" >& ");        break;
		case FLAPPEND:          printf (" >> ");        break;
		case FLSTDERR|FLAPPEND: printf (" >>& ");       break;
		default:                printf (" >? ");        break;
		}
		printf ("%s", r->u.p.outfile);
		break;
	case TARG:                      /* arg string */
		printf ("%s", r->u.a.str);
		break;
	default:
		printf (" ??? ");
	}
}

ShellFree (r)
register Shell *r;
{
	if (! r)
		return;
	switch (r->type) {
	case TNODE:                     /* binary op */
		ShellFree (r->u.n.right);
		ShellFree (r->u.n.left);
		nfree (r);
		break;
	case TPROC:                     /* in/out files */
		ShellFree (r->u.p.left);
		nfree (r);
		break;
	case TARG:                      /* arg string */
		MemFree (r->u.a.str);
		nfree (r);
		break;
	}
}

static makearg (proc, arg)
register Shell *proc, *arg;
{
	register Shell *r, *q;

	/* arg contains arg string */
	r = proc->u.p.left;
	if (! r) {
		proc->u.p.left = arg;
		return;
	}
	if (r->type != TNODE || r->u.n.op != OPLIST) {
		proc->u.p.left = nalloc (TNODE);
		proc->u.p.left->u.n.left = r;
		r = proc->u.p.left;
		r->u.n.right = arg;
		r->u.n.op = OPLIST;
		return;
	}
	/* proc left contains list of args */
	/* append arg to the end of the list */
	while ((q = r->u.n.right) && q->type == TNODE && q->u.n.op == OPLIST)
		r = q;
	if (! q) {
		r->u.n.right = arg;
		return;
	}
	q = nalloc (TNODE);
	q->u.n.op = OPLIST;
	q->u.n.left = r->u.n.right;
	q->u.n.right = arg;
	r->u.n.right = q;
}

canon (r)               /* make canonic presentation of shell tree */
register Shell *r;
{
	register Shell *q;
	int op;

	/*
	 *      r-> X     =>     Y <-r
	 *         / \          / \
	 *    q-> Y   C        A   X <-q
	 *       / \              / \
	 *      A   B            B   C
	 *
	 *      (X, Y) is (&, |) or (&&, ||)
	 */
	if (! r)
		return;
	switch (r->type) {
	case TARG:
		return;
	case TPROC:
		canonproc (r->u.n.left);
		return;
	case TNODE:
		break;
	}
	for (;;) {
		q = r->u.n.left;
		if (! q)
			break;
		if (q->type == TPROC)
			canonproc (q->u.n.left);
		if (q->type != TNODE)
			break;
		if ((r->u.n.op != OPAND && r->u.n.op != OPSEMIC ||
		    q->u.n.op != OPAND && q->u.n.op != OPSEMIC) &&
		    (r->u.n.op != OPANDAND && r->u.n.op != OPOROR ||
		    q->u.n.op != OPANDAND && q->u.n.op != OPOROR)) {
			canon (q);
			break;
		}
		r->u.n.left = q->u.n.left;      /* A */
		q->u.n.left = q->u.n.right;     /* B */
		q->u.n.right = r->u.n.right;    /* C */
		op = r->u.n.op;
		r->u.n.op = q->u.n.op;
		q->u.n.op = op;
		canon (q->u.n.left);            /* B */
		canon (q->u.n.right);           /* C */
		r->u.n.right = q;
	}
}

canonproc (r)
register Shell *r;
{
	/* Proc is TPROC.
	 * Field .left contains list of arguments.
	 * This function goes through args and
	 * calls canon for each one.
	 */
	if (! r)
		return;
	switch (r->type) {
	case TARG:
		break;
	case TPROC:
		canonproc (r->u.n.left);
		break;
	case TNODE:
		if (r->u.n.op == OPLIST) {
			canonproc (r->u.n.left);
			canonproc (r->u.n.right);
		} else
			canon (r);
		break;
	}
}

ShellExecute (r)
register Shell *r;
{
	return (texecute (r, 1));
}

static texecute (r, mustfork)
register Shell *r;
{
	register ret;

	if (! r)
		return (0);
	if (r->type == TPROC)
		return (pexecute (r, mustfork));
	if (r->type != TNODE)
		return (0);
	/* now r has node type */
	switch (r->u.n.op) {
	case OPSEMIC:                   /* cmd ; cmd */
		texecute (r->u.n.left, 1);
		return (texecute (r->u.n.right, mustfork));
	case OPAND:                     /* cmd & [cmd] */
		tbackground (r->u.n.left);
		return (texecute (r->u.n.right, mustfork));
	case OPANDAND:                  /* cmd && cmd */
		ret = texecute (r->u.n.left, 1);
		if (! ret)
			return (texecute (r->u.n.right, mustfork));
		return (ret);
	case OPOROR:                    /* cmd || cmd */
		ret = texecute (r->u.n.left, 1);
		if (ret)
			return (texecute (r->u.n.right, mustfork));
		return (ret);
	case OPORAND:                   /* cmd |& cmd */
		return (texecpipe (r->u.n.left, r->u.n.right, 1));
	case OPOR:                      /* cmd | cmd */
		return (texecpipe (r->u.n.left, r->u.n.right, 0));
	}
	return (0);
}

static tbackground (r)
register Shell *r;
{
	register pid;

	pid = fork ();
	if (pid < 0) {
		syserror ();
		return;
	}
	if (pid) {
		outerr ("%d\n", pid);
		return;
	}
	close (0);
	texecute (r, 0);
	exit (-1);
}

static pexecute (r, mustfork)
register Shell *r;
{
	register pid;

	/* fork process if main shell must be preserved */
	if (mustfork) {
		pid = fork ();
		if (pid < 0) {
			syserror ();
			return (-1);
		}
		if (pid)
			return (pwaitfor (pid));
	}
	/* type of r is TPROC */

	/* open input file */
	if (r->u.p.infile) {
		close (0);
		if (open (r->u.p.infile, 0) != 0) {
			outerr ("cannot open %s for input\n", r->u.p.infile);
			exit (-1);
		}
	}
	/* open output file */
	if (r->u.p.outfile) {
		close (1);
		/* append to the end of the output file */
		if (r->u.p.flags & FLAPPEND) {
			if (open (r->u.p.outfile, 1) != 1)
				goto append;
			lseek (1, 0L, 2);
		} else
append:                 if (creat (r->u.p.outfile, 0664) != 1) {
				outerr ("cannot create %s\n", r->u.p.outfile);
				exit (-1);
			}
	}
	/* let stderr be the same as stdout */
	if (r->u.p.flags & FLSTDERR) {
		close (2);
		dup (1);
	}
	/* execute " ( command ) " as a new shell */
	if (r->u.n.left->type == TNODE && r->u.n.left->u.n.op != OPLIST) {
		texecute (r->u.n.left, 0);
		exit (-1);
	}
	/* left contains the list of arguments */
	pexeclist (r->u.n.left);
	exit (-1);
	return (0);
}

static pexeclist (r)
register Shell *r;
{
	register char **argv;
	register argc, argvsz;

	argvsz = 0;
	argv = 0;
	for (argc=0; r; ++argc) {
		MemCheckIndex (argv, char **, argvsz, QUANT, argc+1);
		if (r->type == TARG) {
			/* r points to the last argument */
			argv [argc++] = r->u.a.str;
			break;
		}
		if (r->type != TNODE || r->u.n.op != OPLIST) {
			outerr ("pexeclist error\n");
			break;
		}
		/* p left contains the first argument */
		argv [argc] = r->u.n.left->u.a.str;
		r = r->u.n.right;
	}
	argv [argc] = 0;
	run (argv);
	syserror ();
	/* MemFree ((mem *) argv); */
	exit (-1);
}

static texecpipe (l, r, errout)
register Shell *l, *r;
{
	register pid;
	int pip [2];

	/* fork main shell process */
	pid = fork ();
	if (pid < 0) {
		syserror ();
		return (-1);
	}
	if (pid)
		return (pwaitfor (pid));
	/* now we are the child */
	/* create pipe */
	if (pipe (pip) < 0) {
		syserror ();
		return (-1);
	}
	/* fork into two processes - writer and reader */
	pid = fork ();
	if (pid < 0) {
		syserror ();
		close (pip [0]);
		close (pip [1]);
		return (-1);
	}
	if (pid) {
		/* pipe reader */
		close (0);
		dup (pip [0]);
		close (pip [0]);
		close (pip [1]);
		texecute (r, 0);
		exit (-1);
	}
	/* pipe writer */
	close (1);
	dup (pip [1]);
	if (errout) {
		close (2);
		dup (pip [1]);
	}
	close (pip [0]);
	close (pip [1]);
	texecute (l, 0);
	exit (-1);
	return (0);
}
