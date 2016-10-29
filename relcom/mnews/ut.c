# include <sys/types.h>
# include <sys/stat.h>
# ifdef BSD
#    include <sys/timeb.h>
# endif
# include <stdio.h>
# include <signal.h>
# include "config.h"

# define TFILENAME      "/tmp/msbXXXXXX"

# ifdef SIGVOID
#    define SIGTYPE void
# else
#    define SIGTYPE int
# endif

static long tzshift;

extern char *ctime (), *mktemp (), *malloc (), *strcpy (), *time2a ();
extern long time ();
FILE *f2open ();

/*
 * Get string from file.
 * Return 1 if ok, else 0.
 */

getstr (fd, s)
FILE *fd;
register char *s;
{
	register c;

	while ((c = getc (fd)) >= 0 && c!='\n')
		*s++ = c;
	*s = 0;
	return (c >= 0);
}

/*
 * Copy file to file.
 * Return 0 on success,
 * -1 on read error,
 * -2 on write error.
 */

ffcopy (from, to)
FILE *from, *to;
{
	char buf [512], *p;
	register n, k;

	for (;;) {
		n = fread (buf, 1, sizeof (buf), from);
		if (n < 0) {
			error ("read error");
			return (-1);
		}
		if (n == 0)
			break;
		p = buf;
		while (n > 0) {
			k = fwrite (p, 1, n, to);
			if (k <= 0) {
				error ("write error");
				return (-2);
			}
			n -= k;
			p += k;
		}
	}
	return (0);
}

/*
 * Copy len bytes from file to file.
 * Return 0 on success,
 * -1 on read error,
 * -2 on write error.
 */

ffncopy (from, to, len)
FILE *from, *to;
long len;
{
	char buf [512], *p;
	register n, k;

	while (len > 0) {
		n = fread (buf, 1, sizeof (buf), from);
		if (n < 0) {
			error ("read error");
			return (-1);
		}
		if (n == 0)
			break;
		if (n > len)
			n = len;
		len -= n;
		p = buf;
		while (n > 0) {
			k = fwrite (p, 1, n, to);
			if (k <= 0) {
				error ("write error");
				return (-2);
			}
			n -= k;
			p += k;
		}
	}
	return (0);
}

log (progname, s, a, b, c, d)
char *progname, *s, *a, *b, *c, *d;
{
	FILE *fd;

	fd = fopen (LOGFILE, "a");
	if (! fd)
		return (-1);
	fprintf (fd, "%s  ", time2a (time ((long *) 0)));
	if (progname)
		fprintf (fd, "%s  ", progname);
	fprintf (fd, s, a, b, c, d);
	fprintf (fd, "\n");
	fclose (fd);
	return (0);
}

/*
 * Wait until queue is unlocked,
 * then lock it by creating lock file.
 */

lock (name)
char *name;
{
	/* wait while queue is locked */
	while (! access (name, 0))
		sleep (1);

	/* lock queue */
	if (creat (name, 0) < 0) {
		error ("cannot create %s", name);
		exit (-1);
	}
}

/*
 * Return 1 if file is locked.
 */

locked (name)
char *name;
{
	return (! access (name, 0));
}

/*
 * Unlock queue by deleting lock file
 */

unlock (name)
char *name;
{
	unlink (name);
}

/*
 * Signal catched, unlock and exit.
 */

SIGTYPE catcher ()
{
	error ("signal catched");
	quit ();
}

/*
 * Set cather of signal, if it is not ignored.
 */

catch (sig, fun)
SIGTYPE (*fun) ();
{
	if (signal (sig, SIG_IGN) != SIG_IGN)
		signal (sig, fun);
}

/*
 * Set up signal catcher.
 */

sigcatch ()
{
	catch (SIGINT, catcher);
	catch (SIGQUIT, catcher);
	catch (SIGTERM, catcher);
}

long filesize (name)
char *name;
{
	struct stat x;

	if (stat (name, &x) < 0)
		return (0);
	return (x.st_size);
}

char *strlower (str)
char *str;
{
	register char *p;

	for (p=str; *p; ++p) {
		if (*p>='A' && *p<='Z')
			*p += 'a' - 'A';
		if (*p>=(char)0340 && *p<=(char)0376)
			*p -= 040;
	}
	return (str);
}

/*
 * Преобразование времени (секунды от 00:00:00 1.1.70)
 * в текстовое представление.  Возвращается указатель
 * на внутренний statiс буфер.
 * Представление времени: dd.mm.yy/hh:mm:ss
 */

char *time2a (tim)
long tim;
{
	register d0, d1;
	long hms, nday;
	int sec, min, hour, year, month, day, dysize;
	static dmsize [12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	static char cdate [20];

	tim -= tzshift;

	hms = tim % 86400L;
	nday = tim / 86400L;
	if (hms < 0) {
		hms += 86400L;
		nday -= 1;
	}

	sec = hms % 60;
	d1 = hms / 60;
	min = d1 % 60;
	d1 /= 60;
	hour = d1;

	if (nday >= 0)
		for (d1=70; nday >= (dysize = d1%4==0?366:365); ++d1)
			nday -= dysize;
	else
		for (d1=70; nday<0; --d1)
			nday += (d1-1) % 4 == 0 ? 366 : 365;
	year = d1;
	d0 = nday;

	if (d1%4 == 0)
		dmsize[1] = 29;
	for (d1=0; d0 >= dmsize[d1]; d1++)
		d0 -= dmsize[d1];
	dmsize[1] = 28;
	day = d0+1;
	month = d1+1;

	if (year >= 100)
		year -= 100;
	sprintf (cdate, "%02d.%02d.%02d/%02d:%02d:%02d",
		day, month, year, hour, min, sec);
	return (cdate);
}

/*
 * Преобразование времени из текстового представления
 * в секунды от 00:00:00 1.1.70.
 * Представление времени: dd.mm.yy/hh:mm:ss
 */

long a2time (cdate)
char *cdate;
{
	int sec, min, hour, year, month, day;
	long tim, nday;
	static dmsize [12] = {0,31,59,90,120,151,181,212,243,273,304,334};

	sec = min = hour = 0;
	year = month = day = 1;
	sscanf (cdate, "%02d.%02d.%02d/%02d:%02d:%02d",
		&day, &month, &year, &hour, &min, &sec);
	if (year < 70)
		year += 100;

	tim = (hour * 60L + min) * 60 + sec;
	nday = day-1 + dmsize [month-1] + (year - 70) * 365L + (year - 69) / 4;
	if (year%4 == 0 && month > 2)
		nday += 1;
	tim += nday * 60 * 60 * 24;
	return (tim + tzshift);
}

timeinit ()
{
# ifdef BSD
	struct timeb tb;

	ftime (&tb);
	tzshift = tb.timezone * 60;
# else
	extern long timezone;

	tzset ();
	tzshift = timezone;
# endif
}

/*
 * Сопоставление с шаблоном. В случае успеха возвращает 1.
 * Шаблон может содержать следующие символы:
 *
 * ^       - в начале шаблона - отрицание шаблона
 * *       - любая строка
 * ?       - любой символ
 * [a-z]   - символ в диапазоне
 * [^a-z]  - символ вне диапазона
 */

match (s, p)
register char *s, *p;
{
	register scc;
	int c, yes;

	yes = 1;
	if (*p == '^') {
		yes = 0;
		++p;
	}
	for (;;) {
		scc = *s++ & 0377;
		switch (c = *p++) {
		default:
			if ((c & 0377) != scc)
				return (! yes);
			continue;
		case 0:
			return (scc==0 ? yes : !yes);
		case '?':
			if (scc == 0)
				return (! yes);
			continue;
		case '*':
			if (! *p)
				return (yes);
			for (--s; *s; ++s)
				if (match (s, p))
					return (yes);
			return (! yes);
		case '[': {
			int ok, lc, good;

			lc = 077777;
			good = 1;
			if (*p == '^') {
				good = 0;
				++p;
			}
			ok = ! good;
			for (;;) {
				int cc;

				cc = *p++;
				if (cc == 0)
					return (! yes);         /* Missing ] */
				if (cc == ']')
					break;
				if (cc == '-') {
					if (lc <= scc && scc <= *p++)
						ok = good;
				} else
					if (scc == (lc = cc))
						ok = good;
			}
			if (! ok)
				return (! yes);
			continue;
		}
		}
	}
}

char *strcopy (str)
char *str;
{
	unsigned len = strlen (str);
	char *p = malloc (len+1);

	if (! p) {
		error ("out of memory in strcopy");
		exit (-1);
	}
	strcpy (p, str);
	return (p);
}

char *time2fname (tim)
long tim;
{
	int i;
	static char buf [10];
	char *p;

	p = buf;
	for (i=28; i>=0; i-=4)
		*p++ = 'a' + (tim >> i & 0xf);
	return (buf);
}

checkpermissions (username, dirname, permfile)
char *username, *dirname, *permfile;
{
	FILE *fd;
	char line [512];
	char *p;
	int permissions;

	fd = f2open (dirname, permfile, "r");
	if (! fd)
		return (0);
	permissions = 0;
	while (getstr (fd, line)) {
		p = line;
		while (*p==' ' || *p=='\t')
			++p;
		if (! *p || *p=='#')
			continue;
		if (match (username, line)) {
			permissions = 1;
			break;
		}
	}
	fclose (fd);
	return (permissions);
}

FILE *f2open (dirname, filename, mode)
char *dirname, *filename, *mode;
{
	char buf [512];

	sprintf (buf, "%s/%s", dirname, filename);
	return (fopen (buf, mode));
}

f2access (dirname, filename, mode)
char *dirname, *filename;
{
	char buf [512];

	sprintf (buf, "%s/%s", dirname, filename);
	return (access (buf, mode));
}

f2clean (dirname, filename)
char *dirname, *filename;
{
	char buf [512];

	sprintf (buf, "%s/%s", dirname, filename);
	close (creat (buf, 0664));
}
