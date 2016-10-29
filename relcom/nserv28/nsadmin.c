/*
 * Управление таблицей подписки сервера.
 */

# include <stdio.h>
# include "tagdefs.h"
# include "groups.h"

# define MAXUSERS 5000
# define MAXGROUPS 3000

# define CMDLIST        1
# define CMDCHECK       2
# define CMDADD         3
# define CMDREM         4
# define CMDSUB         5
# define CMDFEED        6
# define CMDRFEED       7
# define CMDUNS         8
# define CMDDUMP        9
# define CMDSYNC        10
# define CMDRESTART     11
# define CMDDIE         12

struct xtab {
	long user;
	long group;
	long mode;
};

char *progname;
int verbose = 1;
int gflag, uflag;

char *usertab [MAXUSERS];
char *grouptab [MAXGROUPS];
int nusers, ngroups;

char *upattab [MAXUSERS];
char *gpattab [MAXGROUPS];
int nupats, ngpats;

extern char *strcopy (), *malloc (), *realloc ();

usage ()
{
	fprintf (stderr, "Usage:\n");
	fprintf (stderr, "\t%s [-vs] list [ <user> ]\n", progname);
	fprintf (stderr, "\t%s [-vs] list <group>\n", progname);
	fprintf (stderr, "\t%s [-vs] check [ <user> ]\n", progname);
	fprintf (stderr, "\t%s [-vs] check <group>\n", progname);
	fprintf (stderr, "\t%s [-vs] add -u user...\n", progname);
	fprintf (stderr, "\t%s [-vs] add -g group...\n", progname);
	fprintf (stderr, "\t%s [-vs] rem <user>\n", progname);
	fprintf (stderr, "\t%s [-vs] rem <group>\n", progname);
	fprintf (stderr, "\t%s [-vs] sub <user> <group>\n", progname);
	fprintf (stderr, "\t%s [-vs] feed <user> <group>\n", progname);
	fprintf (stderr, "\t%s [-vs] rfeed <limit> <user> <group>\n", progname);
	fprintf (stderr, "\t%s [-vs] uns <user> [ <group> ]\n", progname);
	fprintf (stderr, "\t%s [-vs] uns <group>\n", progname);
# ifdef REMOTEDB
	fprintf (stderr, "\t%s [-vs] restart\n", progname);
	fprintf (stderr, "\t%s [-vs] die\n", progname);
# else
	fprintf (stderr, "\t%s [-vs] dump\n", progname);
# endif
	fprintf (stderr, "\t%s [-vs] sync [ <group> ]\n", progname);
	fprintf (stderr, "Here:\n");
	fprintf (stderr, "\t<limit> maximum length of an article in kbytes\n");
	fprintf (stderr, "\t<user>  -u user... | -U pattern...\n");
	fprintf (stderr, "\t<group> -g group... | -G pattern...\n");
	exit (-1);
}

quit ()
{
	exit (-1);
}

/* VARARGS1 */
messg (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	fprintf (stderr, s, a, b, c, d);
	fprintf (stderr, "\n");
}

/* VARARGS1 */
error (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	fprintf (stderr, s, a, b, c, d);
	fprintf (stderr, "\n");
}

/* VARARGS1 */
fatal (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	fprintf (stderr, s, a, b, c, d);
	fprintf (stderr, "\n");
	quit ();
}

main (argc, argv)
char **argv;
{
	int cmd, limit;
	long tag;
	char *name;

	progname = *(argv++);
	for (--argc; *argv && **argv=='-'; ++argv, --argc) {
		register char *p;

		for (p=1+*argv; *p; ++p)
			switch (*p) {
			case 'v':               /* verbose mode */
				++verbose;
				break;
			case 's':               /* silent mode */
				verbose = 0;
				break;
			default:
				usage ();
			}
	}
	if (argc < 1)
		usage ();

	if (! strcmp (*argv, "list"))
		cmd = CMDLIST;
	else if (! strcmp (*argv, "check"))
	      cmd = CMDCHECK;
	else if (! strcmp (*argv, "add"))
	      cmd = CMDADD;
	else if (! strcmp (*argv, "rem"))
	      cmd = CMDREM;
	else if (! strcmp (*argv, "del"))
	      cmd = CMDREM;
	else if (! strcmp (*argv, "sub"))
	      cmd = CMDSUB;
	else if (! strcmp (*argv, "uns"))
	      cmd = CMDUNS;
	else if (! strcmp (*argv, "feed"))
	      cmd = CMDFEED;
	else if (! strcmp (*argv, "rfeed"))
	      cmd = CMDRFEED;
	else if (! strcmp (*argv, "dump"))
	      cmd = CMDDUMP;
# ifdef REMOTEDB
	else if (! strcmp (*argv, "restart"))
	      cmd = CMDRESTART;
	else if (! strcmp (*argv, "die"))
	      cmd = CMDDIE;
# endif
	else if (! strcmp (*argv, "sync"))
	      cmd = CMDSYNC;
	else
		usage ();

	++argv;
	--argc;
	if (cmd == CMDRFEED) {
		limit = atoi (*argv);
		++argv;
		--argc;
	}

	parseusers (&argc, &argv);
	parsegroups (&argc, &argv);
	if (*argv)
		usage ();

	switch (cmd) {
	case CMDLIST:
	case CMDCHECK:
	case CMDADD:
	case CMDREM:
		if (uflag && gflag)
			usage ();
		break;
	case CMDDUMP:
		if (uflag || gflag)
			usage ();
		break;
	case CMDSYNC:
		if (uflag)
			usage ();
		break;
	}
loop:	switch (loadgroups (0)) {
	case -1:
		printf ("Groups database locked, waiting...\n");
		sleep (2);
		goto loop;
	case 0:
		return (-1);
	case 1:
		break;
	}
	if (nupats) {
		for (tag=firstuser(); tag; tag=nextuser(tag)) {
			if (nusers >= MAXUSERS)
				fatal ("too many users");
			name = username (tag);
			if (! name || ! matchtab (name, upattab, nupats))
				continue;
			usertab[nusers] = strcopy (name);
			++nusers;
		}
		if (! nusers)
			fatal ("No user matching pattern");
	}
	if (ngpats) {
		for (tag=firstgroup(); tag; tag=nextgroup(tag)) {
			if (ngroups >= MAXGROUPS)
				fatal ("too many groups");
			name = groupname (tag);
			if (! name || ! matchtab (name, gpattab, ngpats))
				continue;
			grouptab[ngroups] = strcopy (name);
			++ngroups;
		}
		if (! ngroups)
			fatal ("No group matching pattern");
	}
	switch (cmd) {
	case CMDLIST:
		if (ngroups || gflag)
			doglist ();
		else
			doulist ();
		break;
	case CMDCHECK:
		if (ngroups || gflag)
			dogcheck ();
		else
			doucheck ();
		break;
	case CMDADD:
		if (nusers || uflag)
			douadd ();
		else
			dogadd ();
		break;
	case CMDREM:
		if (nusers || uflag)
			dourem ();
		else
			dogrem ();
		break;
	case CMDSUB:    dosub ();       break;
	case CMDUNS:    douns ();       break;
	case CMDFEED:   dofeed ();      break;
	case CMDRFEED:  dorfeed (limit);break;
	case CMDDUMP:   dodump ();      break;
# ifdef REMOTEDB
	case CMDRESTART: restart ();    return (0);
	case CMDDIE:    die ();         return (0);
# endif
	case CMDSYNC:   dosync ();      break;
	}
	savegroups ();
	return (0);
}

parseusers (ac, av)
int *ac;
char ***av;
{
	nusers = 0;
	nupats = 0;
	for (;;) {
		if (! **av)
			return;
		if (! strcmp (**av, "-u")) {
			++uflag;
			++(*av);
			--(*ac);
			while (**av && ***av != '-') {
				if (nusers >= MAXUSERS)
					fatal ("too many users");
				usertab[nusers] = strcopy (**av);
				++nusers;
				++(*av);
				--(*ac);
			}
		} else if (! strcmp (**av, "-U")) {
			++uflag;
			++(*av);
			--(*ac);
			while (**av && ***av != '-') {
				if (nupats >= MAXUSERS)
					fatal ("too many user patterns");
				upattab[nupats] = strcopy (**av);
				++nupats;
				++(*av);
				--(*ac);
			}
		} else
			return;
	}
}

parsegroups (ac, av)
int *ac;
char ***av;
{
	ngroups = 0;
	ngpats = 0;
	for (;;) {
		if (! **av)
			return;
		if (! strcmp (**av, "-g")) {
			++gflag;
			++(*av);
			--(*ac);
			while (**av && ***av != '-') {
				if (ngroups >= MAXGROUPS)
					fatal ("too many groups");
				grouptab[ngroups] = strcopy (**av);
				++ngroups;
				++(*av);
				--(*ac);
			}
		} else if (! strcmp (**av, "-G")) {
			++gflag;
			++(*av);
			--(*ac);
			while (**av && ***av != '-') {
				if (ngpats >= MAXGROUPS)
					fatal ("too many group patterns");
				gpattab[ngpats] = strcopy (**av);
				++ngpats;
				++(*av);
				--(*ac);
			}
		} else
			return;
	}
}

matchtab (name, tab, n)
char *name;
char **tab;
{
	int i;

	for (i=0; i<n; ++i)
		if (match (name, tab[i]))
			return (1);
	return (0);
}

doulist ()
{
	int n;
	long tag;

	if (nusers)
		for (n=0; n<nusers; ++n)
			do1ulist (usertag (usertab [n]), usertab [n]);
	else
		for (tag=firstuser(); tag; tag=nextuser(tag))
			do1ulist (tag, NULL);
}

do1ulist (u, name)
long u;
char *name;
{
	long mode;

	mode = userflags (u);
	if (verbose > 1)
		printf ("<%lx>", u);
	if (! name)
		name = username (u);
	printf (" %2d  %s\n", mode, name);
}

doglist ()
{
	int n;
	long tag;

	if (ngroups)
		for (n=0; n<ngroups; ++n)
			do1glist (grouptag (grouptab [n]), grouptab [n]);
	else
		for (tag=firstgroup(); tag; tag=nextgroup(tag))
			do1glist (tag, NULL);
}

do1glist (g, name)
long g;
char *name;
{
	int nu;
	long last;
	char *info;

	groupsubscr (g, &nu);
	last = grouplast (g);
	info = groupinfo (g);
	printf (nu ? "%5d " : "      ", nu);
	printf (last ? "%5ld " : "      ", last);
	if (! name)
		name = groupname (g);
	if (info)
		printf ("%-23s %s\n", name, info);
	else
		printf ("%s\n", name);
}

do2dump (s, n)
char *s;
{
	int c;

	for (; --n >= 0; ++s) {
		c = (unsigned char) *s;
		if (c == '\\') {
			putchar ('\\');
			putchar ('\\');
		} else if (c >= ' ' && c <= '~' || c >= 0300 && c <= 0377 ||
		    c == 0xa3 || c==0xb3)
			putchar (c);
		else
			printf ("\\%03o", c);
	}
}

do1dump (k, ks, v, vs)
char *k, *v;
{
	if (ks == sizeof (long) && (*(long*)k & TAGLABEL))
		printf ("0x%08lx", *(long *) k);
	else if (ks) {
		putchar ('"');
		do2dump (k, ks);
		putchar ('"');
	} else
		printf ("<null>");
	putchar ('\t');
	if (vs == sizeof (long) &&
	    ((*(long*)v & TAGLABEL) || (*(unsigned long*)v < 0xffffff)))
		printf ("0x%08lx", *(long *) v);
	else if (vs) {
		putchar ('"');
		do2dump (v, vs);
		putchar ('"');
	} else
		printf ("<null>");
	putchar ('\n');
}

dodump ()
{
# ifdef REMOTEDB
	printf ("No dump in this version");
# else
	groupsdump (do1dump);
# endif
}

cmpxtab (a, b)
struct xtab *a, *b;
{
	if (a->user < b->user)
		return (-1);
	if (a->user > b->user)
		return (1);
	return (0);
}

pr1ucheck (u)
{
	struct subscrtab *s;
	int ns, n;
	char *feedlimit;

	printf ("%s\n", username (u));
	s = usersubscr (u, (char **) 0, &ns);
	if (! s)
		return;
	for (n=0; n<ns; ++n, ++s) {
		switch ((int) s->mode) {
		default:        continue;
		case 'A':       feedlimit = "2";        break;
		case 'B':       feedlimit = "3";        break;
		case 'C':       feedlimit = "5";        break;
		case 'D':       feedlimit = "8";        break;
		case 'E':       feedlimit = "10";       break;
		case 'F':       feedlimit = "15";       break;
		case 'G':       feedlimit = "20";       break;
		case 'H':       feedlimit = "30";       break;
		case 'I':       feedlimit = "50";       break;
		case 'J':       feedlimit = "80";       break;
		case 'K':       feedlimit = "200";      break;
		case 'f':       feedlimit = "f";        break;
		case 's':
			printf ("    %s\n", groupname (s->tag));
			continue;
		}
		printf ("    %-36s\t%s\n", groupname (s->tag), feedlimit);
	}
}

doucheck ()
{
	long *tagtab;
	long u;
	int n;

	if (! nusers) {
		for (u=firstuser(); u; u=nextuser(u))
			pr1ucheck (u);
		return;
	}
	tagtab = (long *) malloc (nusers * sizeof (long));
	if (! tagtab)
		fatal ("out of memory in doucheck!");
	for (n=0; n<nusers; ++n)
		tagtab [n] = usertag (usertab [n]);
	for (n=0; n<nusers; ++n)
		pr1ucheck (tagtab [n]);
	free ((char *) tagtab);
}

# ifdef notdef
	struct xtab *tab;
	int tabsz, ntab;

	ntab = 0;
	tabsz = 256;
	tab = (struct xtab *) malloc (tabsz * sizeof (struct xtab));
	if (! tab)
		fatal ("out of memory in doucheck");
	for (g=firstgroup(); g; g=nextgroup(g)) {
		struct subscrtab *s;
		int ns, is;

		s = groupsubscr (g, &ns);
		for (is=0; is<ns; ++is) {
			if (! nusers)
				goto addrecord;
			for (iu=0; iu<nusers; ++iu)
				if (s[is].tag == tagtab [iu])
					goto addrecord;
			continue;
addrecord:
			if (ntab >= tabsz) {
				tabsz += 256;
				tab = (struct xtab *) realloc (
					(char *) tab,
					tabsz * sizeof (struct xtab));
				if (! tab)
					fatal ("out of memory in doucheck.");
			}
			tab[ntab].user = s[is].tag;
			tab[ntab].group = g;
			tab[ntab].mode = s[is].mode;
			++ntab;
		}
	}
	if (nusers)
		free ((char *) tagtab);
	if (ntab) {
		int i;
		long u, oldu;

		qsort ((char *) tab, ntab, sizeof (struct xtab), cmpxtab);
		oldu = 0;
		for (i=0; i<ntab; ++i) {
			u = tab[i].user;
			if (u != oldu) {
				printf ("%s\n", username (u));
				oldu = u;
			}
			switch ((int) tab[i].mode) {
			default:        continue;
			case 'A':       feedlimit = "2";        break;
			case 'B':       feedlimit = "3";        break;
			case 'C':       feedlimit = "5";        break;
			case 'D':       feedlimit = "8";        break;
			case 'E':       feedlimit = "10";       break;
			case 'F':       feedlimit = "15";       break;
			case 'G':       feedlimit = "20";       break;
			case 'H':       feedlimit = "30";       break;
			case 'I':       feedlimit = "50";       break;
			case 'J':       feedlimit = "80";       break;
			case 'K':       feedlimit = "200";      break;
			case 'f':       feedlimit = "f";        break;
			case 's':
				printf ("    %s\n", groupname (tab[i].group));
				continue;
			}
			printf ("    %-36s\t%s\n", groupname (tab[i].group),
				feedlimit);
		}
	}
	free ((char *) tab);
# endif

do1gcheck (g, name)
long g;
char *name;
{
	int nu, i;
	struct subscrtab *s;
	char *feedlimit;

	printf ("%s\n", name);
	s = groupsubscr (g, &nu);
	for (i=0; i<nu; ++i) {
		switch (s[i].mode) {
		default:        continue;
		case 'A':       feedlimit = "2";        break;
		case 'B':       feedlimit = "3";        break;
		case 'C':       feedlimit = "5";        break;
		case 'D':       feedlimit = "8";        break;
		case 'E':       feedlimit = "10";       break;
		case 'F':       feedlimit = "15";       break;
		case 'G':       feedlimit = "20";       break;
		case 'H':       feedlimit = "30";       break;
		case 'I':       feedlimit = "50";       break;
		case 'J':       feedlimit = "80";       break;
		case 'K':       feedlimit = "200";      break;
		case 'f':       feedlimit = "f";        break;
		case 's':
			printf ("    %s\n", username (s[i].tag));
			continue;
		}
		printf ("    %-36s\t%s\n", username (s[i].tag), feedlimit);
	}
}

dogcheck ()
{
	int n;
	long tag;

	if (ngroups)
		for (n=0; n<ngroups; ++n)
			do1gcheck (grouptag (grouptab [n]), grouptab [n]);
	else
		for (tag=firstgroup(); tag; tag=nextgroup(tag))
			do1gcheck (tag, groupname (tag));
}

douadd ()
{
	int i;

	for (i=0; i<nusers; ++i) {
		if (usertag (usertab [i])) {
			if (verbose)
				printf ("User %s already exists\n", usertab [i]);
			continue;
		}
		adduser (usertab [i]);
		if (verbose)
			printf ("User %s added\n", usertab [i]);
	}
}

dogadd ()
{
	int i;

	for (i=0; i<ngroups; ++i) {
		if (grouptag (grouptab [i])) {
			if (verbose)
				printf ("Group %s already exists\n", grouptab [i]);
			continue;
		}
		addgroup (grouptab [i], 0L);
		if (verbose)
			printf ("Group %s added\n", grouptab [i]);
	}
}

dourem ()
{
	int i;
	long *tagtab;
	int *rez;

	if (! nusers) {
		printf ("No such users\n");
		return;
	}
	tagtab = (long *) malloc (nusers * sizeof (long));
	if (! tagtab) {
		printf ("Out of memory in dourem()\n");
		return;
	}
	for (i=0; i<nusers; ++i) {
		tagtab [i] = usertag (usertab [i]);
		if (verbose && ! tagtab [i])
			printf ("User %s does not exist\n", usertab [i]);
	}
	rez = delusertab (tagtab, nusers);
	if (! rez) {
		printf ("Error in delusertab()\n");
		free ((char *) tagtab);
		return;
	}
	if (verbose)
		for (i=0; i<nusers; ++i)
			printf ("User %s removed (%d)\n", usertab [i], rez [i]);
	free ((char *) tagtab);
}

dogrem ()
{
	int i;

	for (i=0; i<ngroups; ++i) {
		if (! delgroup (grouptab [i])) {
			if (verbose)
				printf ("Group %s does not exist\n", grouptab [i]);
			continue;
		}
		if (verbose)
			printf ("Group %s removed\n", grouptab [i]);
	}
}

dosub ()
{
	do1sub ('s');
}

dofeed ()
{
	do1sub ('f');
}

dorfeed (limit)
{
	int mode;

	if (limit < 2)
		mode = 'f';
	else if (limit < 3)
		mode = 'A';
	else if (limit < 5)
		mode = 'B';
	else if (limit < 8)
		mode = 'C';
	else if (limit < 10)
		mode = 'D';
	else if (limit < 15)
		mode = 'E';
	else if (limit < 20)
		mode = 'F';
	else if (limit < 30)
		mode = 'G';
	else if (limit < 50)
		mode = 'H';
	else if (limit < 80)
		mode = 'I';
	else if (limit < 200)
		mode = 'J';
	else if (limit < 400)
		mode = 'K';
	else
		mode = 's';
	do1sub (mode);
}

do1sub (mode)
{
	int iu, ig;
	long u, g;

	for (ig=0; ig<ngroups; ++ig) {
		g = grouptag (grouptab [ig]);
		if (! g) {
			struct activetab *a = getactive (grouptab [ig]);
			if (! a) {
				printf ("No such group: %s\n",
					grouptab [ig]);
				continue;
			}
			addgroup (grouptab [ig], a->last);
			if (verbose)
				printf ("Added group %s\n",
					grouptab [ig]);
			g = grouptag (grouptab [ig]);
		}
		if (! g)
			continue;
		for (iu=0; iu<nusers; ++iu) {
			u = usertag (usertab [iu]);
			if (! u) {
				adduser (usertab [iu]);
				if (verbose)
					printf ("Added user %s\n",
						usertab [iu]);
				u = usertag (usertab [iu]);
			}
			if (! u)
				continue;
			if (subscribe (u, g, mode) && verbose)
				printf ("User %s subscribed to %s, mode %s\n",
					usertab [iu], grouptab [ig],
					mode=='f' ? "feed" :
					mode=='s' ? "notify" : "rfeed");
		}
	}
}

douns ()
{
	int iu, ig;
	long u, g;
	char *name;
	long *tagtab;

	if (! nusers) {
		for (ig=0; ig<ngroups; ++ig) {
			g = grouptag (grouptab [ig]);
			if (! g)
				continue;
			setsubscr (g, (struct subscrtab *) 0, 0);
			if (verbose)
				printf ("All users unsubscribed from group %s\n",
					grouptab [ig]);
		}
		return;
	}
	tagtab = (long *) malloc (nusers * sizeof (long));
	if (! tagtab) {
		printf ("Out of memory in douns()\n");
		return;
	}
	for (iu=0; iu<nusers; ++iu)
		tagtab [iu] = usertag (usertab [iu]);
	if (! ngroups) {
		int *rez = unsubscrtab (tagtab, nusers);
		if (rez) {
			for (iu=0; iu<nusers; ++iu)
				printf ("User %s unsubscribed from %d groups\n",
					usertab [iu], rez [iu]);
		} else
			printf ("Error while unsubscribing %d users\n", nusers);
		free ((char *) tagtab);
		return;
	}
	for (ig=0; ig<ngroups; ++ig) {
		g = grouptag (grouptab [ig]);
		for (iu=0; iu<nusers; ++iu) {
			u = tagtab [iu];
			if (unsubscribe (u, g) && verbose)
				printf ("User %s unsubscribed from %s\n",
					usertab [iu], grouptab [ig]);
		}
	}
	free ((char *) tagtab);
}

dosync ()
{
	long g, last;
	int ig;
	struct activetab *a;
	char *gname;

	groupslimit (1024);
	if (! ngroups) {
		for (gname=firstactive(); gname; gname=nextactive(gname)) {
			a = getactive (gname);
			if (! a)
				continue;
			g = grouptag (gname);
			if (g) {
				last = grouplast (g);
				if (last == a->last)
					continue;
				setgrouplast (g, a->last);
				if (verbose)
					printf ("Skipped %ld articles in group %s\n",
						a->last - last, gname);
			} else {
				addgroup (gname, a->last);
				if (verbose)
					printf ("Added group %s\n", gname);
			}
		}
	}
	for (ig=0; ig<ngroups; ++ig) {
		g = grouptag (gname = grouptab [ig]);
		a = getactive (gname);
		if (! a)
			continue;
		last = grouplast (g);
		if (last == a->last)
			continue;
		setgrouplast (g, a->last);
		if (verbose)
			printf ("Skipped %ld articles in group %s\n",
				a->last - last, gname);
	}
	groupssync ();
}
