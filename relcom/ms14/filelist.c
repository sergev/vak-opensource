#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/time.h>

struct filtab {
	char *name;
	long size;
	long mtime;
	int mode;
};

int lflag;
int Rflag;
int mflag = 0;
int pagewidth = 80;
char *Iflag = 0;
char *progname;
char *home = "/pub";

struct filtab *tab;
int tabcnt;
int tablen;

char **dirtab;
int dirtabcnt;
int dirtablen;

int uid, gid;
long curtime;

extern int errno;

void usage ()
{
	fprintf (stderr, "Usage: list [-lR] [-m days] [-I ext] [dir...]\n");
	exit (-1);
}

int cmptab (struct filtab *a, struct filtab *b)
{
	if (a->size == b->size)
		return (strcmp (a->name, b->name));
	if (a->size == -1)
		return (-1);
	else if (b->size == -1)
		return (1);
	return (strcmp (a->name, b->name));
}

char *strcopy (char *str)
{
	char *p = malloc (strlen (str) + 1);

	if (! p) {
		fprintf (stderr, "out of memory\n");
		exit (-1);
	}
	strcpy (p, str);
	return (p);
}

char *strcatcopy (char *str1, char *str2, char *str3)
{
	char *p = malloc (strlen (str1) + strlen (str2) + strlen (str3) + 1);

	if (! p) {
		fprintf (stderr, "out of memory\n");
		exit (-1);
	}
	strcpy (p, str1);
	strcat (p, str2);
	strcat (p, str3);
	return (p);
}

char *filter (char *str)
{
	char *p = str, *q;
	int len = strlen (home);

	if (strncmp (str, home, len) == 0)
		p += len;
	if (! *p)
		return ("/");
	if (p[0]=='/' && p[1]=='.' && p[2]>='0' && p[2]<='9' && p[3]=='/')
		p += 3;
	q = strrchr (p, '/');
	if (q && q[1]=='.' && q[2]==0) {
		q[1] = 0;
		if (q > p)
			q[0] = 0;
	}
	return (p);
}

void addtab (char *name, struct stat *pst)
{
	int mode;

	/* Skip files older than mtime days */
	if (mflag && pst->st_size != -1 &&
	    pst->st_mtime < curtime - mflag*24L*60*60)
		return;

	if (tabcnt >= tablen) {
		tablen *= 2;
		tab = (struct filtab *) realloc ((char *) tab,
			tablen * sizeof (struct filtab));
		if (! tab) {
			fprintf (stderr, "out of memory\n");
			exit (-1);
		}
	}
	if (uid == pst->st_uid)
		mode = pst->st_mode >> 6;
	else if (gid == pst->st_gid)
		mode = pst->st_mode >>= 3;
	else
		mode = pst->st_mode;
	tab[tabcnt].name = strcopy (name);
	tab[tabcnt].size = pst->st_size;
	tab[tabcnt].mtime = pst->st_mtime;
	tab[tabcnt].mode = mode & 7;
	++tabcnt;
}

void adddir (char *parent, char *dir)
{
	if (dirtabcnt >= dirtablen) {
		dirtablen *= 2;
		dirtab = (char **) realloc ((char *) dirtab,
			dirtablen * sizeof (char *));
		if (! dirtab) {
			fprintf (stderr, "out of memory\n");
			exit (-1);
		}
	}
	if (! parent)
		dirtab[dirtabcnt] = strcopy (dir);
	else if (parent [strlen (parent) - 1] == '/')
		dirtab[dirtabcnt] = strcatcopy ("", parent, dir);
	else
		dirtab[dirtabcnt] = strcatcopy (parent, "/", dir);
	++dirtabcnt;
}

void scan (char *dir)
{
	DIR *dird;
	struct dirent *d;
	struct stat st;
	char name [512];
	char *p;

	strcpy (name, dir);
	p = name + strlen (name);
	if (p[-1] != '/')
		*p++ = '/';
	dird = opendir (dir);
	if (! dird) {
		perror (dir);
		return;
	}
	for (d=readdir(dird); d; d=readdir(dird)) {
		if (d->d_name[0] == '.')
			continue;
		strcpy (p, d->d_name);
		if (stat (name, &st) < 0) {
			if (errno != ENOENT)
				perror (name);
			continue;
		}
		if ((st.st_mode & S_IFMT) == S_IFDIR) {
			st.st_size = -1;
			addtab (d->d_name, &st);
			continue;
		}
		if ((st.st_mode & S_IFMT) != S_IFREG)
			continue;
		/* Skip '%' files */
		if (mflag && d->d_name[0] == '%')
			continue;
		if (Iflag) {
			/* Skip the specified extension */
			char *q = strrchr (d->d_name, '.');
			if (q && !strcmp (q+1, Iflag))
				continue;
		}
		addtab (d->d_name, &st);
	}
	closedir (dird);
}

int printname (unsigned char *p)
{
	int len = 0;
	for (; *p; ++p) {
		if ((*p>=' ' && *p<='~') || (*p>=0200 && *p<=0377)) {
			putchar (*p);
			++len;
		} else {
			printf ("\\%03o", *p);
			len += 4;
		}
	}
	return (len);
}

void lprint ()
{
	struct filtab *p;
	char *tim;
	char mod [4];

	for (p=tab; p<tab+tabcnt; ++p) {
		/* Skip directories in new files mode */
		if (mflag && p->size == -1)
			return;

		tim = ctime (&p->mtime);
		mod[0] = (p->mode & 4) ? 'r' : '-';
		mod[1] = (p->mode & 2) ? 'w' : '-';
		mod[2] = (p->mode & 1) ? 'x' : '-';
		mod[3] = 0;

		printf ("%3s ", mod);
		if (p->size == -1)
			printf ("   <dir>  ");
		else
			printf ("%8ld  ", p->size);
		printf ("%2.2s %6.6s %5.5s  ", tim+22, tim+4, tim+11);
		printname (filter (p->name));
		printf ("\n");
	}
}

int namelen (unsigned char *p)
{
	int len = 0;
	for (; *p; ++p) {
		if ((*p>=' ' && *p<='~') || (*p>=0200 && *p<=0377))
			++len;
		else
			len += 4;
	}
	return (len);
}

void print ()
{
	struct filtab *p, *fp;
	int maxlen, wid, hgh;

	/* Skip directories in new files mode */
	fp = tab;
	if (mflag)
		for (; fp<tab+tabcnt; ++fp)
			if (fp->size != -1)
				break;

	/* Compute the maximum file name length */
	maxlen = 0;
	for (p=fp; p<tab+tabcnt; ++p) {
		int len = namelen (p->name);
		if ((p->mode & 1) || p->size==-1)
			++len;
		if (len > maxlen)
			maxlen = len;
	}

	maxlen = 8 + maxlen / 8 * 8;
	wid = (pagewidth + 1) / maxlen;
	hgh = ((tab + tabcnt - fp) + wid - 1) / wid;

	for (p=fp; p<fp+hgh; ++p) {
		int len = 0;
		struct filtab *q;

		for (q=p; q<tab+tabcnt; q+=hgh) {
			if (len)
				for (; len<maxlen; len+=8)
					putchar ('\t');
			len = printname (q->name);
			if (q->size == -1) {
				putchar ('/');
				++len;
			} else if (q->mode & 1) {
				putchar ('*');
				++len;
			}
		}
		printf ("\n");
	}
}

void list (char *dir, int dirnameflag)
{
	struct filtab *p;

	if (tabcnt) {
		qsort ((char *) tab, tabcnt, sizeof (struct filtab),
			(void*) cmptab);

		p = tab;
		if (mflag) {
			/* Don't print names of empty directories */
			for (; p<tab+tabcnt; ++p)
				if (p->size != -1)
					break;
			if (p >= tab+tabcnt)
				dirnameflag = 0;
		}
		if (dirnameflag == 2)
			printf ("\n");
		if (dirnameflag)
			printf ("%s:\n", filter (dir));

		/* Print files */
		if (lflag)
			lprint (p);
		else
			print (p);

		/* Store directories in reverse order and free names. */
		for (p=tab+tabcnt-1; p>=tab; --p) {
			if (Rflag && p->size == -1)
				adddir (dir, p->name);
			free (p->name);
		}
	} else if (! mflag && dirnameflag) {
		if (dirnameflag == 2)
			printf ("\n");
		printf ("%s:\n", filter (dir));
	}
	tabcnt = 0;
}

void add (char *name)
{
	struct stat st;

	if (stat (name, &st) < 0) {
		perror (name);
		return;
	}
	if ((st.st_mode & S_IFMT) == S_IFREG)
		addtab (name, &st);
	else
		adddir ((char *) 0, name);
}

int main (int argc, char **argv)
{
	int dirnameflag;
	char **p;

	progname = *argv++;
	--argc;
	for (; argc>0 && **argv=='-'; ++argv, --argc) {
		char *p;

		for (p=1+*argv; *p; ++p) {
			switch (*p) {
			case 'g':       /* ignored */
			case 'A':       /* ignored */
			case 'a':       /* ignored */
				continue;
			case 'l':
				++lflag;
				continue;
			case 'R':
				++Rflag;
				continue;
			case 'm':
				++argv;
				--argc;
				mflag = atoi (*argv);
				break;
			case 'I':
				++argv;
				--argc;
				Iflag = *argv;
				if (*Iflag == '.')
					++Iflag;
				break;
			case 'd':
				++argv;
				--argc;
				home = *argv;
				break;
			default:
				usage ();
			}
			break;
		}
	}
	uid = getuid ();
	gid = getgid ();
	time (&curtime);
	dirtablen = 64;
	dirtabcnt = 0;
	dirtab = (char **) malloc (dirtablen * sizeof (char *));
	if (! dirtab) {
		fprintf (stderr, "out of memory\n");
		exit (-1);
	}
	tablen = 64;
	tabcnt = 0;
	tab = (struct filtab *) malloc (tablen * sizeof (struct filtab));
	if (! tab) {
		fprintf (stderr, "out of memory\n");
		exit (-1);
	}

	dirnameflag = 0;
	if (argc == 0)
		adddir ((char *) 0, ".");
	else {
		if (argc > 1)
			dirnameflag = 1;
		for (p=argv+argc-1; p>=argv; --p)
			add (*p);
		if (tabcnt) {
			list ((char *) 0, 0);
			dirnameflag = 2;
		}
	}

	while (dirtabcnt > 0) {
		int i = --dirtabcnt;
#ifdef S_IFLNK
		/* Don't follow symlinks during recurse search */
		struct stat st;
		if (dirnameflag > 1 && lstat (dirtab[i], &st) >= 0 &&
		    (st.st_mode & S_IFMT) == S_IFLNK)
			continue;
#endif
		scan (dirtab[i]);
		list (dirtab[i], dirnameflag);
		dirnameflag = 2;
	}
	return (0);
}
