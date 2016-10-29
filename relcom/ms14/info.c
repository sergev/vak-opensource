#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <syslog.h>
#include "match.h"
#include "ut.h"

struct filtab {
	char *name;
	long size;
	long mtime;
};

struct filtab *tab;
int tabcnt;
int tablen;
int timo;
long now;
char dirname [MAXPATHLEN];
int hflag;                              /* print header */
int fflag;                              /* print modification time */
int uflag;                              /* add "uploaded by" field */
int oflag;                              /* show files up to 00:00 today */
int sflag;                              /* DON'T split long fields */
int zflag;                              /* skip '0*' names */
int tflag;                              /* don't print dir.name and comment */
int mflag;                              /* print as mailserv commands */
char *home = "/pub";

extern int daylight;

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

void addtab (char *name, long size, long tim)
{
	if (tabcnt >= tablen) {
		tablen *= 2;
		tab = (struct filtab *) realloc ((char *) tab,
			tablen * sizeof (struct filtab));
		if (! tab) {
			fprintf (stderr, "info: out of memory\n");
			exit (-1);
		}
	}
	tab[tabcnt].name = strcopy (name);
	tab[tabcnt].size = size;
	tab[tabcnt].mtime = tim;
	++tabcnt;
}

int addit (char *name, long tim)
{
	struct stat ist;
	char info [512];
	long itim;

	mkinfoname (name, info);

	itim = tim;
	if (stat (info, &ist) >= 0 && ist.st_mtime > tim)
		itim = ist.st_mtime;
	if (timo && itim < now-timo)
		return (0);
	if (oflag && itim >= now)
		return (0);
	return (1);
}

void scan ()
{
	DIR *dird;
	struct dirent *d;
	struct stat st;

	dird = opendir (".");
	if (! dird) {
		perror (".");
		return;
	}
	for (d=readdir(dird); d; d=readdir(dird)) {
		if (d->d_name[0] == '.')
			continue;
		if (stat (d->d_name, &st) < 0) {
			perror (d->d_name);
			continue;
		}
		if ((st.st_mode & S_IFMT) == S_IFDIR) {
			if (! timo)
				addtab (d->d_name, -1L, st.st_mtime);
			continue;
		}
		if ((st.st_mode & S_IFMT) != S_IFREG)
			continue;
		/* Skip `0*' files */
		if (zflag && d->d_name[0] == '0')
			continue;
		if (addit (d->d_name, st.st_mtime))
			addtab (d->d_name, (long) st.st_size, st.st_mtime);
	}
	closedir (dird);
}

char *getupload (FILE *fd, char *buf, int sz)
{
	while (fgets (buf, sz, fd))
		if (!strnccmp (buf, "upload", 6)) {
			char *p;

			p = buf + strlen (buf);
			while (--p>=buf && (*p==' ' || *p=='\r' || *p=='\n' || *p=='\t'))
				*p = 0;
			p = buf;
			while (*p && *p!=':') ++p;
			if (! *p++)
				continue;
			while (*p==' ' || *p=='\t') ++p;
			return (*p ? p : 0);
		}
	return (0);
}

void printinfo (char *s, int off, int indent)
{
	int c, n;
	char *p;

	n = 74 - indent;
	if (n < 0) {
		putchar ('\n');

		for (n=0; n<off; ++n)
			putchar (' ');
		n = 74-off;
	}
	while (*s==' ' || *s=='\t')             /* skip leading blanks */
		++s;
	for (p=s; *p; ++p)
		if (*p == '\t')                 /* convert tabs to spaces */
			*p = ' ';
	for (--p; p>=s && *p==' '; --p)         /* remove trailing blanks */
		*p = 0;
	while (strlen (s) > n) {
		c = s [n];
		s [n] = 0;
		p = strrchr (s, ' ');
		s [n] = c;
		if (! p)
			p = s + n;

		c = *p;
		*p = 0;
		puts (s);
		*p = c;

		for (n=0; n<off; ++n)
			putchar (' ');
		n = 74-off;

		s = p;
		while (*s == ' ')
			++s;
	}
	puts (s);
}

void procdir (char *name)
{
	char info [512], *p;
	char upload [512], *u;
	FILE *fd = 0;

	if (strcmp (name, "..") != 0) {
		strcpy (info, "../.info/");
		p = strrchr (name, '/');
		if (! p) p = name; else ++p;
		strcat (info, p);
		fd = fopen (info, "r");
	}
	p = "";
	u = 0;
	if (fd && fgetinfo (info, sizeof (info), fd)) {
		p = info;
		if (info[0]>='0' && info[0]<='9' &&
		    info[1]>='0' && info[1]<='9' &&
		    info[2]>='0' && info[2]<='1' &&
		    info[3]>='0' && info[3]<='9' &&
		    info[4]>='0' && info[4]<='3' &&
		    info[5]>='0' && info[5]<='9') {
			p += 6;
		}
		while (*p==' ' || *p=='\t')
			++p;
		if (uflag)
			u = getupload (fd, upload, sizeof (upload));
	}
	if (fd)
		fclose (fd);

	if (mflag)
		printf ("CD ");
	printf ("%s", name);
	if (*p || (uflag && u)) {
		printf (" -- ");
		if (uflag && u) {
			strcat (p, " (uploaded by ");
			strcat (p, u);
			strcat (p, ")");
		}
		if (*p && !sflag)
			printinfo (p, 8, strlen (dirname) + (mflag ? 7 : 4));
		else {
			if (*p)
				printf ("%s", p);
			printf ("\n");
		}
	} else
		printf ("\n");
}

void proc (char *name, long size, long tim)
{
	struct tm *ptm;
	struct stat ist;
	char info [512], *p;
	char upload [512], *u;
	FILE *fd;
	long dat, mdat, itim;

	mkinfoname (name, info);

	ptm = localtime (&tim);
	dat = ptm->tm_year * 10000 + (ptm->tm_mon + 1) * 100 +
		ptm->tm_mday;

	itim = tim;
	if (stat (info, &ist) >= 0 && ist.st_mtime > tim)
		itim = ist.st_mtime;

	ptm = localtime (&itim);
	mdat = ptm->tm_year * 10000 + (ptm->tm_mon + 1) * 100 +
		ptm->tm_mday;

	fd = fopen (info, "r");
	p = "";
	u = 0;
	if (fd && fgetinfo (info, sizeof (info), fd)) {
		p = info;
		if (info[0]>='0' && info[0]<='9' &&
		    info[1]>='0' && info[1]<='9' &&
		    info[2]>='0' && info[2]<='1' &&
		    info[3]>='0' && info[3]<='9' &&
		    info[4]>='0' && info[4]<='3' &&
		    info[5]>='0' && info[5]<='9') {
			dat = (info[0]-'0') * 100000 + (info[1]-'0') * 10000 +
				(info[2]-'0') * 1000 + (info[3]-'0') * 100 +
				(info[4]-'0') * 10 + (info[5]-'0');
			p += 6;
		}
		while (*p==' ' || *p=='\t')
			++p;
		if (uflag)
			u = getupload (fd, upload, sizeof (upload));
	}
	if (fd)
		fclose (fd);

	if (mflag)
		printf (size >= 0 ? "GET " : "    ");
	if (!sflag && strlen (name) > 15) {
		printf ("%s\n                ", name);
		if (mflag)
			printf ("    ");
	} else
		printf ("%-15s ", name);
	if (size >= 0)
		printf ("%-5ld %6ld", (size + 1023) / 1024, dat);
	else
		printf ("<dir>       ");
	if (fflag)
		printf (" %6ld", mdat);
	if (*p || (uflag && u)) {
		printf (" ");
		if (uflag && u) {
			strcat (p, " (uploaded by ");
			strcat (p, u);
			strcat (p, ")");
		}
		if (!sflag) {
			int off = 29;

			if (fflag)
				off += 7;
			if (mflag)
				off += 4;
			printinfo (p, off, off);
		} else {
			if (*p)
				printf ("%s ", p);
			printf ("\n");
		}
	} else
		printf ("\n");
}

char *strfind (char *str, char *substr)
{
	int len = strlen (substr);

	for (; *str; ++str)
		if (! strncmp (str, substr, len))
			return (str);
	return (0);
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

int main (int argc, char **argv)
{
	register struct filtab *p;
	struct tm *tp;

	openlog ("minf", LOG_PERROR, LOG_USER);
	while (argc>1 && argv[1][0] == '-') {
		if (!strcmp (argv[1], "-h")) {
			++hflag;
			++argv;
			--argc;
		} else if (!strcmp (argv[1], "-f")) {
			++fflag;
			++argv;
			--argc;
		} else if (!strcmp (argv[1], "-t")) {
			++tflag;
			++argv;
			--argc;
		} else if (!strncmp (argv[1], "-d", 2)) {
			home = argv[1] + 2;
			++argv;
			--argc;
		} else if (!strcmp (argv[1], "-u")) {
			++uflag;
			++argv;
			--argc;
		} else if (!strcmp (argv[1], "-s")) {
			++sflag;
			++argv;
			--argc;
		} else if (!strcmp (argv[1], "-o")) {
			++oflag;
			++argv;
			--argc;
		} else if (!strcmp (argv[1], "-m")) {
			++mflag;
			++argv;
			--argc;
		} else if (!strcmp (argv[1], "-z")) {
			++zflag;
			++argv;
			--argc;
		} else if (argv[1][0]=='-' && argv[1][1]>='0' && argv[1][1]<='9') {
			timo = 24L * 60 * 60 * atoi (&argv[1][1]);
			++argv;
			--argc;
		} else
			goto usage;
	}
	if (argc > 1) {
		if (chdir (argv[1])<0) {
			char info [512];

			if (access (argv[1], 0) != 0) {
				printf ("%s: No such file or directory\n", argv[1]);
				return (0);
			}

			mkinfoname (argv[1], info);
			if (access (info, 4) != 0) {
				printf ("%s: No information\n", argv[1]);
				return (0);
			}
			fcat (info, 999);
			printf ("\n");
			return (0);
		}
		if (strcmp (argv[1], ".") != 0) {
			char *s;

			while (! strncmp (argv[1], "./", 2))
				++argv[1], ++argv[1];
			strcpy (dirname, argv[1]);
			while (dirname[0] && (s = strfind (dirname+1, "/./")))
				strcpy (s, s+2);
			if (strlen (dirname) > 2 && ! strcmp (dirname+strlen(dirname)-2, "/."))
				dirname[strlen(dirname)-2] = 0;
			if (strlen (dirname) > 1 && ! strcmp (dirname+strlen(dirname)-1, "/"))
				dirname[strlen(dirname)-1] = 0;
		}
		++argv;
		--argc;
	}
	if (argc != 1) {
usage:          fprintf (stderr, "Usage:\n\tinfo [-h] [-f] [-u] [-s] [-#] [dirname]\n");
		fprintf (stderr, "Options:\n\t-h\tprint header\n");
		fprintf (stderr, "\t-f\tprint modification times\n");
		fprintf (stderr, "\t-u\tadd uploaded-by field\n");
		fprintf (stderr, "\t-s\tdon't split long field into several lines\n");
		fprintf (stderr, "\t-o\tlist files older than 00:00 today\n");
		fprintf (stderr, "\t-t\tdon't print directory name and comment\n");
		fprintf (stderr, "\t-z\tskip file names, beginning with symbol '0'\n");
		fprintf (stderr, "\t-m\tprint as mailserv commands\n");
		fprintf (stderr, "\t-#\tlist files newer than # days\n");
		fprintf (stderr, "\t-dhome\tset home directory, default /pub\n");
		exit (-1);
	}
	time (&now);
	tp = localtime (&now);
	/* compute 00:00 of today (local time) */
	now = (now + tp->tm_gmtoff) /24/60/60 *24*60*60 - tp->tm_gmtoff;

	if (! *dirname)
		getwd (dirname);

	tablen = 64;
	tabcnt = 0;
	tab = (struct filtab *) malloc (tablen * sizeof (struct filtab));
	if (! tab) {
		fprintf (stderr, "info: out of memory\n");
		exit (-1);
	}

	scan ();

	if (tabcnt) {
		qsort ((char *) tab, tabcnt, sizeof (struct filtab),
			(void*) cmptab);
		if (*dirname && !tflag) {
			procdir (filter (dirname));
			printf ("\n");
		}
		if (hflag) {
			if (fflag) {
				if (mflag)
					printf ("    ");
				printf ("File name      Kbytes  Date   Mod   Description\n");
				if (mflag)
					printf ("    ");
				printf ("-----------------------------------------------\n");
			} else {
				if (mflag)
					printf ("    ");
				printf ("File name      Kbytes  Date  Description\n");
				if (mflag)
					printf ("    ");
				printf ("----------------------------------------\n");
			}
		}
		for (p=tab; p<tab+tabcnt; ++p)
			proc (p->name, p->size, p->mtime);
		printf ("\n");
	}
	return (0);
}
