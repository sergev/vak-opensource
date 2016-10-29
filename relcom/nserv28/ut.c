# include <stdio.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# ifdef BSD
#    include <sys/timeb.h>
# endif
# include <stdio.h>
# include <signal.h>
# include "config.h"
# include "lock.h"
# include <time.h>

# define TFILENAME      "/tmp/NSuXXXXXX"

# ifdef SIGVOID
#    define SIGTYPE void
# else
#    define SIGTYPE int
# endif

static long tzshift;

extern char *ctime (), *mktemp (), *malloc (), *strcpy (), *time2a ();
extern char *strcat (), *strchr (), *strrchr ();
extern char *rfcdate (), *genrfcdate ();
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
	{
		if ( c == '\\' ) {
		       c = getc(fd);
		       if ( c < 0 ) break;
		       switch (c) {
		case 't': *s++ = '\t' ; break;
		case 'r': *s++ = '\r' ; break;
		case '\\': *s++ = '\\'; break;
		case '\n':
		case 'n': *s++ = '\n'; break;
		default:  *s++ = '\\'; *s++ = c; break;
			}
		} else
		       *s++ = c;
	}
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

sigign ()
{
	signal (SIGINT, SIG_IGN);
	signal (SIGQUIT, SIG_IGN);
	signal (SIGTERM, catcher);
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
	struct tm *ptm, *localtime();
	static char cdate [30];

	ptm = localtime (&tim);
	sprintf (cdate, "%02d.%02d.%02d/%02d:%02d:%02d",
		ptm->tm_mday, ptm->tm_mon+1, ptm->tm_year,
		ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	return (cdate);
}

timeinit ()
{
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
		quit ();
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

checkglist(group,user)
char *group, *user;
{
char buf[512];
       if ( checkpermissions(group,SERVDIR,"groups_list") )
	      return(1);
       if ( !user ) return(0);
       sprintf(buf,"%s/%s",group,user);
       return( checkpermissions(buf,SERVDIR,"groups_list") );
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
		return (1);
	permissions = 0;
	while (getstr (fd, line)) {
		p = line;
		while (*p==' ' || *p=='\t')
			++p;
		if (! *p || *p=='#')
			continue;
		/* Если адрес явно запрещен - достаточно */
		if (line[0] == '^' ) {
		       if ( match (username, line+1))
			    break;
		       continue;
		}
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

long f2size (dirname, filename)
char *dirname, *filename;
{
	char buf [512];

	sprintf (buf, "%s/%s", dirname, filename);
	return (filesize (buf));
}

f2clean (dirname, filename)
char *dirname, *filename;
{
	char buf [512];

	sprintf (buf, "%s/%s", dirname, filename);
	close (creat (buf, 0664));
}

time_t f2modt (dirname, filename)
char *dirname, *filename;
{
	char buf [512];
	struct stat stbuf;

	sprintf (buf, "%s/%s", dirname, filename);
	if ( stat(buf, &stbuf) < 0 )
	       return(-1);
	return(stbuf.st_mtime);
}

f2unlink (dirname, filename)
char *dirname, *filename;
{
	char buf [512];

	sprintf (buf, "%s/%s", dirname, filename);
	return (unlink (buf));
}

f2rename (dir1name, file1name, dir2name, file2name)
char *dir1name, *file1name;
char *dir2name, *file2name;
{
	char buf1 [512];
	char buf2 [512];

	sprintf (buf1, "%s/%s", dir1name, file1name);
	sprintf (buf2, "%s/%s", dir2name, file2name);
# ifdef ultrix
	return(rename(buf1, buf2));
# else
	unlink(buf2);
	if ( link (buf1, buf2) == 0 )
		return(unlink (buf1));
	else
		return(-1);
# endif
}

f2link (dir1name, file1name, dir2name, file2name)
char *dir1name, *file1name;
char *dir2name, *file2name;
{
	char buf1 [512];
	char buf2 [512];

	sprintf (buf1, "%s/%s", dir1name, file1name);
	sprintf (buf2, "%s/%s", dir2name, file2name);
	return(link(buf1, buf2));
}

char *size2a (size)
long size;
{
	static char buf [8];

	if (size < 1000)
		sprintf (buf, "%ld", size);
	else if (size < 10240)
		sprintf (buf, "%ld.%ldK", size/1024, size*10/1024 % 10);
	else if (size < 1000*1024)
		sprintf (buf, "%ldK", size/1024);
	else
		sprintf (buf, "%ld.%ldM", size/1024/1024, size*10/1024/1024 % 10);
	return (buf);
}

/*
 * Get sender address from 'From:' field
 * of mail header.
 */

char *getsendername (field)
char *field;
{
	register c;
	register char *cp, *cp2;
	int gotlt, lastsp, level;
	char addr [256];

	gotlt = 0;
	lastsp = 0;
	cp = field;
	cp2 = addr;
	for (c = *cp++; c; c = *cp++)
	{
		if ( cp2 - addr > 250 ) break;
		switch (c) {
		case '(':
			level = 1;
			while (*cp != '\0' && level) {
				switch (*cp++) {
				case '(':
					level++;
					break;
				case ')':
					level--;
					break;
				}
			}
			if (*cp)
				cp++;
			lastsp = 0;
			break;
		case ' ':
			if (cp[0] == 'a' && cp[1] == 't' && cp[2] == ' ')
				cp += 3, *cp2++ = '@';
			else if (cp[0] == '@' && cp[1] == ' ')
				cp += 2, *cp2++ = '@';
			else
				lastsp = 1;
			break;
		case '<':
			cp2 = addr;
			gotlt++;
			lastsp = 0;
			break;
		case '>':
			if (gotlt)
				goto done;
			/* Fall into . . . */
		default:
			if (lastsp) {
				lastsp = 0;
				*cp2++ = ' ';
			}
			*cp2++ = c;
			break;
		}
	}
done:
	*cp2 = 0;
	if (*addr == '@') {
		char buf [512];

		strcpy (buf, addr);
		strcpy (addr, "root");
		strcat (addr, buf);
	}
	return (strcopy (addr));
}

putsubj (s, n)
char *s;
{
	int c;
	char *p;

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
		printf (">\t\t ");
		n = 60;

		s = p;
		while (*s == ' ')
			++s;
	}
	puts (s);
}

/*
 * Cut an address at 25 chars
 */
char *cut25 (a)
char *a;
{
	register char *p;
	static char buf [100];
	char buf2 [100];

	strcpy (buf, a);
	while (strlen (buf) > 25) {
		if (strchr (buf, '%')) {
			p = strchr (buf, '@');
			if (! p)
				p = strrchr (buf, '%');
			*p = '\0';
			p = strrchr (buf, '%');
			if (p)
				*p = '@';
			continue;
		}
		p = strrchr (buf, '@');
		if (p) {
			*p++ = '\0';
			strcpy (buf2, buf);
			p = strchr (p, '.');
			if (p) {
				strcat (buf2, "@");
				strcat (buf2, p+1);
			}
			strcpy (buf, buf2);
			continue;
		}
		p = strchr (buf, '!');
		if (p) {
			strcpy (buf2, p+1);
			strcpy (buf, buf2);
			continue;
		}
		buf[25] = '\0';
	}
	return (buf);
}

sudomain (addr)
char *addr;
{
	int len;

	len = strlen (addr);
	if (len < 3)
		return (0);
	addr += len-3;
	if (!strcmp (addr, ".su") || !strcmp (addr, ".ua") || 
	    !strcmp (addr, ".by"))
		return (1);
	return (0);
}

char *canondate (dat)
char *dat;
{
	register char *p = 0;

	if (dat)
		p = rfcdate (dat);
	if (! p)
		p = genrfcdate (time ((long *) 0), (int) (tzshift / 60));
	return (p);
}

char *gendate ()
{
	return (genrfcdate (time ((long *) 0), (int) (tzshift / 60)));
}

instat (tab, ntab, addr, size)
char **tab;
char *addr;
long size;
{
	FILE *fd;

	fd = f2open (SERVDIR, "instat", "a");
	if (! fd)
		return (-1);
	fprintf (fd, "%s  %ld  %s ", time2a (time ((long *) 0)),
		size, addr);
	while (--ntab >= 0)
		fprintf (fd, " %s", *tab++);
	fprintf (fd, "\n");
	fclose (fd);
	return (0);
}

outstat (gclass, addr, size)
char *gclass, *addr;
long size;
{
	FILE *fd;

	fd = f2open (SERVDIR, "outstat", "a");
	if (! fd)
		return (-1);
	fprintf (fd, "%s  %ld  %s  %s\n", time2a (time ((long *) 0)),
		size, gclass, addr);
	fclose (fd);
	return (0);
}

savepid (fnam,killit)
char *fnam;
int killit;    /* 0 -ждать, 1 - Kill, 2 - если занято, выйти */
{
	FILE *fd;
	static struct flock fl;
	int pid;

	fd = f2open (SERVDIR, fnam, "r+");
	if ( !fd )
	       fd = f2open (SERVDIR, fnam, "w");
	if (! fd) {
		error ("cannot create %s/%s", SERVDIR, fnam);
		quit ();
	}
	fl.l_type = F_WRLCK;
	if ( killit == 1 ) {
	       while ( fcntl(fileno(fd), F_SETLK, &fl) < 0 ) {
		       fseek(fd,0l,0);
		       if ( fscanf(fd,"%d",&pid) == 1 )
		       {
			       kill(pid,SIGTERM);
			       error("Send SIGTERM to %d",pid);
		       }
		       sleep(5);
	       }
	} else if ( fcntl(fileno(fd), killit? F_SETLK: F_SETLKW, &fl) < 0 )
	{
	       error("Can't lock %s",fnam);
	       quit();
	}
	fseek(fd,0l,0);
	fprintf (fd, "%d\n", getpid ());
	fflush(fd);
	/* fclose (fd); */
}

static int n_fd = -1;
void new_unlock()
{
if ( n_fd >= 0 ) close(n_fd);
n_fd = -1;
}

void new_lock (flag)
{
	char name [512];
	int i;

	if ( n_fd >= 0 ) return;
	sprintf (name, "%s/.newlock", SERVDIR);
	if (access (name, 0) != 0)
		close (creat (name, 0600));
	n_fd = open (name, 1);
	if (n_fd < 0) {
		error ("cannot open lock file %s", name);
		quit ();
	}
	fcntl (n_fd, F_SETFD, 1);        /* close on exec */
	while (! LOCK (n_fd)) {
		sleep(30);
		if ( ++i % 10 == 0 )
		       error("'new+' locked > 5 minutes. waiting");
	}
}
