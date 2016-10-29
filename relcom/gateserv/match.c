/*
 * Универсальный пакет выбора файлов по шаблону.
 * Пример:
 *      char *filename;
 *      if (! matchinit ("/sbin/mount*"))
 *              printf ("No such files: /sbin/mount*\n");
 *      else while (filename = matchnext ())
 *              printf ("%s\n", filename);
 *
 * Copyright (C) 1992-1995 Cronyx Ltd.
 * Автор: Сергей Вакуленко, vak@cronyx.ru
 * Wed Feb  8 18:29:31 MSK 1995
 */
#include <dirent.h>
#include <string.h>

static char dirname[256];
static char filename[256];
static char pattern[256];
static DIR *dir = 0;
static struct dirent *ent;

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
int match (register char *s, register char *p)
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

/* Продвижение на следующий файл, соответствующий шаблону */
static void matchforw ()
{
	while ((ent = readdir (dir)))
		if (ent->d_name[0] != '.' && match (ent->d_name, pattern))
			return;
}

int matchinit (char *pat)
{
	char *p;

	if (dir) {
		closedir (dir);
		dir = 0;
	}

	p = strrchr (pat, '/');
	if (p) {
		if (p-pat >= sizeof (pattern))
			return (0);
		strncpy (dirname, pat, p-pat);
		dirname[p-pat] = 0;
		dir = opendir (dirname);
		if (! dir)
			return (0);
		if (strlen (p+1) >= sizeof (pattern))
			return (0);
		strcpy (pattern, p+1);
	} else {
		*dirname = 0;
		dir = opendir (".");
		if (! dir)
			return (0);
		if (strlen (pat) >= sizeof (pattern))
			return (0);
		strcpy (pattern, pat);
	}
	matchforw ();
	if (! ent) {
		closedir (dir);
		dir = 0;
		return (0);
	}
	if (*dirname)
		strcat (dirname, "/");
	return (1);
}

char *matchnext ()
{
	if (! dir)
		return (0);
	strcpy (filename, dirname);
	strcat (filename, ent->d_name);
	matchforw ();
	if (! ent) {
		closedir (dir);
		dir = 0;
	}
	return (filename);
}

int ispattern (char *pattern)
{
	return (strpbrk (pattern, "*?[^") != 0);
}

#ifdef DEBUG
#include <stdio.h>

int main (int argc, char **argv)
{
	char *pattern, *filename;

	if (argc != 2 || argv[1][0]=='-') {
		fprintf (stderr, "Usage: match <pattern>\n");
		return (-1);
	}
	pattern = argv[1];
	if (! matchinit (pattern)) {
		fprintf (stderr, "No such files: %s\n", pattern);
		return (-1);
	}
	while ((filename = matchnext ()))
		printf ("\t%s\n", filename);
	return (0);
}
#endif
