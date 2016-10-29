/*
 * Command summary:
 *
 * HELP                         - get brief description of mailserv.
 * QUIT                         - end of command file.
 * PWD                          - print current directory name.
 * DIR [-R] [-#] [dir]          - list files in current directory.
 * LS [-R] [-#] [dir]           - brief list of files.
 * INDEX [dir...]               - get description of files in directories.
 * CD dir [comment]             - enter directory.
 * CD                           - go to home directory.
 * GET [-#] file [comment]      - get files (not directories!).
 * LIST archive...              - list archive contents.
 * EXTRACT [-#] archive [file...] - extract files from archive.
 * INFO file...                 - get information about files.
 * READ [-#] file...            - list contents of the text file.
 * ARCHIVER compress | gzip | freeze | lharc | zoo | zip | none - select archiver.
 * ENCODER uuencode | btoa | ship - select encoder.
 * SPLIT text | binary          - set split mode.
 * SIZE #                       - set max piece size in kbytes.
 * SIZE                         - print max piece size in kbytes.
 * LIMIT #                      - set max total volume in kbytes.
 * LIMIT                        - print max total volume in kbytes.
 */
#include <sys/types.h>
#include <sys/param.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "server.h"
#include "config.h"
#include "ut.h"
#include "match.h"

#ifndef CLK_TCK
#   define CLK_TCK 60                   /* for BSD etc */
#endif

long starttime;
long endtime;

static char *archiver = " -nopack";
static char *encoder = " -uuencode";
static debug = 0;
static bsplitflag = 0;                  /* text split */
static ksize = 30;                      /* 30 kbytes */
static klimit = 2*1024;                 /* 2 Mbytes */
static txtsize = 29999;                 /* max lines for text files */

extern char *sender;
extern char home[];

void doarchiver (int ac, char **av);
void docd (int ac, char **av);
void dodebug (int ac, char **av);
void dodir (int ac, char **av);
void doencoder (int ac, char **av);
void doextract (int ac, char **av);
void dofind (int ac, char **av);
void doget (int ac, char **av);
void doinfo (int ac, char **av);
void dolimit (int ac, char **av);
void dolist (int ac, char **av);
void dols (int ac, char **av);
void doread (int ac, char **av);
void dosize (int ac, char **av);
void dosplit (int ac, char **av);

void doftp (void);
void dohelp (void);
void donews (void);
void dopwd (void);

struct cmdtab cmdtable [] = {
	{ "article",    donews          },
	{ "archiver",   doarchiver      },
	{ "bye",        doquit          },
	{ "cd",         docd            },
	{ "connect",    doftp           },
	{ "debug",      dodebug         },
	{ "dir",        dodir           },
	{ "encoder",    doencoder       },
	{ "exit",       doquit          },
	{ "extract",    doextract       },
	{ "feed",       donews          },
	{ "find",       dofind          },
	{ "get",        doget           },
	{ "group",      donews          },
	{ "help",       dohelp          },
	{ "host",       doftp           },
	{ "index",      dodir           },
	{ "info",       doinfo          },
	{ "limit",      dolimit         },
	{ "list",       dolist          },
	{ "ls",         dols            },
	{ "mget",       doget           },
	{ "open",       doftp           },
	{ "pack",       donews          },
	{ "pwd",        dopwd           },
	{ "quit",       doquit          },
	{ "read",       doread          },
	{ "send",       doget           },
	{ "show",       dolist          },
	{ "size",       dosize          },
	{ "split",      dosplit         },
	{ "subscribe",  donews          },
	{ "unsubscr",   donews          },
	{ 0 }
};

char *escape (char *str)
{
	static char buf[256];
	char *p = buf;

	while (*str) {
		switch (*str) {
		case '$': case '"': case '<': case '>': case ';': case '`':
		case '|': case '&': case '(': case ')': case '!':
		case '\\': case '\'':
			*p++ = '\\';
		}
		*p++ = *str++;
	}
	*p = 0;
	return (buf);
}

void mksendcmd (char *cmd)
{
	sprintf (cmd, "%s%s/bin/uusplit -%d -p %s/bin/logmessage",
		debug ? "/bin/bash -x " : "/bin/bash ", SERVDIR, ksize, SERVDIR);
	if (strcmp (archiver, " -compress"))
		strcat (cmd, archiver);
	if (strcmp (encoder, " -uuencode"))
		strcat (cmd, encoder);
	if (bsplitflag)
		strcat (cmd, " -bsplit");
}

void dodebug (int ac, char **av)
{
	if (! *++av) {
		printf ("Debug is %s\n", debug ? "ON" : "OFF");
		return;
	}
	strlower (*av);
	if (! strcmp (*av, "on"))
		debug = 1;
	else if (! strcmp (*av, "off"))
		debug = 0;
	else {
		printf ("Usage: debug [ on | off ]\n");
		return;
	}
	printf ("Debug turned %s\n", debug ? "ON" : "OFF");
}

void dolimit (int ac, char **av)
{
	if (*++av) {
		klimit = atoi (*av);
		if (klimit < 1)
			klimit = 1;
		else if (klimit > 30*1024)
			klimit = 30*1024;
	}
	printf ("Limit = %d kbytes\n", klimit);
}

void dosize (int ac, char **av)
{
	if (*++av) {
		ksize = atoi (*av);
		if (ksize < 5)
			ksize = 5;
		else if (ksize > 100)
			ksize = 100;
	}
	printf ("Size = %d kbytes\n", ksize);
}

void dosplit (int ac, char **av)
{
	if (*++av) {
		strlower (*av);
		if (! strcmp (*av, "binary"))
			bsplitflag = 1;
		else if (! strcmp (*av, "text"))
			bsplitflag = 0;
		else
			fatal ("Bad split mode: %s", *av);
	}
	printf ("Split = %s\n", bsplitflag ? "binary" : "text");
}

void doencoder (int ac, char **av)
{
	if (*++av) {
		strlower (*av);
		if (! strcmp (*av, "uuencode"))
			encoder = " -uuencode";
		else if (! strcmp (*av, "btoa"))
			encoder = " -btoa";
		else if (! strcmp (*av, "ship"))
			encoder = " -ship";
		else
			fatal ("Bad encoder name: %s", *av);
	}
	printf ("Encoder = %s\n", encoder+2);
}

void doarchiver (int ac, char **av)
{
	if (*++av) {
		strlower (*av);
		if (! strcmp (*av, "compress"))
			archiver = " -compress";
		else if (! strcmp (*av, "gzip"))
			archiver = " -gzip";
		else if (! strcmp (*av, "freeze"))
			archiver = " -freeze";
		else if (! strcmp (*av, "lharc"))
			archiver = " -lharc";
		else if (! strcmp (*av, "zoo"))
			archiver = " -zoo";
		else if (! strcmp (*av, "zip"))
			archiver = " -zip";
		else if (! strcmp (*av, "none"))
			archiver = " -nopack";
		else
			fatal ("Bad archiver name: %s", *av);
	}
	printf ("Archiver = %s\n", archiver+2);
}

/*
 * Make full path name from PUBDIR-relative name.
 * Remove all '../' sequences and check if the name is legal.
 */
int makename (char *pattern, char *name)
{
	char *p;

	/* Convert all '\' to '/' */
	p = pattern;
	for (;;) {
		p = strchr (p, '\\');
		if (! p)
			break;
		*p++ = '/';
	}

	if (*pattern == '/')
		strcpy (name, home);
	else
		getwd (name);
	while (*pattern == '/')
		++pattern;

	do {
		p = strchr (pattern, '/');
		if (! p)
			p = pattern + strlen (pattern);

		if (pattern[0]=='.' && (p-pattern==1 ||
		    (p-pattern == 2 && pattern[1]=='.'))) {
			if (p-pattern == 2) {
				char *q = strrchr (name, '/');
				*q = 0;

				/* Remove trailing /.[0-9] */
				q = strrchr (name, '/');
				if (q && q[1]=='.' && q[2]>='0' &&
				    q[2]<='9' && q[3]==0)
					*q = 0;
			}
			if (! *p)
				break;
			pattern = p+1;
			continue;
		}
		strcat (name, "/");
		strncat (name, pattern, p-pattern);
		pattern = p+1;
	} while (*p);

	/* Remove trailing /.[0-9] */
	p = strrchr (name, '/');
	if (p && p[1]=='.' && p[2]>='0' && p[2]<='9' && p[3]==0)
		*p = 0;

	return (strncmp (name, home, strlen (home)) == 0);
}

void dolist (int ac, char **av)
{
	char name[MAXPATHLEN];

	while (*++av) {
		if (! makename (*av, name)) {
			message ("Bad file name: %s", *av);
			continue;
		}
		run ("%s/bin/arlist %s", SERVDIR, escape (name));
	}
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

void doread1 (char *name)
{
	struct stat st;

	if (stat (name, &st) < 0) {
		message ("No such file: %s", filter (name));
		return;
	}
	if ((st.st_mode & S_IFMT) != S_IFREG || !textfile (name)) {
		message ("Nontext file: %s", filter (name));
		return;
	}
	message ("-------- Begin of file '%s' --------", filter (name));
	fcat (name, txtsize);
	message ("-------- End of file '%s' --------", filter (name));
	error ("read file %s", strip (name));
}

void doread (int ac, char **av)
{
	int nflag = 0;
	char name[MAXPATHLEN];

	/* parse flags */
	for (++av; *av && **av=='-'; ++av) {
		register char *p;

		for (p=1+*av; *p; ++p)
			switch (*p) {
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				nflag = nflag * 10 + *p - '0';
				break;
			default:
				message ("Bad option: %s", *av);
				return;
			}
	}
	if (nflag > 0)
		txtsize = nflag;
	if (! makename (*av, name)) {
		message ("Bad file name: %s", *av);
		return;
	}
	if (ispattern (name)) {
		char *file;

		if (! matchinit (name)) {
			message ("No such file or directory: %s", filter (name));
			return;
		}
		while ((file = matchnext ()))
			doread1 (file);
	} else
		doread1 (name);
}

void doinfo1 (char *name)
{
	char info[256];

	mkinfoname (name, info);
	if (access (info, 4)) {
		message ("No information about %s", filter (name));
		return;
	}
	if (access (name, 0) == 0)
		message ("\nFile %s: ", filter (name));
	fcat (info, 9999);
	error ("send info about %s to %s", strip (name), sender);
}

void doinfo (int ac, char **av)
{
	char name[MAXPATHLEN], *file;

	while (*++av) {
		if (! makename (*av, name)) {
			message ("Bad file name: %s", *av);
			continue;
		}
		if (ispattern (name)) {
			if (! matchinit (name)) {
				message ("No such file or directory: %s", filter (name));
				continue;
			}
			while ((file = matchnext ()))
				doinfo1 (file);
		} else
			doinfo1 (name);
	}
}

void doextract (int ac, char **av)
{
	char *p, cmd[5*1024], parts[80], name[MAXPATHLEN], dir[MAXPATHLEN], buf[MAXPATHLEN];
	int len;

	if (klimit <= 0) {
		message ("transfer limit exceeded");
		return;
	}
	parts[0] = 0;
	for (++av; **av=='-'; ++av) {
		if (parts [0])
			strcat (parts, ",");
		strcat (parts, *av+1);
	}

	if (! *av || ! makename (*av, name)) {
		message ("Bad archive name: %s", *av);
		return;
	}
	mksendcmd (cmd);
	strcat (cmd, " -a ");
	strcat (cmd, name);
	klimit -= filesize (name) / 1024;

	if (parts[0]) {
		strcat (cmd, " -o ");
		strcat (cmd, parts);
	}
	strcat (cmd, " ");
	strcat (cmd, sender);

	if (! *++av) {
		strcat (cmd, " \\*");
		runs (cmd);
		return;
	}

	strcpy (dir, name);
	p = strrchr (dir, '/');
	if (*p)
		p[1] = 0;
	else
		strcpy (dir, "./");
	len = strlen (dir);

	for (; *av; ++av) {
		strcpy (buf, dir);
		strcat (buf, *av);
		if (! makename (buf+strlen(home), name)) {
			message ("Bad file name: %s", *av);
			return;
		}
		if (strncmp (name, dir, len) != 0) {
			message ("Incorrect file name: %s", *av);
			if (debug)
				message ("name=`%s' should be `%s...' (%d bytes)",
					name, dir, len);
			return;
		}
		strcat (cmd, " ");
		strcat (cmd, escape (name+len));
	}

	if (debug)
		strcat (cmd, " 2>&1");
	runs (cmd);
}

void doget (int ac, char **av)
{
	char cmd [5*1024];
	char parts [80];
	char name [MAXPATHLEN];

	if (klimit <= 0) {
		message ("transfer limit exceeded");
		return;
	}
	parts[0] = 0;
	for (++av; *av && **av=='-'; ++av) {
		if (parts [0])
			strcat (parts, ",");
		strcat (parts, *av+1);
	}
	if (! *av) {
		message ("nothing to get");
		return;
	}
	mksendcmd (cmd);
	strcat (cmd, " -d");
	if (parts [0]) {
		strcat (cmd, " -o ");
		strcat (cmd, parts);
	}
	strcat (cmd, " ");
	strcat (cmd, sender);

	/* Обрабатываем только первый аргумент, остальные - комментарий. */
	if (! makename (*av, name)) {
		message ("Bad file name: %s", *av);
		return;
	}
	if (ispattern (name)) {
		char *file;

		if (! matchinit (name)) {
			message ("No such file or directory: %s", filter (name));
			return;
		}
		while ((file = matchnext ())) {
			if (strlen (cmd) > sizeof (cmd) - 80) {
				message ("Too many files, aborted");
				return;
			}
			strcat (cmd, " ");
			strcat (cmd, escape (file));
			klimit -= filesize (file) / 1024;
			if (klimit <= 0) {
				message ("transfer limit exceeded");
				break;
			}
		}
	} else {
		strcat (cmd, " ");
		strcat (cmd, escape (name));
		klimit -= filesize (name) / 1024;
	}

	if (debug)
		strcat (cmd, " 2>&1");
	runs (cmd);
}

void dofind (int ac, char **av)
{
	char *dir = PUBLICDIR, *rflag = "-r", wd [MAXPATHLEN];

	getwd (wd);
	if (strcmp (wd, home) != 0 && access ("./.index", 0) == 0) {
		dir = ".";
		rflag = "";
	}
	for (++av; *av; ++av)
		run ("/usr/local/bin/lfind %s -d%s %s",
			rflag, dir, escape (*av));
}

void docd (int ac, char **av)
{
	char name[MAXPATHLEN];

	++av;
	if (! *av) {
		if (chdir (PUBLICDIR) < 0)
			fatal ("Cannot chdir to %s", PUBLICDIR);
		return;
	}
	if (! makename (*av, name))
		fatal ("Bad directory name: %s", *av);
	if (chdir (name) < 0)
		fatal (access (name, 0) ? "No such directory: %s" :
			"Not a directory: %s", filter (name));
}

void dodir2 (char *name, int lflag, int rflag, int nflag)
{
	if (! lflag)
		if (nflag)
			run ("%s/bin/filelist -d %s -%s -m %d %s", SERVDIR,
				home, rflag ? "R" : "", nflag, escape (name));
		else
			run ("%s/bin/filelist -d %s -%s %s", SERVDIR,
				home, rflag ? "R" : "", escape (name));
	else if (rflag)
		run ("find %s/. -type d -name \\* ! -name .info -exec %s/bin/info -m -h -d%s -%d {} \\;",
			escape (name), SERVDIR, home, nflag);
	else
		run ("%s/bin/info -d%s -m -h -%d %s", SERVDIR, home,
			nflag, escape (name));
	name = strip (name);
	error (rflag ? "send recursive directory %s to %s" :
		"send directory %s to %s", *name ? name : "/", sender);
}

void dodir1 (int ac, char **av, int lflag)
{
	int rflag = 0;          /* list recursively */
	int nflag = 0;          /* list new files */
	char name [MAXPATHLEN];

	/* parse flags */
	for (++av; *av && **av=='-'; ++av) {
		register char *p;

		for (p=1+*av; *p; ++p)
			switch (*p) {
			case 'r':
			case 'R':
				++rflag;
				break;
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				nflag = nflag * 10 + *p - '0';
				break;
			default:
				message ("Bad option: %s", *av);
				return;
			}
	}
	if (! *av)
		getwd (name);
	else if (! makename (*av, name)) {
		message ("Bad file name: %s", *av);
		return;
	}
	if (access (name, 0) != 0) {
		message ("No such directory: %s", *av);
		return;
	}
	if (! isdir (name)) {
		message ("Not a directory: %s", *av);
		return;
	}
	dodir2 (name, lflag, rflag, nflag);
	printf ("\n");
}

void dodir (int ac, char **av)
{
	dodir1 (ac, av, 1);
}

void dols (int ac, char **av)
{
	dodir1 (ac, av, 0);
}

void dopwd ()
{
	char wd [MAXPATHLEN];

	getwd (wd);
	printf ("%s\n", filter (wd));
}

void dohelp ()
{
	if (f2access (SERVDIR, "help", 4))
		fatal ("No help available");
	f2cat (SERVDIR, "help");
	doquit ();
}

void donews ()
{
	if (f2access (SERVDIR, "newsmsg", 4))
		fatal ("Unknown command, try newsserv@kiae.su");
	else
		f2cat (SERVDIR, "newsmsg");
	doquit ();
}

void doftp ()
{
	if (f2access (SERVDIR, "ftpmsg", 4))
		fatal ("Unknown command, try ftpmail@kiae.su");
	else
		f2cat (SERVDIR, "ftpmsg");
	doquit ();
}

void doquit ()
{
	struct tms t;
	register long rt;
	register float ut, st;

	time (&endtime);
	times (&t);
	printf ("\n%s", ctime (&endtime));
	ut = (float) (t.tms_utime + t.tms_cutime) / CLK_TCK;
	st = (float) (t.tms_stime + t.tms_cstime) / CLK_TCK;
	rt = endtime - starttime;
	printf ("Time Total %.1f User %.1f System %.1f Real %ld:%02ld:%02ld\n",
		ut + st, ut, st, rt/3600, rt/60%60, rt%60);
	exitloop ();
}
