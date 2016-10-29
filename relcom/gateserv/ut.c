/*
 * Вспомогательные функции сервера.
 *
 * Copyright (C) 1992-1995 Cronyx Ltd.
 * Автор: Сергей Вакуленко, vak@cronyx.ru
 * Wed Feb  8 18:29:31 MSK 1995
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#define TFILENAME       "/tmp/msbXXXXXX"

#ifdef SIGVOID
#   define SIGTYPE void
#else
#   define SIGTYPE int
#endif

extern void quit (void);
extern int error (char*, ...);

/*
 * Get string from file.
 * Return 1 if ok, else 0.
 */
int getstr (FILE *fd, register char *s)
{
	char *b = s;
	register c;
again:
	while ((c = getc (fd)) >= 0 && c!='\n')
		*s++ = c;
	*s = 0;
	while (s>b && (s[-1]==' ' || s[-1]=='\t' || s[-1]=='\r'))
		*--s = 0;
	if (s>b && s[-1] == '\\') {
		*--s = 0;
		while (c>=0 && (c = getc (fd)) >= 0 && (c==' ' || c=='\t'))
			continue;
		if (c >= 0) {
			*s++ = ' ';
			ungetc (c, fd);
		}
		goto again;
	}
	return (c >= 0);
}

/*
 * Copy file to file.
 * Return 0 on success,
 * -1 on read error,
 * -2 on write error.
 */
int ffcopy (FILE *from, FILE *to)
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
int ffncopy (FILE *from, FILE *to, long len)
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
void catch (int sig, SIGTYPE (*fun) ())
{
	if (signal (sig, SIG_IGN) != SIG_IGN)
		signal (sig, fun);
}

/*
 * Set up signal catcher.
 */
void sigcatch ()
{
	catch (SIGINT, catcher);
	catch (SIGQUIT, catcher);
	catch (SIGTERM, catcher);
	signal (SIGPIPE, SIG_IGN);
}

void sigign ()
{
	signal (SIGINT, SIG_IGN);
	signal (SIGQUIT, SIG_IGN);
	signal (SIGPIPE, SIG_IGN);
	signal (SIGTERM, catcher);
}

long filesize (char *name)
{
	struct stat x;

	if (stat (name, &x) < 0)
		return (0);
	return (x.st_size);
}

int isdir (char *name)
{
	struct stat x;

	if (stat (name, &x) < 0)
		return (0);
	return (S_ISDIR (x.st_mode));
}

/*
 * Преобразование времени (секунды от 00:00:00 1.1.70)
 * в текстовое представление.  Возвращается указатель
 * на внутренний statiс буфер.
 * Представление времени: dd.mm.yy/hh:mm:ss
 */
char *time2a (long tim)
{
	struct tm *ptm, *localtime();
	static char cdate [30];

	ptm = localtime (&tim);
	sprintf (cdate, "%02d.%02d.%02d/%02d:%02d:%02d",
		ptm->tm_mday, ptm->tm_mon+1, ptm->tm_year,
		ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	return (cdate);
}

char *strcopy (char *str)
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

FILE *f2open (char *dirname, char *filename, char *mode)
{
	char buf [512];

	sprintf (buf, "%s/%s", dirname, filename);
	return (fopen (buf, mode));
}

int f2access (char *dirname, char *filename, int mode)
{
	char buf [512];

	sprintf (buf, "%s/%s", dirname, filename);
	return (access (buf, mode));
}

void f2clean (char *dirname, char *filename)
{
	char buf [512];

	sprintf (buf, "%s/%s", dirname, filename);
	close (creat (buf, 0664));
}
