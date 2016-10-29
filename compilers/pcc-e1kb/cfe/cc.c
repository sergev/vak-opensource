
# include <stdio.h>
# include <signal.h>

# ifndef ONEPASS
#   ifndef TWOPASS
#     define ONEPASS
#   endif
# endif

# ifdef ONEPASS
#   define CCOM "ccom"
# else
#   define PASS1 "pass1"
#   define PASS2 "pass2"
# endif

# define CPP    "cpp"
# define AS     "as"
# define LD     "ld"
# define M4     "m4"
# define MCRT   "/usr/SVSB/lib/mcrt0.o"

# define MAXARGC 512
# define MAXNAMLEN 32

# ifdef ONEPASS
char *ccom      = "/usr/SVSB/lib/ccom";
# else
char *pass1     = "/usr/SVSB/lib/pass1";
char *pass2     = "/usr/SVSB/lib/pass2";
# endif

char *cpp       = "/usr/SVSB/lib/cpp";
char *as        = "/usr/SVSB/bin/as";
char *ld        = "/usr/SVSB/bin/ld";
char *m4        = "/usr/SVSB/bin/m4";
char *crt0      = "/usr/SVSB/lib/crt0.o";

char *tmpi = "/tmp/cciXXXXXX";
char *tmps = "/tmp/ccsXXXXXX";

# ifdef TWOPASS
char *tmpc = "/tmp/cccXXXXXX";
# endif

char *outfile;
char *savestr (), *setsuf ();
int idexit ();
extern char *mktemp (), *malloc (), *strncpy ();
char *av [MAXARGC], *clist [MAXARGC], *llist [MAXARGC], *plist [MAXARGC];
int cflag, errflag, Pflag, Sflag, Eflag, pflag, gflag, aflag, Oflag;
int xflag, Xflag, mflag, vflag, Mflag, Aflag, Lflag;
int nc, nl, np, nxo, na;

# define MSG(l,r) (msg ? (r) : (l))

char msg;

initmsg ()
{
	register char *p;
	extern char *getenv ();

	msg = (p = getenv ("MSG")) && *p == 'r';
}

main (argc, argv)
char **argv;
{
	char *t;
	char *assource;
	int i, j, c;

	/* ld currently adds upto 5 args; 10 is room to spare */

	initmsg ();
	if (argc > MAXARGC-10) {
		error (MSG ("too many arguments", "слишком много аргументов"));
		exit (1);
	}
	for (i=1; i<argc; i++) {
		if (*argv[i] == '-') switch (argv[i][1]) {
		case 'L':       /* enable inline functions */
			Lflag++;
			continue;
		case 'A':       /* skip ccom, preprocess & assemble only */
			Aflag++;
			continue;
		case 'M':       /* generate dependency list to stdout */
			Mflag++;
			cflag++;
			continue;
		case 'v':       /* print passes */
			vflag++;
			continue;
		case 'm':       /* m4 rather than cpp */
			mflag++;
			continue;
		case 'a':       /* no preprocessor */
			aflag++;
			continue;
		case 'x':       /* pass to as */
			xflag++;
			continue;
		case 'X':       /* pass to as */
			Xflag++;
			continue;
		case 'S':       /* compile to assembler only */
			Sflag++;
			cflag++;
			continue;
		case 'O':       /* optimize */
			Oflag++;
			continue;
		case 'o':       /* output file name */
			if (++i < argc) {
				outfile = argv[i];
				switch (getsuf (outfile)) {
				case 'c':
				case 'o':
					error (MSG ("-o would overwrite %s",
						"-o может испортить %s"),
						outfile);
					exit (1);
				}
			}
			continue;
		case 'p':       /* profile */
			pflag++;
			crt0 = MCRT;
			continue;
		case 'g':       /* save debugger info */
			gflag++;
			continue;
		case 'E':       /* preprocess to stdout */
			Eflag++;
		case 'P':       /* preprocess to .i */
			Pflag++;
			plist[np++] = argv[i];
		case 'c':       /* compile to object */
			cflag++;
			continue;
		case 'D':       /* cpp flags */
		case 'I':
		case 'U':
		case 'C':
			plist[np++] = argv[i];
			continue;
		}
		t = argv[i];
		c = getsuf (t);
		if (c == 'c' || c == 's' || Eflag || Mflag) {
			clist[nc++] = t;
			t = setsuf (t, 'o');
		}
		if (nodup (llist, t)) {
			llist[nl++] = t;
			if (getsuf (t) == 'o') nxo++;
		}
	}
	if (!nc) goto load;
	if (Aflag && (Pflag || Eflag || Mflag)) {
		if (Pflag) error (MSG ("flags -A and -P incompatible",
			"флаг -A несовместим с -P"));
		if (Eflag) error (MSG ("flags -A and -E incompatible",
			"флаг -A несовместим с -E"));
		if (Mflag) error (MSG ("flags -A and -M incompatible",
			"флаг -A несовместим с -M"));
		exit (1);
	}
	if (aflag && (Pflag || Eflag || Mflag)) {
		if (Pflag) error (MSG ("flags -a and -P incompatible",
			"флаг -a несовместим с -P"));
		if (Eflag) error (MSG ("flags -a and -E incompatible",
			"флаг -a несовместим с -E"));
		if (Mflag) error (MSG ("flags -a and -M incompatible",
			"флаг -a несовместим с -M"));
		exit (1);
	}
	if (signal (SIGINT, SIG_IGN) != SIG_IGN) signal (SIGINT, idexit);
	if (signal (SIGTERM, SIG_IGN) != SIG_IGN) signal (SIGTERM, idexit);
	mktemp (tmpi);
# ifdef TWOPASS
	mktemp (tmpc);
# endif
	mktemp (tmps);
	for (i=0; i<nc; i++) {
		if (nc > 1 && ! Mflag) printf ("%s:\n", clist[i]);
		if (Sflag) tmps = setsuf (clist[i], 's');
		if (getsuf (clist[i]) == 's') {
			assource = clist[i];
			goto assemble;
		}
		else assource = tmps;
		if (Pflag) tmpi = setsuf (clist[i], 'i');
		else if (Aflag) tmpi = tmps;
		else if (aflag) {
			tmpi = clist[i];
			goto compile;
		}

		na = 0;
		if (mflag) {
			av [na++] = M4;
			if (! Eflag) {
				av [na++] = "-o";
				av [na++] = tmpi;
			}
			av [na++] = clist[i];
		} else {
			av [na++] = CPP;
			if (Mflag)
				av [na++] = "-M";
			av [na++] = clist[i];
			if (! Mflag)
				av [na++] = Eflag ? "-" : tmpi;
			for (j=0; j<np; j++)
				av [na++] = plist[j];
		}
		av [na++] = 0;
		if (callsys (mflag ? m4 : cpp, av)) {
			errflag++;
			cflag++;
		}
		if (Pflag || Eflag || Mflag) continue;
		if (Aflag) goto assemble;

compile:

# ifdef ONEPASS
		na = 0;
		av [na++] = CCOM;
		av [na++] = tmpi;
		av [na++] = tmps;
		if (pflag) av [na++] = "-Xp";
		if (gflag) av [na++] = "-l";
		if (Lflag) av [na++] = "-L";
		av [na] = 0;
		if (callsys (ccom, av)) {
			cflag++;
			errflag++;
			continue;
		}
		if (Sflag) continue;
# else
		na = 0;
		av [na++] = PASS1;
		av [na++] = tmpi;
		av [na++] = tmpc;
		if (pflag) av [na++] = "-Xp";
		if (gflag) av [na++] = "-l";
		av [na] = 0;
		if (callsys (pass1, av)) {
			cflag++;
			errflag++;
			continue;
		}
		na = 0;
		av [na++] = PASS2;
		av [na++] = tmpc;
		av [na++] = tmps;
		if (Lflag) av [na++] = "-L";
		av [na] = 0;
		if (callsys (pass2, av)) {
			cflag++;
			errflag++;
			continue;
		}
		if (Sflag) continue;
# endif

assemble:
		if (!aflag && !Aflag) unlink (tmpi);
# ifdef TWOPASS
		unlink (tmpc);
# endif
		na = 0;
		av [na++] = AS;
		av [na++] = xflag ? "-x" : "-X";
		av [na++] = "-o";
		av [na++] = setsuf (clist[i], 'o');
		av [na++] = assource;
		av [na] = 0;
		if (callsys (as, av)) {
			cflag++;
			errflag++;
			continue;
		}
	}

load:
	if (!cflag && nl) {
		na = 0;
		av [na++] = LD;
		av [na++] = xflag ? "-x" : "-X";
		av [na++] = crt0;
		if (outfile) {
			av [na++] = "-o";
			av [na++] = outfile;
		}
		i = 0;
		while (i < nl) av [na++] = llist[i++];
		if (pflag) av [na++] = "-lc_p";
		else av [na++] = "-lc";
		av [na++] = 0;
		if (callsys (ld, av))
			errflag++;

		if (nc==1 && nxo==1 && !errflag)
			unlink (setsuf (clist[0], 'o'));
	}
	dexit ();
}

idexit ()
{
	errflag++;
	dexit ();
}

dexit ()
{
	if (!Pflag) {
		if (!Sflag) unlink (tmps);
		if (!aflag) unlink (tmpi);
# ifdef TWOPASS
		unlink (tmpc);
# endif
	}
	exit (errflag != 0);
}

/* VARARGS1 */

error (s, x)
char *s, *x;
{
	fprintf (stderr, "cc: ");
	fprintf (stderr, s, x);
	fprintf (stderr, "\n");
	cflag++;
	errflag++;
}

getsuf (as)
char as[];
{
	register int c;
	register char *s;
	register int t;

	s = as;
	c = 0;
	while (t = *s++)
		if (t == '/') c = 0;
		else c++;
	s -= 3;
	if (c<=MAXNAMLEN && c>2 && *s++ == '.') return (*s);
	return (0);
}

char *setsuf (as, ch)
char *as;
{
	register char *s, *s1;

	s = s1 = savestr (as);
	while (*s) if (*s++ == '/') s1 = s;
	s[-1] = ch;
	return (s1);
}

callsys (f, v)
char *f, **v;
{
	int t, status;

	if (vflag) {
		register char **p;

		for (p=v; *p; ++p)
			printf ("%s ", *p);
		putchar ('\n');
	}
	if ((t = fork ()) == -1) {
		error (MSG ("cannot create process",
			"не могу создать процесс"));
		return (1);
	}
	if (t == 0) {
		/* порожденный процесс */
		execv (f, v);
		error (MSG ("cannot find %s", "не могу найти %s"), f);
		exit (1);
	}
	while (t != wait (&status));
	if (t = status & 0377) {
		if (t != SIGINT)
			error (MSG ("fatal error in %s",
				"фатальная ошибка в %s"), f);
		dexit ();
	}
	return ((status >> 8) & 0377);
}

nodup (l, os)
char **l, *os;
{
	register char *t, *s;
	register int c;

	s = os;
	if (getsuf (s) != 'o') return (1);
	while (t = *l++) {
		while (c = *s++) if (c != *t++) break;
		if (!*t && !c) return (0);
		s = os;
	}
	return (1);
}

char *savestr (cp)
register char *cp;
{
	register int len;
	register char *new;

	len = strlen (cp) + 1;
	if (!(new = malloc (len))) {
		error (MSG ("out of memory", "мало памяти"));
		exit (1);
	}
	return (strncpy (new, cp, len));
}
